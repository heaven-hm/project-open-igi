// IGI native provenance: GOStart - 0x00403660 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00403660-GOStart.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GOStart.h"
int GOStart(int context, float arg2 )

// Sig: unknown GOStart(unknown param1, unknown* out_buffer)
// Note: Registered at 0x403006 via registrar 0x4B8890 (push 0x403660 + name
// string 'GOStart'). Init counterpart of cataloged GOPlayer : zeroes
// global 0xBC2380 and stores fcn.004B8A20 result to 0xBC2384; GOPlayer
// reads/increments 0xBC2380. Copies 6 dwords (24 bytes) into second args
// buffer, embedding pointer 0x567C74.  Verified: r2 forced-analysis confirmed
// exact start (size 75); P3 byte-level name<->address binding at registrar
// callsite 0x403006 via 0x4B8890; P6 SHA-256(first 32 code
// bytes)=6a2c07b5ab39d7b6373c45d2561127acfc24cfe9792a2d232bb848aef92d9a6a;
// re-agent reverser/checker loop ran on this native (C decompilation pending
// ARM64 native port).
{
    int item_24 = 0;
    g_bc2380 = 0;
    g_bc2384 = Script_GetArg(arg2, item_24);
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
