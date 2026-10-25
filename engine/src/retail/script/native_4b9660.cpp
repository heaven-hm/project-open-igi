// Native: Qvm_EvaluateArgumentValue; retail VA 0x004B9660 (catalog name kept; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B9660-0x004B9B53 (1265 bytes, 75 blocks, 75 edges, 447 instructions), bytes re-read from the image (83 ec 20 56 8b 74 24 28 57 33 ff 3b f7 0f 84 .. 0f be 46 01 83 f8 1d 77 .. 33 c9 8a 0c 05 a4 9b 4b 00 ff 24 8d 54 9b 4b 00 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image plus a linear disassembly walk of all 447 instructions; the 30-byte index table at 0x004B9BA4 [0,1,2,3,4,5,6,19,19,19,19,19,19,19,7,8,9,10,11,12,13,19,19,19,14,15,19,16,17,18], the 20-entry jump table at 0x004B9B54 and the 6-entry jump table at 0x004B9BC4 were read as raw bytes/dwords and verified byte-exactly; the comparison constant 0x005333B0 is eight zero bytes (0.0). radare2 reports 75 blocks / 75 edges, 48-byte frame.
// Behavior/CFG: cdecl floating-point expression evaluator, one stack argument ([esp+4] expression node), result returned in ST0 as long double. Null node or node[1] outside 0..0x1D returns 0.0 (0x004B9B45 `fldl 0x005333B0`). node[1] selects one of twenty blocks through the 30-byte index table: 0 subtype dispatch, 1 add, 2 subtract, 3 unary plus, 4 negate, 5 multiply, 6 divide, 7..13 and 21..23 and 26 return 0.0, 14 equal, 15 not equal, 16 less, 17 greater, 18 less-or-equal, 19 greater-or-equal, 20 logical not, 24 integer sub-expression converted with `fildl`, 25 assignment, 27 logical or, 28 logical and, 29 ternary. Evaluation order per case is retail's, not a uniform left-to-right: add and subtract evaluate node[0x18] first and keep it as a 64-bit local (`fstpl`), multiply and divide evaluate node[0x1C] first, and the divide keeps the divisor as an 80-bit local (`fstl`) before the zero test. Every comparison stores the node[0x18] value as a 64-bit local and compares it against the node[0x1C] value in ST0 with `fcompl`, then converts the x87 condition flags (C3 0x40 / C0 0x01 in AH) into a rounded integer 0/1 with `fildl`; the NaN behaviour of each flag test is reproduced exactly (unordered counts as equal for 14, unequal-for- false for 15, and true for 19). Runtime checks: 0x004B9876-0x004B988B sets the interpreter error flag 0x00A965A8 to 1 and returns 0.0 when a divisor compares equal-or-unordered to 0.0; 0x004B9A8F-0x004B9AA0 is the Debug_Error assignment self-loop at 0x004B9AA0. Subtype dispatch (only node[1] == 0): 1 returns `fildl` of the dword at node[0x08], 2 returns 0.0, 3 returns `fldl` of the double at node[0x08], 4 and 5 resolve the symbol record at node[0x08] (creating it through 0x004BABB0 from node[0x10]), call the record node's handler at +0x10 with a 16-byte buffer and return `fldl` of the double at result+0x08, 6 recurses into node[0x08]; any other subtype returns 0.0. Case 25 mirrors the other two evaluators: null or non-4/5 target calls Debug_Error("Internal script error. Invalid assignment, line %d") then loops forever, kind 5 resolves through 0x004BABB0, resolved dword 0 == 0 stores via 0x004B9040 and returns the float value of node[0x1C], == 2 reports "Expression: Cannot assign to constant, line %d", anything else reports "Syntax error in assignment, line %d", and all of those error returns are 0.0. Identifier reporting prefers node[0x18][0x10] and falls back to node[0x10] with "Unknown identifier '%s' in expression, line %d" (0x00547478). All four literals are byte-identical to the image.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the instruction listing (75 blocks/75 edges, all 30 slot indices and all 20 + 6 jump-table targets matched); runtime UNOBSERVED. Link: needs 0x004B9120 (transcribed), 0x004BABB0 (transcribed), 0x004B9040 (transcribed), Qvm_ReportError 0x004B7E10 and 0x005333B0's zero constant; no missing callee. Fidelity note: retail computes in 80-bit x87 and stores two kinds of intermediate - `fstpl` 64-bit for add/sub/mul/comparison operands and `fstl` 80-bit for the divisor - reproduced with `double` and `long double` locals respectively; under the MSVC i386 build `long double` is 64-bit, so the 80-bit divisor local cannot be represented there. The `fistpll` conversions in cases 24 and 29 are reproduced with std::llrint, which honours the same current rounding mode. The handler buffer is left uninitialized exactly as retail leaves it. The value stack and type stack are not touched; only the error flag is.
#include "dev_log.hpp"
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

// 0x004B9120 - integer expression evaluator (transcribed in native_4b9120.cpp).
std::int32_t NATIVE_DECL Qvm_EvaluateIntegerNode_4B9120(const void* node);
// 0x004BABB0 - symbol record creation (transcribed in native_4babb0.cpp); its out value is the symbol record, not the name.
void* NATIVE_DECL Script_QHashSymbolNodeCreate_4BABB0(const char* name);
// 0x004B9040 - type-directed store helper (transcribed in native_4b9040.cpp).
void NATIVE_DECL Qvm_StoreArgumentValue_4B9040(const void* node, void* target_object);
long double NATIVE_DECL Qvm_EvaluateArgumentValue(const void* node);

long double NATIVE_DECL Qvm_EvaluateArgumentValue(const void* node_pointer)
{
    NATIVE_LOG_ENTER("Qvm_EvaluateArgumentValue", "0x004B9660");
    const std::uint8_t* const node = static_cast<const std::uint8_t*>(node_pointer);          // esi throughout, as in retail
    if (node == nullptr) {                                                                    // 0x004B966B/0x004B966D (cmp edi=0)
        return 0.0L;                                                                          // 0x004B9B45: fldl 0x005333B0
    }
    std::int32_t const operation = static_cast<std::int8_t>(node[1]);                         // 0x004B9673: movsbl 0x1(%esi)
    if (static_cast<std::uint32_t>(operation) > 0x1DU) {                                      // 0x004B9677/0x004B967A: cmp/ja
        return 0.0L;                                                                          // 0x004B9B45
    }

    switch (operation) {
    case 0: {                                                                                 // 0x004B96B2: second dispatch
        std::int32_t const subtype = static_cast<std::int8_t>(node[0]);                       // 0x004B96B2: movsbl (%esi)
        if (static_cast<std::uint32_t>(subtype - 1) > 5U) {                                   // 0x004B96B5/0x004B96B9
            return 0.0L;                                                                      // 0x004B9B45
        }
        switch (subtype) {
        case 1:
            return static_cast<long double>(*reinterpret_cast<const std::int32_t*>(node + 8));   // 0x004B96C6: fildl 0x8(%esi)
        case 2:
            return 0.0L;                                                                      // 0x004B9B45
        case 3:
            return static_cast<long double>(*reinterpret_cast<const double*>(node + 8));      // 0x004B96CF: fldl 0x8(%esi)
        case 4:
        case 5: {                                                                             // 0x004B96D8: symbol value as real
            if (*reinterpret_cast<void* const*>(node + 8) == nullptr) {                       // 0x004B96D8/0x004B96DB
                void* const created = Script_QHashSymbolNodeCreate_4BABB0(
                    *reinterpret_cast<const char* const*>(node + 0x10));                      // 0x004B96DD-0x004B96E1
                *reinterpret_cast<void**>(const_cast<std::uint8_t*>(node) + 8) = created;      // 0x004B96E9
            }
            const std::uint8_t* symbol = *reinterpret_cast<const std::uint8_t* const*>(node + 8);   // 0x004B96EC
            const std::uint8_t* resolved = nullptr;
            if (symbol != nullptr) {                                                          // 0x004B96F1
                resolved = *reinterpret_cast<const std::uint8_t* const*>(symbol + 8);         // 0x004B96F3
            }
            if (resolved == nullptr) {                                                        // 0x004B96F8 -> 0x004B9716
                const std::uint8_t* const name_node = *reinterpret_cast<const std::uint8_t* const*>(node + 0x18);
                const char* const identifier = (name_node != nullptr)
                    ? *reinterpret_cast<const char* const*>(name_node + 0x10)                 // 0x004B971D-0x004B9725
                    : *reinterpret_cast<const char* const*>(node + 0x10);                     // 0x004B972A-0x004B9732
                igi::retail::script::Qvm_ReportError("Unknown identifier '%s' in expression, line %d",
                                                     identifier,
                                                     *reinterpret_cast<const std::int32_t*>(node + 0x24));   // 0x004B9B38
                return 0.0L;                                                                  // 0x004B9B45
            }
            std::int32_t const selector = *reinterpret_cast<const std::int32_t*>(node + 0x0C);   // 0x004B96FA
            void* const handler_argument = *reinterpret_cast<void* const*>(resolved + 0x18);       // 0x004B96FD
            std::uint32_t handler_buffer[4];                                                  // 0x004B9701: 16-byte local
            using HandlerFn = void* (*)(void*, void*, std::int32_t);
            HandlerFn const handler = reinterpret_cast<HandlerFn>(
                *reinterpret_cast<void* const*>(resolved + 0x10));                            // 0x004B9707: call *0x10(%eax)
            void* const result = handler(handler_buffer, handler_argument, selector);         // (buffer, resolved+0x18, node+0x0C)
            return static_cast<long double>(*reinterpret_cast<const double*>(
                static_cast<const std::uint8_t*>(result) + 8));                               // 0x004B970A: fldl 0x8(%eax)
        }
        case 6:
            return Qvm_EvaluateArgumentValue(*reinterpret_cast<void* const*>(node + 8));      // 0x004B9737
        default:
            return 0.0L;                                                                      // 0x004B9B45
        }
    }
    case 1: {                                                                                 // 0x004B97DA: left + right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B97DA-0x004B97E6 (fstpl local)
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B97EA/0x004B97EB
        return second + static_cast<long double>(first);                                      // 0x004B97F0: faddl
    }
    case 2: {                                                                                 // 0x004B97FD: left - right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B97FD-0x004B9809
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B980D/0x004B980E
        return static_cast<long double>(first) - second;                                      // 0x004B9813: fsubrl
    }
    case 3:
        return Qvm_EvaluateArgumentValue(*reinterpret_cast<void* const*>(node + 0x1C));       // 0x004B9785: unary plus
    case 4:
        return -Qvm_EvaluateArgumentValue(*reinterpret_cast<void* const*>(node + 0x1C));      // 0x004B9797/0x004B97A3: fchs
    case 5: {                                                                                 // 0x004B9820: left * right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C)));                                   // 0x004B9820-0x004B982C
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18));                                    // 0x004B9830/0x004B9831
        return second * static_cast<long double>(first);                                      // 0x004B9836: fmull
    }
    case 6: {                                                                                 // 0x004B9843: left / right
        long double const divisor = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B9843-0x004B984C (fstl local)
        if (!(divisor < 0.0L) && !(divisor > 0.0L)) {                                         // 0x004B9850-0x004B985E: fcompl 0.0 + test $0x40
            igi::retail::script::g_a965a8 = 1;                                                // 0x004B987D: movl $0x1,0xa965a8
            return 0.0L;                                                                      // 0x004B9876: fldl 0x005333B0
        }
        long double const dividend = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18));                                    // 0x004B9860/0x004B9864
        return dividend / divisor;                                                            // 0x004B9869: fdivl
    }
    case 14: {                                                                                // 0x004B9974: left == right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B9974-0x004B9980
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B9984/0x004B9985
        const long double left = static_cast<long double>(first);
        // 0x004B9999-0x004B99A4: C3 set (equal or unordered) keeps 1.
        return (!(second < left) && !(second > left)) ? 1.0L : 0.0L;
    }
    case 15: {                                                                                // 0x004B99AE: left != right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B99AE-0x004B99BA
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B99BF
        const long double left = static_cast<long double>(first);
        // 0x004B99D3-0x004B99DE: C3 clear (ordered and not equal) keeps 1.
        return ((second < left) || (second > left)) ? 1.0L : 0.0L;
    }
    case 16: {                                                                                // 0x004B988C: left < right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B988C-0x004B9898
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B989C/0x004B989D
        const long double left = static_cast<long double>(first);
        // 0x004B98B1-0x004B98BC: (C3|C0) clear means ST0 > operand, i.e. right > left, strictly ordered.
        return (second > left) ? 1.0L : 0.0L;
    }
    case 17: {                                                                                // 0x004B98C6: left > right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B98C6-0x004B98D2
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B98D6/0x004B98D7
        const long double left = static_cast<long double>(first);
        // 0x004B98EB-0x004B98F6: C0 set means ST0 < operand or unordered, so unordered keeps 1.
        return (!(second > left) && !(second == left)) ? 1.0L : 0.0L;
    }
    case 18: {                                                                                // 0x004B9900: left <= right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B9900-0x004B990C
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B9910/0x004B9911
        const long double left = static_cast<long double>(first);
        // 0x004B9925-0x004B9930: C0 clear keeps 1, so unordered (C0 set) is false here.
        return ((second > left) || (second == left)) ? 1.0L : 0.0L;
    }
    case 19: {                                                                                // 0x004B993A: left >= right
        double const first = static_cast<double>(Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18)));                                   // 0x004B993A-0x004B9946
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B994A/0x004B994B
        const long double left = static_cast<long double>(first);
        // 0x004B995F-0x004B996A: (C3|C0) set keeps 1, so unordered counts as true.
        return (!(second > left)) ? 1.0L : 0.0L;
    }
    case 20: {
        // 0x004B97AB-0x004B97D0: logical not, C3 set (zero or unordered) keeps 1.
        long double const value = Qvm_EvaluateArgumentValue(*reinterpret_cast<void* const*>(node + 0x1C));
        return (!(value < 0.0L) && !(value > 0.0L)) ? 1.0L : 0.0L;
    }
    case 24:
        // 0x004B968F: integer sub-expression - node[0x18] evaluated for effect, node[0x1C] converted with `fildl`.
        static_cast<void>(Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x18)));
        return static_cast<long double>(
            Qvm_EvaluateIntegerNode_4B9120(*reinterpret_cast<void* const*>(node + 0x1C)));   // 0x004B96A8: fildl 0x2c(%esp)
    case 25: {                                                                                // 0x004B9A78: assignment
        const std::uint8_t* const target = *reinterpret_cast<const std::uint8_t* const*>(node + 0x18);   // 0x004B9A78
        if (target == nullptr) {                                                              // 0x004B9A7B/0x004B9A7D
            return 0.0L;                                                                      // 0x004B9B45
        }
        std::int32_t const target_kind = static_cast<std::int8_t>(target[0]);                 // 0x004B9A83: mov (%eax),%cl
        if (target_kind != 4 && target_kind != 5) {                                           // 0x004B9A85/0x004B9A8A/0x004B9A8D
            Debug_Error("Internal script error. Invalid assignment, line %d",
                        *reinterpret_cast<const std::int32_t*>(node + 0x24));                 // 0x004B9A8F-0x004B9A98
            for (;;) {                                                                        // 0x004B9AA0: jmp $ self-loop
            }
        }
        if (target_kind == 5) {                                                               // 0x004B9AA2
            if (*reinterpret_cast<void* const*>(target + 8) == nullptr) {                     // 0x004B9AA7/0x004B9AAA
                void* const created = Script_QHashSymbolNodeCreate_4BABB0(
                    *reinterpret_cast<const char* const*>(target + 0x10));                    // 0x004B9AAC-0x004B9AB0
                *reinterpret_cast<void**>(const_cast<std::uint8_t*>(
                    *reinterpret_cast<const std::uint8_t* const*>(node + 0x18)) + 8) = created;   // 0x004B9AB5/0x004B9ABB
            }
        }
        const std::uint8_t* const symbol = *reinterpret_cast<const std::uint8_t* const*>(target + 8);    // 0x004B9AC1
        const std::uint8_t* resolved = nullptr;
        if (symbol != nullptr) {                                                              // 0x004B9AC6
            resolved = *reinterpret_cast<const std::uint8_t* const*>(symbol + 8);             // 0x004B9AC8
        }
        if (resolved == nullptr) {                                                            // 0x004B9ACD -> 0x004B9B30
            igi::retail::script::Qvm_ReportError("Unknown identifier '%s' in expression, line %d",
                                                 *reinterpret_cast<const char* const*>(target + 0x10),   // 0x004B9B33
                                                 *reinterpret_cast<const std::int32_t*>(node + 0x24));   // 0x004B9B30
            return 0.0L;                                                                      // 0x004B9B45
        }
        std::int32_t const resolved_kind = *reinterpret_cast<const std::int32_t*>(resolved);  // 0x004B9ACF
        if (resolved_kind == 0) {                                                             // 0x004B9AD1/0x004B9AD3
            Qvm_StoreArgumentValue_4B9040(*reinterpret_cast<void* const*>(node + 0x1C),
                                          const_cast<std::uint8_t*>(resolved));               // 0x004B9ADA
            return Qvm_EvaluateArgumentValue(*reinterpret_cast<void* const*>(node + 0x1C));   // 0x004B9AE3
        }
        if (resolved_kind == 2) {                                                             // 0x004B9AF1/0x004B9AF4
            igi::retail::script::Qvm_ReportError("Expression: Cannot assign to constant, line %d",
                                                 *reinterpret_cast<const std::int32_t*>(node + 0x24));   // 0x004B9AFA
            return 0.0L;                                                                      // 0x004B9B04: fldl 0x005333B0
        }
        igi::retail::script::Qvm_ReportError("Syntax error in assignment, line %d",
                                             *reinterpret_cast<const std::int32_t*>(node + 0x24));       // 0x004B9B17
        return 0.0L;                                                                          // 0x004B9B21
    }
    case 27: {                                                                                // 0x004B9A30: logical or
        long double const first = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18));                                    // 0x004B9A30/0x004B9A33
        // 0x004B9A44-0x004B9A47: C3 clear (not zero, not unordered) keeps the 1 at 0x004B9A66.
        if (!(!(first < 0.0L) && !(first > 0.0L))) {
            return 1.0L;
        }
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B9A49/0x004B9A4D
        // 0x004B9A61-0x004B9A64: C3 clear keeps the 1, otherwise the zero from 0x004B9A5B stands.
        return (!(second < 0.0L) && !(second > 0.0L)) ? 0.0L : 1.0L;
    }
    case 28: {                                                                                // 0x004B99E8: logical and
        long double const first = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x18));                                    // 0x004B99E8/0x004B99EC
        // 0x004B99FC-0x004B99FF: C3 set (zero or unordered) jumps to the zero at 0x004B9A22.
        if (!(first < 0.0L) && !(first > 0.0L)) {
            return 0.0L;
        }
        long double const second = Qvm_EvaluateArgumentValue(
            *reinterpret_cast<void* const*>(node + 0x1C));                                    // 0x004B9A01/0x004B9A05
        // 0x004B9A1D-0x004B9A20: C3 clear keeps the 1 from 0x004B9A13, otherwise 0x004B9A22 zeroes it.
        return (!(second < 0.0L) && !(second > 0.0L)) ? 0.0L : 1.0L;
    }
    case 29:
        // 0x004B9749: ternary - condition is the `fistpll` of the node[0x08] value, compared with zero.
        if (std::llrint(Qvm_EvaluateArgumentValue(
                *reinterpret_cast<void* const*>(node + 8))) != 0) {                           // 0x004B9752/0x004B975D
            return Qvm_EvaluateArgumentValue(*reinterpret_cast<void* const*>(node + 0x18));   // 0x004B9761-0x004B9765
        }
        return Qvm_EvaluateArgumentValue(*reinterpret_cast<void* const*>(node + 0x1C));       // 0x004B9773/0x004B9777
    default:
        return 0.0L;                                                                          // 0x004B9B45 (operations 7..13, 21..23, 26)
    }
}

} // extern "C"

#undef NATIVE_DECL
