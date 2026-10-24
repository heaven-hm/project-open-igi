// Retail runtime observation: DynObj_LoadModelTextures - 0x004CF790 - 4 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Package texture traversal observed; texture Resource_Load requests originate at return address 0x004B24CE.
// IGI native provenance: DynObj_LoadModelTextures - 0x004CF790 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CF790-DynObj_LoadModelTextures.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CF790-DynObj_LoadModelTextures.hpp"
int* DynObj_LoadModelTextures(void* context)

{
    void* context_val = context;
    int flagP = 0;
    int* data_1 = Ilff_FindFormChunk(*(context_val + 8), 0x54455846, nullptr);
    int* source_val = data_1;
    int __saved_count_2 = *source_val;
    *(context_val + 0x40) = __saved_count_2;

    if (__saved_count_2 <= 0)
    {
        *(context_val + 0x34) = 0;
        *(context_val + 0x38) = 0;
    }
    else
    {
        *(context_val + 0x34) = Mem_Alloc(__saved_count_2 * 0x24, 4);
        data_1 = Mem_Alloc(__saved_count_2 * 0xc, 4);
        void* source_1 = &source_val[1];
        *(context_val + 0x38) = data_1;

        if (__saved_count_2 > 0)
        {
            context = nullptr;
            int saved_ctx = __saved_count_2;
            int __saved_count_1;
            bool flag_3_1;

            do
            {
                void* item_18_1 = source_1;
                *(*(context_val + 0x38) + flagP + 4) = 0;
                sprintf(0xa4ec6c, "LOCAL:textures/%s.tex");
                Sys_SIsNotAValidTextureF(*(context_val + 0x34) + context, flagP + *(context_val + 0x38), 0xa4ec6c);
                void* dest_1 = source_1;
                int i = 0xffffffff;

                while (i)
                {
                    bool flag_2_1 = 0 != *dest_1;
                    dest_1 += 1;
                    i -= 1;

                    if (!flag_2_1)
                        break;
                }

                flagP += 0xc;
                source_1 = source_1 + ~i - 1 + 1;
                __saved_count_1 = saved_ctx - 1;
                flag_3_1 = saved_ctx != 1;
                context += 0x24;
                saved_ctx = __saved_count_1;
            } while (flag_3_1);
            return __saved_count_1;
        }
    }

    return data_1;
}
