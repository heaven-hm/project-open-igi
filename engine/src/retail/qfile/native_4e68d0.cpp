// Native: QFile_DispatchDirectory_4E68D0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004E68D0-0x004E68E4 (21 bytes, 1 block, 0 edges, 7 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) bytes 8b442404 6a00 68f0684e00 50 e83fabfcff 83c40c c3; radare2 6.2.0 af/afij (21 B, 1 block, 7 instructions, 0 edges); the three call sites are 0x004150D7 ("LOCAL:screens/intro/", from 0x00415070), 0x00415D2C ("LOCAL:common/sounds") and 0x00415D36 ("MISSION:sounds"), each pushing one directory path.
// Behavior/CFG: one basic block, no branch: `mov 0x4(%esp),%eax; push $0x0; push $0x004E68F0; push %eax; call 0x004B1420; add $0xc,%esp; ret`. It forwards its single argument to the transcribed dispatcher 0x004B1420 together with the fixed callback 0x004E68F0 and a zero context, and returns that call's result.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 7-instruction listing and afij (21 B / 1 block / 0 edges); runtime UNOBSERVED. The callback 0x004E68F0 is a raw code immediate in retail and is not transcribed yet (205 bytes / 20 blocks / 86 instructions / 29 edges), so this file links without it but cannot be exercised until it lands. The name is behavior-derived from the three call sites, all of which pass a directory path; it is not a retail symbol.
#include "dev_log.hpp"
#include <cstdint>

// The guarded calling-convention macro is stated here rather than relied on from an include (task_types.hpp #undefs it at its end).
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" std::int32_t QFile_DispatchDevicePath_4B1420(const char* path, void* callback, std::int32_t context); // 0x004B1420 (qfile/native_4b1420.cpp)

extern "C" {

std::int32_t NATIVE_DECL QFile_DispatchDirectory_4E68D0(const char* path)
{
    NATIVE_LOG_ENTER("QFile_DispatchDirectory_4E68D0", "");
    return QFile_DispatchDevicePath_4B1420(
        path,                                                                    // 0x004E68DB: push %eax (the argument read at 0x004E68D0)
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004E68F0U)),      // 0x004E68D6: push $0x004E68F0 (the callback)
        0);                                                                      // 0x004E68D4: push $0x0 (the context)
}

} // extern "C"
