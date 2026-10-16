#include "igi/magic_object_registry.hpp"

namespace igi::gameplay {

namespace {

void trim_quotes(std::string& text) {
    if (text.size() >= 2 && text.front() == '"' && text.back() == '"') {
        text = text.substr(1, text.size() - 2);
    }
}

bool split_define_magic_obj(const std::string& line, MagicObjectEntry& out) {
    const std::string::size_type open = line.find('(');
    const std::string::size_type close = line.rfind(')');
    if (open == std::string::npos || close == std::string::npos || close <= open) {
        return false;
    }
    const std::string args = line.substr(open + 1, close - open - 1);
    std::vector<std::string> parts;
    std::string current;
    bool in_quotes = false;
    for (char c : args) {
        if (c == '"') {
            in_quotes = !in_quotes;
            current.push_back(c);
        } else if (c == ',' && !in_quotes) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    parts.push_back(current);
    // Retail calls are variable-arity (e.g. EXPLODEMAGICOBJ carries
    // explosion/sound parameters after the task type); the first three
    // fields (name, model, task type) are always present.
    if (parts.size() < 3) {
        return false;
    }
    auto trim = [](std::string s) {
        while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
            s.erase(s.begin());
        }
        while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == ';')) {
            s.pop_back();
        }
        return s;
    };
    out.name = trim(parts[0]);
    out.model_id = trim(parts[1]);
    out.task_type = trim(parts[2]);
    trim_quotes(out.name);
    trim_quotes(out.model_id);
    return !out.model_id.empty() && !out.task_type.empty();
}

} // namespace

std::size_t MagicObjectRegistry::load_from_qsc_text(const std::string& qsc_text) {
    std::size_t accepted = 0;
    std::string::size_type cursor = 0;
    while (cursor < qsc_text.size()) {
        const std::string::size_type next = qsc_text.find('\n', cursor);
        std::string line = qsc_text.substr(cursor, next == std::string::npos ? next : next - cursor);
        cursor = (next == std::string::npos) ? qsc_text.size() : next + 1;
        if (line.find("DefineMagicObj") == std::string::npos) {
            continue;
        }
        MagicObjectEntry entry;
        if (split_define_magic_obj(line, entry)) {
            entries_by_model_[entry.model_id] = entry;
            ++accepted;
        }
    }
    return accepted;
}

const MagicObjectEntry* MagicObjectRegistry::find_by_model(const std::string& model_id) const noexcept {
    const auto it = entries_by_model_.find(model_id);
    return it != entries_by_model_.end() ? &it->second : nullptr;
}

std::size_t MagicObjectRegistry::count_by_task(const std::string& task_type) const noexcept {
    std::size_t count = 0;
    for (const auto& [model, entry] : entries_by_model_) {
        if (entry.task_type == task_type) {
            ++count;
        }
    }
    return count;
}

} // namespace igi::gameplay
