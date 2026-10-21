#include "igi/retail/model/mef_native_attachments.hpp"

namespace igi::retail::model
{

#if defined(_M_IX86) || defined(__i386__) || (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4)
static_assert(sizeof(void*) == 4, "Retail model record accessors require the Windows x86 ABI.");
static_assert(sizeof(MefNativeAttachmentRecord) == 0x44);
static_assert(offsetof(MefNativeModelRecord, resource_record) == 0x0C);
static_assert(offsetof(MefNativeResourceRecord, mesh_header) == 0x0C);
static_assert(offsetof(MefNativeMeshHeader, attachment_count) == 0x7E);
static_assert(offsetof(MefNativeMeshHeader, attachments) == 0x8C);
static_assert(sizeof(MefNativeMeshHeader) == 0x9C);
#endif


// Native: Model_GetAttachment; retail VA 0x004D0CF0; signature/calling convention MefNativeAttachmentRecord* __cdecl Model_GetAttachment(MefNativeModelRecord*, int32_t).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Ghidra/REA, Radare2 listings in docs/fidelity/evidence/0x004D0CF0-*; fresh angr CFG and x86 ESIL differential via native_gate.py.
// Behavior/CFG: one read-only leaf block; model+0x0C -> resource+0x0C -> mesh+0x8C; returns attachment base + index*0x44; primary CFG score 1.0.
// Status/limits: VERIFIED for valid Windows x86 record chains and in-array indices; 12 exact retail-vs-compiled-x86 returns; factory integration and invalid pointers are outside this accessor check.
MefNativeAttachmentRecord* Model_GetAttachment(MefNativeModelRecord* model_record,
                                               std::int32_t attachment_index)
{
    return model_record->resource_record->mesh_header->attachments + attachment_index;
}

// Native: Model_GetAttachmentCount; retail VA 0x004D0D10; signature/calling convention int32_t __cdecl Model_GetAttachmentCount(MefNativeModelRecord*).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Ghidra/REA, Radare2 listings in docs/fidelity/evidence/0x004D0D10-*; fresh angr CFG and x86 ESIL differential via native_gate.py.
// Behavior/CFG: one read-only leaf block; model+0x0C -> resource+0x0C; sign-extends the int16 at mesh+0x7E to EAX; primary CFG score 1.0.
// Status/limits: VERIFIED for valid Windows x86 record chains; 14 exact retail-vs-compiled-x86 signed-boundary returns; factory integration and invalid pointers are outside this accessor check.
std::int32_t Model_GetAttachmentCount(MefNativeModelRecord* model_record)
{
    return model_record->resource_record->mesh_header->attachment_count;
}

} // namespace igi::retail::model
