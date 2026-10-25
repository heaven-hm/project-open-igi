// Native: QFile_RegisterAlias_4B12F0; retail VA 0x004B12F0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B12F0-0x004B139E (175 bytes, 7 blocks, 8 edges, 66 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and the raw bytes (53 55 8b6c2410 57 55 e8a3feffff 8bd8 83c404 85db 0f8491000000 8b7c2410 57 e88c000000 83c404 33c0 b9c0239400 833900 7413 81c194000000 40 81f940369400 7cec); section 22.3 of the evidence sheet.
// Behavior/CFG: resolves the device index of the target path with 0x004B11A0 (returns 0 for an unknown device, and then this routine returns without touching the tables); clears any record whose key equals argument1 with 0x004B13A0; finds the first of the 32 free 0x94-byte records by testing the handle word at 0x009423C0 + index*0x94 for zero (bound 0x00943640); copies argument1 into record+0x00 and argument2 into record+0x10 with strlen+1 sized copies; stores the device index at the handle word. Returns without writing when all 32 records are taken.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. 0x00942330 (32 records of 0x94 bytes, ending at 0x009435B0) is owned by native_4b13a0.cpp and 0x009423C0 by native_4b12d0.cpp; both are referred to extern here. The record's name areas are copied into without a length bound like retail, so a key or target longer than the following fields allows retail's own overrun - no guard was added. The eax value at return is a leftover of the copy loop in retail, so no return value is declared. Callers reach this with argument1 = the alias key names observed at runtime (INTROSCREEN:, STATUSSCREEN:, COMPUTER:, MENUSYS:, LANGUAGE:, MISSION:) and argument2 = the device-prefixed target path.
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00942330: the 32 0x94-byte alias records (owned by native_4b13a0.cpp).
extern std::uint8_t g_942330[0x1280];
// 0x009423C0: the parallel handle table (owned by native_4b12d0.cpp).
extern std::uint8_t g_9423c0[0x1280];

// 0x004B11A0 (native_4b11a0.cpp): 1-based device index of a resolved path, 0 when no device matches.
std::int32_t NATIVE_DECL QFile_DeviceIndex_4B11A0(const char* resolved_path);
// 0x004B13A0 (native_4b13a0.cpp): clears the record whose key equals the argument.
void NATIVE_DECL QFile_ClearEntryByName_4B13A0(const char* name);

void NATIVE_DECL QFile_RegisterAlias_4B12F0(const char* key, const char* target)
{
    const std::int32_t device =                                        // 0x004B12F6-0x004B12FD: push ebp; call 0x004B11A0; mov ebx,eax
        QFile_DeviceIndex_4B11A0(target);
    if (device == 0)                                                   // 0x004B1302-0x004B1304: test ebx,ebx; jz 0x004B139B
    {
        return;                                                        // 0x004B139B-0x004B139E: pop edi; pop ebp; pop ebx; ret
    }

    QFile_ClearEntryByName_4B13A0(key);                                // 0x004B130E-0x004B1314: mov edi,[esp+0x10]; push edi; call 0x004B13A0

    std::uint32_t index = 0;                                           // 0x004B1317: xor eax,eax
    std::uint8_t* handle = &::g_9423c0[0];                             // 0x004B1319: mov ecx,0x009423C0
    for (;;)
    {
        if (*reinterpret_cast<const std::uint32_t*>(handle) == 0U)     // 0x004B131E-0x004B1321: cmp dword [ecx],0; jz 0x004B1336
        {
            break;
        }
        handle += 0x94U;                                               // 0x004B1323: add ecx,0x94
        ++index;                                                       // 0x004B1329: inc eax
        if (reinterpret_cast<std::uintptr_t>(handle) >=                // 0x004B132A-0x004B1330: cmp ecx,0x00943640; jl 0x004B131E
            reinterpret_cast<std::uintptr_t>(&::g_9423c0[0]) + 0x1280U)
        {
            return;                                                    // 0x004B1332-0x004B1335: pop edi; pop ebp; pop ebx; ret
        }
    }

    std::uint8_t* const record =                                       // 0x004B1336-0x004B1342: lea ecx,[eax+eax*8]; lea edx,[eax+ecx*4]; shl edx,2; lea esi,[edx+0x00942330]
        &::g_942330[0] + 0x94U * static_cast<std::uintptr_t>(index);
    std::memcpy(record, key, std::strlen(key) + 1U);                   // 0x004B1345-0x004B1369: repne scasb on the key, then rep movsd / rep movsb
    std::memcpy(record + 0x10U, target, std::strlen(target) + 1U);     // 0x004B136B-0x004B1392: repne scasb on the target, then rep movsd / rep movsb into 0x00942340 + index*0x94
    *reinterpret_cast<std::uint32_t*>(handle) =                        // 0x004B1394: mov [edx+0x009423C0],ebx
        static_cast<std::uint32_t>(device);
}

} // extern "C"

#undef NATIVE_DECL
