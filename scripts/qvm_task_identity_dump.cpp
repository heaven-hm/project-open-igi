// Diagnostic file-conversion helper only. Serialize the existing host parser's
// Task_New instruction offsets and explicit integer IDs for trace reconciliation.
// This does not implement the retail task loop or claim parser equivalence.
#include "igi/retail/script/qvm_loader.hpp"
#include "igi/retail/script/qvm_object_calls.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: qvm-task-identity-dump OBJECTS.QVM\n";
        return 2;
    }
    try
    {
        igi::retail::resource::ResourceCache cache;
        const auto loaded = igi::retail::script::Host_LoadQvmScript(cache, argv[1]);
        const auto calls = igi::retail::script::Qvm_ReadObjectCalls(loaded.program());
        for (const auto& call : calls)
        {
            if (call->function_name != "Task_New" || call->arguments.empty()) continue;
            if (const auto* id = std::get_if<std::int32_t>(&call->arguments[0]))
                std::cout << call->instruction_offset << '\t' << *id << '\n';
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
