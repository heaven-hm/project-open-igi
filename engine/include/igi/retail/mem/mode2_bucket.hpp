// Retail mode-2 bucket heap layout, transcribed from the instructions that use it.
//
// The mode-2 allocator keeps a circular list of 16 MB reservation nodes. The list is anchored by a
// static sentinel at retail 0x005446B0 (image bytes: self links, payload pointer 0x005446C8, and -1
// bounds). Each node reserves 0x400 pages of 0x1000 bytes and keeps one 8-byte bookkeeping entry per
// page, written from the end of the node backwards, so:
//   0x004AB733 uses lea edi,[esi+0x2010] and walks slots down by 8 while adding 0x1000 to the page
//   offset, which puts entry i at node+0x18+8*i and page 0x3FF-i at base+0x1000*(0x3FF-i).
// Nothing here adds behaviour: the structure only names the offsets the retail instructions read.
#ifndef IGI_RETAIL_MEM_MODE2_BUCKET_HPP
#define IGI_RETAIL_MEM_MODE2_BUCKET_HPP

#include <cstdint>

namespace igi::retail::mem {

// Layout of one mode-2 reservation node; sizeof is 0x2018, matching the offsets 0x00..0x2010 that
// 0x004AB6DD and 0x004AB733 address.
struct Mode2BucketNode
{
    Mode2BucketNode* prev;          // +0x00
    Mode2BucketNode* next;          // +0x04
    void* payload_08;               // +0x08, image data points at +0x18
    std::uint32_t slot_marker_0c;   // +0x0C, lowest decommitted slot (0 when none)
    void* base_10;                  // +0x10, reservation base; -1 marks the sentinel
    void* end_14;                   // +0x14, reservation end
    std::uint32_t page_entries[0x800]; // +0x18: 0x400 entries of two dwords
};

// Static sentinel at retail 0x005446B0; defined in mode2_bucket_globals.cpp.
extern Mode2BucketNode g_5446b0;
// Retail 0x005466D0: node the free path remembered last; 0 when none.
extern Mode2BucketNode* g_5466d0;
// Retail 0x00936200: number of pages currently committed across all nodes (0x004AB733 decrements it,
// 0x004AB84C increments it, and 0x004AB84C compares it against 0x20).
extern std::int32_t g_936200;

} // namespace igi::retail::mem

#endif
