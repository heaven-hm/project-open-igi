// IGI native provenance: Camera_ApplyPostProcess_12 - 0x004A80BF - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A80BF-Camera_ApplyPostProcess_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyPostProcess_12.h"
void Camera_ApplyPostProcess_12(unsigned int context, int* arg2, char arg3, int arg4, double* arg5, double* arg6) {
    *(context + 4) = 0;
    *(context + 8) = 0;
    *(context + 0xc) = 0;
    char entity_3 = arg3;

    if (entity_3 & 0x10)
    {
        unsigned int data_1 = context;
        arg3 = 0xc000008f;
        *(data_1 + 4) |= 1;
    }

    if (entity_3 & 2)
    {
        unsigned int data_2 = context;
        arg3 = 0xc0000093;
        *(data_2 + 4) |= 2;
    }

    if (1 & entity_3)
    {
        unsigned int data_3 = context;
        arg3 = 0xc0000091;
        *(data_3 + 4) |= 4;
    }

    if (entity_3 & 4)
    {
        unsigned int data_4 = context;
        arg3 = 0xc000008e;
        *(data_4 + 4) |= 8;
    }

    if (entity_3 & 8)
    {
        unsigned int data_5 = context;
        arg3 = 0xc0000090;
        *(data_5 + 4) |= 0x10;
    }

    unsigned int data_6 = context;
    *(data_6 + 8) = (~*arg2 & 1) << 4 | (*(data_6 + 8) & 0xffffffef);
    unsigned int data_7 = context;
    *(data_7 + 8) = (~*arg2 & 4) << 1 | (*(data_7 + 8) & 0xfffffff7);
    unsigned int data_8 = context;
    *(data_8 + 8) = (~*arg2 >> 1 & 4) | (*(data_8 + 8) & 0xfffffffb);
    unsigned int data_9 = context;
    *(data_9 + 8) = (~*arg2 >> 3 & 2) | (*(data_9 + 8) & 0xfffffffd);
    unsigned int data_10 = context;
    *(data_10 + 8) = (~*arg2 >> 5 & 1) | (*(data_10 + 8) & 0xfffffffe);
    char data_11 = Camera_UpdateViewCone_13();

    if (1 & data_11)
    {
        unsigned int entity_29 = context;
        *(entity_29 + 0xc) |= 0x10;
    }

    if (data_11 & 4)
    {
        unsigned int entity_30 = context;
        *(entity_30 + 0xc) |= 8;
    }

    if (data_11 & 8)
    {
        unsigned int entity_31 = context;
        *(entity_31 + 0xc) |= 4;
    }

    if (data_11 & 0x10)
    {
        unsigned int entity_32 = context;
        *(entity_32 + 0xc) |= 2;
    }

    if (data_11 & 0x20)
    {
        unsigned int data_12 = context;
        *(data_12 + 0xc) |= 1;
    }

    int data_14 = *arg2 & 0xc00;

    if (!data_14)
    {
        unsigned int data_17 = context;
        *data_17 &= 0xfffffffc;
    }
    else
    {
        unsigned int data_16;

        if (data_14 == 0x400)
        {
            data_16 = context;
            *data_16 = (*data_16 & 0xfffffffd) | 1;
        }
        else if (data_14 == 0x800)
        {
            data_16 = context;
            *data_16 = (*data_16 & 0xfffffffe) | 2;
        }
        else if (data_14 == 0xc00)
        {
            unsigned int data_15 = context;
            *data_15 |= 3;
        }
    }

    int data_19 = *arg2 & 0x300;
    unsigned int data_21;

    if (!data_19)
    {
        data_21 = context;
        *data_21 = (*data_21 & 0xffffffeb) | 8;
    }
    else if (data_19 == 0x200)
    {
        data_21 = context;
        *data_21 = (*data_21 & 0xffffffe7) | 4;
    }
    else if (data_19 == 0x300)
    {
        unsigned int data_20 = context;
        *data_20 &= 0xffffffe3;
    }
    unsigned int data_22 = context;
    *data_22 = (arg4 & 0xfff) << 5 | (*data_22 & 0xfffe001f);
    unsigned int data_23 = context;
    *(data_23 + 0x20) |= 1;
    unsigned int data_24 = context;
    *(data_24 + 0x20) = (*(data_24 + 0x20) & 0xffffffe3) | 2;
    *(context + 0x10) = *arg5;
    unsigned int data_27 = context;
    *(data_27 + 0x50) |= 1;
    unsigned int data_28 = context;
    *(data_28 + 0x50) = (*(data_28 + 0x50) & 0xffffffe3) | 2;
    *(context + 0x40) = *arg6;
    Camera_ResetMatrix_13();
    RaiseException(arg3, 0, 1, &context);
    /* no return */
}
