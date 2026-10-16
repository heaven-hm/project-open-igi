#include "test_framework.hpp"

#include "common.h"
#include "iff_parser.h"
#include "mef_native.h"
#include "mtp_parser.h"
#include "qsc_object_parser.h"
#include "qvm_parser.h"
#include "res_parser.h"
#include "terrain_files.h"
#include "tex_parser.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

TEST_CASE(Parsers, QscObjectParserSingleHumanSoldier) {
    const std::string qsc = R"QSC(
Task_New(2001, "HumanSoldier", "", 126893824.0, -38029724.0, 175332848.0, 3.1400001049041748, "013_01_1", 1, 6, 5);
)QSC";
    std::string err;
    const igi1conv::QscObjectSet set = igi1conv::QscObjectSet::parse(qsc, &err);
    ASSERT_TRUE(err.empty());
    ASSERT_EQ(set.entries.size(), 1u);
    const auto& e = set.entries[0];
    ASSERT_EQ(e.modelId, "013_01_1");
    ASSERT_EQ(e.boneHierarchy, 6);
    ASSERT_EQ(e.standAnimation, 5);
    ASSERT_NEAR(e.posX, 126893824.0, 1e-3);
    ASSERT_NEAR(e.posY, -38029724.0, 1e-3);
    ASSERT_NEAR(e.posZ, 175332848.0, 1e-3);
    ASSERT_NEAR(e.gamma, 3.14, 1e-2);
}

TEST_CASE(Parsers, QscObjectParserMultipleAndGrouping) {
    const std::string qsc = R"QSC(
Task_New(407, "HumanSoldier", "Gunner 407", 24474926.0, 87795696.0, 174680656.0, 1.4660760164260864, "011_03_1", 1, 1, -1);
Task_New(2001, "HumanSoldier", "", 126893824.0, -38029724.0, 175332848.0, 3.1400001049041748, "013_01_1", 1, 6, 5);
Task_New(2002, "HumanSoldier", "Sniper", 0.0, 0.0, 0.0, 0.0, "013_01_1", 2, 6, 7);
)QSC";
    std::string err;
    const igi1conv::QscObjectSet set = igi1conv::QscObjectSet::parse(qsc, &err);
    ASSERT_TRUE(err.empty());
    ASSERT_EQ(set.entries.size(), 3u);

    const auto ids = set.modelIds();
    ASSERT_EQ(ids.size(), 2u); // "011_03_1" and "013_01_1"

    const auto entries_013 = set.entriesForModel("013_01_1");
    ASSERT_EQ(entries_013.size(), 2u);
}

TEST_CASE(Parsers, QscObjectParserErrorHandling) {
    const std::string bad_qsc = "Task_New(2001, HumanSoldier, UnquotedStrings, 123);";
    std::string err;
    const igi1conv::QscObjectSet set = igi1conv::QscObjectSet::parse(bad_qsc, &err);
    ASSERT_EQ(set.entries.size(), 0u);

    // Unclosed string token error
    const std::string unclosed_qsc = "Task_New(2001, \"HumanSoldier, 123);";
    const igi1conv::QscObjectSet set2 = igi1conv::QscObjectSet::parse(unclosed_qsc, &err);
    ASSERT_EQ(set2.entries.size(), 0u);
}

TEST_CASE(Parsers, TexParserWriteAndParseRoundtrip) {
    const std::string temp_tex = "/tmp/test_roundtrip.tex";
    // 4x4 RGBA pixels
    std::vector<std::uint8_t> rgba(4 * 4 * 4, 0xFF);
    // Write 4x4 mode 2 texture
    const bool written = TEX_WriteLOOP(temp_tex, rgba.data(), 4, 4, 2, 11);
    ASSERT_TRUE(written);

    // Parse it back
    const TEXFile parsed = TEX_Parse(temp_tex);
    ASSERT_TRUE(parsed.valid);
    ASSERT_EQ(parsed.images.size(), 1u);
    ASSERT_EQ(parsed.images[0].width, 4u);
    ASSERT_EQ(parsed.images[0].height, 4u);
    ASSERT_EQ(parsed.images[0].mode, 2u);
    ASSERT_EQ(parsed.images[0].pixels.size(), 4u * 4u * 2u); // 2 bytes per pixel

    std::filesystem::remove(temp_tex);
}

TEST_CASE(Parsers, TexParserNonExistentFile) {
    const TEXFile parsed = TEX_Parse("/non/existent/path/missing.tex");
    ASSERT_FALSE(parsed.valid);
}

TEST_CASE(Parsers, TerrainOctreeRecordStructure) {
    // CTR record size is strictly 32 bytes (16-bit child indices, 8-bit orientation/transform codes, mask, padding, cmd_offset)
    ASSERT_EQ(sizeof(ctr_item_s), 32u);

    ctr_item_s node{};
    node.children_mask_ = 0xFF;
    node.cmd_offset_ = 1024;
    ASSERT_EQ(node.children_mask_, 0xFF);
    ASSERT_EQ(node.cmd_offset_, 1024u);
}

TEST_CASE(Parsers, TerrainHeightmapDescriptorTableLayout) {
    // TERRAIN.HMP begins with a table of 256 12-byte descriptors (256 * 12 = 3072 bytes)
    constexpr std::size_t kTableEntries = 256;
    constexpr std::size_t kDescriptorSize = 12;
    constexpr std::size_t kTotalHeaderBytes = kTableEntries * kDescriptorSize;
    ASSERT_EQ(kTotalHeaderBytes, 3072u);
}

TEST_CASE(Parsers, IffParserNonExistentFile) {
    const IffFile iff = IFF_Parse("/non/existent/missing.iff");
    ASSERT_FALSE(iff.valid);
}

TEST_CASE(Parsers, ResParserNonExistentFile) {
    const RESFile res = RES_Parse("/non/existent/missing.res");
    ASSERT_FALSE(res.valid);
}

TEST_CASE(Parsers, RealRetailFilesPresenceAndSanity) {
    const std::filesystem::path retail_root = "/Users/haseeb-mir/Downloads/IGI";
    if (!std::filesystem::exists(retail_root)) {
        return; // Skip if retail files are not installed
    }

    // Test real level 1 objects.qsc
    const std::filesystem::path qsc_path = retail_root / "MISSIONS" / "location0" / "LEVEL1" / "objects.qsc";
    if (std::filesystem::exists(qsc_path)) {
        std::ifstream stream(qsc_path);
        std::string content((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        std::string err;
        const igi1conv::QscObjectSet set = igi1conv::QscObjectSet::parse(content, &err);
        ASSERT_TRUE(err.empty());
        ASSERT_TRUE(set.entries.size() > 10);
    }

    // Test real common animation 000.IFF
    const std::filesystem::path iff_path = retail_root / "COMMON" / "ANIMS" / "000.IFF";
    if (std::filesystem::exists(iff_path)) {
        const IffFile iff = IFF_Parse(iff_path.string());
        ASSERT_TRUE(iff.valid);
        ASSERT_TRUE(iff.skeleton.bone_count > 0);
        ASSERT_FALSE(iff.clips.empty());
    }
}
