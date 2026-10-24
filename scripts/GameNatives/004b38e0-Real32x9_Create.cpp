// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Real32x9_Create
// Retail VA: 0x004b38e0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(float* arg1, float arg2, float arg3, float arg4)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t Real32x9_Create(float* matrix, float angle_x, float angle_y, float angle_z);

int32_t Real32x9_Create(float* matrix, float angle_x, float angle_y, float angle_z)

{
    Mat33_RotateZ(matrix, angle_z);
    float rot_00;
    Mat33_RotateY(&rot_00, angle_y);
    float rot_01;
    float rot_02;
    float out_00 = rot_00 * *matrix + rot_02 * matrix[2] + rot_01 * matrix[1];
    float rot_10;
    float rot_11;
    float rot_12;
    float out_10 = rot_10 * *matrix + rot_12 * matrix[2] + rot_11 * matrix[1];
    float rot_20;
    float rot_21;
    float rot_22;
    float out_20 = rot_20 * *matrix + rot_22 * matrix[2] + rot_21 * matrix[1];
    float out_01 = rot_00 * matrix[3] + rot_02 * matrix[5] + rot_01 * matrix[4];
    float out_11 = rot_10 * matrix[3] + rot_12 * matrix[5] + rot_11 * matrix[4];
    float out_21 = rot_20 * matrix[3] + rot_22 * matrix[5] + rot_21 * matrix[4];
    float out_02 = rot_00 * matrix[6] + rot_02 * matrix[8] + rot_01 * matrix[7];
    int32_t result;
    int32_t out_9 = result + matrix[9] + 1;
    float out_12 = rot_10 * matrix[6] + rot_12 * matrix[8] + rot_11 * matrix[7];
    float out_22 = rot_20 * matrix[6] + rot_22 * matrix[8] + rot_21 * matrix[7];
    __builtin_memcpy(matrix, &out_00, 0x28);
    Mat33_RotateX(&rot_00, angle_x);
    out_00 = rot_00 * *matrix + rot_02 * matrix[2] + rot_01 * matrix[1];
    float out_10_x = rot_10 * *matrix + rot_12 * matrix[2] + rot_11 * matrix[1];
    float out_20_x = rot_20 * *matrix + rot_22 * matrix[2] + rot_21 * matrix[1];
    float out_01_x = rot_00 * matrix[3] + rot_02 * matrix[5] + rot_01 * matrix[4];
    int32_t out_9_x = result + matrix[9] + 1;
    float out_11_x = rot_10 * matrix[3] + rot_12 * matrix[5] + rot_11 * matrix[4];
    float out_21_x = rot_20 * matrix[3] + rot_22 * matrix[5] + rot_21 * matrix[4];
    float out_02_x = rot_00 * matrix[6] + rot_02 * matrix[8] + rot_01 * matrix[7];
    float out_12_x = rot_10 * matrix[6] + rot_12 * matrix[8] + rot_11 * matrix[7];
    float out_22_x = rot_20 * matrix[6] + rot_22 * matrix[8] + rot_21 * matrix[7];
    __builtin_memcpy(matrix, &out_00, 0x28);
    matrix[9] = 0;
    return result;
}
