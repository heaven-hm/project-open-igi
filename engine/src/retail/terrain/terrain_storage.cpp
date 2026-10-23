// Native storage: retail terrain globals for the contour traversal, its render consumers and the
// address-bound render-chunk pool initialiser.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
//   1,384,448 bytes, PE i386 (machine 0x014C), image base 0x00400000, entry 0x004A50F5.
// Tools/evidence (retail image only): pefile 2024.8.26 section/raw map, re-read for this revision
//   (.data VA 0x00536000, virtual size 0x6FC558, file-backed 0x00536000..0x0054FFFF at raw
//   0x00136000, file offset 0x001470B4 = 01 00 00 00, entry 0x004A50F5); Binary Ninja MCP 0.1.0
//   (`bn_function_decompile`, `bn_function_disassembly`, `bn_function_basic_blocks`,
//   `bn_function_callers`, `bn_function_callees`, `bn_memory_read`, `bn_data_xrefs_to` for
//   0x00516640, 0x004B0B00, 0x004B0C60, 0x00516600, 0x00495170, 0x00495190 and 0x00491CA0 and for
//   the globals quoted below); radare2 6.2.0 (`aaa`, `af`, `afbj`, `afij`, `pdf`, `axt`, `px`);
//   angr 10.0.4.post1 CFGFast (exact entries 0x00516640 and 0x004B0B00, no unresolved jumps).
//
// What this unit covers:
//   * The 22 terrain globals left unresolved by the earlier link that followed the addition of
//     terrain_draw.cpp, terrain_out_of_terrain_renderch.cpp, terrain_quadtree.cpp and
//     terrain_unknown_blendmode.cpp to the igi target (the 24-symbol task set is those 22 plus the
//     functions Debug_Error 0x004AF7B0 and DynObj_ParseConfigFile_6 0x004C5810). Nineteen are
//     retail storage and are defined here; the other three are documented at the bottom with the
//     correct fix instead of placeholder bytes.
//   * The nine symbols the next link attempt reported: g_5ca114 and g_a7e068 (blend mode),
//     g_bcab08 and g_bcaae0 (quadtree) and g_a7f878, g_a7f87c, g_a7f880, g_a7f888 and g_a7f88c
//     (render-chunk pool). All nine are defined here; terrain_draw.cpp no longer defines them.
//   * The third increment: the address-bound transcription of retail 0x00516640, the render-chunk
//     pool initialiser that also creates the terrain vertex buffer, together with the body of
//     retail 0x004B0B00, the descriptor constructor it calls. That is the code which establishes
//     the 0x00A7F878 pool descriptor in retail, so those definitions now have a native identity.
//
// Initial contents: the PE .data section covers VA 0x00536000..0x00C32558, but only
//   0x00536000..0x0054FFFF is file-backed (PointerToRawData 0x00136000, SizeOfRawData 0x0001A000).
//   Every address below except 0x005470B4 lies past that raw extent and is therefore BSS: the loader
//   zero-fills it and the image on disk contains no bytes for it. The one file-backed global,
//   0x005470B4, holds 01 00 00 00 at file offset 0x001470B4 and is initialised to 1 here. Sizes are
//   the access widths of the retail instructions (or a record stride times a proven element count),
//   not the sizes of the candidate's declarations.
//
// Runtime reachability of the initialiser (analysed, not wired): retail 0x00516640 has no direct
//   call. Retail 0x00516600 pushes its address (`0x00516600 push 0x00516640`) into
//   Display_CreateSurface_7 (0x00495170), whose setter 0x00495190 links it as the callback of an
//   8-byte node in the 10-slot display callback list 0x00BCAD20..0x00BCAD6F (next pointer at
//   +0x00, callback at +0x04, list head in 0x005CA118). Display_CloseDevice_4 (0x00491CA0) walks
//   that list with `for (i = arg1; i; i = i[0]) i[1]()` and is called with the 0x005CA118 head at
//   0x00494219. The callback therefore takes no arguments and its EAX return value is discarded.
//   None of 0x00516600, 0x0050FAC0 (its caller), 0x00531D30 (that caller) or the display list is
//   transcribed in the candidate, so 0x00516640 is NOT reachable from the current candidate; it is
//   transcribed and compiled but not wired into any startup path. The neighbouring bucket seed at
//   0x00516621 belongs to 0x00516600, not to 0x00516640: it fills the 509 12-byte buckets
//   0x00A7E080..0x00A7F85B and is not executed by this unit.
//
// Status/limits: STRUCTURALLY_VERIFIED for the transcribed 0x00516640 at source/operation level
//   (docs/fidelity/evidence/0x00516640-structural-review-20261023.json and
//   -semantic-review-20261023.json): retail Binary Ninja-vs-radare2 graph score 1.0, angr recovered
//   the exact entry, and the candidate-vs-retail comparison records both calls, every constant and
//   the object divergences (GCC if-conversion of the 0x00BCAD74 branch, dead first stores,
//   independent-store reordering, and the specialised .constprop.0 helper). Not RUNTIME_TESTED and
//   not VERIFIED: no execution of the initialiser, in retail or in the candidate, was observed, and
//   the pool base depends on the engine Mem_Alloc port (0x004B0C60) whose retail failure path is not
//   exercised here. The storage definitions themselves remain TRANSCRIBED: retail address ownership,
//   access width and image-initial value are reproduced; they do not by themselves prove the
//   surrounding natives.
//   All eight terrain translation units are listed in the igi target (CMakeLists.txt). Build
//   evidence for this revision is compile-level: `i686-w64-mingw32-g++ -std=gnu++20 -O2` produces a
//   COFF i386 object; `i686-w64-mingw32-objdump -f` reports file format pe-i386, architecture i386
//   (the host LLVM objdump calls the same file coff-i386). The full target was not relinked
//   by this change; a /tmp partial link of the eight terrain objects with the mem/heap objects was
//   used to check the new references. No executable, receipt or runtime claim is made.
//
// Ownership: terrain_draw.cpp defines g_a7f890 (0x00A7F890, the vertex buffer this initialiser
//   creates) and terrain_out_of_terrain_renderch.cpp defines g_a7f884 (the +0x0C capacity field).
//   This unit declares g_a7f890 here (no header owns that declaration) and reads g_a7f884 through
//   terrain_out_of_terrain_renderch.hpp; neither is redefined. This unit is the single definition
//   for g_5ca110 (0x005CA110) and g_bcad74 (0x00BCAD74), which no other engine unit owns.

#include "igi/retail/terrain/terrain_storage.hpp"

#include <cstddef>
#include <cstdint>

#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/terrain/terrain_draw.hpp"
#include "igi/retail/terrain/terrain_out_of_terrain_renderch.hpp"
#include "igi/retail/terrain/terrain_unknown_blendmode.hpp"

namespace igi::retail::terrain
{
// -------------------------------------------------------------------------------------------------
// Render-world pointer: 0x00A44344, 4 bytes, BSS -> null.
//   0x00415C00  mov dword [0xa44344], edx      ; edx = [eax+0x20], render world installed
//   0x004D14EC  mov eax, dword [0xa44344]
//   0x004D14F4  mov eax, dword [eax+0xdfc]     ; height cut-off source, nullptr => 0xe0000000
// Both accesses are dword, so the object is a 32-bit pointer.
// -------------------------------------------------------------------------------------------------
void* g_a44344 = nullptr;

// -------------------------------------------------------------------------------------------------
// LOD save area: 0x00A4EF38, 0x140 bytes, BSS -> zeros.
//   0x004D18B7  lea edi, [eax + 0xa4ef38]     ; eax = slot*0x40, slot 0..4
//   0x004D2340  mov ecx, 0x10
//   0x004D2347  rep movsd                       ; 0x10 dwords = 0x40 bytes per slot, copied back
//   0x004D2349  add eax, 0x40
//   0x004D234D  cmp eax, 0xa4f078
//   0x004D2352  jb  0x4d2331                    ; loop over 0xa4ef38..0xa4f078
// The region is exactly 0x140 bytes = five 0x40-byte node copies.
// -------------------------------------------------------------------------------------------------
std::uint8_t g_a4ef38[0x140]{};

// -------------------------------------------------------------------------------------------------
// Traversal roots: 0x00A53740, 0x140 bytes, BSS -> zeros.
//   0x004D197D  mov dword [eax + 0xa5374c], edx ; eax = slot*0x40, child[3] = node
//   0x004D198C  lea edx, [eax + 0xa53740]       ; published root for the slot
//   0x004D19A8  add edx, 4                      ; output cursor
//   0x004D19AB  add eax, 0x40
//   0x004D19B5  cmp edx, 5                      ; five slots
// Same 5 x 0x40-byte shape as the save area above.
// -------------------------------------------------------------------------------------------------
std::uint8_t g_a53740[0x140]{};

// -------------------------------------------------------------------------------------------------
// Category callback tables: 0x00A9C4E0 and 0x00A9CAE0, 0x180 entries of 4 bytes each
// (0x600 bytes per table), BSS -> all null.
//
// Entry width (4 bytes) and index domain:
//   0x004D1CAB  call dword [edx*4 + 0xa9c4e0]   ; split callback, edx = word [item+0x1c]
//   0x004D1AE3  call dword [ecx*4 + 0xa9cae0]   ; merge callback, ecx = word [item+0x1c]
//   0x004C9C01, 0x004CB267, 0x004CB31D, 0x004CB63B, 0x004CCCA0 (0x00A9C4E0) and
//   0x004CB6B8, 0x004CCC58 (0x00A9CAE0): the same scaled dword calls from other contour
//   consumers.
//   0x004C72B0  mov ecx, dword [eax + 0xa9c4e0] ; eax = category<<2 (DynObj_InvalidCategory)
//   0x004C72BA  mov ecx, dword [eax + 0xa9cae0]
//
// Extent (0x180 entries): the image holds a contiguous family of category tables at an exact
// 0x600-byte stride: 0x00A982E0, 0x00A988E0, 0x00A994E0, 0x00A99AE0, 0x00A9A0E0, 0x00A9A6E0,
// 0x00A9ACE0, 0x00A9C4E0 and 0x00A9CAE0 (0x00A9CAE0 - 0x00A982E0 = 0x4800 = 12 * 0x600). The
// category row width is 0x180 entries:
//   0x005084FC  mov dx, word [esi+0x1c]         ; category
//   0x00508500  lea ecx, [eax + eax*2]
//   0x00508503  shl ecx, 7                       ; eax * 3 * 0x80 = eax * 0x180
//   0x00508506  add ecx, edx
//   0x00508508  mov eax, dword [ecx*4 + 0xa96ae0] ; [row*0x180 + category] dword table
// The dispatch path performs no range check on the uint16 category; 0x180 is the table stride
// proven by the layout, while the highest category the data actually registers is not established
// statically. No direct writer of these two tables exists in .text (they are runtime-populated, and
// the registration path could not be located), so the image state is all-zero.
// -------------------------------------------------------------------------------------------------
TerrainContourCallback g_a9c4e0[0x180]{};
TerrainContourCallback g_a9cae0[0x180]{};

// -------------------------------------------------------------------------------------------------
// Previous-frame contour hash cache: 0x00B04340, 0x800 entries x 0x1C bytes = 0xE000 bytes,
// BSS -> zeros.
//   0x004D214C  and eax, 0x7ff                   ; 0x800 slots, open addressing
//   0x004D2151  lea ecx, [eax*8]
//   0x004D2158  sub ecx, eax                     ; eax * 7
//   0x004D215A  shl ecx, 2                       ; eax * 0x1c
//   0x004D2133  cmp dword [ecx + 0xb04340], esi  ; +0x00 x
//   0x004D213B  cmp dword [ecx + 0xb04344], edi  ; +0x04 y
//   0x004D2143  cmp dword [ecx + 0xb04348], ebx  ; +0x08 z
//   0x004D2184  cmp ecx, dword [eax + 0x10]      ; +0x10 stamp
//   0x004D21CA  mov dword [eax + 0x14], 3        ; +0x14 allocation type
//   0x004D218D  mov ecx, dword [eax + 0x18]      ; +0x18 payload pointer/handle
// Entry stride 0x1C and slot count 0x800 give the 0xE000 extent (TerrainContourHashEntry already
// static_asserts sizeof == 0x1C in terrain_quadtree.hpp).
// -------------------------------------------------------------------------------------------------
TerrainContourHashEntry g_b04340[0x800]{};

// -------------------------------------------------------------------------------------------------
// Dropped-record counter: 0x00B12B80, 4 bytes, BSS -> 0.
//   0x004D1539  mov dword [0xb12b80], edx        ; edx = 0 at traversal entry
//   0x004D21ED  inc dword [0xb12b80]             ; records above var_2C > 0xfff
// -------------------------------------------------------------------------------------------------
std::uint32_t g_b12b80 = 0;

// -------------------------------------------------------------------------------------------------
// Child-mask suppression flag: 0x00B12BB8, 4 bytes, BSS -> 0.
//   0x004D1C3D  mov dword [0xb12bb8], ecx        ; ecx = 0 before the descent
//   0x004D1DE7  mov ecx, dword [0xb12bb8]        ; consulted by the octant pass
//   0x0052C0B0  mov dword [0xb12bb8], 1          ; set by the render-side caller
// -------------------------------------------------------------------------------------------------
std::int32_t g_b12bb8 = 0;

// -------------------------------------------------------------------------------------------------
// Camera-space offsets: 0x00B12BC0 / 0x00B12BC4 / 0x00B12BC8, 4 bytes each, BSS -> 0.
//   0x004D257E  mov dword [0xb12bc0], ecx        ; ecx = [context+0xde0]
//   0x004D2584  mov edx, dword [esi+0xde4]
//   0x004D258A  mov dword [0xb12bc4], edx
//   0x004D2590  mov ecx, dword [esi+0xde8]
//   0x004D2596  mov dword [0xb12bc8], ecx
//   0x004D1640  mov esi, dword [0xb12bc0]        ; read back: -([camera]+offset)
//   0x004D165D  mov eax, dword [0xb12bc4]
//   0x004D166B  mov ecx, dword [0xb12bc8]
// All accesses are dword.
// -------------------------------------------------------------------------------------------------
std::int32_t g_b12bc0 = 0;
std::int32_t g_b12bc4 = 0;
std::int32_t g_b12bc8 = 0;

// -------------------------------------------------------------------------------------------------
// Octant permutation table: 0x00B12BE0, 0x40 bytes, BSS -> zeros.
// Writer (0x004D0EF0):
//   0x004D100D  mov esi, 0xb12be0
//   0x004D1037  mov byte [esi + eax], dl         ; eight bytes per pass
//   0x004D1040  add esi, 8
//   0x004D1044  cmp esi, 0xb12c20
//   0x004D104A  jl  0x4d1012                     ; stops at 0xb12c20
// Reader (0x004D14E0):
//   0x004D15A1  mov ecx, 0x10
//   0x004D15A6  mov esi, 0xb12be0
//   0x004D15B6  rep movsd                        ; 0x10 dwords = 0x40 bytes to the stack
// The writer therefore fills exactly 0x40 bytes before the g_b12c20 counter.
// -------------------------------------------------------------------------------------------------
std::uint8_t g_b12be0[0x40]{};

// -------------------------------------------------------------------------------------------------
// Descriptor count: 0x00B12C20, 4 bytes, BSS -> 0.
//   0x004D2568  mov dword [0xb12c20], eax        ; eax = 0 at frame setup
//   0x004D1B3E  inc edi / 0x004D1B3F mov dword [0xb12c20], edi
//   0x004D1CFB  inc ecx / 0x004D1D02 mov dword [0xb12c20], ecx
// -------------------------------------------------------------------------------------------------
std::uint32_t g_b12c20 = 0;

// -------------------------------------------------------------------------------------------------
// Allocation-record cursor: 0x00B12C58, 4 bytes, BSS -> null.
//   0x004D2548  mov dword [0xb12c58], eax        ; eax = [0xb16c98], the record base
//   0x004D1B18  mov edi, dword [0xb12c58]
//   0x004D1B1E  mov dword [edi + 4], eax         ; {uint8 type, 3 pad, item pointer}
//   0x004D1B27  mov byte [edi], 5
//   0x004D1B30  add edi, edx                     ; edx = 8
//   0x004D1B32  mov dword [0xb12c58], edi
// The value is a 32-bit address, so the global is a pointer.
// -------------------------------------------------------------------------------------------------
std::uint8_t* g_b12c58 = nullptr;

// -------------------------------------------------------------------------------------------------
// Contour detail cut-off: 0x00B16C6C, 4 bytes, BSS -> 0.
//   0x004D2624  cmp eax, 0x46
//   0x004D2627  mov dword [0xb16c6c], eax
//   0x004D262E  mov dword [0xb16c6c], 0x46       ; floor
//   0x004D1543  mov eax, dword [0xb16c6c]        ; captured by the traversal
//   0x0050FC53  fimul dword [0xb16c6c]           ; consumed as an integer
// -------------------------------------------------------------------------------------------------
std::int32_t g_b16c6c = 0;

// -------------------------------------------------------------------------------------------------
// View angles: 0x00BCAB20 / 0x00BCAB24, 4 bytes each, BSS -> 0.0f.
//   0x004D157E  mov ecx, dword [0xbcab20]
//   0x004D1584  mov edx, dword [0xbcab24]
//   0x004D1FB1  fmul dword [0xbcab20]
//   0x004D0435  fmul dword [0xbcab20] / 0x004D046D fmul dword [0xbcab24]
// 0x00BCAB24 is loaded with dword moves and used as a single-precision x87 operand, so both
// globals are 4-byte floats.
// -------------------------------------------------------------------------------------------------
float g_bcab20 = 0.0f;
float g_bcab24 = 0.0f;

// -------------------------------------------------------------------------------------------------
// Frame counter: 0x00C10380, 4 bytes, BSS -> 0.
//   0x00491CDA  inc dword [0xc10380]             ; once per rendered frame
//   0x004D151C  mov eax, dword [0xc10380]
//   0x004D1534  and eax, 0xffff                  ; low half is the cache stamp
// -------------------------------------------------------------------------------------------------
std::uint32_t g_c10380 = 0;

// -------------------------------------------------------------------------------------------------
// Fog enable: 0x005470B4, 4 bytes, file-backed .data -> 1.
//   0x00514547  mov edx, dword [0x5470b4]        ; radare2 annotates [0x5470b4:4]=1
//   0x004E7FA0  mov dword [0x5470b4], ecx        ; overwritten from [ebx+0xBC]
// The PE file at raw offset 0x001470B4 holds 01 00 00 00 (pefile; .data VA 0x00536000 with
// PointerToRawData 0x00136000), so the image initial value is 1, not 0 like the BSS globals.
// -------------------------------------------------------------------------------------------------
std::int32_t g_5470b4 = 1;

// -------------------------------------------------------------------------------------------------
// D3D7 device pointer: 0x005CA114, 4 bytes, BSS -> null.
//   Installer: 0x00494619 `lea ecx, [var_10h]` / 0x0049461A `push ecx` (out slot) /
//     0x00494625 `call dword [edx+0x10]` / 0x00494628 `mov edx, dword [var_10h_2]` /
//     0x00494630 `mov dword [0x5ca114], edx`.
//   Release path (0x00493BB2): 0x00493B3C `xor esi, esi`; 0x00493BA8 `cmp eax, esi`;
//     0x00493BB2 `mov dword [0x5ca114], esi`  ; clears the slot.
//   Readers treat it as a pointer, not an integer: 0x00512D96 `mov eax, dword [0x5ca114]`,
//     0x00512DC7 `mov ecx, dword [eax]` (vtable), 0x00512DCD `call dword [ecx+0x2c]`.
// The adjacent dwords 0x005CA110 and 0x005CA11C are separate renderer pointers, and every access
// to this slot is a full dword, so the object is one 32-bit pointer.
// -------------------------------------------------------------------------------------------------
Device7* g_5ca114 = nullptr;

// -------------------------------------------------------------------------------------------------
// Packed fog colour: 0x00A7E068, 4 bytes, BSS -> 0.
//   Writer (0x00512D8D, moffs32 `a3 68 e0 a7 00`): the value is assembled in Terrain_Draw from the
//     float components at 0x00A7DA30/0x00A7DA34/0x00A7DA38:
//       0x00512D03 fld [0xa7da30] / 0x00512D09 fmul [0x5342d8] (-255.0f) / 0x00512D37 shl ecx, 8
//       0x00512D46 fld [0xa7da34] / 0x00512D4C fmul (-255.0f) / 0x00512D60 add ecx, edx
//       0x00512D69 shl ecx, 8 / 0x00512D6C fld [0xa7da38] / 0x00512D72 fmul 255.0f
//       0x00512D86 sub eax, ecx / 0x00512D8D mov [0xa7e068], eax
//     i.e. eax = int(A7DA38 * 255) - ((int(A7DA30 * -255) + int(A7DA34 * -255)) << 8).
//   Reader (0x00514620-0x0051462C): ecx = [0xa7e068]; push ecx; push 0x22; ... call [edx+0x50].
//     Render-state 0x22 is D3DRS_FOGCOLOR, so the value is a 32-bit render-state argument.
// The next established global is 0x00A7E06C (the strip-winding dword), so the slot is exactly one
// dword and not part of a wider object.
// -------------------------------------------------------------------------------------------------
std::int32_t g_a7e068 = 0;

// -------------------------------------------------------------------------------------------------
// Camera origin: 0x00BCAB08, 0x18 bytes (three doubles), BSS -> 0.0 for each component.
//   0x004D15FE  fld qword [0xbcab08]            ; component 0
//   0x004D1604  fld qword [0xbcab10]            ; component 1, +0x08
//   0x004D161B  fld qword [0xbcab18]            ; component 2, +0x10
//   0x00530D05  fld qword [0xbcab08]
//   0x00530D17  fld qword [0xbcab10]
//   0x00530D35  fld qword [0xbcab18]
//   0x00512FF7  mov ecx, 6
//   0x00512FFC  mov esi, 0xbcab08
//   0x00513011  rep movsd                        ; six dwords = 0x18 bytes copied as a block
// The same six-dword copy appears at 0x00472406, 0x00498D56, 0x004F7904, 0x0052DDEC and
// 0x00530CF2. The next established global, g_bcab20 at 0x00BCAB20, is exactly +0x18, so the array
// ends at 0x00BCAB20.
// -------------------------------------------------------------------------------------------------
double g_bcab08[3] = {};

// -------------------------------------------------------------------------------------------------
// View transform: 0x00BCAAE0, 0x28 bytes (ten floats), BSS -> 0.0f for each element.
//   0x004D160A  mov ecx, 0xa
//   0x004D160F  mov esi, 0xbcaae0
//   0x004D162F  rep movsd                        ; ten dwords = 0x28 bytes copied as a block
//   0x0051301F  mov ecx, 0xa
//   0x00513024  mov esi, 0xbcaae0
//   0x00513043  rep movsd
//   0x004F7916  mov ecx, 0xa / 0x004F791B mov esi, 0xbcaae0 / 0x004F7925 rep movsd
//   0x00499B4C  fmul dword [0xbcaae0]            ; elements are used as single-precision floats
// 0x00BCAAE0 + 0x28 = 0x00BCAB08, the proven g_bcab08 base, so the array ends at 0x00BCAB08.
// -------------------------------------------------------------------------------------------------
float g_bcaae0[10] = {};

// -------------------------------------------------------------------------------------------------
// Visible-render-chunk pool descriptor: 0x00A7F878..0x00A7F88F, 0x18 bytes, BSS -> all zero.
//
// The descriptor is established by retail 0x00516640, which calls the descriptor constructor at
// 0x004B0B00 with four 32-bit arguments:
//   0x0051669D push 0            ; arg4 alignment
//   0x0051669F push 0x80         ; arg3 capacity         (68 80 00 00 00)
//   0x005166A4 push 0x2ef4       ; arg2 element size     (68 f4 2e 00 00)
//   0x005166A9 push 0xa7f878     ; arg1 descriptor address (68 78 f8 a7 00)
//   0x005166AE call 0x004b0b00   ; e8 4d a4 f9 ff
// Inside 0x004B0B00, esi = arg1 = 0x00A7F878 and the field stores are, in this order:
//   0x004B0B1D  mov dword [esi+0x14], ecx   ; ecx = arg2 = 0x2EF4            -> 0x00A7F88C
//   0x004B0B3A  mov dword [esi], eax        ; eax = Mem_Alloc result        -> 0x00A7F878
//   0x004B0B48  mov dword [esi+4], ecx      ; ecx = base + 0x2EF8 * 0x80    -> 0x00A7F87C
//   0x004B0B4B  mov dword [esi+0xc], ebx    ; ebx = arg3 = 0x80              -> 0x00A7F884
//   0x004B0B4E  mov dword [esi+0x10], edi   ; edi = (0x2EF4 + 4 + 3) & ~3    -> 0x00A7F888
//   0x004B0B51  mov dword [esi+8], eax      ; eax = 0                        -> 0x00A7F880
// The stride is computed by 0x004B0B19 `lea edi, [ecx+eax+3]` and 0x004B0B2A `and edi, ~(eax-1)`
// with eax = max(4, arg4) = 4, giving 0x2EF8; the allocation is
// `(edi+4) * arg3` = 0x2EFC * 0x80 = 0x177E00 bytes via Mem_Alloc (0x004B0C60), whose port is
// igi::retail::mem::Mem_Alloc. The index list at +0x04 is then seeded with its own indices by
// 0x004B0B56..0x004B0B5F, and the constructor returns the final index (0x80 here).
// 0x00A7F890 (g_a7f890) is the next established global, so the descriptor ends exactly there.
//
// Retail keeps these six dwords contiguous inside that one object. The candidate's consumers
// declare them as separate extern variables, so this unit keeps separate definitions (the public
// extern declarations cannot be re-bound to struct members) and the initialiser writes them
// through a view bundle in the retail store order. The bundle is a transcription device, not a
// claim that the candidate lays the objects out contiguously; the retail offsets are asserted
// below. g_a7f884 (+0x0C capacity) is defined once in terrain_out_of_terrain_renderch.cpp and is
// only read here.
// -------------------------------------------------------------------------------------------------
std::uint32_t g_a7f878 = 0;   // +0x00 element base, read at 0x0051447C and 0x00514704
std::uint32_t* g_a7f87c = nullptr; // +0x04 index list, read at 0x005143CE and 0x005146FB
std::uint32_t g_a7f880 = 0;   // +0x08 used count, read/incremented at 0x005146DD/0x00514724
std::uint32_t g_a7f888 = 0;   // +0x10 aligned stride, used at 0x005143E0 and 0x0051470A
std::uint32_t g_a7f88c = 0;   // +0x14 requested element size, loaded at 0x00514713

// -------------------------------------------------------------------------------------------------
// 0x005CA110: 32-bit Direct3D 7 interface pointer, 4 bytes, BSS -> null, and 0x00BCAD74: a shared
// renderer dword, 4 bytes, BSS -> 0. Neither has an owner in the engine; both are defined here
// because the transcribed initialiser reads them:
//   0x00516665  a1 74 ad bc 00   mov eax, dword [0xbcad74]  ; test/jne at 0x0051666A/0x0051666C
//   0x00516676  a1 10 a1 5c 00   mov eax, dword [0x5ca110]  ; vtable read at 0x00516691
// A zero 0x00BCAD74 adds D3DVBCAPS_SYSTEMMEMORY (0x800) to D3DVBCAPS_WRITEONLY (0x10000). The same
// zero-means-system-memory idiom appears at 0x0049383B..0x0049384B (a vertex buffer with FVF 0x112
// and 4000 vertices) and at 0x005166FF, so the value is a renderer-wide mode flag, not a terrain
// local; its writers and other readers are outside this unit's scope. The display registration
// helper 0x00495190 uses the *address* 0x00BCAD74 as the exclusive end of its 10-slot callback scan
// (0x00BCAD20..0x00BCAD6F) but never reads the dword there: that is layout adjacency, not the
// variable's meaning. The types below are reconstructed from the one call shape this native uses;
// the first five vtable slots are left unnamed because the unit never calls them.
// -------------------------------------------------------------------------------------------------
struct Direct3D7;

struct Direct3DVertexBufferDesc
{
    std::uint32_t size;         // +0x00: 0x10
    std::uint32_t caps;         // +0x04: 0x10000, or 0x10800 when 0x00BCAD74 == 0
    std::uint32_t fvf;          // +0x08: 0x1C4
    std::uint32_t vertex_count; // +0x0C: 0x8000
};

struct Direct3D7Vtbl
{
    void* unused_00[5]; // +0x00..+0x13: not called by this native
    // +0x14: 0x0051669A `call dword [ecx+0x14]` with (this, &desc, &g_a7f890, 0). The __stdcall
    // cleanup is proved by the epilogue: after the two pushes-only argument groups the function
    // ends with `add esp, 0x20` (0x005166B3), which balances exactly the 0x10-byte local frame plus
    // four still-owned allocator arguments, so the vtable call removed its own four arguments.
    std::int32_t(__stdcall* create_vertex_buffer)(Direct3D7* self,
                                                   const Direct3DVertexBufferDesc* description,
                                                   VertexBuffer7** vertex_buffer,
                                                   std::uint32_t flags);
};

struct Direct3D7
{
    const Direct3D7Vtbl* vtbl;
};

Direct3D7* g_5ca110 = nullptr;
std::int32_t g_bcad74 = 0;

// 0x00A7F890 is defined in terrain_draw.cpp; no header declares it, so this unit declares the
// same object here (terrain_draw.hpp supplies the VertexBuffer7 type).
extern VertexBuffer7* g_a7f890;

namespace
{
// The six retail fields of the 0x00A7F878 descriptor, bound to the candidate's extern objects.
struct RenderChunkPoolView
{
    std::uint32_t& element_base;  // +0x00 0x00A7F878
    std::uint32_t*& index_list;   // +0x04 0x00A7F87C
    std::uint32_t& used;          // +0x08 0x00A7F880
    std::uint32_t& capacity;      // +0x0C 0x00A7F884
    std::uint32_t& stride;        // +0x10 0x00A7F888
    std::uint32_t& element_size;  // +0x14 0x00A7F88C
};

RenderChunkPoolView render_chunk_pool_view()
{
    return {g_a7f878, g_a7f87c, g_a7f880, g_a7f884, g_a7f888, g_a7f88c};
}

// Retail 0x004B0B00 body: build a pool descriptor from (descriptor, element size, capacity,
// alignment). Binary Ninja's symbol name `Sys_FreeMemory_2` contradicts the body (the function
// allocates, stores six fields and seeds an index list), so no name from the binary is claimed.
// The body is shared by 32 retail callers; this unit transcribes it once because the transcribed
// terrain initialiser is its only candidate caller. Every operation below mirrors the retail
// instruction at the quoted address, including the write order and the 32-bit wrap of the size
// arithmetic. The noinline directive is a code-generation choice, not a behavior change: it keeps
// the retail call boundary 0x005166AE -> 0x004B0B00 visible in the candidate object (otherwise GCC
// inlines the body and its seed loop into the caller, which no longer has a loop in retail) so the
// two functions can be compared as separate bodies.
[[nodiscard]] __attribute__((noinline)) std::int32_t build_render_chunk_pool(
    RenderChunkPoolView pool, std::int32_t element_size, std::int32_t capacity,
    std::int32_t alignment_value)
{
    // 0x004B0B00..0x004B0B09: eax = arg4; if (eax < 4) eax = 4  (unsigned compare, `jae`).
    std::uint32_t alignment = static_cast<std::uint32_t>(alignment_value);
    if (alignment < 4U)
    {
        alignment = 4U;
    }

    // 0x004B0B1D: the requested element size is stored before the allocation.
    pool.element_size = static_cast<std::uint32_t>(element_size);

    // 0x004B0B19/0x004B0B24..0x004B0B2A: (size + alignment + 3) & ~(alignment - 1).
    const std::uint32_t stride =
        (static_cast<std::uint32_t>(element_size) + alignment + 3U) & ~(alignment - 1U);

    // 0x004B0B2C..0x004B0B33: Mem_Alloc((stride + 4) * capacity, alignment). Retail does not test
    // the result: a null block yields the same arithmetic and the seed loop below then writes
    // through the derived address, so that behaviour is preserved rather than guarded.
    const std::uint32_t bytes = (stride + 4U) * static_cast<std::uint32_t>(capacity);
    void* const block = mem::Mem_Alloc(static_cast<std::int32_t>(bytes),
                                       static_cast<std::int32_t>(alignment));
    const std::uint32_t base = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(block));

    // 0x004B0B3A..0x004B0B51: field stores in retail order.
    pool.element_base = base;
    pool.index_list = reinterpret_cast<std::uint32_t*>(
        static_cast<std::uintptr_t>(base + stride * static_cast<std::uint32_t>(capacity)));
    pool.capacity = static_cast<std::uint32_t>(capacity);
    pool.stride = stride;
    pool.used = 0;

    // 0x004B0B56..0x004B0B5F: index[i] = i for i in [0, capacity). The function returns the final
    // index, which is capacity on this path (and 0 when capacity <= 0).
    std::int32_t index = 0;
    if (capacity > 0)
    {
        do
        {
            pool.index_list[index] = static_cast<std::uint32_t>(index);
            ++index;
        } while (index < capacity);
    }
    return index;
}
} // namespace

// -------------------------------------------------------------------------------------------------
// Native: Terrain_SelectTerrainLOD_3; retail VA 0x00516640; the catalog/Binary Ninja name is a misnomer (the body creates the terrain vertex buffer and builds the 0x00A7F878 pool, it selects no LOD) and is kept only as the address-bound identity.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01, 1,384,448 bytes, PE i386, image base 0x00400000, entry 0x004A50F5.
// Tools/evidence: Binary Ninja MCP 0.1.0 and radare2 6.2.0 instruction comparison, angr 10.0.4.post1 CFGFast; docs/fidelity/evidence/0x00516640-{binaryninja,radare2,angr}-20261023.json, -structural-review-20261023.json and -semantic-review-20261023.json.
// Behavior/CFG: 119 bytes (0x00516640..0x005166B6), 3 basic blocks / 3 edges, 29 instructions, 1 indirect call (vtable +0x14) and 1 direct call (0x004B0B00), one `retn`; builds a four-dword vertex-buffer descriptor (0x10, 0x10000 or 0x10800, 0x1C4, 0x8000), calls vtable slot +0x14 of 0x005CA110 with (&desc, &g_a7f890, 0), then builds the 0x00A7F878 pool with (0x2EF4, 0x80, 0) and returns the pool index count (0x80); the display callback list invokes it with no arguments and discards the return value.
// Status/limits: STRUCTURALLY_VERIFIED at source/object level (structural and semantic review artifacts above); not RUNTIME_TESTED and not VERIFIED, and not reachable from the current candidate (registrar 0x00516600 is not transcribed, so nothing calls it yet). The candidate object is 1 block / 0 edges because GCC if-converted the [0x00BCAD74] branch; the stored values and both calls are identical and the divergence is recorded in the structural review.
// -------------------------------------------------------------------------------------------------
extern "C" std::int32_t Terrain_SelectTerrainLOD_3()
{
    // 0x00516640..0x00516661: the four zero/default descriptor stores, in retail order.
    Direct3DVertexBufferDesc desc;
    desc.size = 0;
    desc.size = 0x10;
    desc.caps = 0;
    desc.caps = 0x10000;
    desc.fvf = 0;
    desc.vertex_count = 0;

    // 0x00516665..0x0051666E: when the renderer mode dword is zero, add
    // D3DVBCAPS_SYSTEMMEMORY (0x800) to D3DVBCAPS_WRITEONLY (0x10000).
    if (g_bcad74 == 0)
    {
        desc.caps = 0x10800;
    }

    // 0x00516676: the interface pointer is loaded before the last two descriptor fields.
    Direct3D7* const d3d7 = g_5ca110;
    desc.fvf = 0x1C4;           // 0x00516681
    desc.vertex_count = 0x8000; // 0x00516689
    // 0x00516691..0x0051669A: vtbl slot +0x14, __stdcall(this, &desc, &g_a7f890, 0).
    d3d7->vtbl->create_vertex_buffer(d3d7, &desc, &g_a7f890, 0);

    // 0x0051669D..0x005166B3: build the pool and return the constructor's result.
    return build_render_chunk_pool(render_chunk_pool_view(), 0x2EF4, 0x80, 0);
}

// -------------------------------------------------------------------------------------------------
// Compile-time checks: the layout that the natives index must match the declarations above.
// -------------------------------------------------------------------------------------------------
static_assert(sizeof(TerrainContourHashEntry) == 0x1C, "retail hash entry is 0x1C bytes");
static_assert(sizeof g_b04340 == 0x800 * 0x1C, "hash cache must span 0xE000 bytes");
static_assert(sizeof g_a4ef38 == 5 * 0x40 && sizeof g_a53740 == 5 * 0x40,
              "LOD save area and roots are five 0x40-byte slots");
static_assert(sizeof g_a9c4e0 == 0x180 * sizeof(TerrainContourCallback),
              "callback table is 0x180 dwords (0x600 bytes)");
static_assert(sizeof g_a9cae0 == 0x180 * sizeof(TerrainContourCallback),
              "callback table is 0x180 dwords (0x600 bytes)");
static_assert(sizeof g_b12be0 == 0x40, "octant table ends at 0x00B12C20");
static_assert(sizeof g_a7e068 == 4, "fog colour is one dword");
static_assert(sizeof g_bcab08 == 0x18, "camera origin is three doubles");
static_assert(sizeof g_bcaae0 == 0x28, "view transform is ten floats");
static_assert(0x00BCAAE0u + sizeof g_bcaae0 == 0x00BCAB08u,
              "view transform ends at the camera origin");
static_assert(0x00BCAB08u + sizeof g_bcab08 == 0x00BCAB20u,
              "camera origin ends at g_bcab20");
static_assert(0x00A7F878u + 0x18 == 0x00A7F890u, "pool descriptor ends at g_a7f890");
static_assert(sizeof g_a7f878 == 4 && sizeof g_a7f880 == 4 && sizeof g_a7f888 == 4 &&
                  sizeof g_a7f88c == 4,
              "pool fields are 32-bit dwords");
// The 0x00A7F878 descriptor offsets as constants: base 0, index list +4, used +8, capacity +0x0C,
// stride +0x10 and requested element size +0x14 (proved by the 0x004B0B1D store and the 32-bit
// engine declarations; g_a7f884 is +0x0C in terrain_out_of_terrain_renderch.hpp).
static_assert(0x00A7F87Cu - 0x00A7F878u == 0x04, "index list is descriptor +0x04");
static_assert(0x00A7F884u - 0x00A7F878u == 0x0C, "capacity is descriptor +0x0C");
// The reconstructed Direct3D structures used by the one vtable call at 0x0051669A.
static_assert(sizeof(Direct3DVertexBufferDesc) == 0x10, "vertex-buffer descriptor is 0x10 bytes");
static_assert(offsetof(Direct3DVertexBufferDesc, caps) == 0x04, "caps at +0x04");
static_assert(offsetof(Direct3DVertexBufferDesc, fvf) == 0x08, "FVF at +0x08");
static_assert(offsetof(Direct3DVertexBufferDesc, vertex_count) == 0x0C, "vertex count at +0x0C");
static_assert(offsetof(Direct3D7Vtbl, create_vertex_buffer) == 0x14,
              "retail calls vtable slot +0x14");

// -------------------------------------------------------------------------------------------------
// The three earlier unresolved names: current state (checked against the branch, not assumed).
//
// 0x00B16D10 is not a retail variable, so this unit defines nothing for it. The only reference is
//   0x004D1526 `mov dword [var_a0h], 0xb16d10`, which copies the *address constant* into the local
//   record cursor; no instruction reads or writes memory at 0x00B16D10 (r2 `axt` and a
//   `/x 10 6d b1 00` scan find only that immediate). terrain_quadtree.hpp no longer declares a
//   g_b16d10 symbol and terrain_quadtree.cpp now models the boundary directly:
//   `record_cursor = g_b16ce0.data() + 0x30` (line 355), with records written backwards at
//   `record = record_cursor - 0x30` (line 901). No second buffer is defined anywhere.
//
// 0x00B97B1C has no separate storage in the candidate and none is invented here. The reader
//   0x004D2226 `mov eax, dword [eax*4 + 0xb97b1c]` uses a base exactly four bytes below the
//   established 20-dword token stack at 0x00B97B20 (g_data_b97b20, defined once in
//   terrain_loaders.cpp), so element n aliases g_data_b97b20[n - 1]; terrain_quadtree.cpp performs
//   that bounded read (lines 905-914). The retail extent of that stack is still unresolved, but it
//   is owned and documented by terrain_loaders.cpp/.hpp, not by this unit, and the aliased object
//   has no independent extent to define.
//
// 0x00BA2058 is defined once, at terrain_loaders.cpp:32 `std::uint32_t g_ba2058 = 0;`, outside that
//   file's anonymous namespace (which opens at line 96), so it now has external linkage and
//   satisfies terrain_quadtree.hpp:116 and terrain_scene_state.cpp. Retail storage is a 4-byte
//   reset/read dword (0x004C5810 `mov dword [0xba2058], eax`; 0x004D2220 `mov edx, dword
//   [0xba2058]`). A second copy here would split the state, so this unit defines none.
//
// Debug_Error (0x004AF7B0) and DynObj_ParseConfigFile_6 (0x004C5810) are functions, not storage.
//   Both are now transcribed elsewhere: Debug_Error in engine/src/retail/core/retail_error.cpp:23,
//   and the twelve-instruction scene reset (which zeroes g_b97df4, g_b97d24, g_b97df8, g_ba2058 and
//   g_ba5060 and returns 0) in terrain_scene_state.cpp, called from terrain_quadtree.cpp:363.
//   A storage translation unit still must not fabricate them.
// -------------------------------------------------------------------------------------------------
} // namespace igi::retail::terrain
