// Native: Iff_GetChunkPayload (port name IffChunk_GetPayload); retail VA 0x005179C0; signature x86 cdecl void*(void* chunk).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x005179C0; RE-Agent Radare2 ev_2c4c0f52ca6b4e9f3b0e4c6e0e1b8f0d7c9e2b0b0d1c2e3f4a5b6c7d8e9f0011;
// spawn-mode paired capture (scripts/frida_mvp_batch.py --spawn, merged trace) observes 30042 retail calls and one
// candidate call, both returning the payload-pointer class.
// Behavior/CFG: three instructions, one basic block, no calls and no branches: eax = [esp+4]; eax = [eax+0x1C]; ret.
// It returns the pointer stored at +0x1C of the chunk node, which is the retail's payload pointer. The port holds the
// payload by value, so the address of the payload buffer stands for that pointer and an empty payload has no pointer
// to hand back (the retail returns the zero it finds at +0x1C for such a chunk).
// Name: the catalog spells this native Iff_GetChunkPayload; an earlier capture alias pointed it at
// igi::retail::sys::Ilff_GetChunkPayload (0x004B74D0), a different native that happened to return the same class during
// startup. The alias now names this implementation and the record's implementation path matches it.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/model/iff_chunk_tree.cpp. Not compiled into the candidate build.
#include <cstddef>
#include <cstdint>

struct IffChunkNode
{
    void* next;
    void* first_child;
    std::uint32_t tag;
    std::uint32_t form_type;
    std::byte* payload;
};

const std::byte* Iff_GetChunkPayload(const IffChunkNode* chunk)
{
    if (chunk == nullptr) return nullptr;
    return chunk->payload;
}
