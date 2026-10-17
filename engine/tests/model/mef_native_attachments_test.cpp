#include "igi/retail/model/mef_native_attachments.hpp"

#include <array>
#include <iostream>
#include <stdexcept>

namespace
{
void require(bool condition, const char* description)
{
    if (!condition)
    {
        throw std::runtime_error(description);
    }
}
} // namespace

int main()
{
    try
    {
        using namespace igi::retail::model;
        std::array<MefNativeAttachmentRecord, 3> records{};
        MefNativeMeshHeader mesh_header{};
        mesh_header.attachment_count = 3;
        mesh_header.attachments = records.data();
        MefNativeResourceRecord resource_record{};
        resource_record.mesh_header = &mesh_header;
        MefNativeModelRecord model_record{};
        model_record.resource_record = &resource_record;
        require(Model_GetAttachmentCount(&model_record) == 3, "Attachment count differs.");
        for (std::size_t record_index = 0; record_index < records.size(); ++record_index)
        {
            require(Model_GetAttachment(&model_record, static_cast<std::int32_t>(record_index)) == &records[record_index],
                    "ATTA pointer chain or 68-byte stride differs.");
        }

        mesh_header.attachment_count = -1;
        require(Model_GetAttachmentCount(&model_record) == -1, "Signed count must preserve MOVSX.");
        mesh_header.attachment_count = 0;
        require(Model_GetAttachmentCount(&model_record) == 0, "Empty count differs.");
        std::cout << "Windows x86 retail ATTA accessor tests passed.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ATTA accessor test failed: " << exception.what() << '\n';
        return 1;
    }
}
