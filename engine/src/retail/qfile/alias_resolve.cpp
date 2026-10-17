#include "igi/retail/qfile/alias_resolve.hpp"

#include "igi/retail/error/error_show.hpp"

#include <cstring>

namespace igi::retail::qfile {
namespace {

DeviceEntry device_table[device_entry_count]{};
char resolved_alias[0x100]{};

} // namespace

// 0x004B1020
std::int32_t QFile_AliasResolve(char* destination, const char* alias, std::int32_t depth)
{
    for (std::size_t index = 0; index < device_entry_count; ++index)
    {
        DeviceEntry& entry = device_table[index];
        if (entry.active != 0)
        {
            const std::size_t name_length = std::strlen(entry.name);
            if (std::strncmp(alias, entry.name, name_length) == 0)
            {
                char* cursor = entry.alias;
                for (std::int32_t step = depth; step != 0; --step)
                {
                    char* separator = std::strchr(cursor, ';');
                    if (separator == nullptr)
                    {
                        error::Error_Show("Error in alias path: %s", alias);
                        return 0;
                    }
                    cursor = separator + 1;
                }

                const bool has_remainder = std::strchr(cursor, ';') != nullptr;
                const std::int32_t result = has_remainder ? depth + 1 : 0;

                char* out = destination;
                char character = *cursor;
                while (character != ';' && character != '\0')
                {
                    *out = character;
                    ++out;
                    ++cursor;
                    character = *cursor;
                }

                std::strcpy(out, alias + name_length);
                std::strcpy(resolved_alias, destination);
                return result;
            }
        }
    }

    std::strcpy(destination, alias);
    std::strcpy(resolved_alias, destination);
    return 0;
}

DeviceEntry* QFile_GetDeviceTable()
{
    return device_table;
}

const char* QFile_GetResolvedAlias()
{
    return resolved_alias;
}

} // namespace igi::retail::qfile
