#pragma once

#include <cstdint>

namespace igi::retail::sys {

// ILFF chunk navigation, transcribed from the retail IGI.EXE decompilations in
// assets/BinaryNinja/natives/CLEAN/sys/.
//
// Chunk header layout used by all four functions (32-bit fields, byte offsets from the chunk):
//   +0x00  tag (FourCC dword)
//   +0x04  declared size
//   +0x08  alignment
//   +0x0C  next-sibling relative offset

// Native: Ilff_GetNextChunk() - VA 0x004B7470 - complete port.
// Previous catalog alias: Sys_PopStack_19.
// Returns base + [base+0x0C] when that offset is non-zero, otherwise null.
void* Ilff_GetNextChunk(void* context);
inline void* Sys_PopStack_19(void* context) { return Ilff_GetNextChunk(context); }

// Native: Ilff_GetFirstChunk() - VA 0x004B7490 - complete port.
// Previous catalog alias: Sys_ResolveSymbol_19.
// Returns the first child position, base + ((alignment + 0x13) & ~(alignment - 1)),
// when the alignment is a power of two and that position is below the declared size.
void* Ilff_GetFirstChunk(void* context);
inline void* Sys_ResolveSymbol_19(void* context) { return Ilff_GetFirstChunk(context); }

// Native: Ilff_GetChunkPayload() - VA 0x004B74D0 - complete port.
// Previous catalog alias: Sys_CheckHeap_19.
// Resolves a child payload inside its parent, validating both alignments and the
// child's position against the parent's declared size.
void* Ilff_GetChunkPayload(void* context, void* arg2);
inline void* Sys_CheckHeap_19(void* context, void* arg2) { return Ilff_GetChunkPayload(context, arg2); }

// Native: Ilff_ReadContainerHeader() - VA 0x004B7540 - complete port.
// Previous catalog alias: Sys_AllocateMemory_20.
// Reads chunk declared size (+0x04) and alignment (+0x08) into optional output pointers.
void Ilff_ReadContainerHeader(std::int32_t* out_size, std::int32_t* out_alignment, const void* context);
inline void Sys_AllocateMemory_20(std::int32_t* out_size, std::int32_t* out_alignment, const void* context)
{
    Ilff_ReadContainerHeader(out_size, out_alignment, context);
}

// Native: Ilff_GetFormType() - VA 0x004B7570 - complete port.
// Previous catalog alias: Sys_FreeMemory_20.
// Returns the 32-bit form FourCC at chunk offset +0x10 when `context` is non-null, otherwise 0.
std::int32_t Ilff_GetFormType(const void* context);
inline std::int32_t Sys_FreeMemory_20(const void* context) { return Ilff_GetFormType(context); }

// Native: Ilff_GetChunkTag() - VA 0x004B7580 - complete port.
// Previous catalog alias: Sys_OpenFileStream_20.
// Returns the 32-bit chunk tag FourCC at offset +0x00 when `context` is non-null, otherwise 0.
std::int32_t Ilff_GetChunkTag(const std::int32_t* context);
inline std::int32_t Sys_OpenFileStream_20(const std::int32_t* context) { return Ilff_GetChunkTag(context); }

// Native: Ilff_FindChunk() - VA 0x004B7590 - complete port.
// Previous catalog alias: Sys_ReadFileBlock_20.
// Walks the sibling chain and returns the first chunk whose tag matches.
std::int32_t* Ilff_FindChunk(std::int32_t* context, std::int32_t tag);
inline std::int32_t* Sys_ReadFileBlock_20(std::int32_t* context, std::int32_t tag)
{
    return Ilff_FindChunk(context, tag);
}

// Global 0x00A43E5C - set to 1 by the validators above when a chunk layout is malformed.
[[nodiscard]] std::int32_t Ilff_GetErrorFlag();
void Ilff_ClearErrorFlag();

} // namespace igi::retail::sys
