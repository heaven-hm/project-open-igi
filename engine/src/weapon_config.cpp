#include "igi/weapon_config.hpp"

#include <cstdlib>

namespace igi::gameplay {

namespace {

void trim_token(std::string& text) {
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t')) {
        text.erase(text.begin());
    }
    while (!text.empty() &&
           (text.back() == ' ' || text.back() == '\t' || text.back() == ';')) {
        text.pop_back();
    }
    if (text.size() >= 2 && text.front() == '"' && text.back() == '"') {
        text = text.substr(1, text.size() - 2);
    }
}

std::vector<std::string> split_top_level(const std::string& args) {
    std::vector<std::string> parts;
    std::string current;
    bool in_quotes = false;
    int depth = 0;
    for (char c : args) {
        if (c == '"') {
            in_quotes = !in_quotes;
            current.push_back(c);
        } else if (!in_quotes && c == '(') {
            ++depth;
            current.push_back(c);
        } else if (!in_quotes && c == ')') {
            --depth;
            current.push_back(c);
        } else if (!in_quotes && c == ',' && depth == 0) {
            trim_token(current);
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    trim_token(current);
    parts.push_back(current);
    return parts;
}

double parse_double(const std::string& token) {
    if (token.empty()) {
        return 0.0;
    }
    char* end = nullptr;
    const double value = std::strtod(token.c_str(), &end);
    return (end == token.c_str()) ? 0.0 : value;
}

long parse_long(const std::string& token) {
    if (token.empty()) {
        return 0;
    }
    char* end = nullptr;
    const long value = std::strtol(token.c_str(), &end, 10);
    return (end == token.c_str()) ? 0 : value;
}

bool decode_weapon_record(const std::string& call_text, RetailWeaponConfig& out) {
    const std::string::size_type open = call_text.find('(');
    const std::string::size_type close = call_text.rfind(')');
    if (open == std::string::npos || close == std::string::npos || close <= open) {
        return false;
    }
    const std::vector<std::string> args =
        split_top_level(call_text.substr(open + 1, close - open - 1));
    // Need at least the gun-model field (index 32) for a usable record.
    if (args.size() < 33 || args[1] != "WeaponConfig") {
        return false;
    }
    RetailWeaponConfig parsed;
    parsed.display_name = args[2];
    parsed.weapon_id = static_cast<int>(parse_long(args[3]));
    parsed.script_id = args[4];
    parsed.mass_grams = static_cast<int>(parse_long(args[11]));
    parsed.calibre_id = static_cast<int>(parse_long(args[12]));
    parsed.damage_factor = static_cast<float>(parse_double(args[13]));
    parsed.penetration_power = static_cast<float>(parse_double(args[14]));
    parsed.reload_seconds = static_cast<float>(parse_double(args[15]));
    parsed.muzzle_velocity = static_cast<float>(parse_double(args[16]));
    parsed.bullets_per_round = static_cast<int>(parse_long(args[17]));
    parsed.rounds_per_minute = static_cast<int>(parse_long(args[18]));
    parsed.rounds_per_clip = static_cast<int>(parse_long(args[19]));
    parsed.max_rounds_per_burst = static_cast<int>(parse_long(args[20]));
    parsed.min_spread = static_cast<float>(parse_double(args[21]));
    parsed.max_spread = static_cast<float>(parse_double(args[22]));
    parsed.fixed_recoil_x = static_cast<float>(parse_double(args[23]));
    parsed.fixed_recoil_z = static_cast<float>(parse_double(args[24]));
    parsed.random_recoil_x = static_cast<float>(parse_double(args[25]));
    parsed.random_recoil_z = static_cast<float>(parse_double(args[26]));
    parsed.weapon_class = args[27];
    parsed.effective_range = static_cast<float>(parse_double(args[28]));
    parsed.gun_model = args[32];
    if (args.size() > 33) {
        parsed.casing_model = args[33];
    }
    if (parsed.script_id.empty() || parsed.rounds_per_clip <= 0) {
        return false;
    }
    out = parsed;
    return true;
}

} // namespace

std::size_t WeaponConfigRegistry::load_from_qsc_text(const std::string& qsc_text) {
    // NOTE: weapon records nest inside a WeaponConfigContainer call, so spans
    // are extracted per Task_New occurrence (nested spans included) instead
    // of consuming to the outer balance.
    std::vector<std::string::size_type> call_sites;
    for (std::string::size_type at = qsc_text.find("Task_New(");
         at != std::string::npos;
         at = qsc_text.find("Task_New(", at + 1)) {
        call_sites.push_back(at);
    }
    std::size_t accepted = 0;
    for (const auto call : call_sites) {
        int depth = 0;
        bool in_quotes = false;
        std::string::size_type end = call;
        for (; end < qsc_text.size(); ++end) {
            const char c = qsc_text[end];
            if (c == '"') {
                in_quotes = !in_quotes;
            } else if (!in_quotes && c == '(') {
                ++depth;
            } else if (!in_quotes && c == ')') {
                --depth;
                if (depth == 0) {
                    ++end;
                    break;
                }
            }
        }
        RetailWeaponConfig entry;
        if (decode_weapon_record(qsc_text.substr(call, end - call), entry)) {
            entries_by_script_id_[entry.script_id] = entry;
            ++accepted;
        }
    }
    return accepted;
}

const RetailWeaponConfig* WeaponConfigRegistry::find_by_script_id(
    const std::string& script_id) const noexcept {
    const auto it = entries_by_script_id_.find(script_id);
    return it != entries_by_script_id_.end() ? &it->second : nullptr;
}

} // namespace igi::gameplay
