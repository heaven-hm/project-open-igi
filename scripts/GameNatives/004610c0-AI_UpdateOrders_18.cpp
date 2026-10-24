// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: AI_UpdateOrders_18
// Retail VA: 0x004610c0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(float arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5335c0 -> g_const_zero_double
//   g_5333ec -> g_const_zero
//   g_533400 -> g_const_pi
//   g_533938 -> g_const_0_06
//   g_533930 -> g_const_533930
//   g_533924 -> g_const_533924

// Readability note: Binary Ninja could not lift several x87 FPU-stack
// instructions here; their `unimplemented` markers, FPU condition flags and
// register temporaries are preserved verbatim rather than guessed. Constants
// read from retail memory: g_const_zero=0.0, g_const_pi=3.1415927,
// g_const_0_06=0.06, g_const_zero_double=0.0 (qword). g_const_533930 and
// g_const_533924 are unclassified FPU constants and keep their addresses.
// Readable signature: int32_t AI_UpdateOrders_18(float yaw);

int32_t AI_UpdateOrders_18(float yaw)

{
    float entity = yaw;
    void* view_matrix = *(entity + 0x76c);
    void* local_matrix = *(entity + 0x4ec);

    if (!view_matrix)
    {
        int16_t fpu_top = 0xffff;
        long double tan_value;
        bool fpu_c1;
        bool fpu_c2;
        tan_value = __fptan(0.78539818525314331);

        if (!fpu_c2)
            fpu_top = 0xfffe;

🚫🚫🚫        /* unimplemented  {fstp st0, st0} */
        /* unimplemented  {fstp st0, st0} */
⋯        long double cmp_value_a = *(local_matrix + 0x1e4);
        /* unimplemented  {fcomp st0, dword [local_matrix+0x1e4]} f- cmp_value_a */ - cmp_value_a;
        bool is_less_a = /* bool is_less_a =
            unimplemented  {fcomp st0, dword [local_matrix+0x1e4]} f< cmp_value_a */ < cmp_value_a;
        bool is_unordered_a = FCMP_UO(/* bool is_unordered_a =
            is_unordered.t(unimplemented  {fcomp st0, dword [local_matrix+0x1e4]}, cmp_value_a) */, cmp_value_a);
        bool is_equal_a = /* bool is_equal_a =
            unimplemented  {fcomp st0, dword [local_matrix+0x1e4]} f== cmp_value_a */ == cmp_value_a;
        /* unimplemented  {fcomp st0, dword [local_matrix+0x1e4]} */
🚫🚫        int16_t fpu_top_next = fpu_top + 1;
        /* unimplemented  {fld st0, dword [entity+0x1b0]} */

        if (*(&((is_less_a ? 1 : 0) << 8 | (is_unordered_a ? 1 : 0) << 0xa | (is_equal_a ? 1 : 0) << 0xe
            | (fpu_top_next & 7) << 0xb) + 1) & 0x40)
        {
🚫🚫            /* unimplemented  {fadd dword [entity+0x110]} */
🚫🚫            /* unimplemented  {fmul st0, qword [&g_const_zero_double]} */
        }

        if (*(entity + 0x510))
        {
🚫🚫🚫            /* unimplemented  {fstp st0, st0} */
            /* unimplemented  {fstp st0, st0} */
🚫🚫            /* unimplemented  {fld st0, dword [&g_const_zero]} */
        }

🚫🚫        /* unimplemented  {fld st0, st0} */
🚫🚫        /* unimplemented  {fmul st0, dword [&g_const_pi]} */
        *(entity + 0x4fc) = 0;
        *(entity + 0x4f8) = 0;
        *(entity + 0x504) = 0;
        *(entity + 0x500) = 0;
🚫🚫        /* unimplemented  {fchs } */
🚫🚫        /* unimplemented  {fmul st0, dword [local_matrix+0x1e4]} */
🚫🚫🚫        yaw = /* yaw = unimplemented  {fstp dword [esp+0x3c], st0} */;
        /* unimplemented  {fstp dword [esp+0x3c], st0} */
🚫🚫🚫🚫🚫        long double cmp_value_b = 0f;
        /* unimplemented  {fcom st0, dword [&g_const_zero]} f- cmp_value_b */ - cmp_value_b;
        bool is_less_b = /* bool is_less_b =
            unimplemented  {fcom st0, dword [&g_const_zero]} f< cmp_value_b */ < cmp_value_b;
        bool is_unordered_b = FCMP_UO(/* bool is_unordered_b =
            is_unordered.t(unimplemented  {fcom st0, dword [&g_const_zero]}, cmp_value_b) */, cmp_value_b);
        bool is_equal_b = /* bool is_equal_b =
            unimplemented  {fcom st0, dword [&g_const_zero]} f== cmp_value_b */ == cmp_value_b;
🚫🚫        /* unimplemented  {fld st0, st0} */

        if (*(&((is_less_b ? 1 : 0) << 8 | (is_unordered_b ? 1 : 0) << 0xa | (is_equal_b ? 1 : 0) << 0xe
                | ((fpu_top_next - 1) & 7) << 0xb) + 1) & 1)
🚫🚫            /* unimplemented  {fchs } */

⋯        long double cmp_value_c = 0.0599999987f;
        /* unimplemented  {fcomp st0, dword [&g_const_0_06]} f- cmp_value_c */ - cmp_value_c;
        bool is_less_c = /* bool is_less_c =
            unimplemented  {fcomp st0, dword [&g_const_0_06]} f< cmp_value_c */ < cmp_value_c;
        bool is_unordered_c = FCMP_UO(/* bool is_unordered_c =
            is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_0_06]}, cmp_value_c) */, cmp_value_c);
        bool is_equal_c = /* bool is_equal_c =
            unimplemented  {fcomp st0, dword [&g_const_0_06]} f== cmp_value_c */ == cmp_value_c;
        /* unimplemented  {fcomp st0, dword [&g_const_0_06]} */

        if (!(*(&((is_less_c ? 1 : 0) << 8 | (is_unordered_c ? 1 : 0) << 0xa | (is_equal_c ? 1 : 0) << 0xe
            | (fpu_top_next & 7) << 0xb) + 1) & 1))
        {
🚫🚫🚫            /* unimplemented  {fstp st0, st0} */
            /* unimplemented  {fstp st0, st0} */
        }
        else
        {
🚫🚫🚫🚫🚫            long double cmp_value_d = 0f;
            /* unimplemented  {fcom st0, dword [&g_const_zero]} f- cmp_value_d */ - cmp_value_d;
            bool is_less_d = /* bool is_less_d =
                unimplemented  {fcom st0, dword [&g_const_zero]} f< cmp_value_d */ < cmp_value_d;
            bool is_unordered_d = FCMP_UO(/* bool is_unordered_d =
                is_unordered.t(unimplemented  {fcom st0, dword [&g_const_zero]}, cmp_value_d) */, cmp_value_d);
            bool is_equal_d = /* bool is_equal_d =
                unimplemented  {fcom st0, dword [&g_const_zero]} f== cmp_value_d */ == cmp_value_d;

            if (*(&((is_less_d ? 1 : 0) << 8 | (is_unordered_d ? 1 : 0) << 0xa | (is_equal_d ? 1 : 0) << 0xe
                    | (fpu_top_next & 7) << 0xb) + 1) & 1)
🚫🚫                /* unimplemented  {fchs } */

🚫🚫            /* unimplemented  {fmul st0, qword [&g_const_533930]} */
🚫🚫            /* unimplemented  {fsubr st0, qword [&g_const_533924[4]]} */
🚫🚫            /* unimplemented  {fdivr st0, dword [esp+0x3c]} */
🚫🚫🚫            yaw = /* yaw = unimplemented  {fstp dword [esp+0x3c], st0} */;
            /* unimplemented  {fstp dword [esp+0x3c], st0} */
        }

🚫🚫        /* unimplemented  {fld st0, dword [esp+0x3c]} */
🚫🚫        /* unimplemented  {fadd dword [entity+0x764]} */
🚫🚫        /* unimplemented  {fadd dword [entity+0x50c]} */
🚫🚫        *(entity + 0x50c) = /* *(entity i+ 0x50c) = unimplemented  {fst dword [entity+0x50c], st0} */;
🚫🚫        /* unimplemented  {fadd dword [local_matrix+0x134]} */
🚫🚫🚫        *(entity + 0x508) = /* *(entity i+ 0x508) = unimplemented  {fstp dword [entity+0x508], st0} */;
        /* unimplemented  {fstp dword [entity+0x508], st0} */
    }
    else
    {
        long double dot_00 = *(entity + 0x7a0) * *(view_matrix + 0x78) + *(view_matrix + 0x74) * *(entity + 0x794)
            + *(entity + 0x788) * *(view_matrix + 0x70);
        float rot_00 = dot_00;
        long double dot_10 = *(entity + 0x7a4) * *(view_matrix + 0x78) + *(view_matrix + 0x74) * *(entity + 0x798)
            + *(entity + 0x78c) * *(view_matrix + 0x70);
        float rot_10 = dot_10;
        long double dot_20 = *(entity + 0x7a8) * *(view_matrix + 0x78) + *(view_matrix + 0x74) * *(entity + 0x79c)
            + *(entity + 0x790) * *(view_matrix + 0x70);
        float rot_20 = dot_20;
        long double dot_01 = *(entity + 0x788) * *(view_matrix + 0x7c) + *(entity + 0x7a0) * *(view_matrix + 0x84)
            + *(entity + 0x794) * *(view_matrix + 0x80);
        float rot_01 = dot_01;
        long double dot_11 = *(entity + 0x78c) * *(view_matrix + 0x7c) + *(entity + 0x7a4) * *(view_matrix + 0x84)
            + *(entity + 0x798) * *(view_matrix + 0x80);
        float rot_11 = dot_11;
        long double dot_21 = *(entity + 0x790) * *(view_matrix + 0x7c) + *(entity + 0x7a8) * *(view_matrix + 0x84)
            + *(entity + 0x79c) * *(view_matrix + 0x80);
        float rot_21 = dot_21;
        long double dot_02 = *(entity + 0x7a0) * *(view_matrix + 0x90) + *(entity + 0x794) * *(view_matrix + 0x8c)
            + *(entity + 0x788) * *(view_matrix + 0x88);
        float rot_02 = dot_02;
        long double dot_12 = *(entity + 0x7a4) * *(view_matrix + 0x90) + *(entity + 0x798) * *(view_matrix + 0x8c)
            + *(entity + 0x78c) * *(view_matrix + 0x88);
        float rot_12 = dot_12;
        long double dot_22 = *(entity + 0x7a8) * *(view_matrix + 0x90) + *(entity + 0x79c) * *(view_matrix + 0x8c)
            + *(entity + 0x790) * *(view_matrix + 0x88);
        float rot_22 = dot_22;
        int32_t matrix_slot_9 = *(entity + 0x7ac) + *(view_matrix + 0x94) + 1;
        __builtin_memcpy(&rot_00, &rot_00, 0x28);
        float euler_pitch;
        float* pitch_ptr = &euler_pitch;
        float euler_roll;
        float* roll_ptr = &euler_roll;
        float* yaw_ptr = &yaw;
        float* matrix_ptr = &rot_00;
        Mat33_ToEulerAngles(matrix_ptr, yaw_ptr, roll_ptr, pitch_ptr);
        float out_yaw = yaw;
        float out_roll = euler_roll;
        long double roll_offset = euler_pitch;
        *(entity + 0x4fc) = out_yaw;
        *(entity + 0x4f8) = out_yaw;
        float out_pitch = euler_pitch;
        *(entity + 0x504) = out_roll;
        *(entity + 0x500) = out_roll;
        *(entity + 0x50c) = out_pitch;
        *(entity + 0x508) = roll_offset + *(local_matrix + 0x134);
    }

    return Real32x9_Create(entity + 0x78, *(entity + 0x4f8), *(entity + 0x500), *(entity + 0x508));
}
