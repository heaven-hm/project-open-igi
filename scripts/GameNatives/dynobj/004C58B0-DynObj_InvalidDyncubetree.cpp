// IGI native provenance: DynObj_InvalidDyncubetree - 0x004C58B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C58B0-DynObj_InvalidDyncubetree.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C58B0-DynObj_InvalidDyncubetree.hpp"
int DynObj_InvalidDyncubetree(int context)

{
    if (context > 5)
    {
        Error_Show("Invalid dyncubetree");

        while (true)
            /* nop */
    }
    else
        switch (context)
        {
            case 0:
            {
                return 1;
                break;
            }
            case 1:
            case 2:
            case 3:
            {
                return 2;
                break;
            }
            case 4:
            {
                return 4;
                break;
            }
            case 5:
            {
                return 8;
                break;
            }
        }
}
