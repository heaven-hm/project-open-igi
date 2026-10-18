#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::dynobj {

// Native: DynObj_LoadModelMesh() - VA 0x004CF390 - complete port.
// Previous catalog alias: DynObj_LocalModelsSMef.
// Behavior: allocates a 24-byte (6-dword) model record via Mem_Alloc(0x18, 4) (0x004B0C60),
//           clears fields +0x00, +0x04, +0x10, +0x14, formats `"LOCAL:models/%s.mef"` into
//           global buffer 0x00A4EC6C, loads the resource via Resource_Load(0x00A4EC6C, nullptr)
//           (0x004B5F00) into `result[2]` (+0x08), locates the `'MESH'` (0x4D455348) chunk via
//           Ilff_GetFirstChunk / Ilff_FindChunk / Ilff_GetChunkPayload into `result[3]` (+0x0C),
//           verifies `*(uint32_t*)result[3] == 0x3DCCCCCD` (0.1f), resolves optional chunks
//           `'ATTA'` (0x41545441 -> mesh+0x8C), `'MVTX'` (0x4D565458 -> mesh+0x88),
//           `'PORT'` (0x504F5254 -> mesh+0x98), `'PTFC'` (0x50544643 -> mesh+0x90),
//           `'PTVX'` (0x50545658 -> mesh+0x94), counts and populates any `'NAXT'` (0x4E415854)
//           20-byte texture animation records (`result[4]` = count, `result[5]` = array), and
//           invokes the active render backend callback at `0x00A94E84[Render_GetActiveBackendId()]`
//           with `{result[3], result[2]}`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004CF390-0x004CF623, 659 bytes, digest 3c192ccc9ea19c77aeb0aa4ff7e828e45e4629d29743d3f9f83442360bab6366;
//                 CLEAN/dynobj/004CF390-DynObj_LocalModelsSMef.c.
// CFG: radare2 15 blocks/23 edges; Binary Ninja 15 blocks; 99.1% structural agreement.
// Confidence: high for exact chunk tags, MESH header offsets (+0x88..+0x98), NAXT loop, and backend dispatch.
// Signature: x86 cdecl void*(const char* model_name); ported as void* DynObj_LoadModelMesh(const char* model_name).
// Usage/tests: retail MEF model loader; exercised by local_models_mef_test.cpp.
void* DynObj_LoadModelMesh(const char* model_name);

// Backward-compatible alias for existing retail catalog references:
inline void* DynObj_LocalModelsSMef(const char* model_name)
{
    return DynObj_LoadModelMesh(model_name);
}

// Backend callback signature invoked by DynObj_LoadModelMesh at `0x00A94E84[backend_id]`.
// Receives a 2-pointer pair `{ mesh_payload, model_container }`.
using ModelBackendInitFn = void (*)(void** mesh_and_container);

[[nodiscard]] const char* DynObj_GetModelPathBuffer();
void Host_SetModelBackendInit(std::int32_t backend_id, ModelBackendInitFn callback);

} // namespace igi::retail::dynobj
