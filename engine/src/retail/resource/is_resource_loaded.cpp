#include "igi/retail/resource/is_resource_loaded.hpp"

#include "igi/retail/qfile/alias_resolve.hpp"
#include "igi/retail/sys/resource_lookup.hpp"

namespace igi::retail::resource {
namespace {

ResourceHashNode hash_root{};
char alias_buffer[0x100]{};
char name_buffer[0x100]{};

// Hand-rolled three-way comparison, matching the retail byte loop.
[[nodiscard]] std::int32_t compare_names(const char* left, const char* right)
{
    while (true)
    {
        const auto left_byte = static_cast<std::uint8_t>(*left);
        const auto right_byte = static_cast<std::uint8_t>(*right);
        const bool left_less = left_byte < right_byte;

        if (left_byte != right_byte)
        {
            return left_less ? -1 : 1;
        }
        if (left_byte == 0)
        {
            return 0;
        }
        ++left;
        ++right;
    }
}

} // namespace

// 0x004B5B90
std::int32_t Is_ResourceLoaded(const char* resource_path, ResourceHashNode** out_entry)
{
    std::int32_t depth = 0;

    do
    {
        depth = qfile::QFile_AliasResolve(alias_buffer, resource_path, depth);
        sys::Sys_ResolveSymbol_15(name_buffer, alias_buffer);

        if (name_buffer[0] != 0)
        {
            char* cursor = name_buffer;
            char character = name_buffer[0];
            do
            {
                char replacement = 0x2F;
                if (character != 0x5C)
                {
                    replacement = *cursor;
                }
                *cursor = replacement;
                character = cursor[1];
                ++cursor;
            } while (character != 0);
        }

        const auto hash = static_cast<std::int32_t>(sys::Sys_CheckHeap_15(name_buffer));
        ResourceHashNode* bucket = &hash_root;
        ResourceHashNode* node = hash_root.next;

        while (node != nullptr)
        {
            if (bucket->hash == hash)
            {
                if (compare_names(bucket->name(), name_buffer) == 0)
                {
                    if (out_entry != nullptr)
                    {
                        *out_entry = bucket;
                    }
                    return 1;
                }
            }
            bucket = node;
            node = node->next;
        }

        if (depth == 0)
        {
            return 0;
        }
    } while (true);
}

ResourceHashNode& Resource_GetHashRoot()
{
    return hash_root;
}

} // namespace igi::retail::resource
