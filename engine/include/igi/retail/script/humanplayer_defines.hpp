#pragma once

// The retail's human player and configuration script natives.
//
// The human player's movement values come from humanplayer.qvm's
// DefineHumanPlayerGeneral, which scales the script's real arguments with the
// retail's constants, and the human player's parameters come from
// LOCAL:humanplayer/humanplayer.qsc, which the retail loads, executes and
// releases. The configuration file is read the same way.

#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::script {

// The nine dwords DefineHumanPlayerGeneral writes, 0x0056E1E8..0x0056E208.
// Sources: 0x00410082 (arg 0), 0x0041009F (arg 1), 0x004100B9 (arg 2),
// 0x004100D3 (arg 3), 0x004100E4 (arg 4), 0x004100F8 (arg 5),
// 0x00410117 (arg 6, the fistp integer), 0x00410129 (arg 7) and
// 0x00410137 (arg 8). The block is a plain 0x24-byte array; the retail never
// stores a different type in it.
struct HumanPlayerGeneralParameters
{
    float argument_0;         // +0x00, 0x0056E1E8, the first script argument unscaled
    float argument_1;         // +0x04, 0x0056E1EC
    float argument_2;         // +0x08, 0x0056E1F0
    float argument_3;         // +0x0C, 0x0056E1F4
    float argument_4;         // +0x10, 0x0056E1F8
    float argument_5;         // +0x14, 0x0056E1FC
    std::int32_t argument_6;  // +0x18, 0x0056E200, 30.0-scaled and truncated with fistp
    float argument_7;         // +0x1C, 0x0056E204, the eighth script argument unscaled
    float argument_8;         // +0x20, 0x0056E208, the ninth script argument unscaled
};

static_assert(sizeof(HumanPlayerGeneralParameters) == 0x24U,
              "the retail human player general parameters are nine dwords");
static_assert(offsetof(HumanPlayerGeneralParameters, argument_6) == 0x18U,
              "the retail fistp integer slot is +0x18");
static_assert(offsetof(HumanPlayerGeneralParameters, argument_8) == 0x20U,
              "the retail ninth slot is +0x20");

// The record of the two 17-entry parameter tables DefineHumanPlayerGeneral
// fills: 0x005389F8 (script arguments 9..25, 0x00410140-0x0041015D) and
// 0x00538B08..0x00538C18 (script arguments 26..42, 0x0041015F-0x0041017C).
// The retail walks both with a 0x10-byte stride and stores one float in the
// first dword of each record; the rest of the record is not written here.
struct HumanPlayerScriptParameter
{
    float value;                // +0x00, the only dword this native writes
    std::uint32_t unwritten[3]; // +0x04..+0x0C, not written by this native
};

inline constexpr std::size_t human_player_script_parameter_count = 17U;

static_assert(sizeof(HumanPlayerScriptParameter) == 0x10U,
              "the retail parameter record stride is 0x10");

// The retail's scale constants, read as x87 operands by 0x00410090-0x0041010F:
//   0x005334A4  float  4096000.0  (0x4A7A0000), arguments 1..3
//   0x005334A0  float  1/108000   (0x371B5837, 9.259259059035685e-06), arguments 1..3
//   0x00533498  double 4096.0     (0x40B0000000000000), arguments 4 and 5
//   0x00533488  double 30.0       (0x403E000000000000), argument 6
// The float literal is written in hexadecimal so the constant is bit-exact;
// 0x00533490 holds 60.0 but this native never reads it.
inline constexpr float human_player_scale_4096000 = 4096000.0F;
inline constexpr float human_player_scale_inverse_108000 = 0x1.36B06Ep-17F;
inline constexpr double human_player_scale_4096 = 4096.0;
inline constexpr double human_player_scale_30 = 30.0;

} // namespace igi::retail::script

extern "C" {

// The retail's human player general parameters at 0x0056E1E8.
extern igi::retail::script::HumanPlayerGeneralParameters HumanPlayer_GeneralParameters;

// The two retail parameter tables, 0x005389F8 and 0x00538B08. The port owns
// relocated storage for them because the retail addresses are process image
// addresses; the layout and the values written are the retail's.
extern igi::retail::script::HumanPlayerScriptParameter
    HumanPlayer_GeneralParameterTableA[igi::retail::script::human_player_script_parameter_count];
extern igi::retail::script::HumanPlayerScriptParameter
    HumanPlayer_GeneralParameterTableB[igi::retail::script::human_player_script_parameter_count];

// Native: DefineHumanPlayerGeneral() - VA 0x00410070 - complete transcription.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                VA 0x00410070-0x004101B5, 326 bytes, 5 blocks/6 edges;
//                docs/fidelity/evidence/0x00410070-*.json.
// Signature: x86 cdecl void*(void* context, int32_t argument_slot, void* frame).
//            The body reads the frame from [esp+0x34] at 0x00410075 (= the third stack argument)
//            and the result pointer from [esp+0x30] at 0x0041017E (= the first stack argument);
//            the second stack argument is never read. The value it returns in EAX is that same
//            result pointer (0x0041017E-0x004101B5), which is what the script VM's native-call
//            site 0x004BD8FE consumes with `rep movsd` from EAX.
// Behavior: fetches script arguments 0..8 through the interpreter's real-argument reader
//           0x004B8A50 and stores them at 0x0056E1E8..0x0056E208 with the retail scaling:
//           0 raw; 1..3 * 4096000.0f * (1/108000); 4,5 * 4096.0; 6 * 30.0 truncated by fistp;
//           7 and 8 raw. It then fills the two 17-entry parameter tables (arguments 9..25 and
//           26..42) with unscaled values and copies the 0x18-byte script value block
//           {int32 1, undefined +0x04, float64 1.0, type descriptor 0x00567C74} into the
//           caller's context, returning that context pointer.
// Status/limits: STRUCTURALLY_VERIFIED; the block layout, the argument indices, the scales and
//           the x87 order are the retail's. The port leaves the block's undefined +0x04 dword
//           zero where retail copies stack residue. No runtime capture exists.
void* NATIVE_DECL DefineHumanPlayerGeneral(void* context, std::int32_t argument_slot,
                                               void* frame);

// Native: HumanPlayer_LoadParameters() - VA 0x004137A0 - complete transcription.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Tools/evidence: Binary Ninja, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                VA 0x004137A0-0x004137D4, 57 bytes, 4 blocks/4 edges;
//                docs/fidelity/evidence/0x004137A0-*.json.
// Signature: x86 cdecl int32_t(void). EAX is the result of the final Qvm_Cleanup call
//            (0x004137C0-0x004137C4 leaves it untouched); the only retail caller (0x004020D3)
//            discards it.
// Behavior: Qvm_Load("LOCAL:humanplayer/humanplayer.qsc") (0x00539188); when it returns
//           non-null, Qvm_Execute(qvm) then Qvm_Cleanup(qvm) and return the cleanup result.
//           When it returns null, Debug_Error("HumanPlayer_LoadParameters: Couldn't load script
//           \"%s\"", path) (0x00539150) and then an unconditional self-loop at 0x004137D7, so
//           the failure path never returns.
// Status/limits: STRUCTURALLY_VERIFIED; the path, the call order, the error format and the halt
//           loop are the retail's. No runtime capture exists.
std::int32_t NATIVE_DECL HumanPlayer_LoadParameters();

// Native: Config_Read() - VA 0x00405850 - complete transcription.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Tools/evidence: Binary Ninja, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                VA 0x00405850-0x00405899, 74 bytes, 3 blocks/2 edges;
//                docs/fidelity/evidence/0x00405850-*.json.
// Signature: x86 cdecl int32_t(const char* path). The path is passed straight to Qvm_Load
//            (0x00405855-0x00405856); the return value is the EAX of the final Config_Language_2
//            call (0x0040587D / 0x00405893), which the only retail caller (0x004020C9) discards.
// Behavior: Qvm_Load(path). On success: Qvm_Execute(qvm), Qvm_Cleanup(qvm), then natives
//           0x004058A0, 0x00405980 and 0x00405B30 in that order. On failure: natives
//           0x00405300, 0x004058A0, 0x00405980 and 0x00405B30 in that order. The branches do
//           not share a tail: the failed-load path calls the graphic-options save first.
// Status/limits: STRUCTURALLY_VERIFIED; the branch order and the call set are the retail's. The
//           four callees are declared below and are not transcribed yet, so this unit compiles
//           but does not link. No runtime capture exists.
std::int32_t NATIVE_DECL Config_Read(const char* path);

} // extern "C"

#undef NATIVE_DECL
