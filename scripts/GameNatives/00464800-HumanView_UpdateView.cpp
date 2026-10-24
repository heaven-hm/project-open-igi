// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: HumanView_UpdateView
// Retail VA: 0x00464800
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: float(float arg1, float arg2) __location("eax")
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5335c0 -> g_const_zero_double
//   g_533400 -> g_const_pi
//   g_5333ec -> g_const_zero
//   g_533938 -> g_const_0_06
//   g_533930 -> g_const_533930
//   g_533924 -> g_const_533924
//   g_533468 -> g_const_half_pi
//   g_5339d0 -> g_const_neg_half_pi
//   g_5339cc -> g_const_0_96
//   g_5339c8 -> g_const_1_04
//   g_5339c0 -> g_const_2_0
//   g_5337f4 -> g_const_0_0524

// Readability note: Binary Ninja could not lift several x87 FPU-stack
// instructions in this 571-line function; their `unimplemented` markers, FPU
// condition flags and register temporaries are preserved verbatim rather than
// guessed. Constants read from retail memory: g_const_zero=0.0,
// g_const_pi=3.1415927, g_const_half_pi=1.5707964, g_const_neg_half_pi=-1.5707964,
// g_const_zero_double=0.0, g_const_0_06=0.06, g_const_0_96=0.96, g_const_1_04=1.0417,
// g_const_2_0=2.0, g_const_0_0524=0.0524. g_const_533930 / g_const_533924 are
// unclassified FPU constants and keep their addresses.
// Readable signature: float HumanView_UpdateView(float yaw, float pitch);

float HumanView_UpdateView(float yaw, float pitch)

{
    int16_t top = 0xffff;
    long double st0;
    bool c1;
    bool c2;
    st0 = __fptan(0.78539818525314331);

    if (!c2)
        top = 0xfffe;

    float view = yaw;
🚫🚫🚫    /* unimplemented  {fstp st0, st0} */
    /* unimplemented  {fstp st0, st0} */
🚫🚫🚫    yaw = /* yaw = unimplemented  {fstp dword [esp+0x14], st0} */;
    /* unimplemented  {fstp dword [esp+0x14], st0} */
🚫🚫    /* unimplemented  {fld st0, dword [view+0x1e4]} */
⋯    long double temp0 = yaw;
    /* unimplemented  {fcomp st0, dword [esp+0x14]} f- temp0 */ - temp0;
    bool c0 = /* bool c0 = unimplemented  {fcomp st0, dword [esp+0x14]} f< temp0 */ < temp0;
    bool c2_1 = FCMP_UO(/* bool c2_1 =
        is_unordered.t(unimplemented  {fcomp st0, dword [esp+0x14]}, temp0) */, temp0);
    bool c3 = /* bool c3 = unimplemented  {fcomp st0, dword [esp+0x14]} f== temp0 */ == temp0;
    /* unimplemented  {fcomp st0, dword [esp+0x14]} */
    int16_t top_4 = top + 2;
    int16_t scratch_value =
        (c0 ? 1 : 0) << 8 | (c2_1 ? 1 : 0) << 0xa | (c3 ? 1 : 0) << 0xe | (top_4 & 7) << 0xb;

    if (*(&scratch_value + 1) & 0x40)
    {
🚫🚫        /* unimplemented  {fld st0, dword [esi+0x1b4]} */
🚫🚫        /* unimplemented  {fadd dword [esi+0x114]} */
🚫🚫        /* unimplemented  {fmul st0, qword [&g_const_zero_double]} */
🚫🚫🚫        pitch = /* pitch = unimplemented  {fstp dword [esp+0x18], st0} */;
        /* unimplemented  {fstp dword [esp+0x18], st0} */
    }
    else
        pitch = *(pitch + 0x1b4);

    scratch_value = *(view + 0x138);
    char is_null_vector;

    if (!scratch_value)
        is_null_vector = Real64x3_NullVector(view + 0x180);

    if (scratch_value || !is_null_vector)
        pitch = nullptr;

🚫🚫    /* unimplemented  {fld st0, dword [esp+0x18]} */
🚫🚫    /* unimplemented  {fmul st0, dword [&g_const_pi]} */
🚫🚫    /* unimplemented  {fmul st0, dword [view+0x1e4]} */
🚫🚫    /* unimplemented  {fld st0, dword [esp+0x18]} */
⋯    long double temp1 = 0f;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} f- temp1 */ - temp1;
    bool c0_1 = /* bool c0_1 = unimplemented  {fcomp st0, dword [&g_const_zero]} f< temp1 */ < temp1;
    bool c2_2 = FCMP_UO(/* bool c2_2 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_zero]}, temp1) */, temp1);
    bool c3_1 = /* bool c3_1 = unimplemented  {fcomp st0, dword [&g_const_zero]} f== temp1 */ == temp1;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} */
🚫🚫    /* unimplemented  {fld st0, dword [esp+0x18]} */

    if (*(&((c0_1 ? 1 : 0) << 8 | (c2_2 ? 1 : 0) << 0xa | (c3_1 ? 1 : 0) << 0xe
            | ((top_4 - 2) & 7) << 0xb) + 1) & 1)
🚫🚫        /* unimplemented  {fchs } */

⋯    long double temp2 = 0.0599999987f;
    /* unimplemented  {fcomp st0, dword [&g_const_0_06]} f- temp2 */ - temp2;
    bool c0_2 = /* bool c0_2 = unimplemented  {fcomp st0, dword [&g_const_0_06]} f< temp2 */ < temp2;
    bool c2_3 = FCMP_UO(/* bool c2_3 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_0_06]}, temp2) */, temp2);
    bool c3_2 = /* bool c3_2 = unimplemented  {fcomp st0, dword [&g_const_0_06]} f== temp2 */ == temp2;
    /* unimplemented  {fcomp st0, dword [&g_const_0_06]} */
    int16_t top_10 = top_4 - 1;

    if (*(
        &((c0_2 ? 1 : 0) << 8 | (c2_3 ? 1 : 0) << 0xa | (c3_2 ? 1 : 0) << 0xe | (top_10 & 7) << 0xb)
        + 1) & 1)
    {
🚫🚫        /* unimplemented  {fld st0, dword [esp+0x18]} */
⋯        long double temp4_1 = 0f;
        /* unimplemented  {fcomp st0, dword [&g_const_zero]} f- temp4_1 */ - temp4_1;
        bool c0_3 = /* bool c0_3 =
            unimplemented  {fcomp st0, dword [&g_const_zero]} f< temp4_1 */ < temp4_1;
        bool c2_4 = FCMP_UO(/* bool c2_4 =
            is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_zero]}, temp4_1) */, temp4_1);
        bool c3_3 = /* bool c3_3 =
            unimplemented  {fcomp st0, dword [&g_const_zero]} f== temp4_1 */ == temp4_1;
        /* unimplemented  {fcomp st0, dword [&g_const_zero]} */
🚫🚫        /* unimplemented  {fld st0, dword [esp+0x18]} */

        if (*(&((c0_3 ? 1 : 0) << 8 | (c2_4 ? 1 : 0) << 0xa | (c3_3 ? 1 : 0) << 0xe
                | ((top_10 - 1) & 7) << 0xb) + 1) & 1)
🚫🚫            /* unimplemented  {fchs } */

🚫🚫        /* unimplemented  {fmul st0, qword [&g_const_533930]} */
🚫🚫        /* unimplemented  {fsubr st0, qword [&g_const_533924[4]]} */
🚫🚫🚫        /* unimplemented  {fdivp st1, st0} */
        /* unimplemented  {fdivp st1, st0} */
    }

🚫🚫    /* unimplemented  {fadd dword [view+0x124]} */
🚫🚫🚫🚫🚫    long double temp3 = -1.57079637f;
    /* unimplemented  {fcom st0, dword [&g_const_neg_half_pi]} f- temp3 */ - temp3;
    bool c0_4 = /* bool c0_4 = unimplemented  {fcom st0, dword [&g_const_neg_half_pi]} f< temp3 */ < temp3;
    bool c2_5 = FCMP_UO(/* bool c2_5 =
        is_unordered.t(unimplemented  {fcom st0, dword [&g_const_neg_half_pi]}, temp3) */, temp3);
    bool c3_4 = /* bool c3_4 = unimplemented  {fcom st0, dword [&g_const_neg_half_pi]} f== temp3 */ == temp3;
🚫🚫    *(view + 0x124) = /* *(view i+ 0x124) = unimplemented  {fst dword [view+0x124], st0} */;
    bool c1_1 = /* bool c1_1 = unknown  {fst dword [view+0x124], st0} */;
    int16_t top_14;

    if (!(*(&((c0_4 ? 1 : 0) << 8 | (c1_1 ? 1 : 0) << 9 | (c2_5 ? 1 : 0) << 0xa
        | (c3_4 ? 1 : 0) << 0xe | (top_10 & 7) << 0xb) + 1) & 1))
    {
⋯        long double temp5_1 = 1.57079637f;
        /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} f- temp5_1 */ - temp5_1;
        bool c0_5 = /* bool c0_5 =
            unimplemented  {fcomp st0, dword [&g_const_half_pi]} f< temp5_1 */ < temp5_1;
        bool c2_6 = FCMP_UO(/* bool c2_6 =
            is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_half_pi]}, temp5_1) */, temp5_1);
        bool c3_5 = /* bool c3_5 =
            unimplemented  {fcomp st0, dword [&g_const_half_pi]} f== temp5_1 */ == temp5_1;
        /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} */
        top_14 = top_10 + 1;

        if (!(*(&((c0_5 ? 1 : 0) << 8 | (c2_6 ? 1 : 0) << 0xa | (c3_5 ? 1 : 0) << 0xe
                | (top_14 & 7) << 0xb) + 1) & 0x41))
            *(view + 0x124) = 0x3fc90fdb;
    }
    else
    {
🚫🚫🚫        /* unimplemented  {fstp st0, st0} */
        /* unimplemented  {fstp st0, st0} */
        top_14 = top_10 + 1;
        *(view + 0x124) = 0xbfc90fdb;
    }

🚫🚫    /* unimplemented  {fld st0, dword [view+0x128]} */
🚫🚫    /* unimplemented  {fadd dword [view+0x124]} */
🚫🚫🚫    *(view + 0x120) = /* *(view i+ 0x120) = unimplemented  {fstp dword [view+0x120], st0} */;
    /* unimplemented  {fstp dword [view+0x120], st0} */
🚫🚫    /* unimplemented  {fld st0, dword [esi+0x1f0]} */
⋯    long double temp6 = 0f;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} f- temp6 */ - temp6;
    bool c0_6 = /* bool c0_6 = unimplemented  {fcomp st0, dword [&g_const_zero]} f< temp6 */ < temp6;
    bool c2_7 = FCMP_UO(/* bool c2_7 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_zero]}, temp6) */, temp6);
    bool c3_6 = /* bool c3_6 = unimplemented  {fcomp st0, dword [&g_const_zero]} f== temp6 */ == temp6;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} */

    if (!(*(
        &((c0_6 ? 1 : 0) << 8 | (c2_7 ? 1 : 0) << 0xa | (c3_6 ? 1 : 0) << 0xe | (top_14 & 7) << 0xb)
        + 1) & 0x40))
    {
🚫🚫        /* unimplemented  {fld st0, dword [view+0x1e4]} */
🚫🚫        /* unimplemented  {fmul st0, dword [&g_const_0_96]} */
🚫🚫🚫        *(view + 0x1e4) = /* *(view i+ 0x1e4) = unimplemented  {fstp dword [view+0x1e4], st0} */;
        /* unimplemented  {fstp dword [view+0x1e4], st0} */
🚫🚫        /* unimplemented  {fld st0, dword [view+0x1e8]} */
🚫🚫        /* unimplemented  {fmul st0, dword [&g_const_0_96]} */
🚫🚫🚫        *(view + 0x1e8) = /* *(view i+ 0x1e8) = unimplemented  {fstp dword [view+0x1e8], st0} */;
        /* unimplemented  {fstp dword [view+0x1e8], st0} */
    }

🚫🚫    /* unimplemented  {fld st0, dword [esi+0x1f4]} */
⋯    long double temp7 = 0f;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} f- temp7 */ - temp7;
    bool c0_7 = /* bool c0_7 = unimplemented  {fcomp st0, dword [&g_const_zero]} f< temp7 */ < temp7;
    bool c2_8 = FCMP_UO(/* bool c2_8 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_zero]}, temp7) */, temp7);
    bool c3_7 = /* bool c3_7 = unimplemented  {fcomp st0, dword [&g_const_zero]} f== temp7 */ == temp7;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} */

    if (!(*(
        &((c0_7 ? 1 : 0) << 8 | (c2_8 ? 1 : 0) << 0xa | (c3_7 ? 1 : 0) << 0xe | (top_14 & 7) << 0xb)
        + 1) & 0x40))
    {
🚫🚫        /* unimplemented  {fld st0, dword [view+0x1e4]} */
🚫🚫        /* unimplemented  {fmul st0, dword [&g_const_1_04]} */
🚫🚫🚫        *(view + 0x1e4) = /* *(view i+ 0x1e4) = unimplemented  {fstp dword [view+0x1e4], st0} */;
        /* unimplemented  {fstp dword [view+0x1e4], st0} */
🚫🚫        /* unimplemented  {fld st0, dword [view+0x1e8]} */
🚫🚫        /* unimplemented  {fmul st0, dword [&g_const_1_04]} */
🚫🚫🚫        *(view + 0x1e8) = /* *(view i+ 0x1e8) = unimplemented  {fstp dword [view+0x1e8], st0} */;
        /* unimplemented  {fstp dword [view+0x1e8], st0} */
    }

🚫🚫    /* unimplemented  {fld st0, dword [esi+0x1f8]} */
⋯    long double temp8 = 0f;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} f- temp8 */ - temp8;
    bool c0_8 = /* bool c0_8 = unimplemented  {fcomp st0, dword [&g_const_zero]} f< temp8 */ < temp8;
    bool c2_9 = FCMP_UO(/* bool c2_9 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_zero]}, temp8) */, temp8);
    bool c3_8 = /* bool c3_8 = unimplemented  {fcomp st0, dword [&g_const_zero]} f== temp8 */ == temp8;
    /* unimplemented  {fcomp st0, dword [&g_const_zero]} */
    int16_t top_28 = top_14;

    if (!(*(
        &((c0_8 ? 1 : 0) << 8 | (c2_9 ? 1 : 0) << 0xa | (c3_8 ? 1 : 0) << 0xe | (top_28 & 7) << 0xb)
        + 1) & 0x40))
    {
🚫🚫        int16_t top_29 = top_28 - 1;
        /* unimplemented  {fld st0, qword [&g_const_2_0]} */
🚫🚫🚫🚫        long double st0_1;
        bool c1_2;
        bool c2_10;
        st0_1 = __fptan(/* st0_1, c1_2, c2_10 = __fptan(unimplemented  {fptan }) */);
        /* unimplemented  {fptan } */

        if (!c2_10)
        {
            top_29 -= 1;
            /* unimplemented  {fptan } */
        }

        *(view + 0x1e4) = yaw;
🚫🚫🚫        /* unimplemented  {fstp st0, st0} */
        /* unimplemented  {fstp st0, st0} */
🚫🚫🚫        *(view + 0x1e8) = /* *(view i+ 0x1e8) = unimplemented  {fstp dword [view+0x1e8], st0} */;
        /* unimplemented  {fstp dword [view+0x1e8], st0} */
        top_28 = top_29 + 2;
    }

    float result = Real64x3_NullVector(view + 0x180);

    if (!result)
    {
        void orientation_copy;
        __builtin_memcpy(&orientation_copy, view + 0x180, 0x18);
        WeaponType_InitType();
🚫🚫🚫        pitch = /* pitch = unimplemented  {fstp dword [esp+0x30], st0} */;
        /* unimplemented  {fstp dword [esp+0x30], st0} */
        __builtin_memcpy(&orientation_copy, view + 0x180, 0x18);
        result = WeaponType_GetConfig();
🚫🚫        /* unimplemented  {fld st0, dword [esp+0x30]} */
⋯        long double temp13_1 = -1.57079637f;
        /* unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f- temp13_1 */ - temp13_1;
        bool c0_9 = /* bool c0_9 =
            unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f< temp13_1 */ < temp13_1;
        bool c2_11 = FCMP_UO(/* bool c2_11 =
            is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]}, temp13_1) */, temp13_1);
        bool c3_9 = /* bool c3_9 =
            unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f== temp13_1 */ == temp13_1;
        /* unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} */
        int16_t top_33 = top_28 + 1;
        result = (c0_9 ? 1 : 0) << 8 | (c2_11 ? 1 : 0) << 0xa | (c3_9 ? 1 : 0) << 0xe
            | (top_33 & 7) << 0xb;

        if (!(*(&result + 1) & 1))
        {
🚫🚫            /* unimplemented  {fld st0, dword [esp+0x18]} */
⋯            long double temp14_1 = 1.57079637f;
            /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} f- temp14_1 */ - temp14_1;
            bool c0_10 = /* bool c0_10 =
                unimplemented  {fcomp st0, dword [&g_const_half_pi]} f< temp14_1 */ < temp14_1;
            bool c2_12 = FCMP_UO(/* bool c2_12 =
                is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_half_pi]}, temp14_1) */,
                temp14_1);
            bool c3_10 = /* bool c3_10 =
                unimplemented  {fcomp st0, dword [&g_const_half_pi]} f== temp14_1 */ == temp14_1;
            /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} */
            result = (c0_10 ? 1 : 0) << 8 | (c2_12 ? 1 : 0) << 0xa | (c3_10 ? 1 : 0) << 0xe
                | (top_33 & 7) << 0xb;

            if (!(*(&result + 1) & 0x41))
                pitch = 1.57079637f;
        }
        else
            pitch = -1.57079637f;

🚫🚫🚫🚫🚫        long double temp15_1 = -1.57079637f;
        /* unimplemented  {fcom st0, dword [&g_const_neg_half_pi]} f- temp15_1 */ - temp15_1;
        bool c0_11 = /* bool c0_11 =
            unimplemented  {fcom st0, dword [&g_const_neg_half_pi]} f< temp15_1 */ < temp15_1;
        bool c2_13 = FCMP_UO(/* bool c2_13 =
            is_unordered.t(unimplemented  {fcom st0, dword [&g_const_neg_half_pi]}, temp15_1) */, temp15_1);
        bool c3_11 = /* bool c3_11 =
            unimplemented  {fcom st0, dword [&g_const_neg_half_pi]} f== temp15_1 */ == temp15_1;
        result = (c0_11 ? 1 : 0) << 8 | (c2_13 ? 1 : 0) << 0xa | (c3_11 ? 1 : 0) << 0xe
            | (top_33 & 7) << 0xb;

        if (!(*(&result + 1) & 1))
        {
🚫🚫🚫🚫🚫            long double temp16_1 = 1.57079637f;
            /* unimplemented  {fcom st0, dword [&g_const_half_pi]} f- temp16_1 */ - temp16_1;
            bool c0_12 = /* bool c0_12 =
                unimplemented  {fcom st0, dword [&g_const_half_pi]} f< temp16_1 */ < temp16_1;
            bool c2_14 = FCMP_UO(/* bool c2_14 =
                is_unordered.t(unimplemented  {fcom st0, dword [&g_const_half_pi]}, temp16_1) */,
                temp16_1);
            bool c3_12 = /* bool c3_12 =
                unimplemented  {fcom st0, dword [&g_const_half_pi]} f== temp16_1 */ == temp16_1;
            result = (c0_12 ? 1 : 0) << 8 | (c2_14 ? 1 : 0) << 0xa | (c3_12 ? 1 : 0) << 0xe
                | (top_33 & 7) << 0xb;

            if (!(*(&result + 1) & 0x41))
            {
🚫🚫🚫                /* unimplemented  {fstp st0, st0} */
                /* unimplemented  {fstp st0, st0} */
🚫🚫                /* unimplemented  {fld st0, dword [&g_const_half_pi]} */
            }
        }
        else
        {
🚫🚫🚫            /* unimplemented  {fstp st0, st0} */
            /* unimplemented  {fstp st0, st0} */
🚫🚫            /* unimplemented  {fld st0, dword [&g_const_neg_half_pi]} */
        }

        result = *(view + 0x198);

        if (!result)
        {
🚫🚫            /* unimplemented  {fld st0, dword [view+0x120]} */
⋯            long double temp17_1 = pitch;
            /* unimplemented  {fcomp st0, dword [esp+0x18]} f- temp17_1 */ - temp17_1;
            bool c0_13 = /* bool c0_13 =
                unimplemented  {fcomp st0, dword [esp+0x18]} f< temp17_1 */ < temp17_1;
            bool c2_15 = FCMP_UO(/* bool c2_15 =
                is_unordered.t(unimplemented  {fcomp st0, dword [esp+0x18]}, temp17_1) */,
                temp17_1);
            bool c3_13 = /* bool c3_13 =
                unimplemented  {fcomp st0, dword [esp+0x18]} f== temp17_1 */ == temp17_1;
            /* unimplemented  {fcomp st0, dword [esp+0x18]} */
🚫🚫            /* unimplemented  {fld st0, dword [view+0x120]} */
            result = (c0_13 ? 1 : 0) << 8 | (c2_15 ? 1 : 0) << 0xa | (c3_13 ? 1 : 0) << 0xe
                | ((top_33 - 1) & 7) << 0xb;
            int16_t top_40;

            if (!(*(&result + 1) & 1))
            {
⋯                long double temp18_1 = pitch;
                /* unimplemented  {fcomp st0, dword [esp+0x18]} f- temp18_1 */ - temp18_1;
                bool c0_15 = /* bool c0_15 =
                    unimplemented  {fcomp st0, dword [esp+0x18]} f< temp18_1 */ < temp18_1;
                bool c2_17 = FCMP_UO(/* bool c2_17 =
                    is_unordered.t(unimplemented  {fcomp st0, dword [esp+0x18]}, temp18_1) */,
                    temp18_1);
                bool c3_15 = /* bool c3_15 =
                    unimplemented  {fcomp st0, dword [esp+0x18]} f== temp18_1 */ == temp18_1;
                /* unimplemented  {fcomp st0, dword [esp+0x18]} */
                top_40 = top_33;
                result = (c0_15 ? 1 : 0) << 8 | (c2_17 ? 1 : 0) << 0xa | (c3_15 ? 1 : 0) << 0xe
                    | (top_40 & 7) << 0xb;

                if (!(*(&result + 1) & 0x41))
                {
🚫🚫                    /* unimplemented  {fld st0, dword [view+0x120]} */
🚫🚫                    /* unimplemented  {fsub st0, dword [&g_const_0_0524]} */  /* "PwV=333333" */
🚫🚫                    *(view + 0x120) = /* *(view i+ 0x120) =
                        unimplemented  {fst dword [view+0x120], st0} */;
⋯                    long double temp21_1 = pitch;
                    /* unimplemented  {fcomp st0, dword [esp+0x18]} f- temp21_1 */ - temp21_1;
                    bool c0_16 = /* bool c0_16 =
                        unimplemented  {fcomp st0, dword [esp+0x18]} f< temp21_1 */ < temp21_1;
                    bool c2_18 = FCMP_UO(/* bool c2_18 =
                        is_unordered.t(unimplemented  {fcomp st0, dword [esp+0x18]}, temp21_1) */,
                        temp21_1);
                    bool c3_16 = /* bool c3_16 =
                        unimplemented  {fcomp st0, dword [esp+0x18]} f== temp21_1 */ == temp21_1;
                    /* unimplemented  {fcomp st0, dword [esp+0x18]} */
                    result = (c0_16 ? 1 : 0) << 8 | (c2_18 ? 1 : 0) << 0xa | (c3_16 ? 1 : 0) << 0xe
                        | (top_40 & 7) << 0xb;

                    if (*(&result + 1) & 1)
                        *(view + 0x120) = pitch;
                }
            }
            else
            {
🚫🚫                /* unimplemented  {fadd dword [&g_const_0_0524]} */  /* "PwV=333333" */
🚫🚫                *(view + 0x120) = /* *(view i+ 0x120) =
                    unimplemented  {fst dword [view+0x120], st0} */;
⋯                long double temp19_1 = pitch;
                /* unimplemented  {fcomp st0, dword [esp+0x18]} f- temp19_1 */ - temp19_1;
                bool c0_14 = /* bool c0_14 =
                    unimplemented  {fcomp st0, dword [esp+0x18]} f< temp19_1 */ < temp19_1;
                bool c2_16 = FCMP_UO(/* bool c2_16 =
                    is_unordered.t(unimplemented  {fcomp st0, dword [esp+0x18]}, temp19_1) */,
                    temp19_1);
                bool c3_14 = /* bool c3_14 =
                    unimplemented  {fcomp st0, dword [esp+0x18]} f== temp19_1 */ == temp19_1;
                /* unimplemented  {fcomp st0, dword [esp+0x18]} */
                top_40 = top_33;
                result = (c0_14 ? 1 : 0) << 8 | (c2_16 ? 1 : 0) << 0xa | (c3_14 ? 1 : 0) << 0xe
                    | (top_40 & 7) << 0xb;

                if (!(*(&result + 1) & 0x41))
                    *(view + 0x120) = pitch;
            }

🚫🚫🚫🚫🚫            long double temp20_1 = *(view + 0x12c);
            /* unimplemented  {fcom st0, dword [view+0x12c]} f- temp20_1 */ - temp20_1;
            bool c0_17 = /* bool c0_17 =
                unimplemented  {fcom st0, dword [view+0x12c]} f< temp20_1 */ < temp20_1;
            bool c2_19 = FCMP_UO(/* bool c2_19 =
                is_unordered.t(unimplemented  {fcom st0, dword [view+0x12c]}, temp20_1) */,
                temp20_1);
            bool c3_17 = /* bool c3_17 =
                unimplemented  {fcom st0, dword [view+0x12c]} f== temp20_1 */ == temp20_1;
            result = (c0_17 ? 1 : 0) << 8 | (c2_19 ? 1 : 0) << 0xa | (c3_17 ? 1 : 0) << 0xe
                | (top_40 & 7) << 0xb;

            if (*(&result + 1) & 0x41)
            {
🚫🚫🚫🚫🚫                long double temp22_1 = *(view + 0x12c);
                /* unimplemented  {fcom st0, dword [view+0x12c]} f- temp22_1 */ - temp22_1;
                bool c0_19 = /* bool c0_19 =
                    unimplemented  {fcom st0, dword [view+0x12c]} f< temp22_1 */ < temp22_1;
                bool c2_21 = FCMP_UO(/* bool c2_21 =
                    is_unordered.t(unimplemented  {fcom st0, dword [view+0x12c]}, temp22_1) */,
                    temp22_1);
                bool c3_19 = /* bool c3_19 =
                    unimplemented  {fcom st0, dword [view+0x12c]} f== temp22_1 */ == temp22_1;
                result = (c0_19 ? 1 : 0) << 8 | (c2_21 ? 1 : 0) << 0xa | (c3_19 ? 1 : 0) << 0xe
                    | (top_40 & 7) << 0xb;

                if (!(*(&result + 1) & 1))
                {
🚫🚫🚫                label_464b42:
                    /* unimplemented  {fstp st0, st0} */
                    /* unimplemented  {fstp st0, st0} */
                    top_28 = top_40 + 1;
                }
                else
                {
🚫🚫                    /* unimplemented  {fld st0, dword [view+0x12c]} */
🚫🚫                    /* unimplemented  {fsub st0, dword [&g_const_0_0524]} */  /* "PwV=333333" */
🚫🚫                    *(view + 0x12c) = /* *(view i+ 0x12c) =
                        unimplemented  {fst dword [view+0x12c], st0} */;
⋯                    /* unimplemented  {fcomp st0, st1} f- unimplemented  {fcomp st0, st1} */
                        - /* unimplemented  {fcomp st0, st1} f- unimplemented  {fcomp st0, st1} */;
                    bool c0_20 = /* bool c0_20 =
                        unimplemented  {fcomp st0, st1} f< unimplemented  {fcomp st0, st1} */
                        < /* bool c0_20 =
                        unimplemented  {fcomp st0, st1} f< unimplemented  {fcomp st0, st1} */;
                    bool c2_22 = FCMP_UO(/* bool c2_22 = is_unordered.t(
                        unimplemented  {fcomp st0, st1}, unimplemented  {fcomp st0, st1}) */,
                        /* bool c2_22 = is_unordered.t(unimplemented  {fcomp st0, st1},
                        unimplemented  {fcomp st0, st1}) */);
                    bool c3_20 = /* bool c3_20 =
                        unimplemented  {fcomp st0, st1} f== unimplemented  {fcomp st0, st1} */
                        == /* bool c3_20 =
                        unimplemented  {fcomp st0, st1} f== unimplemented  {fcomp st0, st1} */;
                    /* unimplemented  {fcomp st0, st1} */
                    result = (c0_20 ? 1 : 0) << 8 | (c2_22 ? 1 : 0) << 0xa | (c3_20 ? 1 : 0) << 0xe
                        | (top_40 & 7) << 0xb;

                    if (!(*(&result + 1) & 1))
                        goto label_464b42;

🚫🚫🚫                    *(view + 0x12c) = /* *(view i+ 0x12c) =
                        unimplemented  {fstp dword [view+0x12c], st0} */;
                    /* unimplemented  {fstp dword [view+0x12c], st0} */
                    top_28 = top_40 + 1;
                }
            }
            else
            {
🚫🚫                /* unimplemented  {fld st0, dword [view+0x12c]} */
🚫🚫                /* unimplemented  {fadd dword [&g_const_0_0524]} */  /* "PwV=333333" */
🚫🚫                *(view + 0x12c) = /* *(view i+ 0x12c) =
                    unimplemented  {fst dword [view+0x12c], st0} */;
⋯                /* unimplemented  {fcomp st0, st1} f- unimplemented  {fcomp st0, st1} */
                    - /* unimplemented  {fcomp st0, st1} f- unimplemented  {fcomp st0, st1} */;
                bool c0_18 = /* bool c0_18 =
                    unimplemented  {fcomp st0, st1} f< unimplemented  {fcomp st0, st1} */
                    < /* bool c0_18 =
                    unimplemented  {fcomp st0, st1} f< unimplemented  {fcomp st0, st1} */;
                bool c2_20 = FCMP_UO(/* bool c2_20 = is_unordered.t(
                    unimplemented  {fcomp st0, st1}, unimplemented  {fcomp st0, st1}) */,
                    /* bool c2_20 = is_unordered.t(unimplemented  {fcomp st0, st1},
                    unimplemented  {fcomp st0, st1}) */);
                bool c3_18 = /* bool c3_18 =
                    unimplemented  {fcomp st0, st1} f== unimplemented  {fcomp st0, st1} */
                    == /* bool c3_18 =
                    unimplemented  {fcomp st0, st1} f== unimplemented  {fcomp st0, st1} */;
                /* unimplemented  {fcomp st0, st1} */
                result = (c0_18 ? 1 : 0) << 8 | (c2_20 ? 1 : 0) << 0xa | (c3_18 ? 1 : 0) << 0xe
                    | (top_40 & 7) << 0xb;

                if (*(&result + 1) & 0x41)
                    goto label_464b42;

🚫🚫🚫                *(view + 0x12c) = /* *(view i+ 0x12c) =
                    unimplemented  {fstp dword [view+0x12c], st0} */;
                /* unimplemented  {fstp dword [view+0x12c], st0} */
                top_28 = top_40 + 1;
            }
        }
        else
        {
            result = pitch;
🚫🚫🚫            *(view + 0x12c) = /* *(view i+ 0x12c) = unimplemented  {fstp dword [view+0x12c], st0} */;
            /* unimplemented  {fstp dword [view+0x12c], st0} */
            top_28 = top_33 + 1;
            *(view + 0x120) = result;
        }
    }

🚫🚫    /* unimplemented  {fld st0, dword [view+0x120]} */
⋯    long double temp9 = -1.57079637f;
    /* unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f- temp9 */ - temp9;
    bool c0_21 = /* bool c0_21 = unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f< temp9 */ < temp9;
    bool c2_23 = FCMP_UO(/* bool c2_23 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]}, temp9) */, temp9);
    bool c3_21 = /* bool c3_21 =
        unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f== temp9 */ == temp9;
    /* unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} */
    result = (c0_21 ? 1 : 0) << 8 | (c2_23 ? 1 : 0) << 0xa | (c3_21 ? 1 : 0) << 0xe
        | (top_28 & 7) << 0xb;

    if (!(*(&result + 1) & 1))
    {
🚫🚫        /* unimplemented  {fld st0, dword [view+0x120]} */
⋯        long double temp10_1 = 1.57079637f;
        /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} f- temp10_1 */ - temp10_1;
        bool c0_22 = /* bool c0_22 =
            unimplemented  {fcomp st0, dword [&g_const_half_pi]} f< temp10_1 */ < temp10_1;
        bool c2_24 = FCMP_UO(/* bool c2_24 =
            is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_half_pi]}, temp10_1) */, temp10_1);
        bool c3_22 = /* bool c3_22 =
            unimplemented  {fcomp st0, dword [&g_const_half_pi]} f== temp10_1 */ == temp10_1;
        /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} */
        result = (c0_22 ? 1 : 0) << 8 | (c2_24 ? 1 : 0) << 0xa | (c3_22 ? 1 : 0) << 0xe
            | (top_28 & 7) << 0xb;

        if (!(*(&result + 1) & 0x41))
            *(view + 0x120) = 0x3fc90fdb;
    }
    else
        *(view + 0x120) = 0xbfc90fdb;

🚫🚫    /* unimplemented  {fld st0, dword [view+0x12c]} */
⋯    long double temp11 = -1.57079637f;
    /* unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f- temp11 */ - temp11;
    bool c0_23 = /* bool c0_23 =
        unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f< temp11 */ < temp11;
    bool c2_25 = FCMP_UO(/* bool c2_25 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]}, temp11) */, temp11);
    bool c3_23 = /* bool c3_23 =
        unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} f== temp11 */ == temp11;
    /* unimplemented  {fcomp st0, dword [&g_const_neg_half_pi]} */
    result = (c0_23 ? 1 : 0) << 8 | (c2_25 ? 1 : 0) << 0xa | (c3_23 ? 1 : 0) << 0xe
        | (top_28 & 7) << 0xb;

    if (*(&result + 1) & 1)
    {
        *(view + 0x12c) = 0xbfc90fdb;
        return result;
    }

🚫🚫    /* unimplemented  {fld st0, dword [view+0x12c]} */
⋯    long double temp12 = 1.57079637f;
    /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} f- temp12 */ - temp12;
    bool c0_24 = /* bool c0_24 =
        unimplemented  {fcomp st0, dword [&g_const_half_pi]} f< temp12 */ < temp12;
    bool c2_26 = FCMP_UO(/* bool c2_26 =
        is_unordered.t(unimplemented  {fcomp st0, dword [&g_const_half_pi]}, temp12) */, temp12);
    bool c3_24 = /* bool c3_24 =
        unimplemented  {fcomp st0, dword [&g_const_half_pi]} f== temp12 */ == temp12;
    /* unimplemented  {fcomp st0, dword [&g_const_half_pi]} */
    result = (c0_24 ? 1 : 0) << 8 | (c2_26 ? 1 : 0) << 0xa | (c3_24 ? 1 : 0) << 0xe
        | (top_28 & 7) << 0xb;

    if (!(*(&result + 1) & 0x41))
        *(view + 0x12c) = 0x3fc90fdb;

    return result;
}
