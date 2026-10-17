#pragma once

#include <cstdint>

namespace igi::retail::dynobj {

using PaletteLoaderFn = std::int32_t (*)(void* dst_record, const char* path);
using AnimIffLoaderFn = void* (*)(const char* path);
using LovsEntryLoaderFn = void** (*)(const char* name);

// Native: DynObj_LoadPackageAnimations() - VA 0x004CF910 - complete port.
// Previous catalog alias: DynObj_RemoveSymbol_14.
// Behavior: locates the `'BANM'` (`0x42414E4D`) chunk inside `*(context + 0x08)` via
//           Ilff_FindFormChunk (0x004CEAA0); when present and `*banm > 0`, formats
//           `"%sanims/%s.iff"` using either `g_a4edf8` (when `g_a4edf8[0] != 0`) or `g_a4ed6c`
//           via Game_DataSymbolLoad (0x004A53B3) and calls Render_FileSIsNotIffFile (0x004D67A0)
//           for each NUL-terminated animation name.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004CF910-0x004CF9CA, 186 bytes, digest 9bc4b0a10b59709630cf47788d176a5bcca0360df50b4e8349e0edafa1a0d524;
//                 CLEAN/dynobj/004CF910-DynObj_RemoveSymbol_14.c.
// CFG: radare2 10 blocks/14 edges; Binary Ninja 10 blocks; 100% structural agreement.
// Confidence: high for exact BANM chunk tag, prefix selection (`0x00A4EDF8` vs `0x00A4ED6C`), and loop.
// Signature: x86 cdecl void*(void* context); ported as void* DynObj_LoadPackageAnimations(void* context).
// Usage/tests: MTP package animation loader called by DynObj_LoadModelTexturePackage; exercised by mtp_package_loader_test.cpp.
void* DynObj_LoadPackageAnimations(void* context);
inline void* DynObj_RemoveSymbol_14(void* context) { return DynObj_LoadPackageAnimations(context); }

// Native: DynObj_LoadPackagePalettes() - VA 0x004CF870 - complete port.
// Previous catalog alias: DynObj_LinkDynamicCube_14.
// Behavior: locates the `'PALF'` (`0x50414C46`) chunk inside `*(context + 0x08)` via
//           Ilff_FindFormChunk (0x004CEAA0), stores `count = *palf` at `*(context + 0x44)`,
//           allocates `count * 0x0C` bytes via Mem_Alloc (0x004B0C60) into `*(context + 0x3C)` when
//           `count > 0` (or clears `*(context + 0x3C) = 0`), and iterates each NUL-terminated
//           palette name formatting `"%spalettes/%s.pal"` into `0x00A4EC6C` via Game_DataSymbolLoad
//           and calling Sys_SIsNotAValidPaletteF (0x004B7D30).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF870-0x004CF909, 153 bytes,
//                 digest 246a42f51d3979bfd848ccefcef6fd53d81b2373c741392b5d719b6b46f29e09;
//                 CLEAN/dynobj/004CF870-DynObj_LinkDynamicCube_14.c.
// CFG: radare2 7 blocks/8 edges; Binary Ninja 7 blocks; 100% structural agreement.
// Confidence: high for exact PALF chunk tag, allocation stride `0x0C`, and callee sequence.
// Signature: x86 cdecl void*(void* context); ported as void* DynObj_LoadPackagePalettes(void* context).
// Usage/tests: MTP package palette loader called by DynObj_LoadModelTexturePackage; exercised by mtp_package_loader_test.cpp.
void* DynObj_LoadPackagePalettes(void* context);
inline void* DynObj_LinkDynamicCube_14(void* context) { return DynObj_LoadPackagePalettes(context); }

// Native: DynObj_LoadPackageMeshes() - VA 0x004CF330 - complete port.
// Previous catalog alias: DynObj_CreateMagicObject_14.
// Behavior: locates the `'MODS'` (`0x4D4F4453`) chunk inside `*(context + 0x08)` via
//           Ilff_FindFormChunk (0x004CEAA0) and iterates `*mods` NUL-terminated model
//           names, calling DynObj_LoadModelMesh (0x004CF390) and appending each returned record
//           to the list at `context + 0x10` via HeapList_Insert (0x004B0E60).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF330-0x004CF381, 81 bytes,
//                 digest 1f15034786839dfd674728a70a7502c4ea2ab4ff265eefdbd0012b2948a9460a;
//                 CLEAN/dynobj/004CF330-DynObj_CreateMagicObject_14.c.
// CFG: radare2 5 blocks/6 edges; Binary Ninja 5 blocks; 100% structural agreement.
// Confidence: high for exact MODS tag, DynObj_LoadModelMesh call, and list insertion at `+0x10`.
// Signature: x86 cdecl void*(void* context); ported as void* DynObj_LoadPackageMeshes(void* context).
// Usage/tests: MTP package MEF model loader called by DynObj_LoadModelTexturePackage; exercised by mtp_package_loader_test.cpp.
void* DynObj_LoadPackageMeshes(void* context);
inline void* DynObj_CreateMagicObject_14(void* context) { return DynObj_LoadPackageMeshes(context); }

// Native: DynObj_LoadPackageLovs() - VA 0x004CF630 - complete port.
// Previous catalog alias: DynObj_ParseConfigFile_14.
// Behavior: locates the optional `'LOVS'` (`0x53564F4C`) chunk inside `*(context + 0x08)` via
//           Ilff_FindFormChunk (0x004CEAA0); when present, iterates `*lovs` NUL-terminated
//           names calling DynObj_UpdateDynTree_14 (0x004CF690) and appending each returned node to
//           the list at `context + 0x1C` via HeapList_Insert (0x004B0E60).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF630-0x004CF685, 85 bytes,
//                 digest e7cb705e985c39decbb0b12edbef88c179529f6dab08e9c9b276e7256ac30c9b;
//                 CLEAN/dynobj/004CF630-DynObj_ParseConfigFile_14.c.
// CFG: radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Confidence: high for exact LOVS tag, null-chunk guard, and list insertion at `+0x1C`.
// Signature: x86 cdecl void*(void* context); ported as void* DynObj_LoadPackageLovs(void* context).
// Usage/tests: MTP package LOVS loader called by DynObj_LoadModelTexturePackage; exercised by mtp_package_loader_test.cpp.
void* DynObj_LoadPackageLovs(void* context);
inline void* DynObj_ParseConfigFile_14(void* context) { return DynObj_LoadPackageLovs(context); }

// Native: DynObj_LoadModelTexturePackage() - VA 0x004CF080 - complete port.
// Previous catalog alias: DynObj_InsertHashItem_14.
// Behavior: loads an MTP package into `context`: calls DynObj_LoadPackageAnimations(context) (0x004CF910);
//           when `load_textures != 0` (`arg3`), loads `"%stextures/%s.res"` via Resource_Load,
//           calls DynObj_LoadModelTextures(context) (0x004CF790), DynObj_LoadPackagePalettes(context)
//           (0x004CF870), and Resource_Unload; loads `"%smodels/%s.res"` into `*(context + 0x0C)`,
//           calls DynObj_LoadPackageMeshes(context) (0x004CF330) and DynObj_LoadPackageLovs(context)
//           (0x004CF630); resolves `'VNAM'` (`0x564E414D`), `'INST'` (`0x494E5354`), and `'GTT '`
//           (`0x47545420`) chunks via Ilff_FindFormChunk (0x004CEAA0), allocates each
//           instance record (`(mat_count << 3) + 0x1C` bytes) via Mem_Alloc (0x004B0C60), links it
//           into `context + 0x28` via HeapList_Insert (0x004B0E60), resolves local/shared
//           model (`< 0x2000` vs `>= 0x2000` in `g_a4ec60`) via DynObj_LookupHashItem (0x004CFA30),
//           and populates the instance texture/palette pointer tables.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF080-0x004CF2F3, 627 bytes,
//                 digest 2cf87f358150e1e4f5d5b5d2b7fbdf9597d8faddeaa1e0cb25e06f434d1e50e5;
//                 CLEAN/dynobj/004CF080-DynObj_InsertHashItem_14.c.
// CFG: radare2 28 blocks/43 edges; Binary Ninja 28 blocks; 100% structural agreement.
// Confidence: high for exact VNAM/INST/GTT chunk tags, 0x2000 shared-package threshold, and offsets.
// Signature: x86 cdecl void*(void* context, const char* arg2, int32_t arg3).
// Usage/tests: retail MTP package loader; exercised by mtp_package_loader_test.cpp.
void* DynObj_LoadModelTexturePackage(void* context, const char* arg2, std::int32_t arg3);
inline void* DynObj_InsertHashItem_14(void* context, const char* arg2, std::int32_t arg3)
{
    return DynObj_LoadModelTexturePackage(context, arg2, arg3);
}

// Host/test configuration for global package state (`0x00A4EC60`, `0x00A4ED6C`, `0x00A4EDF8`).
void Host_SetSharedPackageContext(void* shared_pkg) noexcept;
void Host_SetPackagePrefix(const char* prefix) noexcept;
void Host_SetAnimOverridePrefix(const char* prefix) noexcept;
void Host_SetPaletteLoader(PaletteLoaderFn fn) noexcept;
void Host_SetAnimIffLoader(AnimIffLoaderFn fn) noexcept;
void Host_SetLovsEntryLoader(LovsEntryLoaderFn fn) noexcept;

} // namespace igi::retail::dynobj
