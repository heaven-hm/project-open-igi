#include "dev_log.hpp"
#include "igi/retail/script/humanplayer_defines.hpp"

#include "igi/retail/config/config_graphic_options.hpp"
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/script/qvm_execute.hpp"
#include "igi/retail/script/qvm_native_loader.hpp"

#include <cmath>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// The retail's human player general parameters (0x0056E1E8..0x0056E208) and the two parameter
// tables (0x005389F8 and 0x00538B08..0x00538C18). The retail addresses are process image
// addresses; the port owns relocated storage with the retail layout, and DefineHumanPlayerGeneral
// writes every dword the retail body writes.
igi::retail::script::HumanPlayerGeneralParameters HumanPlayer_GeneralParameters = {};
igi::retail::script::HumanPlayerScriptParameter HumanPlayer_GeneralParameterTableA
    [igi::retail::script::human_player_script_parameter_count] = {};
igi::retail::script::HumanPlayerScriptParameter HumanPlayer_GeneralParameterTableB
    [igi::retail::script::human_player_script_parameter_count] = {};

// Native: DefineHumanPlayerGeneral; retail VA 0x00410070.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                 VA 0x00410070-0x004101B5, 326 bytes, 5 blocks/6 edges;
//                 docs/fidelity/evidence/0x00410070-*.json.
// Signature/ABI: x86 cdecl. The body reads the argument frame from [esp+0x34] at 0x00410075,
//                 which is the third stack argument, and the result block pointer from
//                 [esp+0x30] at 0x0041017E, which is the first stack argument; the second stack
//                 argument is never read. The script VM calls its natives with that layout at
//                 0x004BD8F3-0x004BD8FE (arg1 = the VM's value block slot, arg2 = the symbol
//                 record's +0x18 dword, arg3 = the argument frame) and copies six dwords from the
//                 returned EAX, so the native returns the context pointer it was given.
// Behavior/CFG: one entry block, then the straight-line argument stores; two do-while table
//                 walks at 0x00410145 and 0x00410164; the value-block build and rep movsd at
//                 0x00410187-0x004101AD; return.
void* NATIVE_DECL DefineHumanPlayerGeneral(void* context, std::int32_t argument_slot,
                                                void* frame)
{
    NATIVE_LOG_ENTER("DefineHumanPlayerGeneral", "0x00410070");
    // The second stack argument is passed by the script VM but never read by the retail body
    // (0x00410070-0x004101B5 reads only [esp+0x34] at the entry and [esp+0x30] at the exit).
    (void)argument_slot;

    // 0x0041007A-0x00410082: argument 0 is stored unscaled.
    HumanPlayer_GeneralParameters.argument_0 = static_cast<float>(Qvm_GetFloatArg(frame, 0));

    // 0x00410088-0x004100D3: arguments 1..3 are multiplied by the 4096000.0f float at 0x005334A4
    // and then by the 1/108000 float at 0x005334A0. Both multiplies are x87 80-bit operations on
    // the reader's 80-bit result (fmul dword / fmul dword) before the fstp dword store, which is
    // what the long double intermediates and the final float cast reproduce.
    HumanPlayer_GeneralParameters.argument_1 = static_cast<float>(
        Qvm_GetFloatArg(frame, 1) * igi::retail::script::human_player_scale_4096000 *
        igi::retail::script::human_player_scale_inverse);
    HumanPlayer_GeneralParameters.argument_2 = static_cast<float>(
        Qvm_GetFloatArg(frame, 2) * igi::retail::script::human_player_scale_4096000 *
        igi::retail::script::human_player_scale_inverse);
    HumanPlayer_GeneralParameters.argument_3 = static_cast<float>(
        Qvm_GetFloatArg(frame, 3) * igi::retail::script::human_player_scale_4096000 *
        igi::retail::script::human_player_scale_inverse);

    // 0x004100D9-0x004100F8: arguments 4 and 5 are multiplied by the 4096.0 double at 0x00533498
    // (fmul qword).
    HumanPlayer_GeneralParameters.argument_4 = static_cast<float>(
        Qvm_GetFloatArg(frame, 4) * igi::retail::script::human_player_scale_4096);
    HumanPlayer_GeneralParameters.argument_5 = static_cast<float>(
        Qvm_GetFloatArg(frame, 5) * igi::retail::script::human_player_scale_4096);

    // 0x00410101-0x00410117: argument 6 is multiplied by the 30.0 double at 0x00533488 and
    // converted with `fistp qword`; the low dword of that 64-bit result is what retail stores.
    // `fistp` converts under the x87 control word in force at that moment, which is not
    // truncation: retail's chop-mode setter 0x00498460 runs before the task loop (from
    // 0x0048F530) while this native runs during startup. std::rintl rounds under the current
    // control word exactly like `fistp`, so the port follows the same control word as retail
    // instead of forcing the C++ cast's truncation.
    const auto argument_6_scaled = static_cast<std::int64_t>(
        std::rintl(Qvm_GetFloatArg(frame, 6) * igi::retail::script::human_player_scale_30));
    HumanPlayer_GeneralParameters.argument_6 =
        static_cast<std::int32_t>(argument_6_scaled & 0xFFFFFFFFLL);

    // 0x0041011C-0x00410137: arguments 7 and 8 are stored unscaled.
    HumanPlayer_GeneralParameters.argument_7 = static_cast<float>(Qvm_GetFloatArg(frame, 7));
    HumanPlayer_GeneralParameters.argument_8 = static_cast<float>(Qvm_GetFloatArg(frame, 8));

    // 0x00410140-0x0041015D: the 17 records at 0x005389F8 take arguments 9..25, unscaled, one
    // float in the first dword of each 0x10-byte record.
    std::int32_t argument_index = 9;
    for (std::size_t index = 0;
         index < igi::retail::script::human_player_script_parameter_count; ++index) {
        HumanPlayer_GeneralParameterTableA[index].value =
            static_cast<float>(Qvm_GetFloatArg(frame, argument_index));
        ++argument_index;
    }

    // 0x0041015F-0x0041017C: the 17 records at 0x00538B08..0x00538C18 take arguments 26..42,
    // continuing the same index.
    for (std::size_t index = 0;
         index < igi::retail::script::human_player_script_parameter_count; ++index) {
        HumanPlayer_GeneralParameterTableB[index].value =
            static_cast<float>(Qvm_GetFloatArg(frame, argument_index));
        ++argument_index;
    }

    // 0x00410187-0x004101AD: the 0x18-byte script value block. Retail writes +0x00 = 1,
    // +0x08..+0x0F = float64 1.0 and +0x10 = the type descriptor 0x00567C74, then copies six
    // dwords to the context. The +0x04 and +0x14 dwords are never written by retail; the port's
    // zero-initialised block makes them zero instead of stack residue, the same documented
    // difference as the config value blocks in config_graphic_options.cpp.
    igi::retail::config::ScriptValueBlock block{};
    block.integer = 1;
    block.real = 1.0;
    block.type_descriptor = igi::retail::config::script_real_type_descriptor;
    std::memcpy(context, &block, sizeof(block));

    // 0x0041017E-0x004101B5: EAX = the context argument and return.
    return context;
}

// Native: HumanPlayer_LoadParameters; retail VA 0x004137A0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Tools/evidence: Binary Ninja, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                 VA 0x004137A0-0x004137D4, 57 bytes, 4 blocks/4 edges;
//                 docs/fidelity/evidence/0x004137A0-*.json.
// Behavior/CFG: 0x004137A0-0x004137A6 calls Qvm_Load with the literal "LOCAL:humanplayer/
//                 humanplayer.qsc" (0x00539188). The non-null branch (0x004137B4-0x004137C4)
//                 calls Qvm_Execute then Qvm_Cleanup and returns EAX, which the `pop esi; ret`
//                 leaves untouched, so the result is the Qvm_Cleanup result. The null branch
//                 (0x004137C5-0x004137D4) calls Debug_Error("HumanPlayer_LoadParameters: Couldn't
//                 load script \"%s\"", path) and then jumps to itself at 0x004137D7: the failure
//                 path has no return.
std::int32_t NATIVE_DECL HumanPlayer_LoadParameters()
{
    NATIVE_LOG_ENTER("HumanPlayer_LoadParameters", "0x00410070");
    void* const qvm = igi::retail::script::Qvm_Load("LOCAL:humanplayer/humanplayer.qsc");
    if (qvm == nullptr) {
        Debug_Error("HumanPlayer_LoadParameters: Couldn't load script \"%s\"",
                    "LOCAL:humanplayer/humanplayer.qsc");
        // 0x004137D7: `jmp 0x4137d7`, the retail's own self-loop.
        for (;;) {
        }
    }

    Qvm_Execute(qvm);
    return igi::retail::script::Qvm_Cleanup(qvm);
}

// The four config natives Config_Read calls. They are separate retail natives and are not
// transcribed on this branch, so the unit compiles but these symbols are missing from the link.
// Their bodies are:
//   0x00405300 Config_SaveGraphicOptions   248 bytes,  4 blocks
//   0x004058A0 Config_SaveSoundOptions     209 bytes, 18 blocks
//   0x00405980 Config_NoD3DDriversWithHard 417 bytes, 26 blocks
//   0x00405B30 Config_Language_2           152 bytes,  3 blocks, returns through EAX
// All four are cdecl. Config_Read discards the first three results; Config_Language_2's EAX is
// what Config_Read returns.
void NATIVE_DECL Config_SaveGraphicOptions();
void NATIVE_DECL Config_SaveSoundOptions();
void NATIVE_DECL Config_NoD3DDriversWithHard();
std::int32_t NATIVE_DECL Config_Language_2();

// Native: Config_Read; retail VA 0x00405850.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Tools/evidence: Binary Ninja, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                 VA 0x00405850-0x00405899, 74 bytes, 3 blocks/2 edges;
//                 docs/fidelity/evidence/0x00405850-*.json.
// Behavior/CFG: 0x00405850-0x00405862 reads the path argument, calls Qvm_Load(path) and tests the
//                 result. The non-null
//                 branch (0x00405864-0x00405883) calls Qvm_Execute(qvm), Qvm_Cleanup(qvm), then
//                 natives 0x004058A0, 0x00405980 and 0x00405B30 in that order and returns the last
//                 call's EAX. The null branch (0x00405884-0x00405899) calls natives
//                 0x00405300, 0x004058A0, 0x00405980 and 0x00405B30 in that order and returns the
//                 last call's EAX. Each branch cleans its own 8 bytes of Qvm_Execute/Qvm_Cleanup
//                 arguments at 0x00405870.
std::int32_t NATIVE_DECL Config_Read(const char* path)
{
    NATIVE_LOG_ENTER("Config_Read", "0x00410070");
    void* const qvm = igi::retail::script::Qvm_Load(path);
    if (qvm == nullptr) {
        Config_SaveGraphicOptions();
        Config_SaveSoundOptions();
        Config_NoD3DDriversWithHard();
        return Config_Language_2();
    }

    Qvm_Execute(qvm);
    igi::retail::script::Qvm_Cleanup(qvm);
    Config_SaveSoundOptions();
    Config_NoD3DDriversWithHard();
    return Config_Language_2();
}

} // extern "C"

#undef NATIVE_DECL
