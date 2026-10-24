// Storage for region R8 (retail 0x00A758A4..0x00A758C7): the script task/parse state block that the
// class-method 2 handlers clear. Data only, no native.
//
// The block is declared as its individual dword members rather than one struct because only these five
// addresses are referenced by transcribed code, and the extent between them includes an unreferenced hole
// (0x00A758A8) plus write-only members not yet reached. Declaring a struct would assert a layout nothing has
// proved - see docs/fidelity/evidence/0x4EE970-data-extents.md section 6 for the proven boundaries.
//
// These must stay external symbols: 0x00A758A4, 0x00A758AC, 0x00A758B4 and 0x00A758B8 have no reader
// anywhere in the image, so their stores are the only observable effect of the handlers that write them, and
// a file-local declaration would let the optimiser delete those stores.
#include <cstdint>

extern "C" {

std::uint32_t g_a758a4 = 0; // 0x00A758A4, R8 +0x00, written by 0x004F0610
std::uint32_t g_a758ac = 0; // 0x00A758AC, R8 +0x08, written by 0x004F06F0
std::uint32_t g_a758b0 = 0; // 0x00A758B0, R8 +0x0C, read and cleared by 0x004F0640
std::uint32_t g_a758b4 = 0; // 0x00A758B4, R8 +0x10, written by 0x004F0710
std::uint32_t g_a758b8 = 0; // 0x00A758B8, R8 +0x14, written by 0x004F0730

} // extern "C"
