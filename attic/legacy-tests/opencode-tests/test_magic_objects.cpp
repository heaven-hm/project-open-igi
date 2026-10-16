#include "test_framework.hpp"

#include "igi/magic_object_registry.hpp"
#include "igi/mission_world.hpp"
#include "parsers/qvm_decompiler.h"
#include "parsers/qvm_parser.h"

#include <filesystem>

using namespace igi::test;
using igi::gameplay::MagicObjectRegistry;

TEST_CASE(MagicObjects, ParsesDefineMagicObjLines) {
    MagicObjectRegistry registry;
    const std::string text =
        "DefineMagicObj(\"108_02_1\", \"108_02_1\", TASKTYPE_GUNCLIP);\n"
        "DefineMagicObj(\"410_03_1\", \"410_03_1\", TASKTYPE_GLASS);\n"
        "SomeOtherCall(1, 2);\n"
        "DefineMagicObj(\"broken\", );\n";
    ASSERT_EQ(registry.load_from_qsc_text(text), 2u);
    ASSERT_EQ(registry.entry_count(), 2u);
    const auto* clip = registry.find_by_model("108_02_1");
    ASSERT_TRUE(clip != nullptr);
    ASSERT_TRUE(clip->task_type == "TASKTYPE_GUNCLIP");
    ASSERT_TRUE(clip->name == "108_02_1");
    ASSERT_TRUE(registry.find_by_model("nope") == nullptr);
    ASSERT_EQ(registry.count_by_task("TASKTYPE_GUNCLIP"), 1u);
    ASSERT_EQ(registry.count_by_task("TASKTYPE_GLASS"), 1u);
    ASSERT_EQ(registry.count_by_task("TASKTYPE_GUNFLAME"), 0u);
}

TEST_CASE(MagicObjects, EmptyTextLoadsNothing) {
    MagicObjectRegistry registry;
    ASSERT_EQ(registry.load_from_qsc_text(""), 0u);
    ASSERT_EQ(registry.entry_count(), 0u);
}

// Retail ground truth: MAGICOBJ.QVM decompiles to 374 DefineMagicObj calls
// (verified with igi_qvm_dump during development).
TEST_CASE(MagicObjects, RetailMagicObjCatalogShape) {
    const std::filesystem::path retail_qvm = "/Users/haseeb-mir/Downloads/IGI/MAGICOBJ/MAGICOBJ.QVM";
    if (!std::filesystem::exists(retail_qvm)) {
        return; // Skip if retail files are not installed
    }
    const QVMFile parsed = QVM_Parse(retail_qvm.string());
    ASSERT_TRUE(parsed.valid);
    MagicObjectRegistry registry;
    ASSERT_EQ(registry.load_from_qsc_text(QVM_DecompileToString(parsed)), 374u);
    const auto* clip = registry.find_by_model("108_02_1");
    ASSERT_TRUE(clip != nullptr);
    ASSERT_TRUE(clip->task_type == "TASKTYPE_GUNCLIP");
    const auto* glass = registry.find_by_model("410_03_1");
    ASSERT_TRUE(glass != nullptr);
    ASSERT_TRUE(glass->task_type == "TASKTYPE_GLASS");
    ASSERT_TRUE(registry.count_by_task("TASKTYPE_MAGICOBJ") > 100u);
}

TEST_CASE(MagicObjects, RetailAttaMountsPreservedOnModels) {
    const std::filesystem::path retail_root = "/Users/haseeb-mir/Downloads/IGI";
    if (!std::filesystem::exists(retail_root)) {
        return; // Skip if retail files are not installed
    }
    igi::gameplay::MissionWorld world;
    ASSERT_TRUE(world.load_from_retail_install(retail_root, 1));
    // Find any loaded retail model that carries ATTA mounts and prove they
    // survived the conversion into the model store.
    const igi::gameplay::ModelMesh* mounted = nullptr;
    for (const auto& placed : world.placed_objects()) {
        if (placed.model_id.empty()) {
            continue;
        }
        const auto* mesh = world.find_model(placed.model_id);
        if (mesh != nullptr && !mesh->attachments.empty()) {
            mounted = mesh;
            break;
        }
    }
    ASSERT_TRUE(mounted != nullptr);
    ASSERT_TRUE(!mounted->attachments.front().name.empty());
}
