#pragma once

// The retail IFF chunk tree.
//
// igi.exe does not walk the IFF bytes when it reads an animation file. The
// resource loader parses the file into a tree of chunk nodes, and the
// animation loader navigates that tree with four helpers:
//
//   0x00517890  find a chunk or form by tag, descending into nested forms
//   0x00517940  find a chunk by tag among a form's children
//   0x005179C0  the chunk's payload pointer
//   0x005178E0  find a chunk by tag, starting below a matching form
//
// The node layout the helpers use is the retail's, and the offsets below are
// the ones the helpers read.
//
// The four definitions in iff_chunk_tree.cpp already have external linkage in
// their translation unit, so --whole-archive cannot drop them. Their emitted
// names were the port's descriptive C++ identifiers, though, which left the
// retail catalog names unresolvable in the linked candidate. Each declaration
// below therefore pins its assembler name to the retail catalog name with a
// GCC asm-label, so the out-of-line definition itself is addressable under the
// name the symbol map, the coverage report and the paired probes resolve. The
// assembler name does not alter the body, the cdecl argument layout, the
// return value or any side effect.
//
// Artifact: retail igi.exe SHA256
// dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja decompilation of the four helpers above;
// radare2 CFG for each; angr CFGFast for the exact entries.
// Behavior: see the transcription notes on each function.
// Status/limits: STRUCTURALLY_VERIFIED; the byte-level parse that fills the
// tree is the retail resource loader's work (0x004B5F00) and is reimplemented
// here as a host adapter over the same container layout.

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

namespace igi::retail::model
{

// The retail chunk node. The offsets are the ones the navigation helpers read.
struct IffChunkNode
{
    // +0x00 the next sibling node.
    IffChunkNode* next = nullptr;
    // +0x08 the first child of a form.
    IffChunkNode* first_child = nullptr;
    // +0x10 the chunk's tag, or the FORM marker for a form.
    std::uint32_t tag = 0U;
    // +0x14 the form type of a form.
    std::uint32_t form_type = 0U;
    // +0x1C the chunk's payload.
    std::vector<std::byte> payload;
    // The child nodes, owned so the sibling pointers stay valid.
    std::vector<std::unique_ptr<IffChunkNode>> children;
};

// The retail's FORM marker, as the helpers compare it.
inline constexpr std::uint32_t iff_form_marker = 0x464F524DU;

// Native: 0x00517890, the retail's find-by-tag over a chunk list.
//
// Finds the first chunk or form whose tag matches, descending into the
// children of a nested form. The retail's source name for this routine is a
// terrain function; the body is the chunk search.
[[nodiscard]] IffChunkNode* IffChunk_FindByTag(IffChunkNode* list, std::uint32_t tag)
    asm("Iff_FindFormChunk");

// Native: 0x00517940, the retail's find-child-by-tag.
//
// Searches a form's children for a matching chunk, and descends into a nested
// form to search its children.
[[nodiscard]] IffChunkNode* IffChunk_FindChildByTag(IffChunkNode* form, std::uint32_t tag)
    asm("Iff_FindChunk");

// Native: 0x005179C0, the retail's chunk payload pointer.
[[nodiscard]] const std::byte* IffChunk_GetPayload(const IffChunkNode* chunk)
    asm("Iff_GetChunkPayload");

// Native: 0x005178E0, the retail's find-below-a-form.
//
// Like the find-by-tag, but when the node it is given already carries the
// tag, the search starts at that node's sibling.
[[nodiscard]] IffChunkNode* IffChunk_FindBelowForm(IffChunkNode* node, std::uint32_t tag)
    asm("Iff_FindChunkBelowForm");

// HOST HELPER: parses a retail IFF file into the retail's chunk tree.
//
// The retail's resource loader (0x004B5F00) builds this tree when it loads
// the file; the container layout is the same one the retail reads: a form is
// "FORM", a big-endian size and a four byte type, followed by chunks that
// are a four byte tag, a big-endian size and the payload.
[[nodiscard]] std::vector<std::unique_ptr<IffChunkNode>>
IffChunkTree_Parse(const std::filesystem::path& path);

} // namespace igi::retail::model
