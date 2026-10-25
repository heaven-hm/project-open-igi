// Native: Task_Handler_449E50 (placeholder: the first event handler of the class registered by Task_DefineGlassShatter_449D30); retail VA 0x00449E50-0x0044A19A.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 843 bytes (0x34B), 12 blocks, 17 edges,
//   261 instructions, measured with af + afbj over the exact image. Per-block instruction counts in address order are
//   21, 1, 13, 4, 4, 3, 196, 6, 2, 4, 3, 4 and sum to the 261 the instruction count reports; the matching per-block byte counts
//   93, 2, 50, 16, 14, 11, 604, 21, 2, 15, 9, 6 sum to 843. The extent ends at the `ret` at 0x0044A19A and the next function starts at
//   0x0044A1A0 (Task_Handler_44A1A0).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt, walked instruction by instruction across the whole
//   extent. Calls: exactly one, `call 0x4015F0` at 0x0044A18D (retail 1 / port 1), reached only when the counter at context+0xBC reaches
//   zero. Branches: jle x2, jl x2, je x1, jne x1. Back edges: exactly two - 0x00449EDF -> 0x00449EAF (the 13-instruction record loop) and
//   0x0044A175 -> 0x00449EFF (the per-element loop tail). The 196-instruction block at 0x00449F0A-0x0044A165 holds no branch of any kind:
//   it is straight-line repetition, not a loop, and its single successor is 0x0044A166. Constants: 0x005338C4 is read three times by
//   `fmuls` (bytes C2 32 67 4F, the 32-bit float -44.6008873) and 0x00533678 once (bytes 3D 08 88 89, the 32-bit float 0.03333333507);
//   both are float operands of x87 chains, so both are written below as the exact hex literals of those floats (-0x1.64CE9Ep+5L,
//   0x1.111112p-5L) rather than as decimal literals, for the reason recorded in this tree's float-literal finding. 0x00533678 is NOT the
//   value fixed in core/native_449d30.cpp: that one is 0x005338BC, bytes 3A 91 A2 B4 = 0.001111111138, a different address and value.
// Behavior/CFG: the event 1 handler of the "GlassShatter" class (Task_DefineGlassShatter_449D30 links it as that class's first event at
//   0x00449E2F; the second event is 0x0044A1A0), ABI void(void* context). It (1) scales context+0xA8/+0xAC/+0xB0 by
//   -44.6008873 and context+0xBC (a signed int, `fildl`) by 0.03333333507 into 0x0057C248; (2) writes that shared value into +0x30/+0x34/+0x38
//   of every 0x50-byte record of the object at context+0x2B040 (record base at object+0x6C, count at object+0x9C); (3) walks context+0xB8
//   elements, each a 0x60-byte source element at context+0xD8 and a 0x50-byte destination element at context+0x17800, and for every element
//   whose +0x44 flag is non-zero transforms three vertex differences and the destination's three floats by the 3x3 float matrix selected by
//   the source's +0x30 index into the 30-record array 0x0057C280 (stride 0x28 = ten floats), re-bases the source's three vertices on the
//   origin offset at +0x34/+0x38/+0x3C, and advances that offset by the three scaled values from step 1; (4) decrements context+0xBC and
//   calls the task destructor 0x004015F0 when it reaches zero. Extended precision: every flds/fildl/fmuls/fadds/fsubs operand is a 32-bit
//   float or integer widened exactly, results are rounded only by their 32-bit store, and the first vertex's third component stays in
//   extended precision from 0x00449F9E until its single store at 0x0044A14, so it is kept as a long double here. The association order of
//   every faddp chain is reproduced as the retail bytes have it.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the full 261-instruction listing and afbj (843 B / 12 blocks / 17 edges);
//   runtime UNOBSERVED. Recorded limits: (a) 0x00449E50 is an address-suffixed placeholder; the retail symbol is not established, only its
//   role as the event 1 handler linked by 0x00449D30. (b) The element layouts are not established beyond the field offsets this body reads
//   and writes; they are handled as byte offsets, never as invented structs. (c) The object at context+0x2B040 and its +0x6C/+0x9C fields are
//   read through the offsets the listing shows; their types are not established here. (d) Retail stores the first three floats' sums twice -
//   once to a frame slot and once to the element - and reuses the incoming argument slot at +0x60 for one copy; the port keeps one local per
//   sum, which is the same value. (e) Retail loads the shared value 0x0057C248 from memory before each of the three record stores and reloads
//   the object pointer and count on every record-loop iteration; the port issues those loads as separate expressions too, but a compiler may
//   merge them, which cannot change the values because nothing in this body writes them. (f) Retail's three stores into the destination's
//   three floats are interleaved with the last computation; the port computes all three before storing, which is the same result because the
//   three slots do not alias. (g) The two `push %ebx` / `push %ebp` at 0x00449EF1/0x00449EF2 and their pops are register saves the compiler
//   places itself, and the destroy call is a plain call here as in retail.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern "C" void QTask_DestroyTask_4015F0(void* task); // 0x004015F0: the task destructor (core/native_4015f0.cpp)

// 0x0057C248: the shared life fraction this handler writes (zero fill, no other writer in this body).
float g_57c248 = 0.0F;

// 0x0057C280: the 30 rotation records of ten floats each, built by Task_DefineGlassShatter_449D30 (core/native_449d30.cpp).
extern float g_57c280[30 * 10];

void NATIVE_DECL Task_Handler_449E50(void* const context)
{
    char* const ctx = static_cast<char*>(context);

    // 0x00449E5B-0x00449E97: three scaled offsets and the shared life fraction; each is computed in extended precision and rounded by its
    // 32-bit store (fstps), as flds/fmuls/fstps do. The -44.6008873 operand is the 32-bit float at 0x005338C4 and the 0.03333333507 operand
    // the 32-bit float at 0x00533678, written as their exact hex values.
    float const offset_a = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(ctx + 0xA8)) * -0x1.64CE9Ep+5L);
    float const offset_b = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(ctx + 0xAC)) * -0x1.64CE9Ep+5L);
    float const offset_c = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(ctx + 0xB0)) * -0x1.64CE9Ep+5L);
    ::g_57c248 = static_cast<float>(static_cast<long double>(*reinterpret_cast<std::int32_t*>(ctx + 0xBC)) * 0x1.111112p-5L);

    // 0x00449E9D-0x00449EDF: every 0x50-byte record of the object at context+0x2B040 receives the shared value at +0x30/+0x34/+0x38.
    // The object pointer (0x00449ED3) and its +0x9C count (0x00449ED9) are re-read on each iteration, exactly as the retail loop does.
    if (*reinterpret_cast<std::int32_t*>(*reinterpret_cast<char**>(ctx + 0x2B040) + 0x9C) > 0)
    {
        std::int32_t index = 0;  // ecx, 0x00449E58
        std::int32_t offset = 0; // edx, 0x00449EAD
        do
        {
            char* const record = *reinterpret_cast<char**>(*reinterpret_cast<char**>(ctx + 0x2B040) + 0x6C) + offset;
            ++index;          // 0x00449EBA
            offset += 0x50;   // 0x00449EBB
            *reinterpret_cast<float*>(record + 0x30) = ::g_57c248; // 0x00449EBE
            *reinterpret_cast<float*>(record + 0x34) = ::g_57c248; // 0x00449EC7
            *reinterpret_cast<float*>(record + 0x38) = ::g_57c248; // 0x00449ED0
        } while (index < *reinterpret_cast<std::int32_t*>(*reinterpret_cast<char**>(ctx + 0x2B040) + 0x9C));
    }

    // 0x00449EE1-0x00449EEB: the per-element pass runs only when context+0xB8 is positive.
    if (*reinterpret_cast<std::int32_t*>(ctx + 0xB8) > 0)
    {
        char* src = ctx + 0xD8;    // eax, 0x00449EF3: the 0x60-byte source element
        char* dst = ctx + 0x17800; // edx, 0x00449EF9: the destination element, whose three floats sit at -4, 0 and +4
        std::int32_t element = 0;  // edi, 0x00449EE7
        do
        {
            // 0x00449EFF-0x00449F04: an element whose +0x44 flag is zero is skipped entirely.
            if (*reinterpret_cast<std::int32_t*>(src + 0x44) != 0)
            {
                // 0x00449F0A-0x00449F5E: the three vertex differences. The first three stay in extended precision (the x87 stack's
                // st(2)/st(1)/st(0) at 0x00449F62); the other six are rounded to 32-bit floats by their stores.
                long double const v0 = static_cast<long double>(*reinterpret_cast<float*>(src + 0x0C)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x00)); // st(2)
                long double const v1 = static_cast<long double>(*reinterpret_cast<float*>(src + 0x10)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x04)); // st(1)
                long double const v2 = static_cast<long double>(*reinterpret_cast<float*>(src + 0x14)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x08)); // st(0)
                // 0x00449F20-0x00449F30: the matrix is the +0x30 index's ten-float record in 0x0057C280.
                float const* const mat = ::g_57c280 + (*reinterpret_cast<std::int32_t*>(src + 0x30)) * 10;
                float const d18 = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x18)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x00))); // 0x00449F26
                float const d1c = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x1C)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x04))); // 0x00449F37
                float const d20 = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x20)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x08))); // 0x00449F41
                float const d24 = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x24)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x00))); // 0x00449F4A
                float const d28 = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x28)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x04))); // 0x00449F54
                float const d2c = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x2C)) - static_cast<long double>(*reinterpret_cast<float*>(src + 0x08))); // 0x00449F5E

                // 0x00449F62-0x00449F8B: the first vertex, three row dot products; the association is the retail faddp chain's.
                float const x1 = static_cast<float>((static_cast<long double>(v2) * static_cast<long double>(mat[2]) + static_cast<long double>(v1) * static_cast<long double>(mat[1])) + static_cast<long double>(v0) * static_cast<long double>(mat[0])); // 0x00449F74
                float const y1 = static_cast<float>((static_cast<long double>(v0) * static_cast<long double>(mat[3]) + static_cast<long double>(v2) * static_cast<long double>(mat[5])) + static_cast<long double>(v1) * static_cast<long double>(mat[4])); // 0x00449F8B
                // 0x00449F8F-0x00449F9E: the third component is left on the x87 stack in extended precision and stored only at 0x0044A14.
                long double const z1 = (static_cast<long double>(v0) * static_cast<long double>(mat[6]) + static_cast<long double>(v2) * static_cast<long double>(mat[8])) + static_cast<long double>(v1) * static_cast<long double>(mat[7]);

                // 0x00449FA0-0x00449FF2: the second vertex difference, same three rows.
                float const x2 = static_cast<float>((static_cast<long double>(d20) * static_cast<long double>(mat[2]) + static_cast<long double>(d1c) * static_cast<long double>(mat[1])) + static_cast<long double>(d18) * static_cast<long double>(mat[0])); // 0x00449FB8
                float const y2 = static_cast<float>((static_cast<long double>(d18) * static_cast<long double>(mat[3]) + static_cast<long double>(d20) * static_cast<long double>(mat[5])) + static_cast<long double>(d1c) * static_cast<long double>(mat[4])); // 0x00449FD5
                float const z2 = static_cast<float>((static_cast<long double>(d18) * static_cast<long double>(mat[6]) + static_cast<long double>(d20) * static_cast<long double>(mat[8])) + static_cast<long double>(d1c) * static_cast<long double>(mat[7])); // 0x00449FF2

                // 0x00449FF6-0x0044A04B: the third vertex difference, same three rows.
                float const x3 = static_cast<float>((static_cast<long double>(d2c) * static_cast<long double>(mat[2]) + static_cast<long double>(d28) * static_cast<long double>(mat[1])) + static_cast<long double>(d24) * static_cast<long double>(mat[0])); // 0x0044A011
                float const y3 = static_cast<float>((static_cast<long double>(d24) * static_cast<long double>(mat[3]) + static_cast<long double>(d2c) * static_cast<long double>(mat[5])) + static_cast<long double>(d28) * static_cast<long double>(mat[4])); // 0x0044A02E
                float const z3 = static_cast<float>((static_cast<long double>(d24) * static_cast<long double>(mat[6]) + static_cast<long double>(d2c) * static_cast<long double>(mat[8])) + static_cast<long double>(d28) * static_cast<long double>(mat[7])); // 0x0044A04B

                // 0x0044A04F-0x0044A0AA: the destination element's own three floats, transformed in place. The three source floats are
                // read before any of the three stores, and the three slots do not alias, so retail's interleaved stores give this result.
                float* const origin = reinterpret_cast<float*>(dst);
                float const ox = static_cast<float>((static_cast<long double>(origin[1]) * static_cast<long double>(mat[2]) + static_cast<long double>(origin[-1]) * static_cast<long double>(mat[0])) + static_cast<long double>(origin[0]) * static_cast<long double>(mat[1]));  // 0x0044A063
                float const oy = static_cast<float>((static_cast<long double>(origin[0]) * static_cast<long double>(mat[4]) + static_cast<long double>(origin[1]) * static_cast<long double>(mat[5])) + static_cast<long double>(origin[-1]) * static_cast<long double>(mat[3])); // 0x0044A07C
                float const oz = static_cast<float>((static_cast<long double>(origin[0]) * static_cast<long double>(mat[7]) + static_cast<long double>(origin[-1]) * static_cast<long double>(mat[6])) + static_cast<long double>(origin[1]) * static_cast<long double>(mat[8])); // 0x0044A0A2
                origin[-1] = ox; // 0x0044A097
                origin[0] = oy;  // 0x0044A0A0
                origin[1] = oz;  // 0x0044A0AA

                // 0x0044A0AD-0x0044A0E2: the element's three floats at -0x18 are copied to -0x0C, and its origin offset at
                // +0x34/+0x38/+0x3C is added into +0x00/+0x04/+0x08. Retail keeps a second copy of each sum in a frame slot and reads the
                // sums back from there at 0x0044A0E5-0x0044A145; one local per sum carries the same value.
                *reinterpret_cast<std::uint32_t*>(src - 0x0C) = *reinterpret_cast<std::uint32_t*>(src - 0x18); // 0x0044A097..0x0044A0A6
                *reinterpret_cast<std::uint32_t*>(src - 0x08) = *reinterpret_cast<std::uint32_t*>(src - 0x14);
                *reinterpret_cast<std::uint32_t*>(src - 0x04) = *reinterpret_cast<std::uint32_t*>(src - 0x10);
                float const sum_x = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x34)) + static_cast<long double>(*reinterpret_cast<float*>(src + 0x00))); // 0x0044A0B0-0x0044A0C0
                *reinterpret_cast<float*>(src + 0x00) = sum_x; // 0x0044A0C0
                float const sum_y = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x38)) + static_cast<long double>(*reinterpret_cast<float*>(src + 0x04))); // 0x0044A0C2-0x0044A0D5
                *reinterpret_cast<float*>(src + 0x04) = sum_y; // 0x0044A0D5
                float const sum_z = static_cast<float>(static_cast<long double>(*reinterpret_cast<float*>(src + 0x3C)) + static_cast<long double>(*reinterpret_cast<float*>(src + 0x08))); // 0x0044A0D8-0x0044A0E2
                *reinterpret_cast<float*>(src + 0x08) = sum_z; // 0x0044A0E2

                // 0x0044A0E5-0x0044A145: the three vertices are re-based on the sums. The first vertex's third component adds the
                // extended-precision z1 that has stayed on the x87 stack since 0x00449F9E, so it is added before that sum is rounded.
                *reinterpret_cast<float*>(src + 0x0C) = static_cast<float>(static_cast<long double>(sum_x) + static_cast<long double>(x1)); // 0x0044A0ED
                *reinterpret_cast<float*>(src + 0x10) = static_cast<float>(static_cast<long double>(sum_y) + static_cast<long double>(y1)); // 0x0044A0F8
                *reinterpret_cast<float*>(src + 0x14) = static_cast<float>(static_cast<long double>(sum_z) + z1);                            // 0x0044A101
                *reinterpret_cast<float*>(src + 0x18) = static_cast<float>(static_cast<long double>(sum_x) + static_cast<long double>(x2)); // 0x0044A10E
                *reinterpret_cast<float*>(src + 0x1C) = static_cast<float>(static_cast<long double>(sum_y) + static_cast<long double>(y2)); // 0x0044A119
                *reinterpret_cast<float*>(src + 0x20) = static_cast<float>(static_cast<long double>(sum_z) + static_cast<long double>(z2)); // 0x0044A124
                *reinterpret_cast<float*>(src + 0x24) = static_cast<float>(static_cast<long double>(sum_x) + static_cast<long double>(x3)); // 0x0044A12F
                *reinterpret_cast<float*>(src + 0x28) = static_cast<float>(static_cast<long double>(sum_y) + static_cast<long double>(y3)); // 0x0044A13A
                *reinterpret_cast<float*>(src + 0x2C) = static_cast<float>(static_cast<long double>(sum_z) + static_cast<long double>(z3)); // 0x0044A145

                // 0x0044A148-0x0044A163: the origin offset advances by the three scaled values from 0x00449E5B-0x00449E87.
                *reinterpret_cast<float*>(src + 0x34) = static_cast<float>(static_cast<long double>(offset_a) + static_cast<long double>(*reinterpret_cast<float*>(src + 0x34))); // 0x0044A14F
                *reinterpret_cast<float*>(src + 0x38) = static_cast<float>(static_cast<long double>(offset_b) + static_cast<long double>(*reinterpret_cast<float*>(src + 0x38))); // 0x0044A159
                *reinterpret_cast<float*>(src + 0x3C) = static_cast<float>(static_cast<long double>(offset_c) + static_cast<long double>(*reinterpret_cast<float*>(src + 0x3C))); // 0x0044A163
            }

            // 0x0044A166-0x0044A175: the element index and both element pointers advance by their strides; the count is re-read.
            ++element;       // 0x0044A16C
            src += 0x60;     // 0x0044A16D
            dst += 0x50;     // 0x0044A170
        } while (element < *reinterpret_cast<std::int32_t*>(ctx + 0xB8));
    }

    // 0x0044A17D-0x0044A194: the life counter at context+0xBC is decremented and the task destroys itself when it reaches zero.
    std::int32_t const remaining = *reinterpret_cast<std::int32_t*>(ctx + 0xBC) - 1;
    *reinterpret_cast<std::int32_t*>(ctx + 0xBC) = remaining;
    if (remaining == 0)
    {
        QTask_DestroyTask_4015F0(ctx); // 0x0044A18D
    }
}

} // extern "C"

#undef NATIVE_DECL
