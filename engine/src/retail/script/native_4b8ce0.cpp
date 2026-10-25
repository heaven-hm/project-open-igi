// Native: Qvm_RenderNodeText_4B8CE0; retail VA 0x004B8CE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B8CE0-0x004B8FE9 (778 bytes,
//   41 blocks, 47 edges, 262 instructions), bytes re-read from the image (81 ec 18 08 00 00 53 56 8b b4 24 24 08 00 00 57 85 f6 75 ..
//   0f be 46 01 83 f8 19 77 .. f6 80 00 90 4b 00 .. ff 24 95 ec 8f 4b 00 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image plus a linear disassembly walk of all 262 instructions; the 26-byte index table at
//   0x004B9000 and the two jump tables at 0x004B8FEC (5 entries) and 0x004B901C (6 entries) were read as raw bytes/dwords and verified
//   byte-exactly (docs/fidelity/evidence/qvm-core-dependency-table.md section 10). radare2 reports 41 blocks / 47 edges, 2104-byte frame.
// Behavior/CFG: cdecl with THREE stack arguments ([esp+4] expression node, [esp+8] destination buffer, [esp+0xC] destination size in bytes).
//   A caller that wants a kept stack word may push it before these three: 0x004B9040 pushes (count-1, scar, 0x800, node) and cleans only twelve
//   bytes at 0x004B907A, using the surviving count word as strncpy's count - that fourth word is not an argument of this function. Null node
//   writes the two bytes 20 00 from 0x005474D8 into the buffer and returns 0 (0x004B8CF4-0x004B8D10). Dispatch 1: signed byte node[1]
//   (0x004B8D11), above 0x19 goes to the syntax-error report (0x004B8FBD), otherwise a 26-byte slot index from 0x004B9000 selects one of five
//   blocks through 0x004B8FEC. Slot 0 is entered for node[1] == 0 and dispatches again on node[0] (0x004B8D5E) through 0x004B901C after
//   rejecting values outside 1..6 (0x004B8E79 writes the two-byte marker and returns 0). Slot 1 (node[1] == 1) is string concatenation: the
//   node[0x18] child is rendered into a 0x800-byte frame buffer and copied into the destination with the MSVC strncpy at 0x004A5500, the
//   node[0x1C] child is rendered the same way and appended with the MSVC strncat at 0x005169B0, and the destination is terminated at size-1.
//   Slot 2 (node[1] == 24) renders node[0x18] and then node[0x1C] into the same destination and returns the second result. Slot 3 (node[1] ==
//   25) is the assignment, structured exactly like the integer evaluator's case 25: null target or target kind other than 4/5 calls Debug_Error
//   ("Internal script error. Invalid assignment, line %d") and enters the retail `jmp $` self-loop; target kind 5 resolves through 0x004BABB0;
//   resolved dword 0 == 0 stores via 0x004B9040 and returns the rendered node[0x1C]; == 2 reports "Expression: Cannot assign to constant, line
//   %d"; any other non-zero reports "Syntax error in assignment, line %d". Slot 4 covers node[1] == 2..23 and the >0x19 default and reports
//   "Syntax error in string expression, line %d". Under slot 0: node[0] 1 and 3 report "Numeric member in string expression. Line %d." and
//   write the marker; 2 copies the string at node[0x08] with strncpy and terminates; 4 and 5 resolve the symbol record at node[0x08] (creating
//   it with 0x004BABB0 from node[0x10]), call the record node's handler at +0x10 indirectly with a 20-byte frame buffer, take the string at
//   result+0x10 and copy it into the destination; 6 renders node[0x08] into a 0x800-byte frame buffer and copies it. Every error path ends by
//   writing the two bytes 20 00 into the destination and returning 0.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the instruction listing (41 blocks/47 edges, all slot and dispatch targets
//   matched); runtime UNOBSERVED. Link: needs Qvm_ReportError 0x004B7E10, Debug_Error 0x004AF7B0, Script_QHashSymbolNodeCreate_4BABB0
//   0x004BABB0, Qvm_StoreArgumentValue_4B9040 0x004B9040, the MSVC strncpy at 0x004A5500 and the MSVC strncat at 0x005169B0 - all present
//   (the two CRT routines are host `std::strncpy`/`std::strncat`, see docs/fidelity/evidence/qvm-4a5500-strncpy.md and the strncat note in
//   qvm-core-dependency-table.md). No callee is missing. Retail quirks kept: the concatenation slot can write up to 2*(size-1)+1 bytes into a
//   destination of `size` and then forces destination[size-1] = 0; the two-byte marker is a single 16-bit store of 0x0020; node[0] 4/5 leaves
//   the handler buffer uninitialized. The identifier error prefers node[0x18][0x10] and falls back to node[0x10], as in 0x004B9120.
#include <cstdint>
#include <cstring>

#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/script/qvm_native_loader.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BABB0 - symbol record creation (transcribed in native_4babb0.cpp); its out value is the symbol record, not the name.
void* NATIVE_DECL Script_QHashSymbolNodeCreate_4BABB0(const char* name);
// 0x004B9040 - type-directed store helper (transcribed in native_4b9040.cpp).
void NATIVE_DECL Qvm_StoreArgumentValue_4B9040(const void* node, void* target_object);
char* NATIVE_DECL Qvm_RenderNodeText_4B8CE0(const void* node, char* buffer, std::uint32_t buffer_size);

char* NATIVE_DECL Qvm_RenderNodeText_4B8CE0(const void* node, char* buffer, std::uint32_t buffer_size)
{
    const std::uint8_t* const expression = static_cast<const std::uint8_t*>(node);            // esi throughout, as in retail
    if (expression == nullptr) {                                                              // 0x004B8CF0/0x004B8CF2
        *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                                  // 0x004B8CFB/0x004B8D02: bytes 20 00
        return nullptr;                                                                       // 0x004B8D05
    }

    std::int32_t const operation = static_cast<std::int8_t>(expression[1]);                   // 0x004B8D11: movsbl 0x1(%esi)
    if (static_cast<std::uint32_t>(operation) > 0x19U) {                                      // 0x004B8D15/0x004B8D18: cmp/ja
        igi::retail::script::Qvm_ReportError("Syntax error in string expression, line %d",
                                             *reinterpret_cast<const std::int32_t*>(expression + 0x24));   // 0x004B8FBD-0x004B8FC6
        *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                                  // 0x004B8FCE-0x004B8FDD
        return nullptr;                                                                       // 0x004B8FE0
    }

    switch (operation) {
    case 0: {                                                                                 // 0x004B8D5E: second dispatch
        std::int32_t const subtype = static_cast<std::int8_t>(expression[0]);                 // 0x004B8D5E: movsbl (%esi)
        if (static_cast<std::uint32_t>(subtype - 1) > 5U) {                                   // 0x004B8D61/0x004B8D62/0x004B8D65
            *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                              // 0x004B8E79-0x004B8E89
            return nullptr;                                                                   // 0x004B8E87
        }
        switch (subtype) {
        case 1:
        case 3:
            // 0x004B8D72: numeric member in a string expression.
            igi::retail::script::Qvm_ReportError("Numeric member in string expression. Line %d.",
                                                 *reinterpret_cast<const std::int32_t*>(expression + 0x24));   // 0x004B8D72-0x004B8D7B
            *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                              // 0x004B8D87-0x004B8D91
            return nullptr;                                                                   // 0x004B8D94
        case 2: {
            // 0x004B8DA0: copy the literal string at node[0x08].
            const std::uint32_t count = buffer_size - 1U;                                     // 0x004B8DAA: lea -0x1(%edi),%edx
            std::strncpy(buffer, *reinterpret_cast<const char* const*>(expression + 8), count);   // 0x004B8E57/0x004B8E60 (retail 0x004A5500)
            buffer[buffer_size - 1U] = 0;                                                     // 0x004B8E68
            return buffer;                                                                    // 0x004B8E6D
        }
        case 4:
        case 5: {                                                                             // 0x004B8DB3: symbol value as text
            if (*reinterpret_cast<void* const*>(expression + 8) == nullptr) {                 // 0x004B8DB6/0x004B8DB8
                void* const created = Script_QHashSymbolNodeCreate_4BABB0(
                    *reinterpret_cast<const char* const*>(expression + 0x10));                // 0x004B8DBA-0x004B8DBE
                *reinterpret_cast<void**>(const_cast<std::uint8_t*>(expression) + 8) = created;   // 0x004B8DC6
            }
            const std::uint8_t* symbol = *reinterpret_cast<const std::uint8_t* const*>(expression + 8);   // 0x004B8DC9
            const std::uint8_t* resolved = nullptr;
            if (symbol != nullptr) {                                                          // 0x004B8DCE
                resolved = *reinterpret_cast<const std::uint8_t* const*>(symbol + 8);         // 0x004B8DD0
            }
            if (resolved == nullptr) {                                                        // 0x004B8DD5 -> 0x004B8DFB
                const std::uint8_t* const name_node = *reinterpret_cast<const std::uint8_t* const*>(expression + 0x18);
                const char* const identifier = (name_node != nullptr)
                    ? *reinterpret_cast<const char* const*>(name_node + 0x10)                 // 0x004B8E02-0x004B8E0A
                    : *reinterpret_cast<const char* const*>(expression + 0x10);               // 0x004B8E1C-0x004B8E24
                igi::retail::script::Qvm_ReportError("Unknown identifier '%s' in expression, line %d",
                                                     identifier,
                                                     *reinterpret_cast<const std::int32_t*>(expression + 0x24));   // 0x004B8E0F/0x004B8E29
                *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                          // 0x004B8FCE
                return nullptr;
            }
            std::int32_t const selector = *reinterpret_cast<const std::int32_t*>(expression + 0x0C);   // 0x004B8DDE
            void* const handler_argument = *reinterpret_cast<void* const*>(resolved + 0x18);           // 0x004B8DE5
            std::uint32_t handler_buffer[5];                                                  // 0x004B8DE9: 20-byte local at frame top
            using HandlerFn = void* (*)(void*, void*, std::int32_t);
            HandlerFn const handler = reinterpret_cast<HandlerFn>(
                *reinterpret_cast<void* const*>(resolved + 0x10));                            // 0x004B8DEF: call *0x10(%eax)
            void* const result = handler(handler_buffer, handler_argument, selector);         // (buffer, resolved+0x18, node+0x0C)
            const char* const text = *reinterpret_cast<const char* const*>(
                static_cast<const std::uint8_t*>(result) + 0x10);                             // 0x004B8DF2
            const std::uint32_t count = buffer_size - 1U;                                     // 0x004B8DE1
            std::strncpy(buffer, text, count);                                                // 0x004B8E58/0x004B8E60
            buffer[buffer_size - 1U] = 0;                                                     // 0x004B8E68
            return buffer;
        }
        case 6: {
            // 0x004B8E36: render the node[0x08] child into a 0x800-byte frame buffer, then copy it.
            char scratch[0x800];
            char* const rendered = Qvm_RenderNodeText_4B8CE0(
                *reinterpret_cast<void* const*>(expression + 8), scratch, 0x800U);            // 0x004B8E36-0x004B8E4F
            std::strncpy(buffer, rendered, buffer_size - 1U);                                 // 0x004B8E57/0x004B8E60
            buffer[buffer_size - 1U] = 0;                                                     // 0x004B8E68
            return buffer;
        }
        default:
            *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                              // 0x004B8E79
            return nullptr;
        }
    }
    case 1: {                                                                                 // 0x004B8E96: string concatenation
        char scratch[0x800];
        char* const first = Qvm_RenderNodeText_4B8CE0(
            *reinterpret_cast<void* const*>(expression + 0x18), scratch, 0x800U);             // 0x004B8EA8-0x004B8EB0
        const std::uint32_t count = buffer_size - 1U;                                         // 0x004B8EA5
        std::strncpy(buffer, first, count);                                                   // 0x004B8EC1 (retail 0x004A5500)
        char* const second = Qvm_RenderNodeText_4B8CE0(
            *reinterpret_cast<void* const*>(expression + 0x1C), scratch, 0x800U);             // 0x004B8ED0-0x004B8ED8
        std::strncat(buffer, second, count);                                                  // 0x004B8EE2 (retail 0x005169B0)
        buffer[buffer_size - 1U] = 0;                                                         // 0x004B8EEA
        return buffer;                                                                        // 0x004B8EEF
    }
    case 24:
        // 0x004B8D2D: sequence - render node[0x18] into the destination, then node[0x1C] into the same destination.
        static_cast<void>(Qvm_RenderNodeText_4B8CE0(
            *reinterpret_cast<void* const*>(expression + 0x18), buffer, buffer_size));         // 0x004B8D3E-0x004B8D41
        return Qvm_RenderNodeText_4B8CE0(
            *reinterpret_cast<void* const*>(expression + 0x1C), buffer, buffer_size);          // 0x004B8D49-0x004B8D4C
    case 25: {                                                                                // 0x004B8EFC: assignment
        const std::uint8_t* const target = *reinterpret_cast<const std::uint8_t* const*>(expression + 0x18);   // 0x004B8EFC
        if (target == nullptr) {                                                              // 0x004B8EFF/0x004B8F01
            *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                              // 0x004B8FCE
            return nullptr;
        }
        std::int32_t const target_kind = static_cast<std::int8_t>(target[0]);                 // 0x004B8F07: movsbl (%eax)
        if (target_kind != 4 && target_kind != 5) {                                           // 0x004B8F09/0x004B8F0E/0x004B8F11
            Debug_Error("Internal script error. Invalid assignment, line %d",
                        *reinterpret_cast<const std::int32_t*>(expression + 0x24));           // 0x004B8F13-0x004B8F1C
            for (;;) {                                                                        // 0x004B8F24: jmp $ self-loop
            }
        }
        if (target_kind == 5) {                                                               // 0x004B8F26
            if (*reinterpret_cast<void* const*>(target + 8) == nullptr) {                     // 0x004B8F2B/0x004B8F30
                void* const created = Script_QHashSymbolNodeCreate_4BABB0(
                    *reinterpret_cast<const char* const*>(target + 0x10));                    // 0x004B8F32-0x004B8F36
                *reinterpret_cast<void**>(const_cast<std::uint8_t*>(
                    *reinterpret_cast<const std::uint8_t* const*>(expression + 0x18)) + 8) = created;   // 0x004B8F3B/0x004B8F41
            }
        }
        const std::uint8_t* const symbol = *reinterpret_cast<const std::uint8_t* const*>(target + 8);    // 0x004B8F47
        const std::uint8_t* resolved = nullptr;
        if (symbol != nullptr) {                                                              // 0x004B8F4C
            resolved = *reinterpret_cast<const std::uint8_t* const*>(symbol + 8);             // 0x004B8F4E
        }
        if (resolved == nullptr) {                                                            // 0x004B8F53 -> 0x004B8FA6
            igi::retail::script::Qvm_ReportError("Unknown identifier '%s' in expression, line %d",
                                                 *reinterpret_cast<const char* const*>(target + 0x10),   // 0x004B8FA9
                                                 *reinterpret_cast<const std::int32_t*>(expression + 0x24));   // 0x004B8FA6
            *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                              // 0x004B8FCE
            return nullptr;
        }
        std::int32_t const resolved_kind = *reinterpret_cast<const std::int32_t*>(resolved);  // 0x004B8F55
        if (resolved_kind == 0) {                                                             // 0x004B8F57/0x004B8F59
            Qvm_StoreArgumentValue_4B9040(*reinterpret_cast<void* const*>(expression + 0x1C),
                                          const_cast<std::uint8_t*>(resolved));               // 0x004B8F60
            return Qvm_RenderNodeText_4B8CE0(*reinterpret_cast<void* const*>(expression + 0x1C),
                                             buffer, buffer_size);                            // 0x004B8F79
        }
        if (resolved_kind == 2) {                                                             // 0x004B8F8B/0x004B8F8E
            igi::retail::script::Qvm_ReportError("Expression: Cannot assign to constant, line %d",
                                                 *reinterpret_cast<const std::int32_t*>(expression + 0x24));   // 0x004B8F94
            *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                              // 0x004B8FCE
            return nullptr;
        }
        igi::retail::script::Qvm_ReportError("Syntax error in assignment, line %d",
                                             *reinterpret_cast<const std::int32_t*>(expression + 0x24));       // 0x004B8F9F
        *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;                                  // 0x004B8FCE
        return nullptr;
    }
    default:
        // 0x004B8FBD: slot 4, covering node[1] == 2..23.
        igi::retail::script::Qvm_ReportError("Syntax error in string expression, line %d",
                                             *reinterpret_cast<const std::int32_t*>(expression + 0x24));
        *reinterpret_cast<std::uint16_t*>(buffer) = 0x0020U;
        return nullptr;
    }
}

} // extern "C"

#undef NATIVE_DECL
