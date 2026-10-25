// Native: AIGraph_Init_508F80 (placeholder name; retail symbol unknown; it runs the AI init chain and then the "AIGraph" registrar, so the class it leaves registered is "AIGraph" at 0x0054E888). retail VA 0x00508F80-0x00508F89 (10 bytes, 1 block, 0 edges, 2 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (bytes e8 4b f4 00 00 e9 16 00 00 00 = `call 0x005183D0` at 0x00508F80 and `jmp 0x00508FA0` at 0x00508F85, followed by NOP padding to 0x00508F90); radare2 6.2.0 af/afij at 0x00508F80 reports 787 bytes / 2 blocks / 198 instructions because the r2 extent is merged with 0x00508FA0 (both extents end at 0x00509293) - the real body is the 10 bytes above, see the size caveat in docs/fidelity/evidence/startup-402930-callee-sizes.md; the caller is the startup sequence 0x00402930, which calls this VA first as a straight-line registration call (0x00402930 e8 4b 66 10 00).
// Behavior/CFG: one block, no branch: an unconditional call to 0x005183D0 (an init chain: 73 direct calls between 0x005183D0 and 0x0051853C and then a tail jump to 0x00519710, which registers "GFXCapsViewer"; not transcribed) followed by an unconditional tail jump into 0x00508FA0 (the "AIGraph" class registrar, transcribed in core/native_508fa0.cpp). The native has no frame and pushes nothing.
// Status/limits: TRANSCRIBED (static only); STRUCTURALLY_VERIFIED only as the two-instruction, no-branch shape; BUILD_VERIFIED (host and i686 single-file compile); runtime UNOBSERVED. Two recorded shape differences. (1) The retail tail jump is written as a call followed by a return. Neither callee reads its caller's stack words, and 0x00508FA0 takes no argument, so no stack slot, argument or caller frame is observable in either form; the return-value word differs in principle only because a C++ void call may leave EAX unspecified, and the 0x00402930 sequence never reads the result of this call (it is a straight line of `call`s with no test of EAX between them). (2) This file is link-blocked on 0x005183D0 alone - 0x00508FA0 and all its own callees must land, and 0x00508FA0's two remaining gaps are the absent handler bodies and 0x004F9DF0 (listed there).
#include "dev_log.hpp"
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005183D0: the init chain this native runs first. Not transcribed yet: its body is 73 direct calls (68 distinct targets) from
// 0x005183D0 to 0x0051853C followed by `jmp 0x00519710`; 370 bytes for the calls plus that jump, 0x005183D0-0x00518541. The
// r2 extent of 4985 bytes / 90 instructions for this VA is a merge artifact and must not be used. Declared with the shape its own
// instructions show: no argument, no return value read.
void NATIVE_DECL AIGraph_InitChain_5183D0();

// 0x00508FA0: the "AIGraph" class registrar, transcribed in core/native_508fa0.cpp. Retail reaches it by tail jump.
void NATIVE_DECL AIGraph_Register_508FA0();

void NATIVE_DECL AIGraph_Init_508F80()
{
    NATIVE_LOG_ENTER("AIGraph_Init_508F80", "0x00508F80");
    AIGraph_InitChain_5183D0();   // 0x00508F80: `call 0x005183D0`

    AIGraph_Register_508FA0();    // 0x00508F85: `jmp 0x00508FA0` - tail jump, written as a call and a return
}   // 0x00508F89-0x00508F8F: NOP padding, no instruction

} // extern "C"

#undef NATIVE_DECL
