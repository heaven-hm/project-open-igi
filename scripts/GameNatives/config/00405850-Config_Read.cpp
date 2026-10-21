// IGI native Config_Read - 0x00405850 - transcribed from retail igi.exe.
// Retail: cdecl, one argument. Loads the config qvm, executes and cleans it up; when loading fails runs 0x405300 first. Both paths then call 0x4058A0, 0x405980 and 0x405B30.
#include <cstdint>
#include <cstring>

void* __cdecl Qvm_Load(void* path);        // 0x004B80B0
void __cdecl Qvm_Execute(void* qvm);       // 0x004B85B0
void __cdecl Qvm_Cleanup(void* qvm);       // 0x004B83D0
void __cdecl Config_SaveGraphicOptions(void); // 0x00405300
void __cdecl Config_SaveSoundOptions(void);   // 0x004058A0
void __cdecl Config_NoD3DDriversWithHard(void); // 0x00405980
void __cdecl Config_Language_2(void);         // 0x00405B30

void __cdecl Config_Read(void* path)
{
    void* qvm = Qvm_Load(path);
    if (qvm != nullptr)
    {
        Qvm_Execute(qvm);
        Qvm_Cleanup(qvm);
        Config_SaveSoundOptions();
        Config_NoD3DDriversWithHard();
        Config_Language_2();
        return;
    }
    Config_SaveGraphicOptions();
    Config_SaveSoundOptions();
    Config_NoD3DDriversWithHard();
    Config_Language_2();
}
