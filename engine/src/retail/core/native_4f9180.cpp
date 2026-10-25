// Native: Graph_Update_4F9180; retail VA 0x004F9180 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F9180-0x004F91F2; radare2 6.2.0 pdfj (115 bytes, 7 blocks, 44 instructions, 6 edges); called only from 0x00509432 (0x005093C0 handler) with two arguments, of which the second is never read.
// Behavior/CFG: reads the type word at [[object+0x14]+0x1C] and the global word from 0x004C48C0, asks 0x00401CF0 whether the type inherits that global, and compares the answer with the byte state flag at object+0x64. When the two disagree it converts the object's 24-byte orientation block through the pair 0x004F92E0/0x004F93B0 and writes the new flag value, then stores the 0x004F1050 list index at object+0x50.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 44-instruction listing; runtime UNOBSERVED. Link-closed: 0x004C48C0, 0x00401CF0, 0x004F1050 and the 0x004F92E0/0x004F93B0 pair are all transcribed. The second argument is unread in retail (no stack read past the first argument) and is kept only because the call site passes it. The 24-byte scratch is zero-initialised to keep indeterminate reads out of the port; 0x004F92E0 overwrites all 24 bytes first, so nothing observable depends on it.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004C48C0: the 16-bit global the type check compares against, transcribed in engine/src/retail/sys/native_4c48c0.cpp.
std::uint16_t NATIVE_DECL Sys_GetGlobal16_4C48C0(void);

// 0x00401CF0: the task-type inheritance walk, transcribed in native_401cf0.cpp; reads only the low word of each argument.
std::uint8_t NATIVE_DECL TaskType_Inherits_401CF0(std::uint32_t type, std::uint16_t ancestor);

// 0x004F92E0: copies object+0x20 into the scratch and, when object+0x64 is set, applies M*scratch + T, transcribed in native_4f92e0.cpp.
void NATIVE_DECL Graph_ApplyParentTransform_4F92E0(void* out, void* object);

// 0x004F93B0: writes object+0x20 from the scratch and, when object+0x64 is set, stores M^T*(scratch - T), transcribed in native_4f93b0.cpp.
void NATIVE_DECL Graph_UnapplyParentTransform_4F93B0(void* object, void* in);

// 0x004F1050: first index in the 0x00A71890 table whose entry points at the argument, transcribed in native_4f1050.cpp.
std::int32_t NATIVE_DECL List_FindIndex_4F1050(void* object);

void NATIVE_DECL Graph_Update_4F9180(void* object, void* arg2)
{
    (void)arg2;                                                                        // retail never reads its second argument

    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);
    std::uint8_t* const transform = *reinterpret_cast<std::uint8_t**>(fields + 0x14U);  // 0x004F9189: mov 0x14(%esi),%eax
    std::uint16_t const type_word =
        *reinterpret_cast<std::uint16_t*>(transform + 0x1CU);                           // 0x004F918C: mov 0x1c(%eax),%di
    std::uint16_t const global = Sys_GetGlobal16_4C48C0();                              // 0x004F9190: call 0x4c48c0
    std::uint8_t const inherits =
        TaskType_Inherits_401CF0(static_cast<std::uint32_t>(type_word), global);        // 0x004F9195..0x004F9197: push %eax; push %edi; call 0x401cf0

    // 0x004F91AA/0x004F91C4: the 24-byte frame at [esp+8] that both transform calls share.
    struct
    {
        double x;
        double y;
        double z;
    } scratch = {};

    if (inherits != 0U)                                                                 // 0x004F919C/0x004F919F: add $0x8,%esp; test %al,%al
    {
        if (*reinterpret_cast<std::uint8_t*>(fields + 0x64U) == 0U)                     // 0x004F91A1/0x004F91A6/0x004F91A8: mov 0x64(%esi),%al; test %al,%al; jne 0x4f91e1
        {
            Graph_ApplyParentTransform_4F92E0(&scratch, object);                        // 0x004F91AA..0x004F91B0: lea [esp+8]; push %esi; push %ecx; call 0x4f92e0
            *reinterpret_cast<std::uint8_t*>(fields + 0x64U) = 1U;                      // 0x004F91B9: movb $0x1,0x64(%esi)
            Graph_UnapplyParentTransform_4F93B0(object, &scratch);                      // 0x004F91BD/0x004F91D7..0x004F91D9: lea [esp+0x10]; push %edx; push %esi; call 0x4f93b0
        }
    }
    else if (*reinterpret_cast<std::uint8_t*>(fields + 0x64U) != 0U)                    // 0x004F91C0/0x004F91C2: test %al,%al; je 0x4f91e1
    {
        Graph_ApplyParentTransform_4F92E0(&scratch, object);                            // 0x004F91C4..0x004F91CA
        *reinterpret_cast<std::uint8_t*>(fields + 0x64U) = 0U;                          // 0x004F91D3: movb $0x0,0x64(%esi)
        Graph_UnapplyParentTransform_4F93B0(object, &scratch);                          // 0x004F91D7..0x004F91D9
    }

    *reinterpret_cast<std::uint32_t*>(fields + 0x50U) =                                 // 0x004F91EA: mov %eax,0x50(%esi)
        static_cast<std::uint32_t>(List_FindIndex_4F1050(object));                       // 0x004F91E1..0x004F91E7: push %esi; call 0x4f1050; add $0x4,%esp
}                                                                                       // 0x004F91ED..0x004F91F2: pop %edi; pop %esi; add $0x18,%esp; ret
} // extern "C"

#undef NATIVE_DECL
