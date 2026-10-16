#include "igi/scripting/script_interpreter.hpp"
#include "igi/asset_io.hpp"

#include <array>
#include <iostream>
#include <stdexcept>

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count != 2)
        {
            throw std::runtime_error("Supply the original game installation directory");
        }
        const auto script_path = igi::assets::resolve_path(arguments[1], "missions/location0/level1/ai/2208.qvm");
        const auto parsed = QVM_Parse(script_path.string());
        if (!parsed.valid)
        {
            throw std::runtime_error(parsed.error);
        }
        for (const int event_type : {0, 4, 7, 8, 999})
        {
            igi::QvmNativeRegistry registry;
            registry.RegisterConstantByName("AIEVENT_CREATE", 0);
            registry.RegisterConstantByName("AIEVENT_IDLE", 4);
            registry.RegisterConstantByName("AIEVENT_COMBAT", 7);
            registry.RegisterConstantByName("AIEVENT_ALARMON", 8);
            registry.RegisterConstantByName("AIACTIONFLAG_NONE", 0);
            int default_calls = 0;
            int patrol_calls = 0;
            int alarm_control_identifier = -1;
            std::array<int, 3> patrol_arguments{};
            registry.RegisterFunctionByName("AIFunction_GetCurrentEventType",
                [event_type](auto&, const auto&) { return igi::QvmRuntimeValue::FromInt(event_type); });
            registry.RegisterFunctionByName("AIFunction_DefaultHandler",
                [&](auto&, const auto&)
                {
                    ++default_calls;
                    return igi::QvmRuntimeValue::FromInt(0);
                });
            registry.RegisterDeferredFunctionByName("AIFunction_SetAlarmControlID",
                [&](auto&, const igi::QvmNativeCallArguments& call_arguments)
                {
                    if (call_arguments.Count() != 1)
                    {
                        throw std::runtime_error("Original alarm registration has an unexpected argument count");
                    }
                    alarm_control_identifier = call_arguments.GetInt(0);
                    return igi::QvmRuntimeValue::FromInt(0);
                });
            registry.RegisterDeferredFunctionByName("AIAction_Patrol",
                [&](auto&, const igi::QvmNativeCallArguments& call_arguments)
                {
                    if (call_arguments.Count() != 3)
                    {
                        throw std::runtime_error("Original patrol call has an unexpected argument count");
                    }
                    ++patrol_calls;
                    for (std::size_t argument_index = 0; argument_index < 3; ++argument_index)
                    {
                        patrol_arguments[argument_index] = call_arguments.GetInt(argument_index);
                    }
                    return igi::QvmRuntimeValue::FromInt(0);
                });
            igi::QvmInterpreter interpreter(registry);
            igi::QvmProgram program;
            if (!interpreter.LoadProgram(parsed, program))
            {
                throw std::runtime_error(interpreter.GetLastError());
            }
            const auto context = interpreter.CreateContext(program);
            if (!context->Run())
            {
                throw std::runtime_error(context->GetLastError());
            }
            const bool patrol_event = event_type == 4 || event_type == 8;
            if (patrol_calls != (patrol_event ? 1 : 0) ||
                default_calls != (event_type == 0 || event_type == 7 || event_type == 999 ? 1 : 0) ||
                context->StackSize() != 0 ||
                alarm_control_identifier != (event_type == 0 ? 98 : -1) ||
                (patrol_event && (patrol_arguments[0] != (event_type == 4 ? 2408 : 2608) ||
                                 patrol_arguments[1] != 0 || patrol_arguments[2] != 0)))
            {
                throw std::runtime_error("Level 1 event dispatch or lazy argument evaluation diverged");
            }
            std::cout << "event=" << event_type << ", patrol=" << patrol_calls
                      << ", default=" << default_calls << ", route=" << patrol_arguments[0]
                      << ", stack=" << context->StackSize() << '\n';
        }
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Reused Level 1 QVM probe failed: " << exception.what() << '\n';
        return 1;
    }
}
