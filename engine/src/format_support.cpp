#include "igi/logger.hpp"

#include "logger.h"
#include "utils.h"

std::string Utils::Trim(const std::string& text)
{
    const auto first_character = text.find_first_not_of(" \r\n\t");
    if (first_character == std::string::npos)
    {
        return {};
    }
    return text.substr(first_character, text.find_last_not_of(" \r\n\t") - first_character + 1);
}
