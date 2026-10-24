// Native: QFile_DeviceCallback_A0_50A5A0; retail VA 0x0050A5A0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0050A5A0-0x0050A5AA (11 bytes, 1 block, 0 edges, 3 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and the raw bytes; section 22.2 of the evidence sheet (one of the code immediates 0x00509F50 installs into a driver record).
// Behavior/CFG: returns the 32-bit word at context+0x160; the first argument (the driver index) is never read. No branch, no call, no store.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The second-slot context convention is inferred from the 0x004B1690-0x004B17F0 gate family, which push (index, context, ...); the field meaning at +0x160 is unproven.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL QFile_DeviceCallback_A0_50A5A0(std::int32_t index, std::int32_t* context)
{
    (void)index;                                                       // 0x0050A5A0: only [esp+0x8] is read, never [esp+0x4]
    return *reinterpret_cast<const std::int32_t*>(                      // 0x0050A5A4: mov eax,[eax+0x160]
        reinterpret_cast<const std::uint8_t*>(context) + 0x160U);
}

} // extern "C"

#undef NATIVE_DECL
