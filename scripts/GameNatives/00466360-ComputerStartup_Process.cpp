// IGI native provenance: ComputerStartup_Process - 0x00466360 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00466360-ComputerStartup_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00466360-ComputerStartup_Process.hpp"
int ComputerStartup_Process(void* context)

{
    Heap_FreeEntry(*(context + 0x8c));
    int* data_1 = *(context + 0x88);
    return (*(*data_1 + 8))(data_1);
}
