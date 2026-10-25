// Native: Core_Unknown_467F30 (placeholder name kept from the declaring file core/native_413730.cpp; this VA has no manifest record); retail VA 0x00467F30-0x004680A7.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode of the exact extent from entry 0x00467F30: 123 instructions, 375 bytes, 1 `ret` at 0x004680A6, 2 conditional branches (`je 0x004680A1` at 0x00467F3F, `je 0x00467F61` at 0x00467F57), no other jump, no unread middle. ENTRY BOUNDARY: an image-wide arithmetic scan of every `E8` displacement finds exactly 4 call sites - 0x0041375C, 0x00443E4F (preceded by `56` = push esi), 0x004603E4 (preceded by `56`), 0x00467D22 - so retail calls it with one argument, matching the `void*(void)` declaration. The extent is bounded by the `ret` at 0x004680A6 and `add esp,0x34` at 0x004680A3 which matches the `sub esp,0x34` prologue.
// STACK FRAME IS TRANSCRIBED AS BYTES, NOT AS NAMED LOCALS: 0x00467F30 is `sub esp,0x34`, and the body passes addresses of its own stack slots to three of its four callees (0x00467FEC `lea ecx,[esp+0x38]`, 0x00468007 `lea edx,[esp+0x30]`, 0x00468028 `lea ecx,[esp+0x30]`, 0x00468033 `lea edx,[esp+0x24]`, 0x00468037 `lea eax,[esp+0x28]`, 0x0046803F `lea ecx,[esp+0x30]`). Those callees may write more bytes through those pointers than the slot's own width, so the RELATIVE LAYOUT IS LOAD-BEARING and named locals would let the compiler choose a different one. The `frame` array below is the retail frame byte for byte: frame[i] is E-0x34+i, where E is the return address. Slot offsets are stated at each use.
// Behavior/CFG: two exits. If the signed dword at object+0x1C equals -1 the body returns immediately (0x00467F39-0x00467F3F). Otherwise it clamps the float at object+0x14 up to 3072.0f (0x00467F45-0x00467F59; the compared constant at 0x005334F0 is the bytes 00 00 40 45 = 3072.0f exactly, so the clamp is `value < 3072.0f ? 3072.0f : value`), then computes the three component differences between the 3 double-precision vectors at *(object+0x0C) and *(object) (offsets 0, 8, 0x10 - `fld qword` / `fsub qword`), multiplies that difference by the 3x4 float matrix at *(object+4) as a row vector (offsets 0,4,8 / 0x0C,0x10,0x14 / 0x18,0x1C,0x20; the three `fmul`/`faddp` chains at 0x00467F87-0x00467FF4), writes the three results back into the frame reusing the same slots the difference occupied (0x00467FDF, 0x00467FE3, 0x00467FF4), and then makes four calls. The last call takes fourteen arguments, including the pair (*(dword*)(0x0053F5F0+index*8), *(dword*)(0x0053F5F4+index*8)) - a pointer and a count from an 8-byte-stride table indexed by object+0x1C - and the three floats at *(float*)(0x0053F608 + (*(object+0x20)*3)*4 + 0,4,8), a 3-float entry indexed by 3*object+0x20. All four calls are cdecl and the body cleans them in two groups: `add esp,0x28` (10 dwords = 3+3+4) at 0x0046804B and `add esp,0x38` (14 dwords) at 0x0046809D. No return value is produced.
// Status/limits: TRANSCRIBED; NOT LINKABLE - its four callees (0x004675B0, 0x00498040, 0x004B3C50, 0x004B5860) have no manifest record and no file, and each is declared below as a measured-arity gap rather than guessed at. STRUCTURALLY_VERIFIED against the 123-instruction listing (2 branches, 1 ret, 4 call sites, one loop-free body); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. LIMITS, stated rather than hidden: (a) the callee parameter types are NOT inferred - each is declared with the exact number of 4-byte stack slots the push sequence shows, and the four float immediates are passed as their exact bit patterns (0x400C0000, 0x3F400000, 0x44051EB8) so no float-vs-int claim is made; (b) the fourteen-argument call's argument ORDER is retail's push order reversed (arg1 = last pushed), which is cdecl; (c) what object is, what the table at 0x0053F5F0 holds, and what the colour array at 0x0053F608 means are not established by this body. Domain: core/, because its caller is core/native_413730.cpp and it operates on the core task object layout.

#include <cstdint>

#ifndef NATIVE_DECL
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
#endif

extern "C" {

// NATIVE GAPS - each arity below is measured from the push sequence at this body's single call site of it, not guessed.
// 0x004675B0: 3 slots (push ecx 0x00467FF3, push edx 0x0046800B, push esi 0x00467F67). Body unread.
void Sub_4675B0(void* a1, void* a2, void* a3);
// 0x00498040: 3 slots (push edx 0x00468027, push eax 0x0046802C, push ecx 0x0046802D). Body unread.
void Sub_498040(void* a1, void* a2, void* a3);
// 0x004B3C50: 4 slots (push edx 0x0046803B, push eax 0x00468043, push ecx 0x00468044, push edx 0x00468045). Body unread.
void Sub_4B3C50(std::uint32_t a1, void* a2, void* a3, void* a4);
// 0x004B5860: 14 slots (0x0046804E through 0x00468097; add esp,0x38 = 14 dwords at 0x0046809D). Body unread.
void Sub_4B5860(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5,
                std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10,
                std::uint32_t a11, std::uint32_t a12, std::uint32_t a13, std::uint32_t a14);

void NATIVE_DECL Core_Unknown_467F30(void* argument)
{
    std::uint8_t* const object = static_cast<std::uint8_t*>(argument);
    std::uint8_t frame[0x34]; // 0x00467F30: sub esp,0x34. frame[i] is E-0x34+i (E = return address); slot offsets appear at each use.

    // 0x00467F39: mov ebx,[esi+0x1c]; 0x00467F3C: cmp ebx,-1; 0x00467F3F: je 0x4680a1 (straight to the epilogue).
    const std::int32_t index = *reinterpret_cast<std::int32_t*>(object + 0x1C);
    if (index == -1) {
        return;
    }

    // 0x00467F45-0x00467F59: fld [esi+0x14]; fst [esp+0x40] (the argument slot, E+4, is reused as scratch); fcomp [0x5334f0];
    // fnstsw ax; test ah,1; je; on carry (below) the slot is overwritten with 0x45400000 = 3072.0f, and 0x5334f0 is also
    // 3072.0f, so the operation is a clamp of the float field to at least 3072.0f.
    float value = *reinterpret_cast<float*>(object + 0x14);
    if (value < 3072.0f) { // 0x005334F0: bytes 00 00 40 45
        value = 3072.0f;   // 0x00467F59: mov dword [esp+0x40],0x45400000
    }

    // 0x00467F61-0x00467F68: eax = [esi+0xc] (second vector), ecx = [esi] (first vector); fld qword [eax]; fsub qword [ecx].
    // The three differences go to frame slots E-0x0c, E-0x08, E-0x04 (frame[0x28], frame[0x2C], frame[0x30]).
    const double* const second = *reinterpret_cast<double**>(object + 0x0C);
    const double* const first = *reinterpret_cast<double**>(object);
    *reinterpret_cast<float*>(frame + 0x28) = static_cast<float>(second[0] - first[0]);      // 0x00467F6C: fstp dword [esp+0x38]
    *reinterpret_cast<float*>(frame + 0x2C) = static_cast<float>(second[1] - first[1]);      // 0x00467F76: fstp dword [esp+0x3c]
    *reinterpret_cast<float*>(frame + 0x30) = static_cast<float>(second[2] - first[2]);      // 0x00467F80: fstp dword [esp+0x40]

    // 0x00467F84-0x00467F9F: eax = [esi+4] (the 3x4 float matrix). row = (frame[0x30], frame[0x2C], frame[0x28]) as a row
    // vector; each output is dz*m[k+0x18] + dy*m[k+0x0C] + dx*m[k+0x00] for k = 0,4,8, accumulated with fmul/faddp in that order.
    const float* const matrix = *reinterpret_cast<float**>(object + 4);
    const float dx = *reinterpret_cast<float*>(frame + 0x28);
    const float dy = *reinterpret_cast<float*>(frame + 0x2C);
    const float dz = *reinterpret_cast<float*>(frame + 0x30);
    const float out0 = dz * matrix[6] + dy * matrix[3] + dx * matrix[0]; // 0x00467F9F: fstp dword [esp+0x2c] (E-0x18)
    const float out1 = dz * matrix[7] + dy * matrix[4] + dx * matrix[1]; // 0x00467FBC: fstp dword [esp+0x30] (E-0x14)
    const float out2 = dz * matrix[8] + dy * matrix[5] + dx * matrix[2]; // 0x00467FF4: fstp dword [esp+0x38] (E-0x10 after the push)

    // 0x00467FCE-0x00467FE3: the two computed slots are copied back over the difference slots - frame[0x2C] = out1 (0x00467FDF)
    // and frame[0x28] = out0 (0x00467FE3) - which is why the frame bytes, not separate locals, are used here.
    *reinterpret_cast<std::uint32_t*>(frame + 0x2C) = *reinterpret_cast<const std::uint32_t*>(&out1);
    *reinterpret_cast<std::uint32_t*>(frame + 0x28) = *reinterpret_cast<const std::uint32_t*>(&out0);

    // 0x00467FF0: lea eax,[eax+eax*2] with eax = [esi+0x20] (three floats per entry); 0x00467FFC: lea edi,[eax*4+0x53f608].
    const float* const colour = reinterpret_cast<const float*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(object + 0x20)) * 12U + 0x0053F608U);

    // 0x00467FF4: fstp dword [esp+0x38] stores out2 into E-0x10, then 0x00468003 copies that dword to E-0x04 (frame[0x30]) with
    // `mov [esp+0x44],edx`; the frame[0x30] slot therefore holds out2's bits and is what the first call receives as its third slot's peer.
    *reinterpret_cast<std::uint32_t*>(frame + 0x30) = *reinterpret_cast<const std::uint32_t*>(&out2);

    // 0x00467FEC-0x0046800C: lea ecx,[esp+0x38] = &frame[0x28]; lea edx,[esp+0x30] = &frame[0x1C]; push ecx; push edx; push esi.
    // cdecl: arg1 = last pushed = edx = &frame[0x1C], arg2 = &frame[0x28], arg3 = object.
    Sub_4675B0(frame + 0x1C, frame + 0x28, object);

    // 0x00468011-0x0046801F: eax = [esp+0x50] = arg1 (E+4 once the three pushes are counted), stored twice into E-0x28 and
    // E-0x24 (frame[0x0C], frame[0x10]).
    *reinterpret_cast<void**>(frame + 0x0C) = argument;
    *reinterpret_cast<void**>(frame + 0x10) = argument;

    // 0x00468015-0x0046802E: edx = [esp+0x48] = the dword at E-0x04 (the clamped value / out2 slot); lea eax,[esp+0x24] = &frame[0x0C];
    // lea ecx,[esp+0x30] = &frame[0x14]; push edx; push eax; push ecx -> arg1 = &frame[0x14], arg2 = &frame[0x0C], arg3 = frame[0x30] dword.
    Sub_498040(frame + 0x14, frame + 0x0C, *reinterpret_cast<void**>(frame + 0x30));

    // 0x00468033-0x00468046: edx = &frame[0x00] (lea edx,[esp+0x24] after two pushes; E-0x34), eax = &frame[0x04], ecx = &frame[0x08],
    // then 0x0046803C reloads edx = [esi+0x10]; pushes are edx(frame0), eax(frame4), ecx(frame8), edx([esi+0x10]);
    // cdecl: arg1 = [esi+0x10], arg2 = &frame[0x08], arg3 = &frame[0x04], arg4 = &frame[0x00].
    Sub_4B3C50(*reinterpret_cast<std::uint32_t*>(object + 0x10), frame + 0x08, frame + 0x04, frame + 0x00);

    // 0x0046804B: add esp,0x28 cleans the ten argument dwords of the three calls above (3+3+4).
    // 0x0046804E-0x00468098: fourteen pushes, therefore fourteen arguments, arg1 = the last pushed (edx at 0x00468097):
    //   arg14 = -1 (0x0046804E, push -1)
    //   arg13 = 0x400C0000 (0x00468050)
    //   arg12 = frame[0x18] (E-0x1C, loaded at 0x00468055)
    //   arg11 = frame[0x14] (E-0x20, loaded at 0x00468059)
    //   arg10 = -frame[0x00] (E-0x34: fld [esp+0x14] at 0x0046805D, fchs at 0x0046806C, fstp [esp] at 0x0046806E overwrites the slot)
    //   arg9  = 0x3F400000 (0x00468071)
    //   arg8  = colour[2] ([edi+8], 0x00468061/0x00468076)
    //   arg7  = colour[1] ([edi+4], 0x00468065/0x0046807B)
    //   arg6  = colour[0] ([edi], 0x0046806A/0x00468080)
    //   arg5  = 0x44051EB8 (0x00468088)
    //   arg4  = frame[0x20] (E-0x14, loaded at 0x00468077)
    //   arg3  = frame[0x1C] (E-0x18, loaded at 0x0046807C)
    //   arg2  = *(std::uint32_t*)(0x0053F5F4 + index*8) (the table count, 0x00468081)
    //   arg1  = *(std::uint32_t*)(0x0053F5F0 + index*8) (the table pointer, 0x0046808E)
    const std::uint32_t table_count = *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(index) * 8U + 0x0053F5F4U);
    const std::uint32_t table_pointer = *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(index) * 8U + 0x0053F5F0U);
    const float negated = -*reinterpret_cast<float*>(frame + 0x00);
    Sub_4B5860(table_pointer,
               table_count,
               *reinterpret_cast<const std::uint32_t*>(frame + 0x1C),
               *reinterpret_cast<const std::uint32_t*>(frame + 0x20),
               0x44051EB8U,
               *reinterpret_cast<const std::uint32_t*>(&colour[0]),
               *reinterpret_cast<const std::uint32_t*>(&colour[1]),
               *reinterpret_cast<const std::uint32_t*>(&colour[2]),
               0x3F400000U,
               *reinterpret_cast<const std::uint32_t*>(&negated),
               *reinterpret_cast<const std::uint32_t*>(frame + 0x14),
               *reinterpret_cast<const std::uint32_t*>(frame + 0x18),
               0x400C0000U,
               0xFFFFFFFFU);

    // 0x0046809D: add esp,0x38 cleans the fourteen argument dwords. 0x004680A0-0x004680A6: pop edi, pop esi, pop ebx,
    // add esp,0x34, ret (cdecl; no return value produced).
}

} // extern "C"

#undef NATIVE_DECL
