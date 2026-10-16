#include "igi/native_registry.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace igi::natives {

std::string_view NativeRegistry::deduce_subsystem(std::string_view name) {
    std::string prefix;
    const auto underscore_pos = name.find('_');
    if (underscore_pos != std::string_view::npos) {
        prefix = std::string(name.substr(0, underscore_pos));
    } else {
        prefix = std::string(name);
    }

    std::string p_lower;
    p_lower.reserve(prefix.size());
    for (char c : prefix) {
        p_lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    std::string lower;
    lower.reserve(name.size());
    for (char c : name) {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    const auto p_has = [&](std::string_view needle) noexcept {
        return p_lower.find(needle) != std::string::npos;
    };
    const auto has = [&](std::string_view needle) noexcept {
        return lower.find(needle) != std::string::npos;
    };

    // Sound Engine
    if (p_has("sound") || p_has("ambient") || p_has("audio") || p_has("sfx") ||
        p_has("music") || p_has("voice") || p_has("listener") || p_has("wave") ||
        p_has("dsp") || p_has("eax") || p_has("sample") || p_has("directsound") ||
        p_has("gosound")) {
        return "Sound";
    }
    if (has("sound") && !p_has("humanplayer") && !p_has("humanai")) {
        return "Sound";
    }

    // Weather & Environment Engine
    if ((p_has("weather") || p_has("rain") || p_has("snow") || p_has("sky") ||
         p_has("cloud") || p_has("fog") || p_has("flatwater") || p_has("water") ||
         p_has("terrain") || p_has("lightmap") || p_has("environment") ||
         p_has("wind")) && !p_has("window")) {
        return "Weather";
    }
    if (has("weather") || has("raineffect") || has("raindyncube") ||
        has("skytask") || has("flatwater") || has("terrainlightmap")) {
        return "Weather";
    }

    // Weapon Engine
    if (p_has("weapon") || p_has("gun") || p_has("bullet") || p_has("ammo") ||
        p_has("recoil") || p_has("crosshair") || p_has("mine") || p_has("grenade") ||
        p_has("laser") || p_has("m16") || p_has("mp5") || p_has("ak47") ||
        p_has("dragunov") || p_has("glock") || p_has("colt") || p_has("uzi") ||
        p_has("spas") || p_has("minimi") || p_has("jackhammer") || p_has("m2hb") ||
        p_has("proximitymine") || p_has("shotgun") || p_has("rifle") || p_has("pistol")) {
        return "Weapon";
    }
    if (has("weapontype") || has("gundragunov") || has("gunm16") ||
        has("gunmp5") || has("gunm2hb") || has("proximitymine") || has("bullet")) {
        return "Weapon";
    }

    // Animation Engine
    if (p_has("anim") || p_has("bone") || p_has("skeleton") || p_has("spline") ||
        p_has("pose") || p_has("keyframe") || p_has("morph") || p_has("motion")) {
        return "Animation";
    }

    // Script / QVM Engine
    if (p_has("qvm") || p_has("qtask") || p_has("script") || p_has("opcode") ||
        p_has("bytecode") || p_has("vm")) {
        return "Script/QVM";
    }

    // AI Engine
    if (p_has("humanai") || p_has("combat") || p_has("patrol") || p_has("guard") ||
        p_has("sight") || p_has("hearing") || p_has("waypoint") || p_has("navigation") ||
        p_has("enemy") || p_has("ai")) {
        return "AI";
    }

    // Physics / Vehicle Engine
    if (p_has("physics") || p_has("vehicle") || p_has("rigid") || p_has("collision") ||
        p_has("contact") || p_has("dynamic") || p_has("gravity") || p_has("velocity") ||
        p_has("momentum") || p_has("friction") || p_has("raycast") || p_has("sphere") ||
        p_has("box") || p_has("body") || p_has("train") || p_has("truck") ||
        p_has("tank") || p_has("jeep") || p_has("car") || p_has("helicopter")) {
        return "Physics";
    }

    // UI / Menu Engine
    if (p_has("menu") || p_has("hud") || p_has("status") || p_has("statusscreen") ||
        p_has("textbox") || p_has("inputbox") || p_has("button") || p_has("dialog") ||
        p_has("cursor") || p_has("inventory") || p_has("briefing") || p_has("compass") ||
        p_has("window")) {
        return "UI/Menu";
    }

    // Render / Display Engine
    if (p_has("render") || p_has("display") || p_has("direct3d") || p_has("directdraw") ||
        p_has("mesh") || p_has("texture") || p_has("shader") || p_has("camera") ||
        p_has("viewport") || p_has("vertex") || p_has("material") || p_has("color") ||
        p_has("draw") || p_has("sprite") || p_has("font") || p_has("bitmap") ||
        p_has("gfx") || p_has("light")) {
        return "Render";
    }

    // Fallbacks
    if (has("sound")) return "Sound";
    if (has("recoil") || has("fire") || has("reload")) return "Weapon";
    if (has("physics") || has("collision")) return "Physics";
    if (has("anim")) return "Animation";
    if (has("render") || has("draw")) return "Render";
    if (has("menu") || has("window")) return "UI/Menu";
    if (has("ai")) return "AI";

    return "System/Math";
}

namespace {

static std::string extract_string_field(const std::string& block, std::string_view key) {
    const std::string search = "\"" + std::string(key) + "\":";
    auto pos = block.find(search);
    if (pos == std::string::npos) return {};
    pos += search.size();
    while (pos < block.size() && std::isspace(static_cast<unsigned char>(block[pos]))) {
        pos++;
    }
    if (pos >= block.size() || block[pos] != '"') return {};
    pos++;
    std::string val;
    while (pos < block.size()) {
        if (block[pos] == '\\' && pos + 1 < block.size()) {
            val.push_back(block[pos + 1]);
            pos += 2;
        } else if (block[pos] == '"') {
            break;
        } else {
            val.push_back(block[pos]);
            pos++;
        }
    }
    return val;
}

static uint32_t extract_uint_field(const std::string& block, std::string_view key) {
    const std::string search = "\"" + std::string(key) + "\":";
    auto pos = block.find(search);
    if (pos == std::string::npos) return 0;
    pos += search.size();
    while (pos < block.size() && std::isspace(static_cast<unsigned char>(block[pos]))) {
        pos++;
    }
    char* end = nullptr;
    const unsigned long val = std::strtoul(block.c_str() + pos, &end, 10);
    return static_cast<uint32_t>(val);
}

static float extract_float_field(const std::string& block, std::string_view key) {
    const std::string search = "\"" + std::string(key) + "\":";
    auto pos = block.find(search);
    if (pos == std::string::npos) return 0.0f;
    pos += search.size();
    while (pos < block.size() && std::isspace(static_cast<unsigned char>(block[pos]))) {
        pos++;
    }
    char* end = nullptr;
    const double val = std::strtod(block.c_str() + pos, &end);
    return static_cast<float>(val);
}

static bool extract_bool_field(const std::string& block, std::string_view key) {
    const std::string search = "\"" + std::string(key) + "\":";
    auto pos = block.find(search);
    if (pos == std::string::npos) return false;
    pos += search.size();
    while (pos < block.size() && std::isspace(static_cast<unsigned char>(block[pos]))) {
        pos++;
    }
    if (block.compare(pos, 4, "true") == 0) return true;
    return false;
}

} // namespace

NativeRegistry::NativeRegistry() = default;

NativeRegistry::NativeRegistry(const std::filesystem::path& catalog_path) {
    (void)load_from_json(catalog_path);
}

bool NativeRegistry::load_from_json(const std::filesystem::path& catalog_path) {
    std::ifstream file(catalog_path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    const auto natives_pos = content.find("\"natives\":");
    if (natives_pos == std::string::npos) {
        return false;
    }

    entries_.clear();
    entries_.reserve(6000);

    std::size_t idx = content.find('[', natives_pos);
    if (idx == std::string::npos) return false;
    idx++;

    while (idx < content.size()) {
        const auto start = content.find('{', idx);
        if (start == std::string::npos) break;

        // Find closing brace of this object, handling nested braces (like layers)
        int depth = 0;
        std::size_t end = start;
        for (; end < content.size(); ++end) {
            if (content[end] == '{') depth++;
            else if (content[end] == '}') {
                depth--;
                if (depth == 0) {
                    end++;
                    break;
                }
            }
        }
        if (depth != 0) break;

        const std::string obj = content.substr(start, end - start);
        NativeEntry entry;
        entry.hash = extract_string_field(obj, "hash");
        entry.name = extract_string_field(obj, "name");
        entry.proto = extract_string_field(obj, "proto");
        entry.cc = extract_string_field(obj, "cc");
        entry.address = extract_uint_field(obj, "addr");
        if (entry.address == 0 && !entry.hash.empty()) {
            char* endp = nullptr;
            entry.address = static_cast<uint32_t>(std::strtoul(entry.hash.c_str(), &endp, 16));
        }

        const std::string gate_str = extract_string_field(obj, "gate");
        if (gate_str == "PASS") entry.gate = CfgGate::Pass;
        else if (gate_str == "FAIL") entry.gate = CfgGate::Fail;
        else if (gate_str == "CANDIDATE") entry.gate = CfgGate::Candidate;
        else entry.gate = CfgGate::Unknown;

        entry.score = extract_float_field(obj, "score");
        entry.confidence = extract_string_field(obj, "confidence");

        entry.layers.l0_static = extract_bool_field(obj, "L0_static");
        entry.layers.l1_cfg = extract_bool_field(obj, "L1_cfg");
        entry.layers.l2_signature = extract_bool_field(obj, "L2_signature");
        entry.layers.l3_link_str = extract_bool_field(obj, "L3_link_str");
        entry.layers.l4_memory = extract_bool_field(obj, "L4_memory");
        entry.layers.l5_runtime = extract_bool_field(obj, "L5_runtime");

        entry.subsystem = std::string(deduce_subsystem(entry.name));

        entries_.push_back(std::move(entry));
        idx = end;
    }

    sort_and_index();
    return !entries_.empty();
}

void NativeRegistry::sort_and_index() {
    std::sort(entries_.begin(), entries_.end(), [](const NativeEntry& a, const NativeEntry& b) {
        return a.address < b.address;
    });
}

const NativeEntry* NativeRegistry::find_by_address(uint32_t address) const {
    const auto it = std::lower_bound(entries_.begin(), entries_.end(), address,
        [](const NativeEntry& item, uint32_t addr) {
            return item.address < addr;
        });
    if (it != entries_.end() && it->address == address) {
        return &(*it);
    }
    return nullptr;
}

const NativeEntry* NativeRegistry::find_by_name(std::string_view name) const {
    for (const auto& entry : entries_) {
        if (entry.name == name) {
            return &entry;
        }
    }
    return nullptr;
}

std::vector<const NativeEntry*> NativeRegistry::find_by_subsystem(std::string_view subsystem) const {
    std::vector<const NativeEntry*> results;
    results.reserve(256);
    for (const auto& entry : entries_) {
        if (entry.subsystem == subsystem) {
            results.push_back(&entry);
        }
    }
    return results;
}

RegistryStats NativeRegistry::stats() const {
    RegistryStats res{};
    res.total_natives = entries_.size();

    std::unordered_map<std::string, SubsystemStats> sub_map;

    for (const auto& entry : entries_) {
        if (entry.gate == CfgGate::Pass) res.pass_count++;
        else if (entry.gate == CfgGate::Fail) res.fail_count++;
        else if (entry.gate == CfgGate::Candidate) res.candidate_count++;

        if (entry.layers.l5_runtime) res.l5_runtime_count++;

        auto& s = sub_map[entry.subsystem];
        s.name = entry.subsystem;
        s.total++;
        if (entry.gate == CfgGate::Pass) s.pass++;
        else if (entry.gate == CfgGate::Fail) s.fail++;
    }

    for (auto& [_, val] : sub_map) {
        res.subsystems.push_back(std::move(val));
    }
    std::sort(res.subsystems.begin(), res.subsystems.end(), [](const auto& a, const auto& b) {
        return a.total > b.total;
    });

    return res;
}

NativeRegistry& NativeRegistry::instance() {
    static NativeRegistry registry;
    if (!registry.is_loaded()) {
        const std::vector<std::filesystem::path> search_paths = {
            "assets/BinaryNinja/IGINatives_VERIFIED.json",
            "BinaryNinja/IGINatives_VERIFIED.json",
            "../assets/BinaryNinja/IGINatives_VERIFIED.json",
            "../../assets/BinaryNinja/IGINatives_VERIFIED.json",
            "/Users/haseeb-mir/Documents/Code/project-open-igi/assets/BinaryNinja/IGINatives_VERIFIED.json",
            "/Users/haseeb-mir/Documents/Code/project-open-igi/BinaryNinja/IGINatives_VERIFIED.json",
            "/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/IGINatives_VERIFIED.json"
        };
        for (const auto& path : search_paths) {
            if (std::filesystem::exists(path)) {
                if (registry.load_from_json(path)) {
                    break;
                }
            }
        }
    }
    return registry;
}

} // namespace igi::natives
