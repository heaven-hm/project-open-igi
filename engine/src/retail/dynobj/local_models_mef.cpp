#include "igi/retail/dynobj/local_models_mef.hpp"

#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/render/backend.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/sys/ilff_navigation.hpp"

#include <cstring>

namespace igi::retail::dynobj {
namespace {

// Port-owned storage for global path buffer 0x00A4EC6C (66-byte limit in sprintf).
char g_a4ec6c[0x100]{};

} // namespace

extern "C" {
// Retail global 0x00A94E84 - the 16-entry render-backend init callback table. It is defined here rather than in the anonymous namespace so a
// transcribed native can index it exactly as retail does (call *0x00A94E84(,%reg,4)), which an internal-linkage object cannot support.
ModelBackendInitFn g_a94e84[0x10]{};
}

// Native: DynObj_LoadModelMesh; retail VA 0x004CF390; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004CF390-0x004CF623, 659 bytes, digest 3c192ccc9ea19c77aeb0aa4ff7e828e45e4629d29743d3f9f83442360bab6366; CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.c.
// Behavior/CFG: allocates a 24-byte (6-dword) model record via Mem_Alloc(0x18, 4) (0x004B0C60), clears fields +0x00, +0x04, +0x10, +0x14, formats `"LOCAL:models/%s.mef"` into global buffer 0x00A4EC6C via sprintf (0x004A53B3), loads the resource via Resource_Load(0x00A4EC6C, nullptr) (0x004B5F00) into `result[2]` (+0x08), locates the `'MESH'` (0x4D455348) chunk via Ilff_GetFirstChunk / Ilff_FindChunk / Ilff_GetChunkPayload into `result[3]` (+0x0C), verifies `*(uint32_t*)result[3] == 0x3DCCCCCD` (0.1f), resolves optional chunks `'ATTA'` (0x41545441 -> mesh+0x8C), `'MVTX'` (0x4D565458 -> mesh+0x88), `'PORT'` (0x504F5254 -> mesh+0x98), `'PTFC'` (0x50544643 -> mesh+0x90), `'PTVX'` (0x50545658 -> mesh+0x94), counts and populates any `'NAXT'` (0x4E415854) 20-byte texture animation records (`result[4]` = count, `result[5]` = array), and invokes the active render backend callback at `0x00A94E84[Render_GetActiveBackendId()]` with `{result[3], result[2]}`. radare2 15 blocks/23 edges; Binary Ninja 15 blocks; 99.1% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact chunk tags, MESH header offsets (+0x88..+0x98), NAXT loop, and backend dispatch, checked against the retail's own bytes in 11 of 11 cases.
void* DynObj_LoadModelMesh(const char* model_name)
{
    // On 32-bit retail ABI each slot in `result` is 4 bytes (total 0x18 = 6 slots).
    // Allocate 6 pointer-sized slots on 64-bit host so `result[0..5]` hold pointers safely,
    // while requesting the exact 0x18 bytes on 32-bit x86.
    constexpr std::int32_t alloc_bytes =
        sizeof(void*) == 4 ? 0x18 : static_cast<std::int32_t>(6 * sizeof(void*));
    auto** const result = static_cast<void**>(mem::Mem_Alloc(alloc_bytes, 4));
    result[1] = nullptr;
    result[0] = nullptr;
    result[4] = nullptr;
    result[5] = nullptr;

    error::sprintf(g_a4ec6c, "LOCAL:models/%s.mef", model_name);
    void* const data_1 = resource::Resource_Load(g_a4ec6c, nullptr);
    result[2] = data_1;

    auto* const mesh_chunk = sys::Ilff_FindChunk(
        static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(data_1)), 0x4D455348);
    auto* const data_4 = static_cast<std::uint8_t*>(sys::Ilff_GetChunkPayload(mesh_chunk, data_1));
    result[3] = data_4;

    if (*reinterpret_cast<const std::uint32_t*>(data_4) == 0x3DCCCCCDU)
    {
        void* const data_5 = result[2];
        void* const atta_payload = sys::Ilff_GetChunkPayload(
            sys::Ilff_FindChunk(
                static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(data_5)), 0x41545441),
            data_5);
        *reinterpret_cast<std::uint32_t*>(data_4 + 0x8C) =
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(atta_payload));

        void* const data_9 = result[2];
        void* const mvtx_payload = sys::Ilff_GetChunkPayload(
            sys::Ilff_FindChunk(
                static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(data_9)), 0x4D565458),
            data_9);
        *reinterpret_cast<std::uint32_t*>(data_4 + 0x88) =
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(mvtx_payload));

        void* const data_13 = result[2];
        void* const port_payload = sys::Ilff_GetChunkPayload(
            sys::Ilff_FindChunk(
                static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(data_13)), 0x504F5254),
            data_13);
        *reinterpret_cast<std::uint32_t*>(data_4 + 0x98) =
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(port_payload));

        void* const data_17 = result[2];
        void* const ptfc_payload = sys::Ilff_GetChunkPayload(
            sys::Ilff_FindChunk(
                static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(data_17)), 0x50544643),
            data_17);
        *reinterpret_cast<std::uint32_t*>(data_4 + 0x90) =
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ptfc_payload));

        void* const data_21 = result[2];
        void* const ptvx_payload = sys::Ilff_GetChunkPayload(
            sys::Ilff_FindChunk(
                static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(data_21)), 0x50545658),
            data_21);
        *reinterpret_cast<std::uint32_t*>(data_4 + 0x94) =
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ptvx_payload));

        if (sys::Ilff_FindChunk(
                static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(result[2])), 0x4E415854)
            != nullptr)
        {
            std::int32_t* const data_29 = sys::Ilff_FindChunk(
                static_cast<std::int32_t*>(sys::Ilff_GetFirstChunk(result[2])), 0x4E415854);
            std::int32_t* dest_1 = data_29;
            std::int32_t i_2 = 1;
            std::int32_t* data_30 = nullptr;

            for (auto* i = static_cast<std::int32_t*>(sys::Ilff_GetNextChunk(data_29));
                 i != nullptr;
                 i = static_cast<std::int32_t*>(sys::Ilff_GetNextChunk(data_30)))
            {
                data_30 = sys::Ilff_FindChunk(i, 0x4E415854);
                if (data_30 == nullptr)
                {
                    break;
                }
                i_2 += 1;
            }

            auto* const naxt_array =
                static_cast<std::uint8_t*>(mem::Mem_Alloc(i_2 * 0x14, 4));
            result[5] = naxt_array;
            result[4] = reinterpret_cast<void*>(static_cast<std::intptr_t>(i_2));

            if (i_2 != 0)
            {
                std::size_t offset = 0U;
                std::int32_t i_1 = 0;
                do
                {
                    auto* const data_32 =
                        static_cast<std::int32_t*>(sys::Ilff_GetChunkPayload(dest_1, result[2]));
                    *reinterpret_cast<std::uint32_t*>(naxt_array + offset + 0x10) =
                        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&data_32[2]));
                    *reinterpret_cast<std::int32_t*>(naxt_array + offset + 0x08) = data_32[0];
                    *reinterpret_cast<std::int32_t*>(naxt_array + offset + 0x00) = data_32[1];
                    *reinterpret_cast<std::int32_t*>(naxt_array + offset + 0x04) = 0;

                    dest_1 = static_cast<std::int32_t*>(sys::Ilff_GetNextChunk(dest_1));
                    if (dest_1 != nullptr)
                    {
                        dest_1 = sys::Ilff_FindChunk(dest_1, 0x4E415854);
                    }

                    offset += 0x14U;
                    i_1 = i_2;
                    i_2 -= 1;
                } while (i_1 != 1);
            }
        }

        // Retail 0x004CF5E7..0x004CF610 passes mesh first, then its container.
        void* pair[2] = {result[3], result[2]};
        const std::int32_t backend_id = render::Render_GetActiveBackendId();
        // Retail performs the indexed callback lookup without a range check.
        // Valid backend/table initialization remains a caller precondition.
        if (g_a94e84[backend_id] != nullptr)
        {
            // Retail tests the first lookup at 0x004CF5FA, then reads the
            // backend ID again at 0x004CF606 for dispatch at 0x004CF610.
            g_a94e84[render::Render_GetActiveBackendId()](pair);
        }

        return result;
    }

    float bad_version = 0.0f;
    std::memcpy(&bad_version, data_4, sizeof(float));
    Debug_Error("Mesh \"%s\" and source code are incompatible. (Mesh \"%s\" is version %0.2f, source code "
                      "is version %0.2f.)",
                      // Retail pushes qword 0x3FB99999A0000000, the widened float32 0.1 value.
                      model_name, model_name, static_cast<double>(bad_version), 0x1.99999ap-4);
    if constexpr (sizeof(void*) == 4)
    {
        // Retail incompatible-version path does not return if Debug_Error returns.
        for (;;) {}
    }
    return nullptr;
}

const char* DynObj_GetModelPathBuffer()
{
    return g_a4ec6c;
}

void Model_SetBackendInit(std::int32_t backend_id, ModelBackendInitFn callback)
{
    if (backend_id >= 0 && backend_id < 0x10)
    {
        g_a94e84[backend_id] = callback;
    }
}

} // namespace igi::retail::dynobj
