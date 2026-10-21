// IGI native String_ToUpperChar - 0x004AD73D - transcribed from retail igi.exe.
// Retail: cdecl (ch). Frame 4 bytes (var_4h/var_3h = 2-byte LCMap result buffer). Locale handle g_009361e4 == 0: ASCII a-z minus 0x20.
// Otherwise single-byte chars use the ctype table g_00544070 (or 0x004A7F2A when g_0054427c > 1), lead bytes are sent as 2 bytes to 0x004AD4D3 with flag 0x200.
// UNRESOLVED: callees 0x004A7F2A and 0x004AD4D3 are not transcribed yet; they are declared as retail addresses and called exactly as retail does.
#include <cstdint>
#include <cstring>

extern std::int32_t g_009361e4;   // 0x009361E4 locale handle
extern std::int32_t g_0054427c;   // 0x0054427C
extern std::uint8_t* g_00544070;  // 0x00544070 ctype table (2 bytes per entry)

int __cdecl Crt_IsCType_004A7F2A(int ch, int mask);  // 0x004A7F2A (unresolved)
int __cdecl Crt_LCMapStringA_004AD4D3(std::int32_t locale, std::uint32_t flags, const char* src, int srcLen, char* dst, int dstLen, int codePage, int flag); // 0x004AD4D3 (unresolved)

int __cdecl String_ToUpperChar(int ch)
{
    std::uint8_t out[4];   // var_4h at ebp-4, var_3h at ebp-3
    std::uint8_t in[4];    // arg_8h .. arg_bh, written in place by retail
    std::memcpy(in, &ch, 4);
    int eax;
    int ebx;
    if (g_009361e4 == 0) {
        eax = ch;
        if (eax >= 0x61 && eax <= 0x7a) {
            eax -= 0x20;
        }
        return eax;
    }
    ebx = ch;
    if (ebx < 0x100) {
        if (g_0054427c > 1) {
            eax = Crt_IsCType_004A7F2A(ebx, 2);
        } else {
            eax = g_00544070[ebx * 2] & 2;
        }
        if (eax == 0) {
            return ebx;
        }
    }
    int len;
    {
        std::uint8_t hi = static_cast<std::uint8_t>(ebx >> 8);
        if (g_00544070[hi * 2 + 1] & 0x80) {
            in[2] = 0;
            in[0] = hi;
            in[1] = static_cast<std::uint8_t>(ebx);
            len = 2;
        } else {
            in[1] = 0;
            in[0] = static_cast<std::uint8_t>(ebx);
            len = 1;
        }
    }
    eax = Crt_LCMapStringA_004AD4D3(g_009361e4, 0x200, reinterpret_cast<const char*>(in), len, reinterpret_cast<char*>(out), 3, 0, 1);
    if (eax == 0) {
        return ebx;
    }
    if (eax == 1) {
        return out[0];
    }
    return (static_cast<int>(out[1]) << 8) | out[0];
}
