#pragma once

// Retail magic-object registry (data layer only).
// Decoded from `MAGICOBJ/MAGICOBJ.QVM` via QVM decompilation:
//   DefineMagicObj("<name>", "<model>", TASKTYPE_*);
// Maps effect/prop models (muzzle flames, clips, glass, ladders, ...) to
// their retail task type. No gameplay or rendering behavior reads this yet;
// it grounds future retail-exact effect handling in scripted data instead of
// hardcoded model lists. Zero OS APIs.

#include <string>
#include <unordered_map>
#include <vector>

namespace igi::gameplay {

struct MagicObjectEntry {
    std::string name;
    std::string model_id;
    std::string task_type;
};

class MagicObjectRegistry final {
public:
    // Parses decompiled QSC text; returns the number of entries accepted.
    std::size_t load_from_qsc_text(const std::string& qsc_text);

    void clear() noexcept { entries_by_model_.clear(); }

    [[nodiscard]] std::size_t entry_count() const noexcept { return entries_by_model_.size(); }
    [[nodiscard]] const MagicObjectEntry* find_by_model(const std::string& model_id) const noexcept;
    [[nodiscard]] std::size_t count_by_task(const std::string& task_type) const noexcept;

private:
    std::unordered_map<std::string, MagicObjectEntry> entries_by_model_;
};

} // namespace igi::gameplay
