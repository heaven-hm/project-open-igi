#include "igi/retail/script/qvm_execute.hpp"

#include "igi/retail/script/qvm_native_loader.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace igi::retail::script {

// Retail QVM state words touched by Qvm_Execute (0x004B85B0).
//
// 0x00A965A0 - Qvm_SetGlobalState()'s dword, read by the source-block loop.
// 0x00A965A4 - execution-abort flag written by BreakScript (0x004B8C10) and here.
// 0x00A965A8 - "an error has been reported" flag, also stored by Qvm_ReportError (0x004B7E10).
// 0x00A96598 - error-reporting enabled dword, written by Sys_ExecuteBytecode_23 (0x004B8970).
//
// qvm_native_loader.cpp currently defines 0x00A965A0/0x00A965A8/0x00A96598 inside its
// anonymous namespace. These declarations intentionally keep the retail storage shared but are
// unresolved until those three definitions are given external linkage; a local duplicate here
// would silently split the state that Qvm_ReportError and Qvm_SetGlobalState update.
extern std::int32_t g_a965a0;
extern volatile std::int32_t g_a96598;
extern volatile std::int32_t g_a965a8;

// 0x00A965A4, retail storage owned by this unit.
std::int32_t g_a965a4 = 0;

namespace {

// Retail offsets inside the 0x94-byte QVM object built by Qvm_Load (0x004B80B0).
constexpr std::size_t kQvmDataOffset = 0x80U;
constexpr std::size_t kQvmDataSizeOffset = 0x84U;
constexpr std::size_t kQvmProgramOffset = 0x8CU;
constexpr std::size_t kProgramErrorTextOffset = 0x268U;

// Retail offsets inside the frame handed to a running script native.
constexpr std::size_t kArgumentTableFlagOffset = 0x04U;
constexpr std::size_t kArgumentTableOffset = 0x08U;

// Qvm_Execute's 0x854-byte frame: a 0x20-byte source stack at -0x850, a 0x30-byte source
// buffer at -0x830 (0x004BFC70 receives the dword at its +0x08 as the parsed-block out slot),
// a 0x800-byte text arena at -0x800 and the 4-byte parsed-block slot at -0x854. The 0x800
// arena is never initialised by the retail before it is passed to 0x004C0200, so it stays
// uninitialised here too.
struct alignas(4) QvmSourceStack
{
    std::byte storage[0x20];
};

struct alignas(4) QvmSourceBuffer
{
    std::uint32_t field_00;
    std::uint32_t field_04;
    std::uint32_t block_slot; // +0x08, the dword 0x004BFC70 receives as its out slot
    std::byte storage[0x24];
};

static_assert(sizeof(QvmSourceStack) == 0x20, "retail source-stack frame size");
static_assert(sizeof(QvmSourceBuffer) == 0x30, "retail source-buffer frame size");

[[nodiscard]] std::uint32_t load_word(const void* base, std::size_t offset) noexcept
{
    std::uint32_t value = 0;
    std::memcpy(&value, static_cast<const std::byte*>(base) + offset, sizeof value);
    return value;
}

[[nodiscard]] void* pointer_from_word(std::uint32_t word) noexcept
{
    // The retail keeps 32-bit pointers in its QVM object and argument table.
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(word));
}

} // namespace

} // namespace igi::retail::script

// Retail callees of Qvm_Execute (0x004B85B0) and Qvm_GetFloatArg (0x004B8A50). None of them is
// transcribed on this branch yet, so they are declared with their retail cdecl signature and
// stay unresolved at link time; the source keeps the retail call order and argument layout.
extern "C" {

// 0x004BD080 - runs the resolved precompiled program and returns the interpreter's error code
// (0 when the program ran); it drives Qvm_ExecuteInstructions (0x004BCFD0).
std::int32_t NATIVE_DECL Qvm_RunProgram(std::uint32_t program);

// 0x004C0200 - initialises the script-source buffer over the loaded resource.
void NATIVE_DECL Qvm_SourceBufferInit(void* buffer, void* source_stack, void* text,
                                                 std::int32_t text_size);

// 0x004BF1C0 - attaches the loaded script bytes to the source reader.
void NATIVE_DECL Qvm_SourceReaderInit(void* source_stack, std::uint32_t data,
                                                  std::uint32_t size);

// 0x004BFC70 - reads the next source block; returns 0 at the end of the buffer.
std::int32_t NATIVE_DECL Qvm_ReadNextBlock(void* buffer, void* out_block);

// 0x004BF1F0 - parses one block; returns non-zero when a block was produced.
std::int32_t NATIVE_DECL Qvm_ParseBlock(void* buffer, void* out_block);

// 0x004BFA80 - recursively releases a parsed block tree.
void NATIVE_DECL Qvm_ReleaseBlockTree(void* block);

// 0x004BF8F0 - releases one parsed block.
void NATIVE_DECL Qvm_ReleaseBlock(void* block);

// 0x004BEFC0 - releases the source reader. Its EAX is returned unchanged as Qvm_Execute's
// result on the source path.
std::int32_t NATIVE_DECL Qvm_ReleaseSourceReader(void* source_stack);

// 0x004BD010 - reads argument `index` from the interpreter frame and returns it as a real in
// ST0 (the retail body ends with `fld dword [esp+0x14]`).
long double NATIVE_DECL Qvm_ReadIndexedArgument(void* argument_frame,
                                                            std::int32_t index);

// 0x004B9660 - evaluates one argument expression node and leaves its real value in ST0
// (`fild` / `fld dword` / `fld qword`); its recursion, type dispatch, "Unknown identifier
// '%s' in expression, line %d" error report and 0x00A965A8 error store live inside it.
long double NATIVE_DECL Qvm_EvaluateArgumentValue(const void* node);

} // extern "C"

namespace igi::retail::script {

extern "C" std::int32_t NATIVE_DECL Qvm_Execute(void* qvm)
{
    g_a965a4 = 0;
    g_a965a8 = 0;

    const std::uint32_t program = load_word(qvm, kQvmProgramOffset);
    if (program != 0U)
    {
        // Precompiled-program path: the retail executes qvm+0x8C directly and only reports
        // when the program runner returns a non-zero error code.
        const std::int32_t run_result =
            Qvm_RunProgram(static_cast<std::uint32_t>(program));
        g_a965a4 = 0;
        if (run_result == 0)
        {
            return 0;
        }

        const std::int32_t report_result = Qvm_ReportError(
            "Error in QVM program: %s\r\n%s", static_cast<const char*>(qvm),
            reinterpret_cast<const char*>(static_cast<std::uintptr_t>(
                static_cast<std::uint32_t>(program + kProgramErrorTextOffset))));
        g_a965a4 = 0;
        return report_result;
    }

    // Script-source path: parse the loaded resource block by block and discard every parsed
    // block. This is the path Qvm_Load leaves available when it did not resolve a precompiled
    // program into the object.
    QvmSourceStack source_stack;
    QvmSourceBuffer source_buffer;
    std::byte text[0x800];

    Qvm_SourceBufferInit(&source_buffer, &source_stack, text, 0x800);
    Qvm_SourceReaderInit(&source_stack, load_word(qvm, kQvmDataOffset),
                                load_word(qvm, kQvmDataSizeOffset));

    for (;;)
    {
        if (g_a965a0 != 0 && g_a965a4 != 0)
        {
            break;
        }

        std::uint32_t parsed_block = 0;
        if (Qvm_ReadNextBlock(&source_buffer, &source_buffer.block_slot) == 0)
        {
            break;
        }

        if (Qvm_ParseBlock(&source_buffer, &parsed_block) != 0)
        {
            void* const block = pointer_from_word(parsed_block);
            Qvm_ReleaseBlockTree(block);
            Qvm_ReleaseBlock(block);
            if (g_a965a8 == 0)
            {
                continue;
            }
        }

        if (g_a96598 != 0)
        {
            Qvm_ReportError("An error occured in script file: %s", static_cast<const char*>(qvm));
        }
        g_a965a8 = 0;
        break;
    }

    const std::int32_t release_result = Qvm_ReleaseSourceReader(&source_stack);
    g_a965a4 = 0;
    return release_result;
}

extern "C" long double NATIVE_DECL Qvm_GetFloatArg(void* argument_frame, std::int32_t index)
{
    if (load_word(argument_frame, kArgumentTableFlagOffset) != 0U)
    {
        return Qvm_ReadIndexedArgument(argument_frame, index);
    }

    const std::uint32_t table = load_word(argument_frame, kArgumentTableOffset);
    const auto* entries = reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uintptr_t>(table));
    const std::uint32_t node = entries[static_cast<std::uint32_t>(index)];
    return Qvm_EvaluateArgumentValue(pointer_from_word(node));
}

} // namespace igi::retail::script
