// Canonical GameNatives candidate: Qvm_Execute - retail VA 0x004B85B0.
//
// THIS IS A RECONSTRUCTION, NOT A VERBATIM ORIGINAL SOURCE.
// The Project IGI 1 retail binary ships without source. This file is a transcription of the
// machine code at 0x004B85B0 in igi.exe, written from the retail disassembly/decompilation and
// kept in C++ form so the candidate can be reviewed. It is not the original source and it does
// not claim instruction-for-instruction identity.
//
// Artifact: retail igi.exe, SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Retail body: VA 0x004B85B0-0x004B86DC, 301 bytes,
//              SHA-256 ae94640768a02e53ab44e0873dda2ded792eca51ddf4f5dc7b912d359151ecdc
//              (verified by hashing the retail .text bytes at file offset 0xB85B0).
// Tools/evidence: Binary Ninja 6.0.10601 MCP, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on the
//              retail IGI.EXE; docs/fidelity/evidence/0x004B85B0-*.json.
// CFG: 14 blocks / 20 edges in the retail image (Binary Ninja and Radare2 agree; angr splits at
//              calls and recovers 24 blocks / 30 edges, no unresolved jumps).
// Signature: x86 cdecl int32_t(void* qvm). Binary Ninja's stored prototype is int32_t(void*) and
//              its user note calls this "the interpreter entry"; the body contains no instruction
//              loop, so that note is recorded as a conflict rather than followed. The precompiled
//              path delegates to 0x004BD080, which drives Qvm_ExecuteInstructions (0x004BCFD0);
//              the fallback path parses and discards script source blocks.
// Behavior: clears the two QVM state words, reads the resolved program pointer at qvm+0x8C. When it
//              is non-null it runs 0x004BD080(program) and returns 0, or reports
//              "Error in QVM program: %s\r\n%s" with the object path and the program error text at
//              program+0x268 and returns the reporter's result. When it is null it initialises a
//              script-source buffer over qvm+0x80/+0x84 and loops: read the next block, parse it,
//              release the parsed node, and stop on end-of-buffer, on the stop/abort words, or on a
//              reported error (reporting "An error occured in script file: %s" when reporting is
//              enabled); then it releases the source reader and returns its result.
// Status: STRUCTURALLY_VERIFIED against retail 0x004B85B0 (14 blocks/20 edges; the recompiled
//              object has 14 blocks/21 edges, the extra edge being GCC's jump from the source-path
//              tail to the shared return block). Arithmetic, calls and side effects are reviewed in
//              docs/fidelity/evidence/0x004B85B0-semantic-review.json. NOT BUILD_VERIFIED as a
//              standalone unit, NOT RUNTIME_TESTED and NOT VERIFIED: the eight retail callees are
//              declared but not transcribed on this branch, and the three shared QVM state words
//              this native reads are still internal to qvm_native_loader.cpp, so the object does
//              not link yet and no execution path past this function can run.
//
// Canonical implementation record: engine/src/retail/script/qvm_execute.cpp (this file is the
// address-bound GameNatives reference copy of the same transcription, not a second implementation).
//
// The previous file at this path was an uncorrected Ghidra/r2 dump that called Sys_FreeMemory_22
// with the report format string instead of Qvm_ReportError (0x004B7E10) and repeated the stale
// "script result value" claim. It was archived to
// docs/fidelity/evidence/superseded/004B85B0-Qvm_Execute-ghidra-dump-20261023.cpp and replaced by
// this transcription of the retail body.

#include <cstddef>
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

// Retail QVM state words touched by Qvm_Execute (0x004B85B0).
//
// 0x00A965A0 - Qvm_SetGlobalState()'s dword, read by the source-block loop.
// 0x00A965A4 - execution-abort flag written by BreakScript (0x004B8C10) and by this native.
// 0x00A965A8 - "an error has been reported" flag, also stored by Qvm_ReportError (0x004B7E10).
// 0x00A96598 - error-reporting enabled dword, written by Sys_ExecuteBytecode_23 (0x004B8970).
//
// Storage is owned by the verified transcription: g_a965a4 by qvm_execute.cpp and the other three
// by qvm_native_loader.cpp. They are declared here, never defined, so this reference copy cannot
// split the shared retail storage.
extern "C" {
extern std::int32_t g_a965a0;
extern std::int32_t g_a965a4;
extern volatile std::int32_t g_a96598;
extern volatile std::int32_t g_a965a8;
}

// Retail callees of 0x004B85B0. None is transcribed on this branch; they are declared with their
// retail cdecl signature and stay unresolved at link time, exactly as recorded for the canonical
// implementation. The call order and argument layout below are the retail ones.
extern "C" {

// 0x004BD080 - runs the resolved precompiled program and returns the interpreter's error code.
std::int32_t IGI_RETAIL_CDECL Qvm_RunProgram_4BD080(std::uint32_t program);

// 0x004C0200 - initialises the script-source buffer over the loaded resource.
void IGI_RETAIL_CDECL Qvm_SourceBufferInit_4C0200(void* buffer, void* source_stack, void* text,
                                                 std::int32_t text_size);

// 0x004BF1C0 - attaches the loaded script bytes to the source reader.
void IGI_RETAIL_CDECL Qvm_SourceReaderInit_4BF1C0(void* source_stack, std::uint32_t data,
                                                  std::uint32_t size);

// 0x004BFC70 - reads the next source block; returns 0 at the end of the buffer.
std::int32_t IGI_RETAIL_CDECL Qvm_ReadNextBlock_4BFC70(void* buffer, void* out_block);

// 0x004BF1F0 - parses one block; returns non-zero when a block was produced.
std::int32_t IGI_RETAIL_CDECL Qvm_ParseBlock_4BF1F0(void* buffer, void* out_block);

// 0x004BFA80 - recursively releases a parsed block tree.
void IGI_RETAIL_CDECL Qvm_ReleaseBlockTree_4BFA80(void* block);

// 0x004BF8F0 - releases one parsed block.
void IGI_RETAIL_CDECL Qvm_ReleaseBlock_4BF8F0(void* block);

// 0x004BEFC0 - releases the source reader; its EAX is returned unchanged on the source path.
std::int32_t IGI_RETAIL_CDECL Qvm_ReleaseSourceReader_4BEFC0(void* source_stack);

// 0x004B7E10 - reports an error and returns the reporter's result.
std::int32_t IGI_RETAIL_CDECL Qvm_ReportError(const char* format, ...);

} // extern "C"

namespace {

// Retail offsets inside the 0x94-byte QVM object built by Qvm_Load (0x004B80B0).
constexpr std::size_t kQvmDataOffset = 0x80U;
constexpr std::size_t kQvmDataSizeOffset = 0x84U;
constexpr std::size_t kQvmProgramOffset = 0x8CU;
constexpr std::size_t kProgramErrorTextOffset = 0x268U;

// Qvm_Execute's 0x854-byte frame: a 0x20-byte source stack at -0x850, a 0x30-byte source buffer at
// -0x830 (0x004BFC70 receives the dword at its +0x08 as the parsed-block out slot), a 0x800-byte
// text arena at -0x800 and the 4-byte parsed-block slot at -0x854. The 0x800 arena is never
// initialised by the retail before it is passed to 0x004C0200, so it stays uninitialised here too.
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

extern "C" std::int32_t IGI_RETAIL_CDECL Qvm_Execute(void* qvm)
{
    g_a965a4 = 0;
    g_a965a8 = 0;

    const std::uint32_t program = load_word(qvm, kQvmProgramOffset);
    if (program != 0U)
    {
        // Precompiled-program path: the retail executes qvm+0x8C directly and only reports when
        // the program runner returns a non-zero error code.
        const std::int32_t run_result = Qvm_RunProgram_4BD080(program);
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

    // Script-source path: parse the loaded resource block by block and discard every parsed block.
    // This is the path Qvm_Load leaves available when it did not resolve a precompiled program.
    QvmSourceStack source_stack;
    QvmSourceBuffer source_buffer;
    std::byte text[0x800];

    Qvm_SourceBufferInit_4C0200(&source_buffer, &source_stack, text, 0x800);
    Qvm_SourceReaderInit_4BF1C0(&source_stack, load_word(qvm, kQvmDataOffset),
                                load_word(qvm, kQvmDataSizeOffset));

    for (;;)
    {
        if (g_a965a0 != 0 && g_a965a4 != 0)
        {
            break;
        }

        std::uint32_t parsed_block = 0;
        if (Qvm_ReadNextBlock_4BFC70(&source_buffer, &source_buffer.block_slot) == 0)
        {
            break;
        }

        if (Qvm_ParseBlock_4BF1F0(&source_buffer, &parsed_block) != 0)
        {
            void* const block = pointer_from_word(parsed_block);
            Qvm_ReleaseBlockTree_4BFA80(block);
            Qvm_ReleaseBlock_4BF8F0(block);
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

    const std::int32_t release_result = Qvm_ReleaseSourceReader_4BEFC0(&source_stack);
    g_a965a4 = 0;
    return release_result;
}
