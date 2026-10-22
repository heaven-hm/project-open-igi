// Native: Iff_FindFormChunk; retail VA 0x00517890; signature/calling convention x86 cdecl void*(void* list, uint32_t tag).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/model/iff_chunk_tree.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00517890-binaryninja.json and 0x00517890-radare2.json; paired retail and candidate execution in scripts/native_iff_diff_runner.py.
// Behavior/CFG: Walks the sibling list. For a node whose +0x10 holds the FORM marker it matches the form type at +0x14 and descends into the form's first child at +0x08; for any other node it matches the tag at +0x10. The port holds the chunk tree by value and the sibling and child pointers replace the retail's raw node links.
// Status/limits: STRUCTURALLY_VERIFIED against the retail instruction sequence; the retail's node links become the port's pointers, and the payload is held by value rather than by the retail's pointer.
// Native: Iff_FindChunk; retail VA 0x00517940; signature/calling convention x86 cdecl void*(void* form, uint32_t tag).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/model/iff_chunk_tree.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00517940-binaryninja.json and 0x00517940-radare2.json; paired retail and candidate execution in scripts/native_iff_diff_runner.py.
// Behavior/CFG: Walks a form's children from the first child at +0x08. A child whose +0x10 holds the FORM marker is descended into; any other child is matched on its tag at +0x10. The port holds the chunk tree by value and the sibling and child pointers replace the retail's raw node links.
// Status/limits: STRUCTURALLY_VERIFIED against the retail instruction sequence; the retail's node links become the port's pointers, and the payload is held by value rather than by the retail's pointer.
// Native: Iff_GetChunkPayload; retail VA 0x005179C0; signature/calling convention x86 cdecl void*(void* chunk).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/model/iff_chunk_tree.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x005179C0-binaryninja.json and 0x005179C0-radare2.json; paired retail and candidate execution in scripts/native_iff_diff_runner.py.
// Behavior/CFG: One basic block, no calls and no branches. It returns the chunk's payload pointer at +0x1C. The port holds the payload by value, so the address of the buffer is what the retail pointer stands for.
// Status/limits: STRUCTURALLY_VERIFIED against the retail instruction sequence; the payload is held by value rather than by the retail's pointer.
// Native: Iff_FindChunkBelowForm; retail VA 0x005178E0; signature/calling convention x86 cdecl void*(void* node, uint32_t tag).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/model/iff_chunk_tree.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x005178E0-binaryninja.json and 0x005178E0-radare2.json; paired retail and candidate execution in scripts/native_iff_diff_runner.py.
// Behavior/CFG: When the node it is given already carries the tag at +0x14 the search starts at that node's sibling, and the walk matches the form type at +0x14 and descends into the form's first child at +0x08. The port holds the chunk tree by value and the sibling and child pointers replace the retail's raw node links.
// Status/limits: STRUCTURALLY_VERIFIED against the retail instruction sequence; the retail's node links become the port's pointers, and the payload is held by value rather than by the retail's pointer.
#include "igi/retail/model/iff_chunk_tree.hpp"

#include <cstddef>
#include <cstring>
#include <fstream>
#include <vector>

namespace igi::retail::model
{
namespace
{

constexpr std::size_t kChunkHeaderSize = 8U;
constexpr std::size_t kFormTypeSize = 4U;

std::uint32_t ReadBigEndian32(const std::vector<unsigned char>& bytes, std::size_t offset)
{
    if (offset + 4U > bytes.size())
    {
        return 0U;
    }
    return (static_cast<std::uint32_t>(bytes[offset]) << 24) |
           (static_cast<std::uint32_t>(bytes[offset + 1U]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 2U]) << 8) |
           static_cast<std::uint32_t>(bytes[offset + 3U]);
}

// The retail's chunk tag constant for a form type, as the retail compares it.
std::uint32_t FormTypeOf(const std::vector<unsigned char>& bytes, std::size_t offset)
{
    return ReadBigEndian32(bytes, offset);
}

// HOST HELPER: parses the chunks of a form body into the retail's node layout.
void ParseChunks(const std::vector<unsigned char>& bytes, std::size_t offset, std::size_t end,
                 std::vector<std::unique_ptr<IffChunkNode>>& out);

void ParseFormBody(const std::vector<unsigned char>& bytes, std::size_t body, std::size_t body_end,
                   IffChunkNode& node)
{
    node.tag = iff_form_marker;
    node.form_type = FormTypeOf(bytes, body);
    ParseChunks(bytes, body + kFormTypeSize, body_end, node.children);
    for (std::size_t index = 0; index < node.children.size(); ++index)
    {
        node.children[index]->next =
            (index + 1U < node.children.size()) ? node.children[index + 1U].get() : nullptr;
    }
    node.first_child = node.children.empty() ? nullptr : node.children.front().get();
}

void ParseChunks(const std::vector<unsigned char>& bytes, std::size_t offset, std::size_t end,
                 std::vector<std::unique_ptr<IffChunkNode>>& out)
{
    while (offset + kChunkHeaderSize <= end)
    {
        const std::uint32_t tag = ReadBigEndian32(bytes, offset);
        const std::size_t size = ReadBigEndian32(bytes, offset + 4U);
        auto node = std::make_unique<IffChunkNode>();
        node->tag = tag;
        if (tag == iff_form_marker)
        {
            // A form's body ends at its own declared size.
            const std::size_t body_end = std::min(end, offset + kChunkHeaderSize + size);
            ParseFormBody(bytes, offset + kChunkHeaderSize, body_end, *node);
        }
        else
        {
            const std::size_t payload = offset + kChunkHeaderSize;
            const std::size_t payload_size = std::min(size, bytes.size() - payload);
            if (payload <= bytes.size())
            {
                node->payload.resize(payload_size);
                if (payload_size > 0U)
                {
                    std::memcpy(node->payload.data(), &bytes[payload], payload_size);
                }
            }
        }
        out.push_back(std::move(node));
        if (size == 0U)
        {
            break;
        }
        offset = offset + kChunkHeaderSize + size + (size & 1U);
    }
}

} // namespace

IffChunkNode* IffChunk_FindByTag(IffChunkNode* list, std::uint32_t tag)
{
    // Transcribed from retail 0x00517890. The retail walks the sibling list,
    // and for a form it matches the form's own type and then descends into
    // the form's first child.
    for (IffChunkNode* node = list; node != nullptr; node = node->next)
    {
        if (node->tag == iff_form_marker)
        {
            // Retail 0x5178B1 compares +0x14 against the query for a form.
            if (node->form_type == tag)
            {
                return node;
            }
            if (node->first_child != nullptr)
            {
                IffChunkNode* found = IffChunk_FindByTag(node->first_child, tag);
                if (found != nullptr)
                {
                    return found;
                }
            }
        }
        else if (node->form_type == tag)
        {
            // Retail 0x5178AB also compares +0x14, not the +0x10 tag. A plain
            // chunk therefore matches on the field at +0x14.
            return node;
        }
    }
    return nullptr;
}

IffChunkNode* IffChunk_FindChildByTag(IffChunkNode* form, std::uint32_t tag)
{
    // Transcribed from retail 0x00517940. The retail walks a form's children
    // and descends into a nested form to search its children.
    for (IffChunkNode* node = form->first_child; node != nullptr; node = node->next)
    {
        if (node->tag == iff_form_marker)
        {
            IffChunkNode* found = IffChunk_FindChildByTag(node, tag);
            if (found != nullptr)
            {
                return found;
            }
        }
        else if (node->tag == tag)
        {
            return node;
        }
    }
    return nullptr;
}

const std::byte* IffChunk_GetPayload(const IffChunkNode* chunk)
{
    // Transcribed from retail 0x005179C0, which returns the chunk's payload
    // pointer. The port holds the payload by value, so the address of the
    // buffer is what the retail pointer stands for.
    return chunk->payload.empty() ? nullptr : chunk->payload.data();
}

IffChunkNode* IffChunk_FindBelowForm(IffChunkNode* node, std::uint32_t tag)
{
    // Transcribed from retail 0x005178E0. When the node it is given already
    // carries the tag, the retail starts the search at that node's sibling.
    IffChunkNode* start = node;
    if (start->form_type == tag)
    {
        start = start->next;
    }
    for (IffChunkNode* current = start; current != nullptr; current = current->next)
    {
        if (current->tag == iff_form_marker)
        {
            if (current->form_type == tag)
            {
                return current;
            }
            if (current->first_child != nullptr)
            {
                IffChunkNode* found = IffChunk_FindByTag(current->first_child, tag);
                if (found != nullptr)
                {
                    return found;
                }
            }
        }
        else if (current->form_type == tag)
        {
            return current;
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<IffChunkNode>> IffChunkTree_Parse(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        return {};
    }
    const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)),
                                           std::istreambuf_iterator<char>());
    if (bytes.size() < kChunkHeaderSize)
    {
        return {};
    }
    std::vector<std::unique_ptr<IffChunkNode>> roots;
    ParseChunks(bytes, 0U, bytes.size(), roots);
    for (std::size_t index = 0; index < roots.size(); ++index)
    {
        roots[index]->next = (index + 1U < roots.size()) ? roots[index + 1U].get() : nullptr;
    }
    return roots;
}

} // namespace igi::retail::model
