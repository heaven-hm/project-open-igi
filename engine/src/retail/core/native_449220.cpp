// Native: Task_ResolveTwoFloats_449220 (behavior-derived placeholder: it resolves one symbol then reads two float arguments into the object at +0x20/+0x24; retail symbol unknown); retail VA 0x00449220-0x0044925F.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 64 bytes (0x40), 1 block, 0 edges,
//   30 instructions, counted over the exact image. The body ends at the `ret` at 0x0044925F. 0x00449260 is a 0x90 NOP:
//   alignment padding before Task_ClearFlag_449260, not part of this routine.
// Tools/evidence: build/retail-dis.txt, the complete extent; radare2 `pd 30 @ 0x00449220` agrees byte for byte.
//   `push %ebx` 0x00449220, `push %esi` 0x00449221, `push %edi` 0x00449226 are the only frame; no `sub esp`,
//   no local storage. One argument, read at 0x00449222 as `[esp+0x10]` while two pushes are outstanding, i.e. retail
//   argument 1 after the return address. ABI void(void* argument).
// Behavior/CFG: a QVM native. Let `esi` be the argument, `object = esi[0]`, `index = esi[8]`, `frame = esi[4]`.
//   (1) `push $0x20` then `Sys_ResolveSymbol_23_4B8A80(frame, index, object, 0x20)` - the callee's cdecl order is
//       frame, index, object, 0x20 because retail pushes object, index, frame in that order after the 0x20.
//   (2) `inc %edi` at 0x00449236, so the next index is `index+1`, and `Qvm_GetFloatArg(frame, index+1)`; the x87
//       result is stored by `fstp dword [ebx+0x20]` at 0x00449246 - a FLOAT store, so the value is rounded to
//       float precision at that point, matching Qvm_GetFloatArg's float return.
//   (3) `lea 0x1(%edi),%eax` at 0x0044924C gives `index+2`, and `Qvm_GetFloatArg(frame, index+2)` stores through
//       `fstp dword [ebx+0x24]` at 0x00449256.
//   NOTE FOR THE READER: `frame` is re-loaded from memory before each call - `mov %ecx,[esi+4]` at 0x0044923C and
//   `mov %edx,[esi+4]` at 0x00449249 - rather than being kept in a register across the first call. The port
//   re-reads it for the same reason retail does: the resolve call may have written through the argument.
//   The single epilogue `add $0x20,%esp` at 0x00449259 releases 8 argument dwords = 4 (resolve) + 2 + 2 (the two
//   float reads), confirming all three callees are cdecl.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED for the CFG (1 block, 0 edges - there is no branch structure
//   to disagree about); BUILD_VERIFIED; runtime UNOBSERVED. The register-level `esi[4]` re-read is transcribed
//   because retail re-reads it, not because the port needs it. No helper, no invented constant; the only
//   immediates are retail's 0x20 and the object field offsets 0x20/0x24.

#define NATIVE_DECL __cdecl

#include <cstdint>

// 0x004B8A80 and 0x004B8A50 are transcribed at script/native_4b8a80.cpp and script/native_4b8a50.cpp. Declared
// here with the signatures the call sites establish rather than included, because neither has a public header and
// native_42db00.cpp declares Sys_ResolveSymbol_23_4B8A80 the same way.
extern "C" char* NATIVE_DECL Sys_ResolveSymbol_23_4B8A80(void* argument_frame, std::int32_t index, void* destination, std::int32_t count);
extern "C" float NATIVE_DECL Qvm_GetFloatArg(void* argument_frame, std::int32_t index);

extern "C" void NATIVE_DECL Task_ResolveTwoFloats_449220(void* argument)
{
    // 0x00449229: `mov %edi,[esi+8]` - the symbol index, before the first call.
    std::int32_t index = *reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(argument) + 8);
    // 0x0044922C: `mov %ebx,[esi]` - the object that receives the two float stores.
    void* const object = *reinterpret_cast<void**>(argument);
    // 0x00449232: `mov %eax,[esi+4]` - the QVM frame, loaded before the first call.
    void* frame = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(argument) + 4);

    // 0x00449227: `push $0x20`; 0x00449230/31/35: `push %ebx; push %eax; push %eax` in object, index, frame order.
    Sys_ResolveSymbol_23_4B8A80(frame, index, object, 0x20);

    // 0x00449236: `inc %edi` - the index advances before the first float read.
    ++index;
    // 0x0044923C: `mov %ecx,[esi+4]` - frame re-read from memory, as retail does.
    frame = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(argument) + 4);
    // 0x00449246: `fstp dword [ebx+0x20]` - float store, so the x87 result is rounded to float here.
    *reinterpret_cast<float*>(static_cast<std::uint8_t*>(object) + 0x20) = Qvm_GetFloatArg(frame, index);

    // 0x00449249: `mov %edx,[esi+4]` - frame re-read again; 0x0044924C: `lea %eax,[edi+1]`.
    frame = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(argument) + 4);
    // 0x00449256: `fstp dword [ebx+0x24]`.
    *reinterpret_cast<float*>(static_cast<std::uint8_t*>(object) + 0x24) = Qvm_GetFloatArg(frame, index + 1);

    // 0x00449259: `add $0x20,%esp` releases all 8 argument dwords; 0x0044925C-5E pop edi/esi/ebx; 0x0044925F ret.
}
