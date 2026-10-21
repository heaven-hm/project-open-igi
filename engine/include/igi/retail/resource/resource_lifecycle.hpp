#pragma once

#include "igi/retail/resource/is_resource_loaded.hpp"

#include <cstddef>
#include <cstdint>

namespace igi::retail::resource {

// Native: Resource_Unload() - VA 0x004B6380 - complete port.
// Behavior: probes Is_ResourceLoaded(resource_path, &node) (0x004B5B90); when found, reads
//           `node->kind` at offset +0x24 (`entity_val[9]`); if `kind != 3 && kind != 1`,
//           calls Resource_FreeNode(node) (0x004B5C90); otherwise returns `kind`. When not
//           found, invokes Error_Show("Resource_Unload() : Resource '%s' is not loaded.").
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004B6380-0x004B63C3, 67 bytes, digest 14f9f39cad1672b7ab5c4799581eff2192c542b44cb75b36f6e263c4c74a0345;
//                 CLEAN/resource/004B6380-Resource_Unload.c.
// CFG: radare2 7 blocks/9 edges; Binary Ninja 7 blocks; 100% structural agreement.
// Confidence: high for exact control flow, offset +0x24 check, and callee dispatch.
// Signature: x86 cdecl int32_t(const char* resource_path); ported as std::int32_t Resource_Unload(const char* resource_path).
// Usage/tests: unloads resources by virtual path; exercised by resource_lifecycle_test.cpp.
std::int32_t Resource_Unload(const char* resource_path);

// Native: Resource_Load() - VA 0x004B5F00 - complete port.
// Behavior: probes Is_ResourceLoaded(resource_path, &node) (0x004B5B90); if cached, writes
//           `*(node + 0x28)` to `*out_size` when `out_size != nullptr`, sets `*(node + 0x2C) = 1`,
//           and returns `*(node + 0x34)`. Otherwise calls Resource_PackUnpack(resource_path, &size)
//           (0x004B18B0); when non-null, writes `size` to `*out_size` (if non-null) and returns
//           Sys_Resource(QFile_GetResolvedAlias(), data, size) (0x004B5F80). When null, calls
//           Error_Show("Failed to load resource: '%s'").
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5F00-0x004B5F7D, 125 bytes,
//                 digest 3250eca00c444e0774a6c8ca33df90e7bd63930021d542840e463b93421aa1d6;
//                 CLEAN/resource/004B5F00-Resource_Load.c.
// CFG: radare2 10 blocks/12 edges; Binary Ninja 10 blocks; 100% structural agreement.
// Confidence: high for exact cache hit/miss branches and node offsets (+0x28, +0x2C, +0x34).
// Signature: x86 cdecl void*(const char* resource_path, int32_t* out_size); ported with matching parameters.
// Usage/tests: primary retail resource loader used by MEF/TEX/QVM; exercised by resource_lifecycle_test.cpp.
void* Resource_Load(const char* resource_path, std::int32_t* out_size);

// Host hooks for the two unported callees (Resource_FreeNode at 0x004B5C90 and
// Resource_PackUnpack at 0x004B18B0) and Sys_Resource (0x004B5F80) so unit tests and callers
// can exercise the exact retail Resource_Load / Resource_Unload bodies.
using SysFreeMemory16Fn = std::int32_t (*)(ResourceHashNode* node);
using ResourcePackUnpackFn = void* (*)(const char* resource_path, std::int32_t* out_size);
using SysResourceFn = void* (*)(const char* resolved_alias, void* data, std::int32_t size);

void Sys_SetFreeMemory16(SysFreeMemory16Fn fn);
void Resource_SetPackUnpack(ResourcePackUnpackFn fn);
void Sys_SetResource(SysResourceFn fn);

} // namespace igi::retail::resource
