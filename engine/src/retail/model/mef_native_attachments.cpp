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


// Model_GetAttachment() - 0x004D0CF0 - REA/Ghidra + BN + radare2; source CFG 1.000.
// Branchless native pointer chain and 0x44 stride; no guessed attachment recursion.
// Original x86 instruction differential passed; full factory integration and live retail replay pending.
MefNativeAttachmentRecord* Model_GetAttachment(MefNativeModelRecord* model_record,
                                               std::int32_t attachment_index)
{
    return model_record->resource_record->mesh_header->attachments + attachment_index;
}

// Model_GetAttachmentCount() - 0x004D0D10 - REA/Ghidra + BN + radare2; source CFG 1.000.
// Preserve MOVSX semantics for the signed attachment count; no unsigned reinterpretation.
// Original x86 instruction differential passed; full factory integration and live retail replay pending.
std::int32_t Model_GetAttachmentCount(MefNativeModelRecord* model_record)
{
    return model_record->resource_record->mesh_header->attachment_count;
}

} // namespace igi::retail::model
