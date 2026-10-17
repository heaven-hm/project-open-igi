#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/qfile/alias_resolve.hpp"
#include "igi/retail/sys/resource_lookup.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void reset_state()
{
    igi::retail::resource::Resource_GetHashRoot().next = nullptr;
    for (std::size_t i = 0; i < igi::retail::qfile::device_entry_count; ++i) {
        std::memset(&igi::retail::qfile::QFile_GetDeviceTable()[i], 0,
                    sizeof(igi::retail::qfile::DeviceEntry));
    }
}

// 0x004B5B90 finds a node whose cached hash and name both match the normalised path.
void test_finds_a_matching_node()
{
    reset_state();

    // The retail loop checks the current bucket and then advances, so the final node in the
    // chain is never compared. A matching node therefore needs a successor.
    char storage[sizeof(igi::retail::resource::ResourceHashNode) + 0x40] = {};
    auto* node = reinterpret_cast<igi::retail::resource::ResourceHashNode*>(storage);
    igi::retail::resource::ResourceHashNode tail{};
    node->next = &tail;
    tail.next = nullptr;
    std::strcpy(node->name(), "LOCAL:/TEXTURES/A.TEX");
    node->hash = static_cast<std::int32_t>(
        igi::retail::sys::Sys_CheckHeap_15("LOCAL:/TEXTURES/A.TEX"));
    igi::retail::resource::Resource_GetHashRoot().next = node;

    igi::retail::resource::ResourceHashNode* found = nullptr;
    const std::int32_t result = igi::retail::resource::Is_ResourceLoaded("LOCAL:/TEXTURES/A.TEX", &found);
    require(result == 1, "a matching resource returns 1");
    require(found == node, "the matching node is reported");
}

// Backslashes are converted before hashing, so both spellings resolve to the same node.
void test_backslash_spelling_matches()
{
    reset_state();

    char storage[sizeof(igi::retail::resource::ResourceHashNode) + 0x40] = {};
    auto* node = reinterpret_cast<igi::retail::resource::ResourceHashNode*>(storage);
    igi::retail::resource::ResourceHashNode tail{};
    node->next = &tail;
    tail.next = nullptr;
    std::strcpy(node->name(), "LOCAL:/MODELS/B.MEF");
    node->hash = static_cast<std::int32_t>(
        igi::retail::sys::Sys_CheckHeap_15("LOCAL:/MODELS/B.MEF"));
    igi::retail::resource::Resource_GetHashRoot().next = node;

    igi::retail::resource::ResourceHashNode* found = nullptr;
    require(igi::retail::resource::Is_ResourceLoaded("LOCAL:\\MODELS\\B.MEF", &found) == 1,
            "'\\' is normalised to '/' before the probe");
    require(found == node, "the backslash spelling resolves to the same node");
}

// An unknown name returns 0 and leaves the output untouched.
void test_unknown_resource_returns_zero()
{
    reset_state();
    igi::retail::resource::ResourceHashNode* sentinel = nullptr;
    igi::retail::resource::ResourceHashNode* found = sentinel;
    require(igi::retail::resource::Is_ResourceLoaded("LOCAL:/NOPE", &found) == 0,
            "unknown resource returns 0");
    require(found == sentinel, "the out pointer is untouched on failure");
}

} // namespace

int main()
{
    test_finds_a_matching_node();
    test_backslash_spelling_matches();
    test_unknown_resource_returns_zero();

    std::cout << "All retail Is_ResourceLoaded tests passed.\n";
    return EXIT_SUCCESS;
}
