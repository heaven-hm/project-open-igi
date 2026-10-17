#include "igi/retail/script/qvm_native_loader.hpp"

#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/sys/resource_lookup.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {

bool Expect(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << "\n";
        return false;
    }
    return true;
}

bool TestExtensionFinder()
{
    char path1[64] = "LOCAL:missions/level1/objects.qsc";
    char ext_copy[8]{};
    char* const ext = igi::retail::script::Sys_OpenFileStream_7(ext_copy, 8, path1);
    if (!Expect(ext != nullptr && std::strcmp(ext, ".qsc") == 0,
                "Sys_OpenFileStream_7 should return pointer to .qsc extension"))
    {
        return false;
    }
    if (!Expect(std::strcmp(ext_copy, ".qsc") == 0,
                "Sys_OpenFileStream_7 should copy extension to dst when dst != nullptr"))
    {
        return false;
    }
    return true;
}

bool TestQvmLoadFromResourceAndCleanup()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();

    std::uint8_t fake_qvm_bytes[16] = {
        'I', 'L', 'F', 'F', 0x10, 0x00, 0x00, 0x00,
        'Q', 'V', 'M', ' ', 0x01, 0x02, 0x03, 0x04,
    };

    igi::retail::resource::ResourceHashNode sentinel{};
    igi::retail::resource::ResourceHashNode node{};
    constexpr const char* kUpperPath = "LOCAL:MISSIONS/LEVEL1/OBJECTS.QVM";
    node.next = &sentinel;
    node.hash = static_cast<std::int32_t>(igi::retail::sys::Sys_CheckHeap_15(kUpperPath));
    node.kind = 1;
    node.size = static_cast<std::int32_t>(sizeof(fake_qvm_bytes));
    node.loaded_flag = 1;
    node.data = fake_qvm_bytes;
    std::strcpy(node.name(), kUpperPath);

    auto& root = igi::retail::resource::Resource_GetHashRoot();
    root.next = &node;
    root.hash = -1;

    igi::retail::script::Host_SetQvmDebugCompileFlag(0);
    igi::retail::script::Host_SetQvmForcePrecompiledFlag(0);
    igi::retail::script::Sys_PopStack_23(99);

    if (!Expect(igi::retail::script::Sys_AllocateMemory_16("LOCAL:missions/level1/objects.qvm") == 1,
                "Sys_AllocateMemory_16 should return 1 when resource is present"))
    {
        return false;
    }

    void* const qvm = igi::retail::script::Qvm_Load("LOCAL:missions/level1/objects.qvm");
    if (!Expect(qvm != nullptr, "Qvm_Load should allocate and populate a QVM object"))
    {
        return false;
    }

    if (!Expect(igi::retail::script::Qvm_GetLastPopStack23Value() == 0,
                "Qvm_Load should call Sys_PopStack_23(0)"))
    {
        return false;
    }

    auto* const bytes = static_cast<const std::uint8_t*>(qvm);
    if (!Expect(std::strcmp(reinterpret_cast<const char*>(bytes), "LOCAL:missions/level1/objects.qvm") == 0,
                "Qvm_Load should copy path into +0x00..+0x7F"))
    {
        return false;
    }

    void* const loaded_buf = *reinterpret_cast<void* const*>(bytes + 0x80);
    const std::int32_t loaded_size = sizeof(void*) == 4
        ? *reinterpret_cast<const std::int32_t*>(bytes + 0x84)
        : *reinterpret_cast<const std::int32_t*>(bytes + 0x88);
    if (!Expect(loaded_buf == fake_qvm_bytes && loaded_size == 16,
                "Qvm_Load should store buffer at +0x80 and size at +0x84"))
    {
        return false;
    }

    igi::retail::script::Qvm_Cleanup(qvm);
    return true;
}

} // namespace

int main()
{
    if (!TestExtensionFinder())
    {
        return 1;
    }
    if (!TestQvmLoadFromResourceAndCleanup())
    {
        return 1;
    }
    std::cout << "PASS: retail Qvm_Load + Qvm_Cleanup tests succeeded\n";
    return 0;
}
