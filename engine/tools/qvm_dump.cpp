// Dumps a retail QVM file as decompiled QSC text for inspection.
// Usage: igi_qvm_dump <file.qvm> [output.txt]
#include "parsers/qvm_decompiler.h"
#include "parsers/qvm_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argument_count, char** arguments) {
    if (argument_count < 2) {
        std::cerr << "Usage: igi_qvm_dump <file.qvm> [output.txt]\n";
        return 1;
    }
    const std::string input_path = arguments[1];
    const QVMFile parsed = QVM_Parse(input_path);
    if (parsed.instructions.empty() && parsed.identifiers.empty() && parsed.strings.empty()) {
        std::cerr << "Failed to parse QVM file: " << input_path << '\n';
        return 1;
    }
    const std::string decompiled = QVM_DecompileToString(parsed);
    if (argument_count > 2) {
        std::ofstream out(arguments[2]);
        if (!out) {
            std::cerr << "Cannot open output file: " << arguments[2] << '\n';
            return 1;
        }
        out << decompiled;
    } else {
        std::cout << decompiled;
    }
    std::cerr << "identifiers=" << parsed.identifiers.size()
              << " strings=" << parsed.strings.size()
              << " instructions=" << parsed.instructions.size() << '\n';
    return 0;
}
