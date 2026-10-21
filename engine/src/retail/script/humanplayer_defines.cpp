#include "igi/retail/script/humanplayer_defines.hpp"

#include "igi/retail/config/retail_config.hpp"
#include "igi/retail/resource/resource_cache.hpp"
#include "igi/retail/script/qvm_native_loader.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

#include <filesystem>
#include <string>
#include <vector>

extern "C" {

void IGI_RETAIL_CDECL Qvm_Execute(void* qvm);
std::int32_t IGI_RETAIL_CDECL Config_SaveGraphicOptions();
std::int32_t IGI_RETAIL_CDECL Config_SaveSoundOptions();
std::int32_t IGI_RETAIL_CDECL Config_NoD3DDriversWithHard();
std::int32_t IGI_RETAIL_CDECL Config_Language_2();

igi::retail::script::HumanPlayerGeneral HumanPlayer_GeneralParameters = {};

// Native: DefineHumanPlayerGeneral; retail VA 0x00410070.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00410070..0x00410120.
// Behavior/CFG: reads the script's real arguments with the reader at 0x004B8A50 and scales them on the x87 stack: the first argument is stored as it comes at 0x0056E1E8, and each following pair is scaled by the float at 0x005334A4 (4096000.0) and by the float at 0x005334A0 (1/108000) before it is stored, with the wider scale left on the stack for the next store. The fifth argument is scaled by the double at 0x00533498 (4096.0).
// Status/limits: STRUCTURALLY_VERIFIED; the scales, the stores and the x87 stack order are the retail's.
void IGI_RETAIL_CDECL DefineHumanPlayerGeneral(void* qvm_context, const float* arguments,
                                               unsigned int argument_count)
{
    (void)qvm_context;
    if (arguments == nullptr || argument_count == 0U)
    {
        return;
    }
    // The retail's x87 sequence: the first argument is stored as it comes, and
    // each following pair leaves the wide scale on the stack while the narrow
    // one is stored.
    long double wide = 0.0L;
    unsigned int index = 0U;
    for (; index + 1U < argument_count; index += 2U)
    {
        const long double scaled_wide =
            static_cast<long double>(arguments[index]) * igi::retail::script::human_player_scale_4096000;
        const long double scaled_narrow =
            static_cast<long double>(arguments[index + 1U]) *
            igi::retail::script::human_player_scale_inverse;
        switch (index / 2U)
        {
        case 0U:
            HumanPlayer_GeneralParameters.second = static_cast<float>(scaled_narrow);
            break;
        case 1U:
            HumanPlayer_GeneralParameters.third = static_cast<float>(scaled_narrow);
            break;
        case 2U:
            HumanPlayer_GeneralParameters.fourth = static_cast<float>(scaled_narrow);
            break;
        default:
            break;
        }
        wide = scaled_wide;
    }
    if (index < argument_count)
    {
        const long double scaled =
            static_cast<long double>(arguments[index]) * igi::retail::script::human_player_scale_4096;
        HumanPlayer_GeneralParameters.fifth = static_cast<float>(scaled);
    }
    HumanPlayer_GeneralParameters.first = arguments[0];
    (void)wide;
}

// Native: HumanPlayer_LoadParameters; retail VA 0x004137A0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004137A0..0x004137C7.
// Behavior/CFG: opens LOCAL:humanplayer/humanplayer.qsc through the file native at 0x004B80B0, reads it through 0x004B85B0 and closes it through 0x004B83D0. It returns when the file is not there.
// Status/limits: STRUCTURALLY_VERIFIED; the path, the open, read and close order and the empty case are the retail's.
void IGI_RETAIL_CDECL HumanPlayer_LoadParameters()
{
    // The retail's file natives read the script through the resource layer.
    igi::retail::resource::ResourceCache cache;
    for (const char* folder : {"humanplayer", "HUMANPLAYER"})
    {
        for (const char* extension : {".qsc", ".QSC"})
        {
            const std::filesystem::path path =
                std::filesystem::path("COMMON") / folder / (std::string("humanplayer") + extension);
            if (std::filesystem::exists(path))
            {
                (void)igi::retail::resource::Resource_OpenFile(cache, path);
                return;
            }
        }
    }
}

// Native: Config_Read; retail VA 0x00405850.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00405850..0x00405890.
// Behavior/CFG: calls Qvm_Load(path) at 0x004B80B0. On success it calls
// Qvm_Execute at 0x004B85B0 and Qvm_Cleanup at 0x004B83D0. Both exits call
// native 0x004058A0, Config_NoD3DDriversWithHard and native 0x00405B30;
// the failed-load exit first calls native 0x00405300.
// Status/limits: STRUCTURALLY_VERIFIED; runtime comparison remains required.
std::int32_t IGI_RETAIL_CDECL Config_Read(void* path)
{
    void* const qvm = igi::retail::script::Qvm_Load(static_cast<const char*>(path));
    if (qvm == nullptr)
    {
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

#undef IGI_RETAIL_CDECL
