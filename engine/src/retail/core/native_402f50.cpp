// Native: Core_CheckCdPresent_402F50; retail VA 0x00402F50 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00402F50-0x00402FEB
//   (156 bytes, 7 blocks, 78 instructions; per-block 9 / 41 / 15 / 3 / 3 / 1 / 6 at 0x00402F50, 0x00402F66, 0x00402FA6, 0x00402FD0,
//   0x00402FD7, 0x00402FE4, 0x00402FE6 from af + afbj. Most of block 2's 41 instructions are nop padding at 0x00402F6A-0x00402F6D,
//   0x00402F74/0x00402F75, 0x00402F80-0x00402F97, 0x00402FBC, 0x00402FC0 and 0x00402FC3.)
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image; the import slot 0x0053310C holds KERNEL32 GetDriveTypeA
//   (reloc.KERNEL32.dll_GetDriveTypeA) and is loaded once into EBP at 0x00402F58, then called indirectly at 0x00402F6F. Data read from the image:
//   0x00536408 = ".\\" (bytes 2E 5C 00 00), 0x00536298 = "rb", 0x00536328 = ".\\ymbe.afp", 0x005363D8 = "IGI CD not found. Please insert CD in
//   drive.". The four CRT callees are stdio by their bodies in the CRT cluster: 0x004A5350 pushes 0x40 and forwards to 0x004A5330 (fopen, i.e.
//   MSVC6 fopen -> _fsopen with _SH_DENYNO), 0x004A52A4 walks the FILE buffer fields (fseek), 0x004A528A decrements the buffer count at +0x04
//   (fgetc), 0x004A5234 tears the stream down (fclose).
// Behavior/CFG: builds the 4-byte root string ".\\" on the stack, then loops: GetDriveTypeA(root); fopen(".\\ymbe.afp","rb"); if the stream is
//   null, or if its first character is not '@', the loop counter is incremented and the loop repeats while it is at most 'Z' (26 iterations from
//   'A'); on a successful '@' read the stream is closed and the routine returns. If all 26 iterations fail it reports
//   Debug_Error("IGI CD not found. Please insert CD in drive.") and spins at 0x00402FE4 (`jmp $`). Branch conditions translated: stream == null
//   (`test esi,esi; je 0x00402FD0` at 0x00402FA2/0x00402FA4), first character == '@' (`cmp eax,0x40; sete dl` at 0x00402FBB/0x00402FBE, tested
//   by `test edi,edi; jne 0x00402FE6` at 0x00402FCC/0x00402FCE), and the loop bound (`cmp bl,0x5A; jle 0x00402F66` at 0x00402FD2/0x00402FD5).
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Two retail facts are reproduced rather than tidied, and both are visible in the bytes:
//   (a) the loop counter in BL is never stored into the root string, and the string is written once before the loop (0x00402F60), so all 26
//   iterations probe the same relative root ".\\" and open the same relative path - there is no per-drive path construction anywhere in the
//   extent; (b) the GetDriveTypeA result is compared with 5 (DRIVE_CDROM) at 0x00402F71 and then discarded, because the following pushes clobber
//   the flags before any conditional use, so no branch depends on it. Neither is "fixed" here. The IAT load into EBP is a register-caching detail;
//   the call is written as a direct GetDriveTypeA call. CRT stdio is imported (std::fopen/fseek/fgetc/fclose), not re-implemented. Domain: core/,
//   because 0x00402930 calls it as part of startup; it is a drive/filesystem probe, so sys/ or qfile/ would be the alternative.
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Callee 0x004AF7B0 (Debug_Error), declared exactly as core/native_401900.cpp declares it.
std::int32_t NATIVE_DECL Debug_Error(const char* format, ...);

void NATIVE_DECL Core_CheckCdPresent_402F50(void)
{
    char root[4] = ".\\";                                 // 0x00402F51/0x00402F60: mov eax,[0x00536408] (".\\"); mov [esp+0x10],eax
    std::uint8_t drive = 'A';                             // 0x00402F64: mov bl,0x41
    do
    {
        std::uint32_t const drive_type =
            static_cast<std::uint32_t>(::GetDriveTypeA(root)); // 0x00402F66/0x00402F6E/0x00402F6F: lea ecx,[esp+0x10]; push ecx; call ebp
        (void)drive_type;                                 // 0x00402F71: cmp eax,5 - dead in retail (flags clobbered by the pushes below), see Status/limits
        std::FILE* stream = std::fopen(".\\ymbe.afp", "rb"); // 0x00402F76/0x00402F7B/0x00402F98: push "rb"; push ".\\ymbe.afp"; call 0x4A5350
        if (stream != nullptr)                            // 0x00402F9F/0x00402FA2/0x00402FA4: add esp,8; test esi,esi; je 0x00402FD0
        {
            std::fseek(stream, 0, 0);                     // 0x00402FA6/0x00402FA8/0x00402FAD/0x00402FAE: push 0; push 0; push esi; call 0x4A52A4
            std::uint32_t const first =
                static_cast<std::uint32_t>(std::fgetc(stream)); // 0x00402FB3/0x00402FB4: push esi; call 0x4A528A
            std::uint32_t const found = (first == 0x40U) ? 1U : 0U; // 0x00402FB9/0x00402FBB/0x00402FBE: xor edx,edx; cmp eax,0x40; sete dl
            std::fclose(stream);                          // 0x00402FC1/0x00402FC4: push esi; call 0x4A5234
            if (found != 0U)                              // 0x00402FC9/0x00402FCC/0x00402FCE: add esp,0x14; test edi,edi; jne 0x00402FE6
            {
                return;                                   // 0x00402FE6-0x00402FEB: pop edi/esi/ebp/ebx/ecx; ret
            }
        }
        ++drive;                                          // 0x00402FD0: inc bl
    } while (drive <= 'Z');                               // 0x00402FD2/0x00402FD5: cmp bl,0x5A; jle 0x00402F66
    Debug_Error("IGI CD not found. Please insert CD in drive."); // 0x00402FD7/0x00402FDC: push 0x005363D8; call 0x4AF7B0
    for (;;)                                              // 0x00402FE4: jmp 0x00402FE4 (retail spins here)
    {
    }
}

} // extern "C"

#undef NATIVE_DECL
