#include "igi/retail/model/model_texture_package.hpp"
#include "igi/retail/resource/resource_archive.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using igi::retail::model::MtpFormatError;
using igi::retail::model::MtpMaterialReference;
using igi::retail::model::MtpPackage;

void write_u32(std::vector<std::byte>& bytes, std::size_t offset, std::uint32_t value)
{
    for (std::size_t byte_index = 0U; byte_index < sizeof(value); ++byte_index) {
        bytes[offset + byte_index] =
            static_cast<std::byte>((value >> (byte_index * 8U)) & 0xFFU);
    }
}

void write_big_endian_u32(std::vector<std::byte>& bytes, std::size_t offset, std::uint32_t value)
{
    for (std::size_t byte_index = 0U; byte_index < sizeof(value); ++byte_index) {
        const std::size_t shift = (sizeof(value) - byte_index - 1U) * 8U;
        bytes[offset + byte_index] = static_cast<std::byte>((value >> shift) & 0xFFU);
    }
}

void append_chunk(
    std::vector<std::byte>& destination,
    std::uint32_t tag,
    std::span<const std::byte> payload)
{
    const std::size_t chunk_offset = destination.size();
    const std::size_t payload_size = payload.size();
    const std::size_t padded_payload_size = (payload_size + 3U) & ~std::size_t{3U};
    destination.resize(chunk_offset + 8U + padded_payload_size);
    write_big_endian_u32(destination, chunk_offset, tag);
    write_big_endian_u32(destination, chunk_offset + 4U, static_cast<std::uint32_t>(payload_size));
    std::copy(payload.begin(), payload.end(), destination.begin()
        + static_cast<std::ptrdiff_t>(chunk_offset + 8U));
}

void require(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

std::vector<std::byte> bytes_from_string(std::string_view value)
{
    std::vector<std::byte> bytes;
    bytes.reserve(value.size());
    for (const char character : value) {
        bytes.push_back(static_cast<std::byte>(static_cast<unsigned char>(character)));
    }
    return bytes;
}

std::vector<std::byte> make_fixture_mtp()
{
    std::vector<std::byte> bytes(12U);
    write_big_endian_u32(bytes, 0U, 0x494C4646U);
    write_big_endian_u32(bytes, 8U, 0x4D545031U);

    std::vector<std::byte> mods_payload(4U);
    write_u32(mods_payload, 0U, 1U);
    const std::vector<std::byte> model_name = bytes_from_string(std::string_view("model_a.mef\0", 12U));
    mods_payload.insert(mods_payload.end(), model_name.begin(), model_name.end());
    append_chunk(bytes, 0x4D4F4453U, mods_payload);

    std::vector<std::byte> vnam_payload(8U);
    write_u32(vnam_payload, 0U, 1U);
    write_u32(vnam_payload, 4U, 0U);
    const std::vector<std::byte> instance_name =
        bytes_from_string(std::string_view("placed_model\0", 13U));
    vnam_payload.insert(vnam_payload.end(), instance_name.begin(), instance_name.end());
    append_chunk(bytes, 0x564E414DU, vnam_payload);

    std::vector<std::byte> inst_payload(16U);
    write_u32(inst_payload, 0U, 0U);
    write_u32(inst_payload, 4U, 1U);
    write_u32(inst_payload, 8U, 0U);
    append_chunk(bytes, 0x494E5354U, std::span<const std::byte>(inst_payload).first(12U));

    std::vector<std::byte> gtt_payload(12U);
    write_u32(gtt_payload, 0U, 1U);
    write_u32(gtt_payload, 4U, 3U);
    write_u32(gtt_payload, 8U, 5U);
    append_chunk(bytes, 0x47545420U, gtt_payload);

    write_big_endian_u32(bytes, 4U, static_cast<std::uint32_t>(bytes.size() - 8U));
    return bytes;
}

[[nodiscard]] std::vector<std::byte> read_file(const std::filesystem::path& file_path)
{
    std::ifstream input_stream(file_path, std::ios::binary | std::ios::ate);
    if (!input_stream) {
        throw std::runtime_error("Could not open Level 1 MTP file: " + file_path.string());
    }
    const std::streamoff file_size = input_stream.tellg();
    if (file_size < 0) {
        throw std::runtime_error("Could not determine MTP size: " + file_path.string());
    }
    std::vector<char> raw_bytes(static_cast<std::size_t>(file_size));
    input_stream.seekg(0, std::ios::beg);
    input_stream.read(raw_bytes.data(), static_cast<std::streamsize>(raw_bytes.size()));
    if (!input_stream) {
        throw std::runtime_error("Could not read complete MTP file: " + file_path.string());
    }

    std::vector<std::byte> file_bytes(raw_bytes.size());
    for (std::size_t byte_index = 0U; byte_index < raw_bytes.size(); ++byte_index) {
        file_bytes[byte_index] =
            static_cast<std::byte>(static_cast<unsigned char>(raw_bytes[byte_index]));
    }
    return file_bytes;
}

void test_native_mtp_instance_and_gtt_decoding()
{
    const std::vector<std::byte> file_bytes = make_fixture_mtp();
    const MtpPackage package = MtpPackage::DynObj_InsertHashItem_14(file_bytes);
    require(package.root_tag == 0x494C4646U, "MTP root tag is retained without interpreting it");
    require(package.model_names.size() == 1U && package.model_names[0U] == "model_a.mef",
        "MODS model names are loaded");
    require(package.instances.size() == 1U
            && package.instances[0U].instance_name == "placed_model"
            && package.instances[0U].model_index == 0U,
        "VNAM and INST records are joined in retail order");
    require(package.material_references.size() == 1U
            && package.material_references[0U].first_native_index == 3U
            && package.material_references[0U].second_native_index == 5U
            && package.instances[0U].material_reference_indices[0U] == 0U,
        "INST material reference resolves to the indexed GTT pair");
}

void test_rejects_truncated_mtp_chunks()
{
    std::vector<std::byte> file_bytes = make_fixture_mtp();
    file_bytes.resize(24U);
    bool rejected = false;
    try {
        static_cast<void>(MtpPackage::DynObj_InsertHashItem_14(file_bytes));
    } catch (const MtpFormatError&) {
        rejected = true;
    }
    require(rejected, "truncated MTP child payload is rejected");
}

void test_level_one_mtp_file(const std::filesystem::path& mtp_path)
{
    const std::vector<std::byte> file_bytes = read_file(mtp_path);
    const MtpPackage package = MtpPackage::DynObj_InsertHashItem_14(file_bytes);
    const std::filesystem::path level_path = mtp_path.parent_path();
    const igi::retail::resource::ResourceArchive model_archive =
        igi::retail::resource::Resource_Load(level_path / "MODELS" / "LEVEL1.RES");
    const igi::retail::resource::ResourceArchive texture_archive =
        igi::retail::resource::Resource_Load(level_path / "TEXTURES" / "LEVEL1.RES");

    std::size_t model_texture_count = 0U;
    for (const auto& entry : texture_archive.entries) {
        if (entry.name.ends_with(".tex")) {
            ++model_texture_count;
        }
    }
    require(model_texture_count > 0U, "Level 1 texture RES contains model TEX assets");

    std::size_t matched_model_count = 0U;
    for (const std::string& model_name : package.model_names) {
        std::string resource_name = model_name;
        if (!resource_name.starts_with("LOCAL:")) {
            resource_name = "LOCAL:models/" + resource_name;
        }
        if (!resource_name.ends_with(".mef")) {
            resource_name += ".mef";
        }
        if (model_archive.find(resource_name) != nullptr) {
            ++matched_model_count;
        } else {
            std::cerr << "MTP model name did not resolve in the Level 1 model RES: "
                      << model_name << '\n';
        }
    }
    require(matched_model_count == package.model_names.size(),
        "every MTP model name resolves to a MEF in the Level 1 model RES");

    for (const MtpMaterialReference& material_reference : package.material_references) {
        require(material_reference.first_native_index >= 0x2000U
                || material_reference.first_native_index < model_texture_count,
            "first GTT index resolves in the local texture table or retail global table");
        require(material_reference.second_native_index == 0xFFFFFFFFU
                || material_reference.second_native_index >= 0x2000U
                || material_reference.second_native_index < model_texture_count,
            "second GTT index resolves in the local texture table or retail global table");
    }

    std::size_t mapped_material_count = 0U;
    for (const auto& instance : package.instances) {
        mapped_material_count += instance.material_reference_indices.size();
    }

    std::cout << "Level 1 DynObj_InsertHashItem_14 package verified: file=" << mtp_path
              << "; model names=" << package.model_names.size()
              << "; matched MEFs=" << matched_model_count
              << "; model TEX files=" << model_texture_count
              << "; instances=" << package.instances.size()
              << "; GTT pairs=" << package.material_references.size()
              << "; instance material links=" << mapped_material_count << '\n';
}

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        test_native_mtp_instance_and_gtt_decoding();
        test_rejects_truncated_mtp_chunks();
        if (argument_count == 2) {
            test_level_one_mtp_file(arguments[1]);
        } else if (argument_count != 1) {
            throw std::runtime_error("Expected zero arguments or one Level 1 LEVEL1.MTP path.");
        }
        std::cout << "All retail MTP structure tests passed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Retail MTP test failed: " << exception.what() << '\n';
        return 1;
    }
}
