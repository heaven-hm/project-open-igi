// Native: AIGraph_InitChain_5183D0 (address-suffixed placeholder: the manifest names this VA `Terrain_CalculateNormals_8`, which the body CONTRADICTS - see below; the name used here is the one core/native_508f80.cpp already declares, so the link agrees); retail VA 0x005183D0-0x0051853D.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry. ENTRY VERIFIED INDEPENDENTLY OF THE CALLER: the preceding function's `ret` is at 0x005183C1 followed by 14 `90` padding bytes, so the next instruction boundary is exactly 0x005183D0, and the image's 0xE8 enumeration gives it exactly ONE caller, 0x00508F80. BOUND USED: the forward tail jump at 0x0051853D is NOT followed - it is a tail call into the 0x00519710 routine, and walking into it merges that function's body into this one (that error is how a 5,496-byte extent was first produced here, and radare2 6.2.0 makes the same mistake on tail-jmp bodies - measured on 0x004A5BB0, whose `afij` reports nbbs=43, ninstrs=153, maxaddr=0x004A5E5C against a true 18-block, 66-instruction, 128-byte function whose 43rd block onward is the tail-jumped callee's body). After the tail jump there are 14 `90` nops. SHAPE: 73 contiguous `E8` call instructions, 5 bytes each = 365 bytes from 0x005183D0 to 0x0051853C, followed by the 5-byte `E9` tail jump at 0x0051853D; 74 instructions, 1 block, 1 edge, 68 distinct targets. NOT ONE PUSH BETWEEN THE CALLS: every call site is a bare `call` with no argument setup, so all 73 are transcribed as `void f(void)` and the tail call likewise.
// Behavior/CFG: no frame, no branch, no local state - a straight-line initialisation chain that calls 68 distinct routines in retail order (0x004C1780 is called six times: sites 0x00518498, 0x005184ED, 0x005184F2, 0x005184F7, 0x005184FC, 0x00518501) and then tail-jumps into 0x00519710, which registers the task type "GFXCapsViewer" (byte-exact string at 0x0054ED2C, passed to TaskType_Register 0x00401900). The manifest's name for this VA describes none of this: there is no arithmetic, no branch and no terrain reference anywhere in the extent, so the manifest record needs the same correction. Placeholder names below carry only the ONE verified fact about each target - that this chain calls it - and are to be replaced by behaviour names as each body is read; none of them has a manifest record or a tree file today.
// Status/limits: TRANSCRIBED; NOT LINKABLE - fail-closed on the targets listed in the undefined-symbol report, which is the expected outcome until their own bodies land (62 of the 68 have no tree file). STRUCTURALLY_VERIFIED against the 74-instruction listing (1 block, 1 edge, 68 distinct call targets, extent bounded by the tail jump); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. Domain: core/ (its caller is core/native_508f80.cpp).

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// --- callees that already have a manifest record (names taken from docs/fidelity/native-verification.json) ---
void NATIVE_DECL CamTask_Init(void);  // 0x00530220
void NATIVE_DECL EventId(void);  // 0x00507E70
void NATIVE_DECL InputPort_Init(void);  // 0x004ED510
void NATIVE_DECL EventTable(void);  // 0x004F1EE0
void NATIVE_DECL Script_ParameterEventInit(void);  // 0x004F1840
void NATIVE_DECL Script_InitTypeCatalogue(void);  // 0x004EE970
void NATIVE_DECL DiscardTerrain_Init(void);  // 0x0052C0C0
void NATIVE_DECL TextureModifier_Init(void);  // 0x00505F60
void NATIVE_DECL AnimSound_LoadParameters(void);  // 0x004C1780
void NATIVE_DECL TerrainLightMap_Init(void);  // 0x004F8340
void NATIVE_DECL SplinePathDynCubeObj_Init(void);  // 0x0051A1D0

// --- NATIVE GAPs: no manifest record and no tree file; placeholder names carry only the verified call relationship ---
void NATIVE_DECL AIGraph_InitStep_530270(void);  // 0x00530270
void NATIVE_DECL AIGraph_InitStep_4FCF50(void);  // 0x004FCF50
void NATIVE_DECL AIGraph_InitStep_508D70(void);  // 0x00508D70
void NATIVE_DECL AIGraph_InitStep_504E10(void);  // 0x00504E10
void NATIVE_DECL AIGraph_InitStep_505630(void);  // 0x00505630
void NATIVE_DECL AIGraph_InitStep_52CA70(void);  // 0x0052CA70
void NATIVE_DECL SimBoneDynCubeObj_Register_52CA30(void);  // 0x0052CA30
void NATIVE_DECL AIGraph_InitStep_4F1E30(void);  // 0x004F1E30
void NATIVE_DECL AIGraph_InitStep_52C820(void);  // 0x0052C820
void NATIVE_DECL AIGraph_InitStep_52C6B0(void);  // 0x0052C6B0
void NATIVE_DECL AIGraph_InitStep_4EDCE0(void);  // 0x004EDCE0
void NATIVE_DECL AIGraph_InitStep_4FB1B0(void);  // 0x004FB1B0
void NATIVE_DECL AIGraph_InitStep_503D60(void);  // 0x00503D60
void NATIVE_DECL AIGraph_InitStep_5033F0(void);  // 0x005033F0
void NATIVE_DECL AIGraph_InitStep_52B6C0(void);  // 0x0052B6C0
void NATIVE_DECL AIGraph_InitStep_52B3B0(void);  // 0x0052B3B0
void NATIVE_DECL AIGraph_InitStep_4F3440(void);  // 0x004F3440
void NATIVE_DECL AIGraph_InitStep_4FE7C0(void);  // 0x004FE7C0
void NATIVE_DECL AIGraph_InitStep_502250(void);  // 0x00502250
void NATIVE_DECL AIGraph_InitStep_4F36C0(void);  // 0x004F36C0
void NATIVE_DECL AIGraph_InitStep_4F2740(void);  // 0x004F2740
void NATIVE_DECL AIGraph_InitStep_518EF0(void);  // 0x00518EF0
void NATIVE_DECL AIGraph_InitStep_52B040(void);  // 0x0052B040
void NATIVE_DECL AIGraph_InitStep_4ED6B0(void);  // 0x004ED6B0
void NATIVE_DECL AIGraph_InitStep_504C50(void);  // 0x00504C50
void NATIVE_DECL AIGraph_InitStep_4F4B00(void);  // 0x004F4B00
void NATIVE_DECL AIGraph_InitStep_52AD10(void);  // 0x0052AD10
void NATIVE_DECL AIGraph_InitStep_52AAF0(void);  // 0x0052AAF0
void NATIVE_DECL AIGraph_InitStep_4FAA60(void);  // 0x004FAA60
void NATIVE_DECL AIGraph_InitStep_518760(void);  // 0x00518760
void NATIVE_DECL AIGraph_InitStep_5284A0(void);  // 0x005284A0
void NATIVE_DECL AIGraph_InitStep_5043B0(void);  // 0x005043B0
void NATIVE_DECL AIGraph_InitStep_4F4190(void);  // 0x004F4190
void NATIVE_DECL AIGraph_InitStep_527C90(void);  // 0x00527C90
void NATIVE_DECL AIGraph_InitStep_526EE0(void);  // 0x00526EE0
void NATIVE_DECL AIGraph_InitStep_526330(void);  // 0x00526330
void NATIVE_DECL AIGraph_InitStep_520BB0(void);  // 0x00520BB0
void NATIVE_DECL AIGraph_InitStep_505C90(void);  // 0x00505C90
void NATIVE_DECL AIGraph_InitStep_5207A0(void);  // 0x005207A0
void NATIVE_DECL AIGraph_InitStep_51FEF0(void);  // 0x0051FEF0
void NATIVE_DECL AIGraph_InitStep_51F580(void);  // 0x0051F580
void NATIVE_DECL AIGraph_InitStep_51EEE0(void);  // 0x0051EEE0
void NATIVE_DECL AIGraph_InitStep_51ECB0(void);  // 0x0051ECB0
void NATIVE_DECL AIGraph_InitStep_4EE410(void);  // 0x004EE410
void NATIVE_DECL AIGraph_InitStep_51DDF0(void);  // 0x0051DDF0
void NATIVE_DECL AIGraph_InitStep_51C2B0(void);  // 0x0051C2B0
void NATIVE_DECL AIGraph_InitStep_51BFB0(void);  // 0x0051BFB0
void NATIVE_DECL AIGraph_InitStep_507F10(void);  // 0x00507F10
void NATIVE_DECL AIGraph_InitStep_51BAC0(void);  // 0x0051BAC0
void NATIVE_DECL AIGraph_InitStep_51B960(void);  // 0x0051B960
void NATIVE_DECL AIGraph_InitStep_51B400(void);  // 0x0051B400
void NATIVE_DECL AIGraph_InitStep_4F7360(void);  // 0x004F7360
void NATIVE_DECL AIGraph_InitStep_5034E0(void);  // 0x005034E0
void NATIVE_DECL AIGraph_InitStep_519850(void);  // 0x00519850
void NATIVE_DECL AIGraph_InitStep_51B140(void);  // 0x0051B140
void NATIVE_DECL AIGraph_InitStep_506FA0(void);  // 0x00506FA0
void NATIVE_DECL AIGraph_InitStep_519760(void);  // 0x00519760

// NATIVE GAP: 0x00519710 has no manifest record and no tree file; it registers the task type "GFXCapsViewer"
// (string byte-exact at 0x0054ED2C) through TaskType_Register 0x00401900, so its placeholder carries that behaviour.
std::uint32_t NATIVE_DECL GFXCapsViewer_Register_519710(void);  // 0x00519710

void NATIVE_DECL AIGraph_InitChain_5183D0(void)
{
    AIGraph_InitStep_530270();  // 0x005183D0: call 0x00530270
    AIGraph_InitStep_4FCF50();  // 0x005183D5: call 0x004FCF50
    CamTask_Init();  // 0x005183DA: call 0x00530220
    EventId();  // 0x005183DF: call 0x00507E70
    InputPort_Init();  // 0x005183E4: call 0x004ED510
    AIGraph_InitStep_508D70();  // 0x005183E9: call 0x00508D70
    AIGraph_InitStep_504E10();  // 0x005183EE: call 0x00504E10
    AIGraph_InitStep_505630();  // 0x005183F3: call 0x00505630
    AIGraph_InitStep_52CA70();  // 0x005183F8: call 0x0052CA70
    EventTable();  // 0x005183FD: call 0x004F1EE0
    SimBoneDynCubeObj_Register_52CA30();  // 0x00518402: call 0x0052CA30
    Script_ParameterEventInit();  // 0x00518407: call 0x004F1840
    Script_InitTypeCatalogue();  // 0x0051840C: call 0x004EE970
    AIGraph_InitStep_4F1E30();  // 0x00518411: call 0x004F1E30
    AIGraph_InitStep_52C820();  // 0x00518416: call 0x0052C820
    AIGraph_InitStep_52C6B0();  // 0x0051841B: call 0x0052C6B0
    AIGraph_InitStep_4EDCE0();  // 0x00518420: call 0x004EDCE0
    AIGraph_InitStep_4FB1B0();  // 0x00518425: call 0x004FB1B0
    AIGraph_InitStep_503D60();  // 0x0051842A: call 0x00503D60
    AIGraph_InitStep_5033F0();  // 0x0051842F: call 0x005033F0
    DiscardTerrain_Init();  // 0x00518434: call 0x0052C0C0
    AIGraph_InitStep_52B6C0();  // 0x00518439: call 0x0052B6C0
    AIGraph_InitStep_52B3B0();  // 0x0051843E: call 0x0052B3B0
    AIGraph_InitStep_4F3440();  // 0x00518443: call 0x004F3440
    AIGraph_InitStep_4FE7C0();  // 0x00518448: call 0x004FE7C0
    AIGraph_InitStep_502250();  // 0x0051844D: call 0x00502250
    AIGraph_InitStep_4F36C0();  // 0x00518452: call 0x004F36C0
    AIGraph_InitStep_4F2740();  // 0x00518457: call 0x004F2740
    TextureModifier_Init();  // 0x0051845C: call 0x00505F60
    AIGraph_InitStep_518EF0();  // 0x00518461: call 0x00518EF0
    AIGraph_InitStep_52B040();  // 0x00518466: call 0x0052B040
    AIGraph_InitStep_4ED6B0();  // 0x0051846B: call 0x004ED6B0
    AIGraph_InitStep_504C50();  // 0x00518470: call 0x00504C50
    AIGraph_InitStep_4F4B00();  // 0x00518475: call 0x004F4B00
    AIGraph_InitStep_52AD10();  // 0x0051847A: call 0x0052AD10
    AIGraph_InitStep_52AAF0();  // 0x0051847F: call 0x0052AAF0
    AIGraph_InitStep_4FAA60();  // 0x00518484: call 0x004FAA60
    AIGraph_InitStep_518760();  // 0x00518489: call 0x00518760
    AIGraph_InitStep_5284A0();  // 0x0051848E: call 0x005284A0
    AIGraph_InitStep_5043B0();  // 0x00518493: call 0x005043B0
    AnimSound_LoadParameters();  // 0x00518498: call 0x004C1780
    AIGraph_InitStep_4F4190();  // 0x0051849D: call 0x004F4190
    AIGraph_InitStep_527C90();  // 0x005184A2: call 0x00527C90
    AIGraph_InitStep_526EE0();  // 0x005184A7: call 0x00526EE0
    AIGraph_InitStep_526330();  // 0x005184AC: call 0x00526330
    AIGraph_InitStep_520BB0();  // 0x005184B1: call 0x00520BB0
    AIGraph_InitStep_505C90();  // 0x005184B6: call 0x00505C90
    AIGraph_InitStep_5207A0();  // 0x005184BB: call 0x005207A0
    AIGraph_InitStep_51FEF0();  // 0x005184C0: call 0x0051FEF0
    AIGraph_InitStep_51F580();  // 0x005184C5: call 0x0051F580
    AIGraph_InitStep_51EEE0();  // 0x005184CA: call 0x0051EEE0
    AIGraph_InitStep_51ECB0();  // 0x005184CF: call 0x0051ECB0
    AIGraph_InitStep_4EE410();  // 0x005184D4: call 0x004EE410
    AIGraph_InitStep_51DDF0();  // 0x005184D9: call 0x0051DDF0
    AIGraph_InitStep_51C2B0();  // 0x005184DE: call 0x0051C2B0
    AnimSound_LoadParameters();  // 0x005184E3: call 0x004C1780
    AnimSound_LoadParameters();  // 0x005184E8: call 0x004C1780
    AnimSound_LoadParameters();  // 0x005184ED: call 0x004C1780
    AnimSound_LoadParameters();  // 0x005184F2: call 0x004C1780
    AnimSound_LoadParameters();  // 0x005184F7: call 0x004C1780
    AIGraph_InitStep_51BFB0();  // 0x005184FC: call 0x0051BFB0
    AIGraph_InitStep_507F10();  // 0x00518501: call 0x00507F10
    AIGraph_InitStep_51BAC0();  // 0x00518506: call 0x0051BAC0
    AIGraph_InitStep_51B960();  // 0x0051850B: call 0x0051B960
    AIGraph_InitStep_51B400();  // 0x00518510: call 0x0051B400
    AIGraph_InitStep_4F7360();  // 0x00518515: call 0x004F7360
    TerrainLightMap_Init();  // 0x0051851A: call 0x004F8340
    AIGraph_InitStep_5034E0();  // 0x0051851F: call 0x005034E0
    AIGraph_InitStep_519850();  // 0x00518524: call 0x00519850
    SplinePathDynCubeObj_Init();  // 0x00518529: call 0x0051A1D0
    AIGraph_InitStep_51B140();  // 0x0051852E: call 0x0051B140
    AIGraph_InitStep_506FA0();  // 0x00518533: call 0x00506FA0
    AIGraph_InitStep_519760();  // 0x00518538: call 0x00519760
    GFXCapsViewer_Register_519710();  // 0x0051853D: jmp 0x00519710 (tail call; retail does not return here)
}

} // extern "C"

#undef NATIVE_DECL
