#include "igi/retail/model/iff_chunk_tree.hpp"
#include "igi/retail/model/iff_skeleton.hpp"
#include "igi/retail/model/mef_bone_rig.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

namespace
{

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// The retail game data, when it is installed.
const std::filesystem::path kGameRoot = "/Users/haseeb-mir/Downloads/IGI";

// Writes a big-endian IFF form with the given children.
void WriteBigEndian32(std::vector<unsigned char>& bytes, std::uint32_t value)
{
    bytes.push_back(static_cast<unsigned char>((value >> 24) & 0xFFU));
    bytes.push_back(static_cast<unsigned char>((value >> 16) & 0xFFU));
    bytes.push_back(static_cast<unsigned char>((value >> 8) & 0xFFU));
    bytes.push_back(static_cast<unsigned char>(value & 0xFFU));
}

void WriteLittleEndian32(std::vector<unsigned char>& bytes, std::uint32_t value)
{
    bytes.push_back(static_cast<unsigned char>(value & 0xFFU));
    bytes.push_back(static_cast<unsigned char>((value >> 8) & 0xFFU));
    bytes.push_back(static_cast<unsigned char>((value >> 16) & 0xFFU));
    bytes.push_back(static_cast<unsigned char>((value >> 24) & 0xFFU));
}

void WriteTag(std::vector<unsigned char>& bytes, const char* tag)
{
    for (int index = 0; index < 4; ++index) {
        bytes.push_back(static_cast<unsigned char>(tag[index]));
    }
}

// Builds a "BOBJ" form holding a "BOBH" form with the three skeleton chunks.
void WriteSkeletonIff(const std::filesystem::path& path, std::uint32_t object_id,
                       const std::vector<std::int32_t>& parents,
                       const std::vector<std::array<float, 3>>& translations)
{
    std::vector<unsigned char> bosh;
    WriteLittleEndian32(bosh, object_id);
    WriteLittleEndian32(bosh, static_cast<std::uint32_t>(parents.size()));

    std::vector<unsigned char> plst;
    for (const std::int32_t parent : parents) {
        WriteLittleEndian32(plst, static_cast<std::uint32_t>(parent));
    }

    std::vector<unsigned char> tlst;
    for (const auto& translation : translations) {
        for (const float value : translation) {
            std::uint32_t bits = 0U;
            std::memcpy(&bits, &value, sizeof(bits));
            WriteLittleEndian32(tlst, bits);
        }
    }

    std::vector<unsigned char> bobh;
    for (const auto& chunk : {std::pair<const char*, const std::vector<unsigned char>*>("BOSH", &bosh),
                              std::pair<const char*, const std::vector<unsigned char>*>("PLST", &plst),
                              std::pair<const char*, const std::vector<unsigned char>*>("TLST", &tlst)}) {
        WriteTag(bobh, chunk.first);
        WriteBigEndian32(bobh, static_cast<std::uint32_t>(chunk.second->size()));
        bobh.insert(bobh.end(), chunk.second->begin(), chunk.second->end());
    }

    std::vector<unsigned char> body;
    WriteTag(body, "FORM");
    WriteBigEndian32(body, static_cast<std::uint32_t>(bobh.size() + 4U));
    WriteTag(body, "BOBH");
    body.insert(body.end(), bobh.begin(), bobh.end());

    std::vector<unsigned char> file;
    WriteTag(file, "FORM");
    WriteBigEndian32(file, static_cast<std::uint32_t>(body.size() + 4U));
    WriteTag(file, "BOBJ");
    file.insert(file.end(), body.begin(), body.end());

    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(file.data()), static_cast<std::streamsize>(file.size()));
}

void test_skeleton_reads_the_retail_chunks()
{
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "igi_iff_skeleton_test.iff";
    const std::vector<std::int32_t> parents{-1, 0, 0, 1};
    const std::vector<std::array<float, 3>> translations{
        {0.0F, 0.0F, 100.0F}, {1.0F, 2.0F, 3.0F}, {4.0F, 5.0F, 6.0F}, {7.0F, 8.0F, 9.0F}};
    WriteSkeletonIff(path, 7U, parents, translations);

    const auto skeleton = igi::retail::model::IffSkeleton_Read(path);
    require(skeleton.has_value(), "the skeleton is read from the retail chunks");
    require(skeleton->object_id == 7U, "the BOSH object id is read");
    require(skeleton->BoneCount() == 4U, "the BOSH bone count is read");
    require(skeleton->parents == parents, "the PLST parent chain is read");
    require(skeleton->translations[1][2] == 3.0F, "the TLST rest offsets are read");
    // The world position is the bone's own offset plus every ancestor's.
    require(skeleton->world_positions[1][2] == 103.0F, "the world position adds the ancestors");
    require(skeleton->world_positions[2][2] == 106.0F, "a root child keeps its own offset");
    std::filesystem::remove(path);
}

void test_retail_skeletons_are_read()
{
    const std::filesystem::path anims = kGameRoot / "common" / "anims";
    if (!std::filesystem::exists(anims)) {
        std::cout << "retail animation data is not installed; the retail check is skipped\n";
        return;
    }
    int read = 0;
    for (const auto& entry : std::filesystem::directory_iterator(anims)) {
        if (entry.path().extension() != ".iff" && entry.path().extension() != ".IFF") {
            continue;
        }
        const auto skeleton = igi::retail::model::IffSkeleton_Read(entry.path());
        require(skeleton.has_value(), "every retail animation file carries a skeleton");
        require(skeleton->BoneCount() > 0U, "the retail skeleton has bones");
        for (const std::int32_t parent : skeleton->parents) {
            require(parent < static_cast<std::int32_t>(skeleton->BoneCount()),
                    "every parent index is inside the bone list");
        }
        ++read;
    }
    require(read >= 6, "the retail animation folder holds the character skeletons");
    std::cout << "retail skeletons read: " << read << '\n';
}

} // namespace

int main()
{
    test_skeleton_reads_the_retail_chunks();
    test_retail_skeletons_are_read();
    std::cout << "igi_retail_iff_skeleton_tests passed\n";
    return 0;
}
