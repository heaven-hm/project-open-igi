// Native: Qvm_EvaluateIntegerNode_4B9120; retail VA 0x004B9120 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B9120-0x004B95C3 (1188 bytes,
//   65 blocks, 52 edges, 484 instructions), bytes re-read from the image (83 ec 20 56 8b 74 24 28 85 f6 57 74 .. 0f be 46 01 83 f8 1d 77
//   .. ff 24 85 c4 95 4b 00 ... ; jump tables at 0x004B95C4 and 0x004B963C).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image plus a linear disassembly walk of all 484 instructions; the two dispatch tables were
//   read as 32-bit dwords from the image and their 30-entry / 6-entry target maps verified byte-exactly (docs/fidelity/evidence/
//   qvm-core-dependency-table.md section 10). radare2 reports 65 blocks / 52 edges, 3 arguments, 48-byte frame.
// Behavior/CFG: cdecl expression evaluator with one stack argument ([esp+4] expression node) returning a 32-bit integer in EAX. Null node
//   returns 0 (0x004B912B). Dispatch 1: signed byte node[1] (0x004B9131 `movsbl 0x1(%esi),%eax`), values outside 0..0x1D return 0
//   (0x004B9135/0x004B9138 unsigned `ja`), jump table 0x004B95C4 (30 entries). Dispatch 2 is entered only from node[1] == 0 (0x004B9160):
//   signed byte node[0] minus one, values above 5 return 0 (0x004B9163-0x004B9167), jump table 0x004B963C (6 entries). Every binary case
//   evaluates the node[0x1C] child FIRST and the node[0x18] child second (e.g. 0x004B928E/0x004B9299), and every binary case reloads node[0x18]
//   after the first recursive call; that order is preserved here. Case 0 subtype 4/5 resolves a symbol name: the cached record at node[0x08] is
//   created through 0x004BABB0 when null (0x004B918A-0x004B919D), the record's node at +0x08 is taken (0x004B91A7), its handler pointer at +0x10
//   is called indirectly with (20-byte stack buffer, node+0x18, node[0x0C]) at 0x004B91BB, and dword 0 of the returned pointer is the result
//   (0x004B91BE). Unresolvable names report "Unknown identifier '%s' in expression, line %d" through Qvm_ReportError (0x004B7E10) with the
//   identifier taken from node[0x18][0x10] when present and from node[0x10] otherwise (0x004B91C9-0x004B91F5). Case 25 is the assignment:
//   it rejects a null target (Debug_Error "Internal script error. Invalid assignment, line %d", then the retail's `jmp $` self-loop), accepts
//   target kinds 4 and 5 only, resolves kind 5 through 0x004BABB0 into target+0x08, stores through 0x004B9040 when the resolved dword 0 is 0
//   and returns the assignment's own value by evaluating node[0x1C] (0x004B9551-0x004B955F); resolved dword 0 == 2 reports "Expression: Cannot
//   assign to constant, line %d", any other non-zero value reports "Syntax error in assignment, line %d" (0x004B956D-0x004B959C). Case 6
//   sets the interpreter error flag 0x00A965A8 to 1 and returns 0 on a zero divisor (0x004B9313-0x004B931D). Cases 7, 21, 22, 23, 26 and every
//   out-of-range dispatch return 0. All four literal strings are byte-identical to the image (0x00547414, 0x005473F0, 0x00547444, 0x00547478).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the instruction listing (65 blocks/52 edges, all 30 + 6 dispatch targets matched);
//   runtime UNOBSERVED. Link: needs 0x004BAB80-side 0x004BABB0 (transcribed), 0x004B9040 (transcribed), Qvm_ReportError 0x004B7E10
//   (qvm_native_loader.cpp), Debug_Error 0x004AF7B0 (core/retail_error.cpp) and the error flag g_a965a8 (qvm_native_loader.cpp); no callee is
//   missing from the tree. Signed 32-bit wrap is reproduced with unsigned arithmetic (add/sub/mul/neg/shl) because retail wraps and C++ signed
//   overflow is undefined; the x87 `fistpll` conversion in case 0 subtype 3 is reproduced with std::llrint, which uses the same current
//   rounding mode; `idiv` overflow (INT_MIN / -1) stays unguarded exactly as retail. The 20-byte handler buffer is left uninitialized as retail
//   leaves it. The interpreter state object is not touched by this function; only the global error flag is written.
#include <cmath>
#include <cstdint>

#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/script/qvm_native_loader.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::script {

// 0x00A965A8 - "an error has been reported" flag, defined in qvm_native_loader.cpp.
extern volatile std::int32_t g_a965a8;

} // namespace igi::retail::script

extern "C" {

// 0x004B9040 - type-directed store helper (transcribed in native_4b9040.cpp).
void NATIVE_DECL Qvm_StoreArgumentValue_4B9040(const void* node, void* target_object);
// 0x004BABB0 - looks a name up and creates the symbol record node (transcribed in native_4babb0.cpp). Its out value is the symbol record, so a
// caller must not read the node it returns as holding the name string.
void* NATIVE_DECL Script_QHashSymbolNodeCreate_4BABB0(const char* name);

std::int32_t NATIVE_DECL Qvm_EvaluateIntegerNode_4B9120(const void* node_pointer)
{
    const std::uint8_t* const node = static_cast<const std::uint8_t*>(node_pointer);          // esi throughout, as in retail
    if (node == nullptr) {                                                                    // 0x004B9128/0x004B912B
        return 0;                                                                             // 0x004B91E5
    }

    std::int32_t const operation = static_cast<std::int8_t>(node[1]);                         // 0x004B9131: movsbl 0x1(%esi)
    if (static_cast<std::uint32_t>(operation) > 0x1DU) {                                      // 0x004B9135/0x004B9138: cmp/ja
        return 0;                                                                             // 0x004B91E5
    }

    switch (operation) {
    case 0: {                                                                                 // 0x004B9160: second dispatch
        std::int32_t const subtype = static_cast<std::int8_t>(node[0]);                       // 0x004B9160: movsbl (%esi)
        if (static_cast<std::uint32_t>(subtype - 1) > 5U) {                                   // 0x004B9162/0x004B9165: dec/cmp/ja
            return 0;                                                                         // 0x004B91E5
        }
        switch (subtype) {
        case 1:
            return *reinterpret_cast<const std::int32_t*>(node + 8);                          // 0x004B9170: mov 0x8(%esi),%eax
        case 2:
            return 0;                                                                         // 0x004B91E5
        case 3: {
            double const value = *reinterpret_cast<const double*>(node + 8);                  // 0x004B9179: fldl 0x8(%esi)
            std::int64_t const converted = std::llrint(value);                                // 0x004B917C: fistpll 0x8(%esp)
            return static_cast<std::int32_t>(converted);                                      // 0x004B9180: low dword
        }
        case 4:
        case 5: {                                                                             // 0x004B918A: identifier / field value
            if (*reinterpret_cast<void* const*>(node + 8) == nullptr) {                       // 0x004B918D: test 0x8(%esi)
                void* const created = Script_QHashSymbolNodeCreate_4BABB0(
                    *reinterpret_cast<const char* const*>(node + 0x10));                      // 0x004B9191-0x004B9199
                *reinterpret_cast<void**>(const_cast<std::uint8_t*>(node) + 8) = created;    // 0x004B919D
            }
            const std::uint8_t* const symbol = *reinterpret_cast<const std::uint8_t* const*>(node + 8);   // 0x004B91A0
            if (symbol == nullptr) {                                                          // 0x004B91A5
                std::int32_t const line = *reinterpret_cast<const std::int32_t*>(node + 0x24);        // 0x004B91D0
                const std::uint8_t* const name_node = *reinterpret_cast<const std::uint8_t* const*>(node + 0x18);
                const char* const identifier = (name_node != nullptr)
                    ? *reinterpret_cast<const char* const*>(name_node + 0x10)                 // 0x004B91D3
                    : *reinterpret_cast<const char* const*>(node + 0x10);                     // 0x004B91F0
                igi::retail::script::Qvm_ReportError("Unknown identifier '%s' in expression, line %d",
                                                     identifier, line);                       // 0x004B91DD
                return 0;                                                                     // 0x004B91E5
            }
            const std::uint8_t* const resolved = *reinterpret_cast<const std::uint8_t* const*>(symbol + 8);  // 0x004B91A7
            if (resolved == nullptr) {                                                        // 0x004B91AC
                std::int32_t const line = *reinterpret_cast<const std::int32_t*>(node + 0x24);
                const std::uint8_t* const name_node = *reinterpret_cast<const std::uint8_t* const*>(node + 0x18);
                const char* const identifier = (name_node != nullptr)
                    ? *reinterpret_cast<const char* const*>(name_node + 0x10)
                    : *reinterpret_cast<const char* const*>(node + 0x10);
                igi::retail::script::Qvm_ReportError("Unknown identifier '%s' in expression, line %d",
                                                     identifier, line);
                return 0;
            }
            std::int32_t const selector = *reinterpret_cast<const std::int32_t*>(node + 0x0C);        // 0x004B91AE
            void* const handler_argument = *reinterpret_cast<void* const*>(resolved + 0x18);          // 0x004B91B1
            std::uint32_t buffer[5];                                                          // 0x004B91B5: 20-byte local at frame +0x14
            using HandlerFn = void* (*)(void*, void*, std::int32_t);
            HandlerFn const handler = reinterpret_cast<HandlerFn>(
                *reinterpret_cast<void* const*>(resolved + 0x10));                            // 0x004B91BB: call *0x10(%eax)
            void* const result = handler(buffer, handler_argument, selector);                 // (buffer, resolved+0x18, node+0x0C)
            return *static_cast<const std::int32_t*>(result);                                 // 0x004B91BE: mov (%eax),%eax
        }
        case 6:
            // 0x004B91F7: single child at +0x08.
            return Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 8));
        default:
            return 0;                                                                         // 0x004B91E5
        }
    }
    case 1: {                                                                                 // 0x004B928E: left + right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));
    }
    case 2: {                                                                                 // 0x004B92AD: left - right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right));
    }
    case 3:
        // 0x004B923D: unary, single child at +0x1C.
        return Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
    case 4: {
        // 0x004B924F: negate the +0x1C child.
        std::int32_t const value = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        return static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(value));
    }
    case 5: {                                                                                 // 0x004B92CC: left * right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(left) * static_cast<std::uint32_t>(right));
    }
    case 6: {                                                                                 // 0x004B92EC: left / right (idiv)
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        if (right == 0) {                                                                     // 0x004B92FA/0x004B92FC
            igi::retail::script::g_a965a8 = 1;                                                // 0x004B9313: movl $0x1,0xa965a8
            return 0;                                                                         // 0x004B931D
        }
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return left / right;                                                                  // 0x004B9307/0x004B930B: cltd/idiv
    }
    case 7:
        return 0;                                                                             // 0x004B91E5
    case 8: {                                                                                 // 0x004B9325: left | right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return left | right;
    }
    case 9: {                                                                                 // 0x004B9344: left ^ right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return left ^ right;
    }
    case 10: {                                                                                // 0x004B9363: left & right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return left & right;
    }
    case 11: {                                                                                // 0x004B9382: left << right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        // 0x004B939B: shl %cl,%eax; the count is CL, so x86 masks it to five bits.
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(left)
                                         << (static_cast<std::uint32_t>(right) & 31U));
    }
    case 12: {                                                                                // 0x004B93A3: left >> right (arithmetic)
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        // 0x004B93BC: sar %cl,%eax; five-bit count, arithmetic shift, sign preserved.
        return left >> (static_cast<std::uint32_t>(right) & 31U);
    }
    case 13: {
        // 0x004B9263: bitwise complement of the +0x1C child.
        std::int32_t const value = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        return ~value;
    }
    case 14: {                                                                                // 0x004B945C: left == right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return (left == right) ? 1 : 0;                                                       // 0x004B9477: sete
    }
    case 15: {                                                                                // 0x004B9482: left != right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return (left != right) ? 1 : 0;                                                       // 0x004B949D: setne
    }
    case 16: {                                                                                // 0x004B93C4: left < right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return (left < right) ? 1 : 0;                                                        // 0x004B93DF: setl
    }
    case 17: {                                                                                // 0x004B93EA: left > right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return (left > right) ? 1 : 0;                                                        // 0x004B9405: setg
    }
    case 18: {                                                                                // 0x004B9410: left <= right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return (left <= right) ? 1 : 0;                                                       // 0x004B942B: setle
    }
    case 19: {                                                                                // 0x004B9436: left >= right
        std::int32_t const right = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        std::int32_t const left = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));
        return (left >= right) ? 1 : 0;                                                       // 0x004B9451: setge
    }
    case 20: {
        // 0x004B9277: logical not of the +0x1C child (neg; sbb eax,eax; inc eax).
        std::int32_t const value = Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
        return (value == 0) ? 1 : 0;
    }
    case 21:
    case 22:
    case 23:
        return 0;                                                                             // 0x004B91E5
    case 24:
        // 0x004B9145: sequence - evaluate node+0x18 for effect, return the +0x1C child (re-read after the first call).
        static_cast<void>(Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18)));
        return Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));
    case 25: {                                                                                // 0x004B9501: assignment
        const std::uint8_t* const target = *reinterpret_cast<const std::uint8_t* const*>(node + 0x18);   // 0x004B9501
        if (target == nullptr) {                                                              // 0x004B9504/0x004B9506
            Debug_Error("Internal script error. Invalid assignment, line %d",
                        *reinterpret_cast<const std::int32_t*>(node + 0x24));                 // 0x004B95B1-0x004B95BA
            for (;;) {                                                                        // 0x004B95C2: jmp $ self-loop
            }
        }
        std::int32_t const target_kind = static_cast<std::int8_t>(target[0]);                 // 0x004B950C: movsbl (%eax)
        if (target_kind != 4 && target_kind != 5) {                                           // 0x004B950E/0x004B9513
            Debug_Error("Internal script error. Invalid assignment, line %d",
                        *reinterpret_cast<const std::int32_t*>(node + 0x24));                 // 0x004B95B1
            for (;;) {                                                                        // 0x004B95C2
            }
        }
        if (target_kind == 5) {                                                               // 0x004B9513: jne skips creation
            if (*reinterpret_cast<void* const*>(target + 8) == nullptr) {                     // 0x004B9521/0x004B9526
                void* const created = Script_QHashSymbolNodeCreate_4BABB0(
                    *reinterpret_cast<const char* const*>(target + 0x10));                    // 0x004B9528-0x004B952C
                *reinterpret_cast<void**>(const_cast<std::uint8_t*>(target) + 8) = created;   // 0x004B9537
            }
        }
        const std::uint8_t* const symbol = *reinterpret_cast<const std::uint8_t* const*>(target + 8);    // 0x004B953A/0x004B953D
        const std::uint8_t* resolved = nullptr;
        if (symbol != nullptr) {                                                              // 0x004B9542
            resolved = *reinterpret_cast<const std::uint8_t* const*>(symbol + 8);             // 0x004B9544
        }
        if (resolved == nullptr) {                                                            // 0x004B9549 -> 0x004B95A4
            const char* const identifier = *reinterpret_cast<const char* const*>(target + 0x10);    // 0x004B95A7
            igi::retail::script::Qvm_ReportError("Unknown identifier '%s' in expression, line %d",
                                                 identifier,
                                                 *reinterpret_cast<const std::int32_t*>(node + 0x24));  // 0x004B95AC
            return 0;                                                                         // 0x004B91E5
        }
        std::int32_t const resolved_kind = *reinterpret_cast<const std::int32_t*>(resolved);  // 0x004B954B
        if (resolved_kind == 0) {                                                             // 0x004B954F
            Qvm_StoreArgumentValue_4B9040(*reinterpret_cast<void* const*>(node + 0x1C),       // 0x004B9556
                                          const_cast<std::uint8_t*>(resolved));
            return Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));     // 0x004B955F
        }
        if (resolved_kind == 2) {                                                             // 0x004B956D/0x004B9570
            igi::retail::script::Qvm_ReportError("Expression: Cannot assign to constant, line %d",
                                                 *reinterpret_cast<const std::int32_t*>(node + 0x24));   // 0x004B9572-0x004B957B
            return 0;                                                                         // 0x004B9583
        }
        igi::retail::script::Qvm_ReportError("Syntax error in assignment, line %d",
                                             *reinterpret_cast<const std::int32_t*>(node + 0x24));       // 0x004B958B-0x004B9599
        return 0;                                                                             // 0x004B959C
    }
    case 26:
        return 0;                                                                             // 0x004B91E5
    case 27:                                                                                  // 0x004B94A8: logical or
        if (Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18)) != 0) {
            return 1;                                                                         // 0x004B94F6
        }
        if (Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C)) != 0) {
            return 1;
        }
        return 0;                                                                             // 0x004B94C8-0x004B94CD
    case 28:                                                                                  // 0x004B94CE: logical and
        if (Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18)) == 0) {
            return 0;                                                                         // 0x004B94DC -> 0x004B91E5
        }
        if (Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C)) == 0) {
            return 0;
        }
        return 1;                                                                             // 0x004B94F6
    case 29:
        // 0x004B9209: ternary - test node+0x08, then return the selected child.
        if (Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 8)) != 0) {
            return Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18));     // 0x004B9219
        }
        return Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C));         // 0x004B922B
    default:
        return 0;                                                                             // 0x004B91E5
    }
}

} // extern "C"

#undef NATIVE_DECL
