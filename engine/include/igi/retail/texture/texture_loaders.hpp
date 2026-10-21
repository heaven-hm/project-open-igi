#pragma once

#include <cstdint>

namespace igi::retail::texture {

using TextureBackendDispatchFn = std::int32_t (*)(void* packet);

// Native: Texture_GetActiveBackendId() - VA 0x004B0F80 - complete port.
// Behavior: returns the active render backend identifier stored in `0x00A94A64` (`mov eax, [0x00A94A64]; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004B0F80-0x004B0F86, 6 bytes, digest 29ebd30649f3b57e6ec4d50668338cbb32243052c21fe58371cadd573f365c23.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact global getter semantics.
// Signature: x86 cdecl int32_t(void); ported as std::int32_t Texture_GetActiveBackendId().
// Usage/tests: queries the active texture/palette backend id for Texture_UploadMipmapPacket and Texture_UploadPalettePacket; exercised by texture_loaders_test.cpp.
[[nodiscard]] std::int32_t Texture_GetActiveBackendId();

// Native: Texture_EnableUpload() - VA 0x004B26A0 - complete port.
// Behavior: sets the texture upload state byte `0x00943B3C` to `1` (`mov byte ptr [0x00943B3C], 1; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B26A0-0x004B26A8, 8 bytes,
//                 digest 8209abbcb1723ad9145b7bfe97647edb07b2acc47ff1b02f71fa78fd9ab559e1.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact byte store.
// Signature: x86 cdecl void(void); ported as void Texture_EnableUpload().
// Usage/tests: enables texture upload flag before Texture_UploadMipmapPacket; exercised by texture_loaders_test.cpp.
void Texture_EnableUpload();

// Native: Texture_DisableUpload() - VA 0x004B26B0 - complete port.
// Behavior: clears the texture upload state byte `0x00943B3C` to `0` (`mov byte ptr [0x00943B3C], 0; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B26B0-0x004B26B8, 8 bytes,
//                 digest 4893ff3f0d5654223486e4c269627dbfc9c121aa464471eedbf09c918c805d04.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact byte clear.
// Signature: x86 cdecl void(void); ported as void Texture_DisableUpload().
// Usage/tests: clears texture upload flag after Texture_UploadMipmapPacket; exercised by texture_loaders_test.cpp.
void Texture_DisableUpload();

// Native: Texture_UploadMipmapPacket() - VA 0x004B23B0 - complete port.
// Behavior: writes `Texture_GetActiveBackendId()` into `*arg1`, zero-initializes a 48-byte (12-dword)
//           dispatch packet on the stack (`+0x00=arg1`, `+0x04=arg2`, `+0x08=arg7`, `+0x0C=arg3`,
//           `+0x18=arg4`, `+0x1C=arg5`, `+0x20=arg6`, `+0x24=arg8`, `+0x28=(int8_t)g_943b3c`,
//           `+0x2C=arg9`), and calls the two renderer callbacks at `0x00A94E84[g_546fa8]` and
//           `0x00A94E84[g_546fb0]` with the packet pointer.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B23B0-0x004B2446, 150 bytes,
//                 digest 65ce08ad0b3bfb6914e1cd899467c3bcc788ddb0bd6a4c1a2b476295dfe1c109;
//                 CLEAN/sys/004B23B0-Texture_UploadMipmapPacket.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact 48-byte packet layout and dual backend dispatch.
// Signature: x86 cdecl int32_t(int32_t* arg1, void* arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, void* arg9).
// Usage/tests: texture upload dispatcher called by Texture_LoadLoopTextureFile and Terrain_LoadTexturePackage; exercised by texture_loaders_test.cpp.
std::int32_t Texture_UploadMipmapPacket(std::int32_t* arg1, void* arg2, std::int32_t arg3, std::int32_t arg4,
                                        std::int32_t arg5, std::int32_t arg6, std::int32_t arg7,
                                        std::int32_t arg8, void* arg9);

// Native: Texture_UploadPalettePacket() - VA 0x004B7CF0 - complete port.
// Behavior: writes `Texture_GetActiveBackendId()` into `*arg1`, builds a 12-byte (3-slot) palette
//           packet (`+0x00=arg1`, `+0x04=arg3`, `+0x08=arg2`), and calls the renderer callback
//           at `0x00A94E84[g_a84a48]` with the packet pointer.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B7CF0-0x004B7D28, 56 bytes,
//                 digest 91d6a4b0ea6acca264ed1285d7d516332e1293e63e09487c7fad2bcb3febac92.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact 3-slot packet layout and backend dispatch.
// Signature: x86 cdecl int32_t(int32_t* arg1, void* arg2, int32_t arg3).
// Usage/tests: terrain palette upload dispatcher called by Terrain_LoadTexturePackage; exercised by texture_loaders_test.cpp.
std::int32_t Texture_UploadPalettePacket(std::int32_t* arg1, void* arg2, std::int32_t arg3);

// Native: Texture_LoadLoopTextureFile() - VA 0x004B24C0 - complete port.
// Behavior: loads `resource_path` (`arg3`) via Resource_Load(arg3, nullptr) (0x004B5F00), checks
//           the `'LOOP'` header (`0x4C, 0x4F, 0x4F, 0x50`) and version at `+0x04` (`2` or `11`),
//           calls Texture_EnableUpload() (0x004B26A0), extracts the version-specific 16-bit/32-bit
//           header fields and pixel payload pointer (`+0x18` for v2, `+0x20` for v11), calls
//           Texture_UploadMipmapPacket(...) (0x004B23B0), Resource_Unload(arg3) (0x004B6380), and
//           Texture_DisableUpload() (0x004B26B0). On invalid header, calls
//           Error_Show("'%s' is not a valid Texture file.", arg3) (0x004AF7B0).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B24C0-0x004B25AF, 239 bytes,
//                 digest 01d287da4b443fddcd7d6be5cdf3c1bc3df57987d4b5a46165182aa4d3b9c79c;
//                 CLEAN/sys/004B24C0-Sys_SIsNotAValidTextureF.c.
// CFG: radare2 15 blocks/22 edges; Binary Ninja 15 blocks; 100% structural agreement.
// Confidence: high for exact LOOP v2/v11 field extraction and callee sequence.
// Signature: x86 cdecl int32_t(int32_t* arg1, void* arg2, const char* arg3).
// Usage/tests: model texture loader invoked by DynObj_LoadModelTextures; exercised by texture_loaders_test.cpp.
std::int32_t Texture_LoadLoopTextureFile(std::int32_t* arg1, void* arg2, const char* arg3);
inline std::int32_t Sys_SIsNotAValidTextureF(std::int32_t* arg1, void* arg2, const char* arg3)
{
    return Texture_LoadLoopTextureFile(arg1, arg2, arg3);
}

// Native: Terrain_LoadTexturePackage() - VA 0x005175A0 - complete port.
// Behavior: calls Texture_EnableUpload() (0x004B26A0), loads `resource_path` (`arg1`) via
//           Resource_Load(arg1, nullptr) (0x004B5F00) (calling Error_Show("Couldn't load texture file '%s'.")
//           if null), checks `'LOOP'` (`0x4C, 0x4F, 0x4F, 0x50`) and version `7` or `9` at `+0x04`,
//           allocates and uploads optional palette at `header[6]` via Mem_Alloc(0x0C, 4) and
//           Texture_UploadPalettePacket (0x004B7CF0), allocates `header[8] * 0x24 + 0x30` bytes via Mem_Alloc,
//           iterates `header[8]` records (stride `0x28` for v7 at `+0x3C`, stride `0x20` for v9 at
//           `+0x40`) calling Texture_UploadMipmapPacket (0x004B23B0), then calls Texture_DisableUpload() and
//           Resource_Unload(arg1).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x005175A0-0x005177E2, 578 bytes,
//                 digest 450ad796b5382d56bd90a518553df529c860093b5070d3b3d7af6b886a82f530;
//                 CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.c.
// CFG: radare2 29 blocks/42 edges; Binary Ninja 29 blocks; 100% structural agreement.
// Confidence: high for exact LOOP v7/v9 record strides, palette upload, and callee order.
// Signature: x86 cdecl int32_t*(const char* arg1); ported as std::int32_t* Terrain_LoadTexturePackage(const char* arg1).
// Usage/tests: retail terrain texture loader; exercised by texture_loaders_test.cpp.
std::int32_t* Terrain_LoadTexturePackage(const char* arg1);
inline std::int32_t* Terrain_TextureFileNotOfRequ(const char* arg1)
{
    return Terrain_LoadTexturePackage(arg1);
}

// Native: DynObj_LoadModelTextures() - VA 0x004CF790 - complete port.
// Behavior: locates the `'TEXF'` (`0x54455846`) chunk inside `*(context + 0x08)` via
//           Ilff_FindFormChunk (0x004CEAA0), stores `count = *texf` at `*(context + 0x40)`,
//           allocates `count * 0x24` bytes at `*(context + 0x34)` and `count * 0x0C` bytes at
//           `*(context + 0x38)` via Mem_Alloc (0x004B0C60) when `count > 0` (or clears both to 0),
//           and iterates each NUL-terminated texture name formatting `"LOCAL:textures/%s.tex"` into
//           `0x00A4EC6C` via sprintf (0x004A53B3) and calling
//           Texture_LoadLoopTextureFile (0x004B24C0).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF790-0x004CF865, 213 bytes,
//                 digest 75a65077e61f7571a67d6370e3eec880541dbaafcea3aed7af6dec07cf49002f;
//                 CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.c.
// CFG: radare2 7 blocks/8 edges; Binary Ninja 7 blocks; 100% structural agreement.
// Confidence: high for exact TEXF chunk parsing, allocation sizes, and loop stride (+0x24 / +0x0C).
// Signature: x86 cdecl void*(void* context); ported as void* DynObj_LoadModelTextures(void* context).
// Usage/tests: MTP package texture loader called by DynObj_LoadModelTexturePackage; exercised by texture_loaders_test.cpp.
void* DynObj_LoadModelTextures(void* context);
inline void* DynObj_LocalTexturesSTex(void* context)
{
    return DynObj_LoadModelTextures(context);
}

// Host/test configuration for texture backend state and callbacks.
[[nodiscard]] std::int8_t Texture_GetUploadFlag() noexcept;
void Texture_SetBackendId(std::int32_t id) noexcept;
void Texture_SetCallbackSlots(std::int32_t slot_primary, std::int32_t slot_secondary,
                                  std::int32_t slot_palette) noexcept;
void Texture_SetBackendCallback(std::int32_t slot, TextureBackendDispatchFn fn) noexcept;

} // namespace igi::retail::texture
