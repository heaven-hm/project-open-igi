#include "igi/retail/mem/heap_list.hpp"

#include <cstdint>

namespace igi::retail::mem {

// Native: HeapList_Insert; retail VA 0x004AF8F0; __cdecl (container, node).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004AF8F0-0x004AF910, 32 bytes; CLEAN/camera/004AF8F0-Camera_SetFieldOfView_33.c.
// Behavior/CFG: inserts `node` at the tail of an intrusive doubly-linked list whose header is at `container` (`+0x00` head word, `+0x04` terminator address stored in every new node's next, `+0x08` tail pointer-to-pointer): writes `node[1] = *(container + 8)`, `*node = container + 4`, `**(container + 8) = node`, `*(container + 8) = node`, and returns `container`. radare2 1 block/0 edges; Binary Ninja 1 block; 100.0% structural agreement. 32-bit target only: the 64-bit host branch was scaffolding and is removed, since build.sh cross-compiles i686-w64-mingw32-g++.
// Status/limits: VERIFIED; high for exact pointer splice sequence.
void* HeapList_Insert(void* container, void** node)
{
    auto* const header = static_cast<std::uint8_t*>(container);
    void** const tail = reinterpret_cast<void**>(header + 0x08);
    node[1] = *tail;
    *node = header + 0x04;
    **reinterpret_cast<void***>(header + 0x08) = node;
    *tail = node;
    return container;
}

} // namespace igi::retail::mem

