// Native: Resource_CreateKind3Node_4B6420; retail VA 0x004B6420 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B6420-0x004B643D (30 bytes, 1 block, 0 edges, 12 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes (8b442408 8b4c240c 8b542404 6a00 6a03 50 51 6a00 52 e8b6f5ffff 83c418 c3); section 22.3 of the evidence sheet.
// Behavior/CFG: forwards to Resource_CreateNode (0x004B59F0) with the fixed resource kind 3 and null second and sixth arguments: (argument1, null, argument3, argument2, 3, null). Six dwords are pushed in retail's order, 0x18 bytes are cleaned, and the node pointer the callee returns is returned unchanged. No branch, no data reference, no store.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Resource_CreateNode is transcribed in engine/src/retail/resource/sys_resource.cpp. The meanings of the three wrapper parameters beyond their use as the callee's context/size/data slots are not established.
#include "dev_log.hpp"
#include "igi/retail/resource/sys_resource.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void* NATIVE_DECL Resource_CreateKind3Node_4B6420(const char* context, std::int32_t size, void* data)
{
    NATIVE_LOG_ENTER("Resource_CreateKind3Node_4B6420", "0x004B6420");
    // 0x004B6424-0x004B6435: push 0 (arg6); push 3 (arg5); push size (arg4); push data (arg3); push 0 (arg2); push context (arg1);
    // call 0x004B59F0; add esp,0x18.
    return ::igi::retail::resource::Resource_CreateNode(context, nullptr, data, size, 3, nullptr);
}

} // extern "C"

#undef NATIVE_DECL
