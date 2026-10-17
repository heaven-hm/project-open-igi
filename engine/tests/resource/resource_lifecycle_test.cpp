#include "igi/retail/resource/resource_lifecycle.hpp"

#include "igi/retail/qfile/alias_resolve.hpp"
#include "igi/retail/sys/resource_lookup.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

struct alignas(void*) TestNodeBuffer {
    std::uint8_t bytes[0x100]{};

    [[nodiscard]] igi::retail::resource::ResourceHashNode* node() noexcept
    {
        return reinterpret_cast<igi::retail::resource::ResourceHashNode*>(bytes);
    }
};

igi::retail::resource::ResourceHashNode* g_freed_node = nullptr;
std::int32_t fake_sys_free_memory_16(igi::retail::resource::ResourceHashNode* node)
{
    g_freed_node = node;
    return 99;
}

std::uint8_t g_unpacked_payload[32] = {1, 2, 3, 4};
void* fake_pack_unpack(const char* /*path*/, std::int32_t* out_size)
{
    if (out_size != nullptr)
    {
        *out_size = 32;
    }
    return g_unpacked_payload;
}

const char* g_sys_resource_alias = nullptr;
void* g_sys_resource_data = nullptr;
std::int32_t g_sys_resource_size = 0;

void* fake_sys_resource(const char* resolved_alias, void* data, std::int32_t size)
{
    g_sys_resource_alias = resolved_alias;
    g_sys_resource_data = data;
    g_sys_resource_size = size;
    return data;
}

void setup_cached_node(igi::retail::resource::ResourceHashNode& node,
                       igi::retail::resource::ResourceHashNode& sentinel,
                       const char* canonical_name, std::int32_t kind, std::int32_t byte_size,
                       void* data_ptr)
{
    node = igi::retail::resource::ResourceHashNode{};
    sentinel = igi::retail::resource::ResourceHashNode{};
    node.next = &sentinel;
    node.hash = static_cast<std::int32_t>(igi::retail::sys::Sys_CheckHeap_15(canonical_name));
    std::strcpy(node.name(), canonical_name);
    node.kind = kind;
    node.size = byte_size;
    node.loaded_flag = 0;
    node.data = data_ptr;

    auto& root = igi::retail::resource::Resource_GetHashRoot();
    root.next = &node;
    root.hash = -1;
}

void test_resource_load_cache_hit()
{
    igi::retail::resource::ResourceHashNode node{};
    igi::retail::resource::ResourceHashNode sentinel{};
    std::uint8_t payload[16]{};
    setup_cached_node(node, sentinel, "LOCAL:MODELS/PROP.MEF", 2, 1234, payload);

    std::int32_t size = 0;
    void* const result = igi::retail::resource::Resource_Load("local:models\\prop.mef", &size);
    require(result == payload, "Resource_Load returns *(node + 0x34) on cache hit");
    require(size == 1234, "Resource_Load writes *(node + 0x28) to out_size on cache hit");
    require(node.loaded_flag == 1,
            "Resource_Load sets *(node + 0x2C) = 1 on cache hit");
}

void test_resource_load_cache_miss_calls_pack_unpack_and_sys_resource()
{
    auto& root = igi::retail::resource::Resource_GetHashRoot();
    root.next = nullptr;

    igi::retail::resource::Host_SetResourcePackUnpack(&fake_pack_unpack);
    igi::retail::resource::Host_SetSysResource(&fake_sys_resource);

    std::int32_t size = 0;
    void* const result = igi::retail::resource::Resource_Load("LOCAL:qvm/objects.qvm", &size);
    require(result == g_unpacked_payload, "Resource_Load returns Sys_Resource result on cache miss");
    require(size == 32, "Resource_Load writes unpacked size to out_size");
    require(g_sys_resource_data == g_unpacked_payload, "Sys_Resource receives unpacked buffer");
    require(g_sys_resource_size == 32, "Sys_Resource receives unpacked size");
    require(g_sys_resource_alias == igi::retail::qfile::QFile_GetResolvedAlias(),
            "Sys_Resource receives global resolved alias buffer 0x00943A18");
}

void test_resource_unload_branches_on_kind_at_0x24()
{
    igi::retail::resource::ResourceHashNode node{};
    igi::retail::resource::ResourceHashNode sentinel{};
    std::uint8_t payload[16]{};

    igi::retail::resource::Host_SetSysFreeMemory16(&fake_sys_free_memory_16);

    setup_cached_node(node, sentinel, "LOCAL:MODELS/A.MEF", 1, 64, payload);
    g_freed_node = nullptr;
    require(igi::retail::resource::Resource_Unload("LOCAL:models/a.mef") == 1,
            "Resource_Unload returns kind 1 without calling Sys_FreeMemory_16");
    require(g_freed_node == nullptr, "Sys_FreeMemory_16 not called for kind 1");

    node.kind = 3;
    require(igi::retail::resource::Resource_Unload("LOCAL:models/a.mef") == 3,
            "Resource_Unload returns kind 3 without calling Sys_FreeMemory_16");
    require(g_freed_node == nullptr, "Sys_FreeMemory_16 not called for kind 3");

    node.kind = 2;
    require(igi::retail::resource::Resource_Unload("LOCAL:models/a.mef") == 99,
            "Resource_Unload calls Sys_FreeMemory_16 when kind != 1 && kind != 3");
    require(g_freed_node == &node, "Sys_FreeMemory_16 receives the matched hash node");
}

} // namespace

int main()
{
    test_resource_load_cache_hit();
    test_resource_load_cache_miss_calls_pack_unpack_and_sys_resource();
    test_resource_unload_branches_on_kind_at_0x24();
    std::cout << "All retail Resource_Load / Resource_Unload tests passed.\n";
    return EXIT_SUCCESS;
}
