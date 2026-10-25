// Native: Task_FillTargetNode_413730 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00413730.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00413730-0x00413762 (51 bytes, 1 block, 0 edges, 14 instructions: 8b4c2404 / 8b442408 / 50 / 8d5120 / 89500c / 8d5178 / 895010 / 8b89a0000000 / 894814 / c7401c02000000 / c7402003000000 / e8cf470500 / 59 / c3; the next byte 0x00413763 is nop padding).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the raw bytes decode as `mov ecx,[esp+4]` (first argument), `mov eax,[esp+8]` (second argument), `push eax` (the callee's argument, pushed before the stores but holding an unchanged pointer), `lea edx,[ecx+0x20]; mov [eax+0xc],edx`, `lea edx,[ecx+0x78]; mov [eax+0x10],edx`, `mov ecx,[ecx+0xa0]; mov [eax+0x14],ecx`, `mov dword [eax+0x1c],2`, `mov dword [eax+0x20],3`, `call 0x00467F30`, `pop ecx`, `ret`; the rel32 `cf470500` resolves from 0x00413761 to 0x00467F30. 0x0040FE30 links this address as a HumanPlayer handler through 0x00401400 with the event id from Task_GetCachedEventId_467D90 (0x00467D90). The callee 0x00467F30 is untranscribed and lies in the 0x0046xxxx range that another lane owns: 375 bytes / 5 blocks / 123 instructions, and r2 afvd recovers exactly one argument (arg_40h), so the single push at 0x00413738 is the real argument.
// Behavior/CFG: writes five dwords into the second argument - the addresses first_argument+0x20 and first_argument+0x78, the dword at first_argument+0xA0, and the constants 2 and 3 - and then calls 0x00467F30 with the second argument; one block, no branch, no loop; the first argument is only read at those three offsets.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The two-argument slot is real: this handler is linked through 0x00401400, and both phase-0 dispatchers (0x004012CD, 0x004013DD) push two arguments before `call *eax`. The dword offsets 0x0C, 0x10, 0x14, 0x1C and 0x20 are retail's encoded displacements, not a declared C++ structure, and the meaning of the fields and of the constants 2 and 3 is not established by this body. Retail's `ret` returns with EAX still holding 0x00467F30's result because the body never writes EAX after the call, but every dispatch path discards EAX after `call *eax`, and the callee's return type is not established, so both routines are declared void rather than inventing one. The callee is a new link gap in the 0x0046xxxx range and its name here is an address-suffixed placeholder for that lane's owner to replace. Domain: core/, because it is a handler of the 0x00402930 startup registrar 0x0040FE30.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00467F30: untranscribed callee owned by the 0x0046xxxx lane. Only its argument count (one, r2 afvd arg_40h) is established;
// the return type is unknown, so it is declared void.
void Core_Unknown_467F30(void* argument);

void NATIVE_DECL Task_FillTargetNode_413730(std::uint8_t* source, std::uint8_t* target)
{
    *reinterpret_cast<std::uint32_t*>(target + 0x0C) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(source + 0x20)); // 0x00413739/0x0041373C: lea edx,[ecx+0x20]; mov [eax+0xc],edx
    *reinterpret_cast<std::uint32_t*>(target + 0x10) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(source + 0x78)); // 0x0041373F/0x00413742: lea edx,[ecx+0x78]; mov [eax+0x10],edx
    *reinterpret_cast<std::uint32_t*>(target + 0x14) = *reinterpret_cast<std::uint32_t*>(source + 0xA0);                          // 0x00413745/0x0041374B: mov ecx,[ecx+0xa0]; mov [eax+0x14],ecx
    *reinterpret_cast<std::uint32_t*>(target + 0x1C) = 2U;   // 0x0041374E: mov dword [eax+0x1c],2
    *reinterpret_cast<std::uint32_t*>(target + 0x20) = 3U;   // 0x00413755: mov dword [eax+0x20],3
    Core_Unknown_467F30(target);                            // 0x00413738: push eax; 0x0041375C: call 0x00467F30
}

} // extern "C"

#undef NATIVE_DECL
