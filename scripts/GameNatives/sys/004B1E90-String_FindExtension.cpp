// IGI native String_FindExtension - 0x004B1E90 - transcribed from retail igi.exe.
// Retail: cdecl (out, size, path). Scans backwards from the terminating zero for a dot, copies size-1 bytes of the extension to out when out is set and returns a pointer to it in path.
#include <cstdint>
#include <cstring>

char* __cdecl Camera_ApplyLineFilter_5(char* dst, const char* src, int count); // 0x004A5500

char* __cdecl String_FindExtension(char* out, int size, const char* path)
{
    int index = static_cast<int>(std::strlen(path));
    if (index != 0)
    {
        do
        {
            if (path[index] == '.')
                break;
            index--;
        } while (index != 0);
    }
    const char* extension = path + index;
    if (out == nullptr)
        return const_cast<char*>(extension);
    Camera_ApplyLineFilter_5(out, extension, size - 1);
    out[size - 1] = 0;
    return const_cast<char*>(extension);
}
