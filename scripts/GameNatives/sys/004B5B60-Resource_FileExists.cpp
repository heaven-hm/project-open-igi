// IGI native Resource_FileExists - 0x004B5B60 - transcribed from retail igi.exe.
// Retail: cdecl, one argument (path). Returns 1 when 0x4B5B90 finds the resource loaded, otherwise the result of 0x4B1FC0.
#include <cstdint>
#include <cstring>

int __cdecl Is_ResourceLoaded(const char* path, void** out); // 0x004B5B90
int __cdecl Sys_DispatchOpcode_7(const char* path);          // 0x004B1FC0

int __cdecl Resource_FileExists(const char* path)
{
    if (Is_ResourceLoaded(path, nullptr) != 0)
        return 1;
    return Sys_DispatchOpcode_7(path);
}
