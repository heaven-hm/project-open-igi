// Native: Sys_CompilePattern_4AF9D0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004AF9D0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AF9D0-0x004AFE75 (1190 bytes, 68 blocks, 329 instructions, 97 edges; a 0xB30-byte frame plus four saved registers, and the byte at 0x004AFE76 begins the dispatch data the two switches compile to).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image. The body contains exactly two calls - 0x004AFFB0 at 0x004AF9E7 and 0x004AFF90 at 0x004AFA1E - and no indirect call form, so its closure is exactly those two natives. There are two indirect jumps, and both dispatch tables were read from the image. Switch 1 at 0x004AFA92 is `jmp *0x4AFE78(,%eax,4)` after `movsbl 0x140(%esp,%ebx,1),%eax; lea -0x24(%eax),%edi; cmp $0x59,%edi; ja 0x004AFE04; xor %eax,%eax; mov 0x4AFEA8(%edi),%al`: jump table 0x004AFE78 holds twelve entries - 0x004AFAA5, 0x004AFAB1, 0x004AFB04, 0x004AFBAF, 0x004AFB68, 0x004AFE03, 0x004AFB74, 0x004AFC16, 0x004AFA99, 0x004AFDAA, 0x004AFDD9, 0x004AFE04 - and the 90-byte class table at 0x004AFEA8 over characters 0x24-0x7D assigns class 0 "$", 1 "(", 2 ")", 3 "*", 4 ".", 5 "/", 6 "?", 7 "[", 8 "^", 9 "{", 10 "}" and 11 to the remaining 79 characters. Switch 2 at 0x004AFC77 is `jmp *0x4AFF04(,%ecx,4)` after `movsbl 0x141(%esp,%ebx,1),%edi; inc %ebx; cmp $0x74,%edi; ja 0x004AFC93; xor %ecx,%ecx; mov 0x4AFF18(%edi),%cl`: jump table 0x004AFF04 holds exactly five entries - 0x004AFE69, 0x004AFC8C, 0x004AFC85, 0x004AFC7E, 0x004AFC93 - a length established by reading the sixth dword, which is 0x04040400 and therefore the next table's data, and the 117-byte class table at 0x004AFF18 over characters 0x00-0x74 assigns class 0 to NUL, 1 to "N"/"n", 2 to "R"/"r", 3 to "T"/"t" and 4 to the remaining 110. The 0x004AFF04 table ends exactly where 0x004AFF18 begins, which is the second, independent confirmation of its length.
// Behavior/CFG: compiles the pattern (argument 3) into the node array (argument 1) after 0x004AFFB0 has rewritten it into the frame buffer at [esp+0x140]; returns 1 at 0x004AFE5C and 0 from the single failure exit at 0x004AFE6C, which every rejection branches to. Each node is 0x14 bytes: +0 the saved alternation index, +4 a link, +8 the flags dword (0x100 terminal, 0x200 repeat, 0x400 "{", 0x800 "}", 0x1000 "." and the class-record index in the low byte, 0x2000 "^", 0x4000 "$", 0x8000 "(", 0x10000 ")", 0x20000 alternation set on the node an alternation links to, 0x40000 "?"), +0xC the back link and +0x10 the forward link. Argument 2 is a 32-entry array of 0x100-byte class records; 0x004AFF90 fills record 0 with 0x01 and sets the count to 1. The '[' case parses a character class into a 0x100-byte bitmap at [esp+0x40] with '~' negation, '\' escapes (N/n newline, R/r return, T/t tab, everything else literal), '/' followed by a literal, and "x-y" ranges; the bitmap is then matched against the existing records and appended as a new record when absent, failing once the index reaches 0x20. Two stacks grow in the frame: alternation state at [esp+0x1C] over [esp+0x340] and node state at [esp+0x30] over [esp+0x740], both advanced by 4 per '(' and pulled back by 4 per ')'.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Three arguments, all three used: argument 1 is the node array, read at [esp+0xB44] and written as [edx], [edx+4], [edx+8], [edx+0xC], [edx+0x10] and [edx+0x14]; argument 2 is the class-record array, read at [esp+0xB48] and passed to 0x004AFF90; argument 3 is the pattern, read at entry as [esp+0xC] before any frame exists and passed to 0x004AFFB0. The prologue's `sub $0xB30` puts the return address at entry_esp+0xB30, so the three argument slots land at +0xB34/+0xB38/+0xB3C; the four register pushes are 0x10 bytes, not 0xC, so in body coordinates they are [esp+0xB44]/[esp+0xB48]/[esp+0xB4C] and [esp+0xB40] is the RETURN ADDRESS, not an argument. That same offset rule governs every frame slot whose address is taken before the pushes: `lea 0x130(%esp),%ecx` at 0x004AF9DA names the buffer that the body then parses as [esp+0x140+ebx] (switch 1) and [esp+0x141+ebx] (switch 2), which is exactly the destination 0x004AFFB0 writes, so the translated pattern is parsed in place. Frame slots keep their retail offsets because only some roles are established: [esp+0x10] is the parenthesis depth, [esp+0x14] the pattern index saved across the class scan, [esp+0x18] the class-record count, [esp+0x1C]/[esp+0x30] the two stack pointers, [esp+0x20] the brace flag, [esp+0x24] the node-count mirror, [esp+0x28] a negate flag that later holds a record pointer, [esp+0x2C] the saved alternation index, [esp+0x34] the translated pattern length, [esp+0x38] the node index saved by ')', and [esp+0x3C] the alternation counter. The prologue initializes only [esp+0x18], [esp+0x10], [esp+0x20], [esp+0x3C] and [esp+0x2C] (0x004AFA06-0x004AFA16); the remaining slots are written before every read on the paths that reach them. Retail bounds-checks neither of the two frame stacks nor the 0x004AFFB0 destination, and the declared array sizes are the frame space each region has (0x340 to 0x73F and 0x740 to 0xB2F, and 0x140 to 0x33F), not invented limits. The escape and class-literal writes index the bitmap with a sign-extended character, so a byte at or above 0x80 writes below the bitmap in retail; the source keeps the signed index and does not add a range check. The bitmap clears and the class scans are written as loops where retail uses `rep stos` and `repz cmpsl`, and the translated-pattern length is written as a terminator scan where retail uses `repnz scas`; the flags, edges and order of each retail block are otherwise preserved. GCC's loop-idiom recognition compiles that terminator scan into a call to the C library `strlen`, so the object carries three undefined symbols where retail's body makes two calls - the two natives above plus `_strlen`, which is the compiler's lowering of the scan and not an invented call, the same class of deviation as the `rep stos` in 0x004AFF90 becoming `memset`. Domain: sys/, with 0x004AF8F0, 0x004AF960 and 0x004AFF90/0x004AFFB0 in the same pattern/list-record family.
#include "dev_log.hpp"
#include <cstdint>
#include <cstddef>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Retail calls both of these by address; the declarations only satisfy the compiler.
std::int32_t NATIVE_DECL Sys_TranslateWildcardPattern_4AFFB0(std::uint8_t* destination, char const* pattern);
void NATIVE_DECL Sys_InitRecordDefaults_4AFF90(void* buffer, std::uint32_t* flag);

std::int32_t NATIVE_DECL Sys_CompilePattern_4AF9D0(std::uint8_t* node_array, std::uint8_t* class_records, char const* pattern)
{
    NATIVE_LOG_ENTER("Sys_CompilePattern_4AF9D0", "0x004AF9D0");
    // Frame scalars keep their retail offsets. Registers are named by role: ebx is the pattern
    // index, esi the node count, ebp the node index, ecx the node array base, edx the current
    // node and eax the scratch value.
    std::uint32_t local_10 = 0;        // [esp+0x10]: parenthesis depth
    std::uint32_t local_14 = 0;        // [esp+0x14]: pattern index saved across the class scan
    std::uint32_t local_18 = 0;        // [esp+0x18]: class-record count
    std::uint32_t local_20 = 0;        // [esp+0x20]: brace flag
    std::uint32_t local_24 = 0;        // [esp+0x24]: node-count mirror
    std::uintptr_t local_28 = 0;       // [esp+0x28]: negate flag, later a record pointer
    std::uint32_t local_2c = 0;        // [esp+0x2C]: saved alternation index
    std::uint32_t local_34 = 0;        // [esp+0x34]: translated pattern length
    std::uint32_t local_38 = 0;        // [esp+0x38]: node index saved by ')'
    std::uint32_t local_3c = 0;        // [esp+0x3C]: alternation counter
    std::uint8_t bitmap[0x100];        // [esp+0x40]: character-class bitmap
    std::uint8_t translated[0x200];    // [esp+0x140]: the 0x004AFFB0 destination
    std::uint32_t stack_a[0x100];      // [esp+0x340]: alternation state
    std::uint32_t stack_b[0xFC];       // [esp+0x740]: node state
    std::uint32_t* local_1c = stack_a; // [esp+0x1C]: next free alternation slot
    std::uint32_t* local_30 = stack_b; // [esp+0x30]: next free node slot

    std::uint32_t index = 0;                                        // ebx
    std::uint32_t node_count = 0;                                   // esi
    std::uint32_t node_index = 1;                                   // ebp
    std::uint32_t* nodes = reinterpret_cast<std::uint32_t*>(node_array); // ecx
    std::uint8_t* node = node_array;                                // edx

    // 0x004AF9D0-0x004AF9F1: rewrite the pattern into the frame buffer, then leave with 0 if
    // the translator rejected it.
    if (Sys_TranslateWildcardPattern_4AFFB0(translated, pattern) == 0) goto fail;

    // 0x004AF9F7-0x004AFA1E: argument 2 is the class-record area. The five stores happen with
    // both call arguments already pushed, which is why retail names the slots at +0x18/+0x10/
    // +0x20/+0x3C/+0x2C; 0x004AFF90 then fills record 0 with 0x01 and writes 1 through the
    // count pointer, so the count becomes 1.
    local_18 = 0;                                       // 0x004AFA06
    local_10 = 0;                                       // 0x004AFA0A
    local_20 = 0;                                       // 0x004AFA0E
    local_3c = 0;                                       // 0x004AFA12
    local_2c = 0xFFFFFFFFU;                             // 0x004AFA16
    Sys_InitRecordDefaults_4AFF90(class_records, &local_18);   // 0x004AFA04-0x004AFA1E

    // 0x004AFA23-0x004AFA36: length of the translated pattern, retail's `repnz scas` scan for
    // the terminator.
    while (translated[local_34] != 0) ++local_34;        // 0x004AFA32

    index = 0;                                           // 0x004AFA37
    local_24 = node_count;                               // 0x004AFA3F
    if (static_cast<std::int32_t>(local_34) <= static_cast<std::int32_t>(node_count)) goto done;   // 0x004AFA39-0x004AFA43

    nodes = reinterpret_cast<std::uint32_t*>(node_array);   // 0x004AFA49
    local_1c = stack_a;                                  // 0x004AFA50-0x004AFA57
    local_30 = stack_b;                                  // 0x004AFA5B
    node_index = 1;                                      // 0x004AFA62
    node = node_array;                                   // 0x004AFA67

head:                                                    // 0x004AFA6F on the loop back, 0x004AFA76 on the first pass
    nodes = reinterpret_cast<std::uint32_t*>(node_array);   // 0x004AFA6F

    switch (static_cast<std::int32_t>(static_cast<signed char>(translated[index])))   // 0x004AFA76-0x004AFA92: range check 0x004AFA7E/0x004AFA81/0x004AFA84, class lookup 0x004AFA8A-0x004AFA8C, jump table 0x004AFA92
    {
    case '^':                                            // class 8 - 0x004AFA99
        *reinterpret_cast<std::uint32_t*>(node + 8) = 0x2000U;
        goto emit;                                       // 0x004AFAA0

    case '$':                                            // class 0 - 0x004AFAA5
        *reinterpret_cast<std::uint32_t*>(node + 8) = 0x4000U;
        goto emit;                                       // 0x004AFAAc

    case '(':                                            // class 1 - 0x004AFAB1
        {
            std::uint32_t* slot_a = local_1c;            // 0x004AFAB1
            const std::uint32_t saved = local_2c;        // 0x004AFAB5
            std::uint32_t* slot_b = local_30;            // 0x004AFAC0
            *reinterpret_cast<std::uint32_t*>(node + 8) = 0x8000U;      // 0x004AFAB9
            *slot_a = saved;                             // 0x004AFAC4
            std::uint32_t alt = local_3c;                // 0x004AFAC6
            *reinterpret_cast<std::uint32_t*>(node + 0x10) = 0xFFFFFFFFU;   // 0x004AFACA
            *reinterpret_cast<std::uint32_t*>(node + 0x0C) = node_index;    // 0x004AFAD1
            *reinterpret_cast<std::uint32_t*>(node + 4) = alt;              // 0x004AFAD4
            local_2c = alt;                              // 0x004AFAD7
            ++alt;                                       // 0x004AFADB
            *slot_b = node_count;                        // 0x004AFADC
            local_3c = alt;                              // 0x004AFADE
            ++local_10;                                  // 0x004AFAE2-0x004AFAEA
            ++slot_b;                                    // 0x004AFAE7
            ++slot_a;                                    // 0x004AFAEE-0x004AFAF2
            *reinterpret_cast<std::uint32_t*>(node) = saved;            // 0x004AFAF5
            local_30 = slot_b;                           // 0x004AFAF7
            local_1c = slot_a;                           // 0x004AFAFB
        }
        goto loop_tail;                                  // 0x004AFAFF

    case ')':                                            // class 2 - 0x004AFB04
        if (local_10 == 0) goto fail;                    // 0x004AFB08-0x004AFB0A
        {
            std::uint32_t* slot_b = local_30;            // 0x004AFB10
            std::uint32_t* slot_a = local_1c;            // 0x004AFB14
            const std::uint32_t saved = local_2c;        // 0x004AFB18
            --slot_b;                                    // 0x004AFB1C
            --slot_a;                                    // 0x004AFB1F
            local_30 = slot_b;                           // 0x004AFB22
            const std::uint32_t popped_b = *slot_b;      // 0x004AFB26
            local_1c = slot_a;                           // 0x004AFB28
            const std::uint32_t popped_a = *slot_a;      // 0x004AFB2C
            *reinterpret_cast<std::uint32_t*>(node + 4) = saved;        // 0x004AFB2E
            local_2c = popped_a;                         // 0x004AFB31
            *reinterpret_cast<std::uint32_t*>(node) = popped_a;         // 0x004AFB35
            local_38 = popped_b;                         // 0x004AFB37
            *reinterpret_cast<std::uint32_t*>(node + 8) = 0x10000U;     // 0x004AFB45
            *reinterpret_cast<std::uint32_t*>(node + 0x10) = 0xFFFFFFFFU;   // 0x004AFB4C
            *reinterpret_cast<std::uint32_t*>(node + 0x0C) = node_index;    // 0x004AFB53
            nodes[5 * popped_b + 4] = node_index;        // 0x004AFB3B-0x004AFB56
            --local_10;                                  // 0x004AFB5A-0x004AFB5F
        }
        goto loop_tail;                                  // 0x004AFB63

    case '.':                                            // class 4 - 0x004AFB68
        *reinterpret_cast<std::uint32_t*>(node + 8) = 0x1000U;
        goto emit;                                       // 0x004AFB6F

    case '?':                                            // class 6 - 0x004AFB74
        if (node_index == 1) goto fail;                  // 0x004AFB74-0x004AFB77
        {
            const std::uint32_t flags = *reinterpret_cast<std::uint32_t*>(node - 0x0C);  // 0x004AFB7D: the previous node's +8
            node -= 0x14;                                // 0x004AFB80
            --node_count;                                // 0x004AFB83
            --node_index;                                // 0x004AFB84
            if ((flags & 0x10000U) != 0)                 // 0x004AFB85-0x004AFB8A
            {
                nodes[5 * local_38 + 2] |= 0x20000U;     // 0x004AFB8C-0x004AFB97
            }
            *reinterpret_cast<std::uint32_t*>(node + 0x0C) = node_index;    // 0x004AFB9D
            *reinterpret_cast<std::uint32_t*>(node + 0x10) = node_index;    // 0x004AFBA0
            *reinterpret_cast<std::uint32_t*>(node + 8) |= 0x40000U;        // 0x004AFBA3
        }
        goto loop_tail;                                  // 0x004AFBAA

    case '*':                                            // class 3 - 0x004AFBAF
        if (node_index == 1) goto fail;                  // 0x004AFBAF-0x004AFBB2
        {
            const std::uint32_t flags = *reinterpret_cast<std::uint32_t*>(node - 0x0C);  // 0x004AFBB8: the previous node's +8
            node -= 0x14;                                // 0x004AFBBB
            --node_count;                                // 0x004AFBBE
            --node_index;                                // 0x004AFBBF
            if (flags >= 0x100U)                         // 0x004AFBC0-0x004AFBC5
            {
                if ((flags & 0x11000U) == 0) goto fail;  // 0x004AFBC7-0x004AFBCC
            }
            if ((flags & 0x10000U) != 0)                 // 0x004AFBD2-0x004AFBD7
            {
                *reinterpret_cast<std::uint32_t*>(node + 0x0C) = local_38;  // 0x004AFBD9-0x004AFBDD
                nodes[5 * local_38 + 2] |= 0x20000U;     // 0x004AFBE0-0x004AFBEF
                *reinterpret_cast<std::uint32_t*>(node + 8) |= 0x200U;      // 0x004AFBF1-0x004AFBF7
                *reinterpret_cast<std::uint32_t*>(node + 0x10) = node_index;    // 0x004AFBFA
            }
            else
            {
                *reinterpret_cast<std::uint32_t*>(node + 0x0C) = node_count;    // 0x004AFC02
                *reinterpret_cast<std::uint32_t*>(node + 8) |= 0x200U;          // 0x004AFC05-0x004AFC0B
                *reinterpret_cast<std::uint32_t*>(node + 0x10) = node_index;    // 0x004AFC0E
            }
        }
        goto loop_tail;                                  // 0x004AFC11

    case '[':                                            // class 7 - 0x004AFC16
        {
            ++index;                                     // 0x004AFC21
            for (std::uint32_t i = 0; i < 0x40; ++i)     // 0x004AFC16-0x004AFC22
            {
                reinterpret_cast<std::uint32_t*>(bitmap)[i] = 0;
            }
            local_28 = 0;                                // 0x004AFC24
            if (translated[index] == '~')                // 0x004AFC28-0x004AFC35
            {
                ++index;                                 // 0x004AFC37
                local_28 = 1;                            // 0x004AFC38
            }
            local_14 = index;                            // 0x004AFC31 / 0x004AFC40

            // 0x004AFC44: the head of the class scan. It is reached by fall-through from
            // 0x004AFC40 (the '~' test) and from 0x004AFC35; nothing branches to it, so it
            // carries no label. The continue edge is the separate 0x004AFC53 block below,
            // which is why the ']' test is not repeated on that path.
            if (translated[index] == ']') goto class_end;   // 0x004AFC44-0x004AFC4D
        class_continue:                                  // 0x004AFC53
            if (static_cast<std::int32_t>(index) >= static_cast<std::int32_t>(local_34)) goto fail;   // 0x004AFC53-0x004AFC57
            if (translated[index] == '\\')               // 0x004AFC5D-0x004AFC5F
            {
                const std::int32_t escaped = static_cast<std::int32_t>(static_cast<signed char>(translated[index + 1]));   // 0x004AFC61
                ++index;                                 // 0x004AFC69
                switch (escaped)                         // 0x004AFC6A-0x004AFC77: class lookup 0x004AFC6F, jump table 0x004AFC77
                {
                case 'T':                                // class 3 - 0x004AFC7E
                case 't':
                    bitmap[0x09] = 1;
                    break;
                case 'R':                                // class 2 - 0x004AFC85
                case 'r':
                    bitmap[0x0D] = 1;
                    break;
                case 'N':                                // class 1 - 0x004AFC8C
                case 'n':
                    bitmap[0x0A] = 1;
                    break;
                case 0:                                  // class 0 - the exit block
                    goto fail;
                default:                                 // class 4, and every character above 0x74 - 0x004AFC93
                    reinterpret_cast<std::int8_t*>(bitmap)[escaped] = 1;
                    break;
                }
            }
            else if (translated[index] == '/')           // 0x004AFC9A-0x004AFC9C
            {
                const std::int32_t next = static_cast<std::int32_t>(static_cast<signed char>(translated[index + 1]));   // 0x004AFC9E
                ++index;                                 // 0x004AFCA6
                reinterpret_cast<std::int8_t*>(bitmap)[next] = 1;   // 0x004AFCA7
            }
            else if (translated[index + 1] == '-')       // 0x004AFCAE-0x004AFCB6
            {
                const std::int32_t last = static_cast<std::int32_t>(static_cast<signed char>(translated[index + 2]));   // 0x004AFCB8
                index += 2;                              // 0x004AFCC0
                const std::int32_t first = static_cast<std::int32_t>(static_cast<signed char>(translated[index - 2]));  // 0x004AFCC3
                if (first >= last) goto fail;            // 0x004AFCC6-0x004AFCC8
                const std::int32_t count = last - first + 1;    // 0x004AFCCE-0x004AFCD4
                std::int8_t* target = reinterpret_cast<std::int8_t*>(bitmap) + first;   // 0x004AFCD0
                for (std::int32_t i = 0; i < count; ++i) target[i] = 1;                 // 0x004AFCD5-0x004AFCEA
            }
            else
            {
                const std::int32_t literal = static_cast<std::int32_t>(static_cast<signed char>(translated[index]));    // 0x004AFCEE
                reinterpret_cast<std::int8_t*>(bitmap)[literal] = 1;    // 0x004AFCF1
            }
            {
                const char next = translated[index + 1]; // 0x004AFCF6
                ++index;                                 // 0x004AFCFD
                local_14 = index;                        // 0x004AFD00
                if (next != ']') goto class_continue;    // 0x004AFCFE-0x004AFD04
            }

        class_end:                                       // 0x004AFD0A
            if (local_28 != 0)                           // 0x004AFD0A-0x004AFD10
            {
                for (std::uint32_t i = 0; i < 0x100; ++i)   // 0x004AFD12-0x004AFD27
                {
                    bitmap[i] = (bitmap[i] == 0) ? 1 : 0;   // 0x004AFD14-0x004AFD1D
                }
                node_count = local_24;                   // 0x004AFD29
            }

            std::uint32_t record = 0;                    // eax
            if (static_cast<std::int32_t>(local_18) > 0) // 0x004AFD2D-0x004AFD35
            {
                local_28 = reinterpret_cast<std::uintptr_t>(class_records);   // 0x004AFD37-0x004AFD3E
                for (;;)                                 // 0x004AFD42
                {
                    const std::uint8_t* candidate = reinterpret_cast<const std::uint8_t*>(local_28);
                    std::uint32_t i = 0;
                    index = 0;                           // 0x004AFD4B, restored at 0x004AFD71
                    while (i < 0x100 && candidate[i] == bitmap[i]) ++i;   // 0x004AFD42-0x004AFD4F
                    if (i == 0x100) break;               // 0x004AFD4F
                    ++record;                            // 0x004AFD59
                    local_28 = reinterpret_cast<std::uintptr_t>(candidate + 0x100);   // 0x004AFD51-0x004AFD62
                    if (static_cast<std::int32_t>(record) >= static_cast<std::int32_t>(local_18)) break;   // 0x004AFD55-0x004AFD66
                }
                if (record >= 0x20) goto fail;           // 0x004AFD68-0x004AFD6B
                index = local_14;                        // 0x004AFD71
                node_count = local_24;                   // 0x004AFD75
            }
            if (record == local_18)                      // 0x004AFD7D-0x004AFD7F
            {
                std::uint8_t* appended = class_records + (static_cast<std::size_t>(local_18) << 8);   // 0x004AFD81-0x004AFD90
                for (std::uint32_t i = 0; i < 0x100; ++i)   // 0x004AFD92-0x004AFD96
                {
                    appended[i] = bitmap[i];
                }
                ++local_18;                              // 0x004AFD98-0x004AFDA1
            }
            *reinterpret_cast<std::uint32_t*>(node + 8) = record | 0x1000U;   // 0x004AFDA5, then 0x004AFE0C
        }
        goto emit;                                       // 0x004AFE0F

    case '{':                                            // class 9 - 0x004AFDAA
        if (local_20 != 0) goto fail;                    // 0x004AFDAA-0x004AFDB0
        if (local_10 != 0) goto fail;                    // 0x004AFDB6-0x004AFDBC
        *reinterpret_cast<std::uint32_t*>(node + 8) = 0x400U;               // 0x004AFDC2
        *reinterpret_cast<std::uint32_t*>(node + 0x10) = node_index;        // 0x004AFDC9
        *reinterpret_cast<std::uint32_t*>(node + 0x0C) = node_index;        // 0x004AFDCC
        local_20 = 1;                                    // 0x004AFDCF
        goto loop_tail;                                  // 0x004AFDD7

    case '}':                                            // class 10 - 0x004AFDD9
        if (local_20 != 1) goto fail;                    // 0x004AFDD9-0x004AFDDE
        if (local_10 != 0) goto fail;                    // 0x004AFDE4-0x004AFDEA
        *reinterpret_cast<std::uint32_t*>(node + 8) = 0x800U;               // 0x004AFDEC
        *reinterpret_cast<std::uint32_t*>(node + 0x10) = node_index;        // 0x004AFDF3
        *reinterpret_cast<std::uint32_t*>(node + 0x0C) = node_index;        // 0x004AFDF6
        local_20 = 0;                                    // 0x004AFDF9
        goto loop_tail;                                  // 0x004AFE01

    case '/':                                            // class 5 - 0x004AFE03
        ++index;
        goto emit_char;                                  // falls into 0x004AFE04

    default:                                             // class 11, and every character outside 0x24-0x7D - 0x004AFE04
    emit_char:
        *reinterpret_cast<std::uint32_t*>(node + 8) = static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<signed char>(translated[index])));
        goto emit;                                       // 0x004AFE0C
    }

emit:                                                    // 0x004AFE0F
    *reinterpret_cast<std::uint32_t*>(node + 0x10) = 0xFFFFFFFFU;   // 0x004AFE0F
    *reinterpret_cast<std::uint32_t*>(node + 0x0C) = node_index;    // 0x004AFE16

loop_tail:                                               // 0x004AFE19
    ++index;                                             // 0x004AFE1D
    ++node_count;                                        // 0x004AFE1E
    ++node_index;                                        // 0x004AFE1F
    node += 0x14;                                        // 0x004AFE20
    local_24 = node_count;                               // 0x004AFE25
    if (static_cast<std::int32_t>(index) < static_cast<std::int32_t>(local_34)) goto head;   // 0x004AFE23-0x004AFE29

    if (local_20 != 0) goto fail;                        // 0x004AFE2F-0x004AFE35
    if (local_10 != 0) goto fail;                        // 0x004AFE37-0x004AFE3D

done:                                                    // 0x004AFE3F
    {
        std::uint8_t* terminal = node_array + static_cast<std::size_t>(node_count) * 0x14;   // 0x004AFE3F-0x004AFE4A
        *reinterpret_cast<std::uint32_t*>(terminal + 8) = 0x100U;       // 0x004AFE4D
        *reinterpret_cast<std::uint32_t*>(terminal + 0x10) = node_count;    // 0x004AFE54
        *reinterpret_cast<std::uint32_t*>(terminal + 0x0C) = node_count;    // 0x004AFE57
    }
    return 1;                                            // 0x004AFE5C

fail:                                                    // 0x004AFE69
    return 0;                                            // 0x004AFE6C
}

} // extern "C"

#undef NATIVE_DECL
