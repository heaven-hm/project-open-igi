#include "igi/native_registry.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string_view>

int main(int argc, char** argv) {
    auto& registry = igi::natives::NativeRegistry::instance();
    if (!registry.is_loaded()) {
        std::cerr << "Error: Could not locate IGINatives_VERIFIED.json in default search paths.\n";
        return 1;
    }

    if (argc < 2 || std::string_view(argv[1]) == "--stats") {
        const auto stats = registry.stats();
        std::cout << "=========================================================\n"
                  << " Project Open IGI - Native Engine Registry (C++20)\n"
                  << "=========================================================\n"
                  << " Total Verified Natives : " << stats.total_natives << " (100.00% BN Binary View Coverage)\n"
                  << " CFG Gate Passed (>=0.85): " << stats.pass_count << "\n"
                  << " CFG Gate Failed (Thunks): " << stats.fail_count << "\n"
                  << " Candidates              : " << stats.candidate_count << "\n"
                  << " L5 Runtime Verified     : " << stats.l5_runtime_count << " (requires CrossOver replay)\n"
                  << "---------------------------------------------------------\n"
                  << " Subsystem Breakdown (10 Game Engine Subsystems):\n";
        for (const auto& sub : stats.subsystems) {
            std::cout << "   " << std::left << std::setw(16) << sub.name
                      << ": " << std::right << std::setw(5) << sub.total
                      << "  (PASS: " << std::setw(5) << sub.pass
                      << ", FAIL: " << std::setw(4) << sub.fail << ")\n";
        }
        std::cout << "=========================================================\n";
        return 0;
    }

    const std::string_view arg1(argv[1]);
    if (arg1 == "--subsystem" && argc >= 3) {
        const std::string_view sub_name(argv[2]);
        const auto matches = registry.find_by_subsystem(sub_name);
        std::cout << "Subsystem [" << sub_name << "] matches (" << matches.size() << " natives):\n";
        for (const auto* n : matches) {
            std::cout << "  0x" << std::hex << n->address << std::dec
                      << " | " << std::left << std::setw(38) << n->name
                      << " | score=" << std::fixed << std::setprecision(2) << n->score
                      << " | " << (n->gate == igi::natives::CfgGate::Pass ? "PASS" : "FAIL")
                      << " | " << n->proto << "\n";
        }
        return 0;
    }

    if (arg1 == "--name" && argc >= 3) {
        const std::string_view name(argv[2]);
        const auto* n = registry.find_by_name(name);
        if (!n) {
            std::cerr << "Native not found with name: " << name << "\n";
            return 1;
        }
        std::cout << "Found Native:\n"
                  << "  Address    : 0x" << std::hex << n->address << std::dec << "\n"
                  << "  Name       : " << n->name << "\n"
                  << "  Subsystem  : " << n->subsystem << "\n"
                  << "  Prototype  : " << n->proto << "\n"
                  << "  CC         : " << n->cc << "\n"
                  << "  Gate       : " << (n->gate == igi::natives::CfgGate::Pass ? "PASS" : "FAIL") << "\n"
                  << "  CFG Score  : " << n->score << "\n"
                  << "  Confidence : " << n->confidence << "\n"
                  << "  Layers     : L0=" << n->layers.l0_static
                  << ", L1=" << n->layers.l1_cfg
                  << ", L2=" << n->layers.l2_signature
                  << ", L3=" << n->layers.l3_link_str
                  << ", L4=" << n->layers.l4_memory
                  << ", L5=" << n->layers.l5_runtime << "\n";
        return 0;
    }

    // Default: treat argument as address (hex or decimal)
    char* endp = nullptr;
    uint32_t addr = 0;
    if (arg1.starts_with("0x") || arg1.starts_with("0X")) {
        addr = static_cast<uint32_t>(std::strtoul(argv[1] + 2, &endp, 16));
    } else {
        addr = static_cast<uint32_t>(std::strtoul(argv[1], &endp, 16));
    }

    const auto* n = registry.find_by_address(addr);
    if (!n) {
        std::cerr << "Native not found at address: " << arg1 << " (0x" << std::hex << addr << std::dec << ")\n";
        return 1;
    }

    std::cout << "Found Native:\n"
              << "  Address    : 0x" << std::hex << n->address << std::dec << "\n"
              << "  Name       : " << n->name << "\n"
              << "  Subsystem  : " << n->subsystem << "\n"
              << "  Prototype  : " << n->proto << "\n"
              << "  CC         : " << n->cc << "\n"
              << "  Gate       : " << (n->gate == igi::natives::CfgGate::Pass ? "PASS" : "FAIL") << "\n"
              << "  CFG Score  : " << n->score << "\n"
              << "  Confidence : " << n->confidence << "\n";
    return 0;
}
