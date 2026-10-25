// Native: TaskType_SetInheritedFlags_401FA0; retail VA 0x00401FA0-0x00401FBF (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00401FA0-0x00401FBF (32 bytes, 1 block, 8 instructions, no branches and no calls).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; the store target 0x00AF5F0C is the same slot field that the transcribed 0x00401900 writes in its 0x0181 arm (`movl $0x20000000,0xaf5f0c(%esi)`, native_401900.cpp:71) and that task_types.hpp declares as TaskTypeSlot::inherited_flags at +0x2C, 0x00AF5F0C. Stride 0x30 comes from `lea edx,[eax+eax*2]; shl edx,4`.
// Behavior/CFG: two stack arguments, no control flow: type id (low 16 bits only, `and eax,0xffff`) and a flags dword. It ORs 0x20000000 into the flags and stores the result into the inherited-flags field of that task-type slot, i.e. `slot(type_id).inherited_flags = flags | 0x20000000`. There is no bounds check on the id and no other side effect; the return value is not established by the body.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The table is already declared in the tree as QTask_TypeTable / TaskTypeSlot, so this file writes through that existing declaration and defines no new global; the 0x20000000 marker and the 0x00AF5F0C offset are retail's own, matching the sibling transcription of 0x00401900. Vector is the first caller that exercises this path (grep found no other).
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL TaskType_SetInheritedFlags_401FA0(std::uint32_t type_id, std::uint32_t flags)
{
    NATIVE_LOG_ENTER("TaskType_SetInheritedFlags_401FA0", "0x00401FA0");
    ::QTask_TypeTable[static_cast<std::uint16_t>(type_id)].inherited_flags = flags | 0x20000000U; // 0x00401FA0-0x00401FB9: mov eax,[esp+4]; mov ecx,[esp+8]; and eax,0xffff; or ecx,0x20000000; lea edx,[eax+eax*2]; shl edx,4; mov [edx+0xaf5f0c],ecx
}

} // extern "C"

#undef NATIVE_DECL
