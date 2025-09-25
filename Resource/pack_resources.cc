// Copyright (c) 2025 渟雲. All rights reserved.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <format>
#include <filesystem>
#include <algorithm>
#include <ranges>
// #include <print> bullsh1t gcc
// ctrl+h std::print -> std::cout << std::format
#include "../miniz.h"

namespace fs = std::filesystem;

constexpr std::string_view kResourcePack = "resources.pak";
constexpr std::string_view kPackSignature = "KONATA_PAK_v1";
constexpr size_t kMaxFilenameSize = 32;

#pragma pack(push, 1)
struct PackHeader {
    char signature[16];
    uint32_t file_count;
    uint32_t reserved{};
};

struct FileEntry {
    char filename[kMaxFilenameSize]{};
    uint32_t offset{};
    uint32_t size{};
    uint32_t compressed_size{};
    uint32_t crc32{};
    uint8_t flags{};
};
#pragma pack(pop)

struct ResourceFile {
    std::string_view pack_name;
    std::string_view disk_path;
};

constexpr std::array<ResourceFile, 6> kFilesToPack = {{
    {"dsktop.jpg", "dsktop.jpg"},
    {"startup.wav", "startup.wav"},
    {"shutdown.wav", "shutdown.wav"},
    {"deep1.wav", "deep1.wav"},
    {"deep2.wav", "deep2.wav"},
    {"deep3.wav", "deep3.wav"}
}};

class ResourcePacker {
private:
    std::vector<uint8_t> read_file(const std::string& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cout << std::format("cant open: {}\n", path);
            return {};
        }
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            std::cout << std::format("cant read: {}\n", path);
            return {};
        }
        return buffer;
    }

    std::vector<uint8_t> compress_data(const std::vector<uint8_t>& input) {
        if (input.empty()) return {};
        mz_ulong compressed_size = mz_compressBound(input.size());
        std::vector<uint8_t> compressed(compressed_size);
        int result = mz_compress(compressed.data(), &compressed_size,
                                 input.data(), input.size());
        if (result != MZ_OK) {
            std::cout << std::format("compress failed\n");
            return {};
        }
        compressed.resize(compressed_size);
        return compressed;
    }

    void write_pack_header(std::ofstream& pack_file, uint32_t file_count) {
        PackHeader header{};
        std::copy_n(kPackSignature.begin(),
                    std::min(kPackSignature.size(), sizeof(header.signature) - 1),
                    header.signature);
        header.file_count = file_count;
        pack_file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    }

    void write_file_entry(std::ofstream& pack_file, const FileEntry& entry) {
        pack_file.write(reinterpret_cast<const char*>(&entry), sizeof(entry));
    }

public:
    bool create_resource_pack() {

        std::ofstream pack_file(std::string(kResourcePack), std::ios::binary);
        if (!pack_file.is_open()) {
            return false;
        }

        pack_file.seekp(sizeof(PackHeader) + sizeof(FileEntry) * kFilesToPack.size());

        std::vector<FileEntry> entries;
        uint32_t current_offset = sizeof(PackHeader) + sizeof(FileEntry) * kFilesToPack.size();

        for (const auto& file_info : kFilesToPack) {
            if (!fs::exists(std::string(file_info.disk_path))) {
                std::cout << std::format(" {} skipped\n", file_info.disk_path);
                continue;
            }
            auto original_data = read_file(std::string(file_info.disk_path));
            if (original_data.empty()) continue;

            FileEntry entry{};
            std::copy_n(file_info.pack_name.begin(),
                        std::min(file_info.pack_name.size(), kMaxFilenameSize - 1),
                        entry.filename);

            entry.size = static_cast<uint32_t>(original_data.size());
            entry.crc32 = mz_crc32(MZ_CRC32_INIT, original_data.data(), original_data.size());

            auto compressed_data = compress_data(original_data);

            if (!compressed_data.empty() && compressed_data.size() < original_data.size()) {
                entry.compressed_size = static_cast<uint32_t>(compressed_data.size());
                entry.flags = 1;
                pack_file.write(reinterpret_cast<const char*>(compressed_data.data()),
                                compressed_data.size());
                current_offset += compressed_data.size();
            } else {
                entry.compressed_size = entry.size;
                entry.flags = 0;
                pack_file.write(reinterpret_cast<const char*>(original_data.data()),
                                original_data.size());
                current_offset += original_data.size();
            }

            entry.offset = current_offset - entry.compressed_size;
            entries.push_back(entry);

            std::cout << std::format("size: {} -> {} ({})\n",
                       entry.size, entry.compressed_size,
                       entry.flags ? "compressed" : "uncompressed");
        }

        if (entries.empty()) {
            std::cout << std::format("empty.............??\n");
            return false;
        }

        pack_file.seekp(sizeof(PackHeader));
        for (const auto& entry : entries) {
            write_file_entry(pack_file, entry);
        }

        pack_file.seekp(0);
        write_pack_header(pack_file, static_cast<uint32_t>(entries.size()));

        pack_file.close();

        std::cout << std::format("\nlist:\n");
        for (const auto& entry : entries) {
            std::cout << std::format("  {}: {} bytes -> {} bytes\n",
                       entry.filename, entry.size, entry.compressed_size);
        }
        return true;
    }

};

auto main() -> int{
    std::cout << std::format("XP Konata resource packer\n");
    ResourcePacker packer;
    return packer.create_resource_pack();
}
