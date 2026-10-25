// Retail command-line parser, tokenizer and the CLI handler bodies of Project
// IGI 1, transcribed from igi.exe and integrated after individual retail
// static + full-game runtime verification.
//
// Static evidence: Binary Ninja MCP (selected decompiler: function info, pseudo C,
// disassembly), Radare2 CFG, angr CFGFast per address in
// build/native-analysis/0x0048F360/capture.json and
// build/audit/p05c-20261023T223344Z/per-native/0{48f350,48f920,48f930,48f950}.md,
// build/audit/p05b-20261023T222309Z/{bn/tokenizer.json,per-native/0048f360.md}.
// Retail runtime evidence (Tester, suspended spawn, full game, cwd
// /Users/haseeb-mir/Downloads/IGI): parser 7 calls in `window level1` with
// callers/args/returns under build/tester/20261023T220230Z/p05c/0x0048F360.json
// (runtime/retail-parser-window-level1.json); tokenizer 21 calls in the same
// window; handlers observed with empty suffix pointers in their token
// scenarios: Window in `window level1`, HardcoreGerman in
// `window level1 HardcoreGerman`, UnlimitedAmmo in `window level1
// UnlimitedAmmo` (0x410360 is a later-phase dependency and is NOT implemented
// here). Handler dispatch is token-gated: registration alone never runs a
// handler, so `window level1` executes only the Window handler. Candidate
// runtime is unobserved until main wiring (P13); no status above
// STRUCTURALLY_VERIFIED is claimed.

#include "dev_log.hpp"
#include "igi/retail/core/app_startup_globals.hpp"

#include <cstddef>
#include <cstdint>

extern "C"
{
    // Brokers: retail 0x0048F080 GetCommandLine (linked engine equivalent:
    // app_startup_natives.cpp GameCommandLine).
    char const* NATIVE_DECL GameCommandLine();

    // The parser's cdecl handler ABI: int __cdecl f(char* suffix); the suffix
    // points inside the parser's own stack copy of the command line, right
    // after the matched option name.
    typedef std::int32_t(NATIVE_DECL* AppMain_CmdLineHandler)(char* suffix);
}

// Retail tokenizer continuation pointer 0x00936060 (the retail image stores the
// next scan position there); the port keeps the same state in this file-scope
// pointer because the absolute address is not mapped in the candidate image.
static char* g_936060_token_state = nullptr;

extern "C"
{

// Native: AppMain_TokenizeDelimited; retail VA 0x004A5A0C
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05b-20261023T222309Z/bn/tokenizer.json; BN catalog alias Camera_SetFieldOfView_6 is wrong
// Behavior/CFG: 13 BN blocks, 156 bytes (0x4A5A0C..0x4A5AA7): builds a 32-byte delimiter bitmap from the delimiter string (including its NUL), picks arg1 or the static token pointer 0x936060 when arg1 is NULL, skips leading delimiter bytes (stopping at NUL), then advances until a delimiter byte, writes NUL over that delimiter, stores the advanced pointer in 0x936060 and returns the token start, or 0 when the token would be empty; mutates the input string
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (21 calls in the startup window: start with the parser stack copy and delimiter literal 0x541D6C, continuation with a null first argument, terminating null); candidate runtime unobserved until main wiring; the retail continuation pointer 0x00936060 is a file-scope pointer in the port
char* NATIVE_DECL AppMain_TokenizeDelimited(char* text, char const* delimiters)
{
    NATIVE_LOG_ENTER("AppMain_TokenizeDelimited", "0x004A5A0C");
    unsigned char delimiter_bitmap[0x20] = {};
    for (char const* current = delimiters;; ++current)
    {
        const unsigned int character = static_cast<unsigned char>(*current);
        delimiter_bitmap[character >> 3] |= static_cast<unsigned char>(1u << (character & 7));
        if (character == 0)
        {
            break;
        }
    }

    // Retail keeps the continuation pointer in the image global 0x00936060; the
    // port cannot use that absolute address, so the same role is a file-scope
    // state pointer documented as the retail 0x936060 correspondence.
    char* position = (text != nullptr) ? text : g_936060_token_state;
    while ((delimiter_bitmap[static_cast<unsigned char>(*position) >> 3] &
            static_cast<unsigned char>(1u << (static_cast<unsigned char>(*position) & 7))) != 0)
    {
        if (*position == '\0')
        {
            break;
        }
        ++position;
    }

    char* const token = position;
    while (*position != '\0' &&
           (delimiter_bitmap[static_cast<unsigned char>(*position) >> 3] &
            static_cast<unsigned char>(1u << (static_cast<unsigned char>(*position) & 7))) == 0)
    {
        ++position;
    }
    if (*position != '\0' &&
        (delimiter_bitmap[static_cast<unsigned char>(*position) >> 3] &
         static_cast<unsigned char>(1u << (static_cast<unsigned char>(*position) & 7))) != 0)
    {
        *position = '\0';
        ++position;
    }

    g_936060_token_state = position;
    return (token != position) ? token : nullptr;
}

// Native: AppMain_ParseCmdLineArgs; retail VA 0x0048F360
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG (22 blocks, 33 edges) and angr CFGFast (42 nodes/61 edges, automatic recovery); static capture build/native-analysis/0x0048F360/capture.json and build/audit/p05b-20261023T222309Z/per-native/0048f360.md
// Behavior/CFG: 286 bytes 0x48F360..0x48F47D; frame 0x208 with a 0x200 stack copy of the stored command line 0x48F080; tokenizes that copy with 0x4A5A0C(" \t"); for each token longer than or equal to the option name it folds a-z to A-Z and compares the first strlen(name) bytes case-insensitively, and on equality calls the cdecl handler with token + strlen(name) (suffix inside the stack copy), ignoring the handler return; returns the final tokenizer result (null at end of the line); mutates only its stack copy
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed 7 calls in `window level1` (main Window 0x48F6A6, WinInput 0x48F6B5, Germany 0x48F6C4, USA 0x48F6D3, HardcoreGerman 0x48F7BE; Game_Init "Level" 0x41553A; HumanPlayer_Init "UnlimitedAmmo" 0x410066), all returning 0; absent tokens mean the handler is registered and matched-checked but never dispatched; candidate runtime unobserved until main wiring
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
void* NATIVE_DECL AppMain_ParseCmdLineArgs(char* option_name, AppMain_CmdLineHandler handler)
{
    NATIVE_LOG_ENTER("AppMain_ParseCmdLineArgs", "0x0048F360");
    char copy[0x200];
    char const* command_line = GameCommandLine();

    std::size_t copied = 0;
    while (command_line[copied] != '\0')
    {
        ++copied;
    }
    ++copied; // retail copies strlen+1 bytes, including the NUL
    for (std::size_t index = 0; index < copied; ++index)
    {
        copy[index] = command_line[index];
    }

    char* token = AppMain_TokenizeDelimited(copy, " \t");
    void* result = token;
    if (token == nullptr)
    {
        return result;
    }

    const std::size_t option_length = [&]() {
        std::size_t length = 0;
        while (option_name[length] != '\0')
        {
            ++length;
        }
        return length;
    }();

    for (;;)
    {
        std::size_t token_length = 0;
        while (token[token_length] != '\0')
        {
            ++token_length;
        }

        if (token_length >= option_length && option_length > 0)
        {
            bool matches = true;
            for (std::size_t index = 0; index < option_length; ++index)
            {
                unsigned char token_character = static_cast<unsigned char>(token[index]);
                unsigned char name_character = static_cast<unsigned char>(option_name[index]);
                if (token_character >= 'a' && token_character <= 'z')
                {
                    token_character = static_cast<unsigned char>(token_character - 0x20);
                }
                if (name_character >= 'a' && name_character <= 'z')
                {
                    name_character = static_cast<unsigned char>(name_character - 0x20);
                }
                if (token_character != name_character)
                {
                    matches = false;
                    break;
                }
            }
            if (matches)
            {
                (void)handler(token + option_length);
            }
        }

        result = AppMain_TokenizeDelimited(nullptr, " \t");
        if (result == nullptr)
        {
            break;
        }
        token = static_cast<char*>(result);
    }
    return result;
}

// Native: AppMain_WindowCommand; retail VA 0x0048F350
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05c-20261023T223344Z/per-native/0048f350.md; BN catalog alias CombatState_Invoke23 is wrong
// Behavior/CFG: 9 bytes `push 0; call 0x48F120; pop ecx; retn`; discards the suffix pointer, sets the screen mode to 0 (windowed) and returns the setter result (0); registered by main for the "Window" token at 0x48F680/0x48F687
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (1 call in `window level1` at caller 0x48F455 with empty suffix, EAX 0, then SetScreenMode(0) with the 0x5C8C00 1->0 transition); candidate runtime unobserved until main wiring
std::int32_t NATIVE_DECL AppMain_WindowCommand(char* suffix)
{
    NATIVE_LOG_ENTER("AppMain_WindowCommand", "0x0048F350");
    (void)suffix;
    return Display_SetScreenMode(0);
}

// Native: AppMain_WinInputCommand; retail VA 0x0048F920
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05c-20261023T223344Z/per-native/0048f920.md; BN catalog alias CombatState_ProcessGermany is wrong
// Behavior/CFG: 9 bytes `push 1; call 0x48F280; pop ecx; retn`; discards the suffix pointer, sets the Win32-input flag to 1 and returns the setter result (1); registered by main for the "WinInput" token at 0x48F6A1/0x48F6AB
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (1 call in `window level1 WinInput` at caller 0x48F455 with empty suffix, EAX 1, then F280(1) -> [0x5C8BE0]=1); registration alone never dispatches this handler; candidate runtime unobserved until main wiring; BN catalog alias is wrong
std::int32_t NATIVE_DECL AppMain_WinInputCommand(char* suffix)
{
    NATIVE_LOG_ENTER("AppMain_WinInputCommand", "0x0048F920");
    (void)suffix;
    return Input_SetUseWin32Input(1);
}

// Native: AppMain_GermanyCommand; retail VA 0x0048F930
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05c-20261023T223344Z/per-native/0048f930.md; BN catalog alias CombatState_ProcessUSA is wrong
// Behavior/CFG: 21 bytes: `push "Germany" (0x541D60); call 0x48F300; push 1; call 0x48F2B0; add esp,8; retn`; discards the suffix pointer, sets the language string to "Germany" and the German-version flag to 1, returns the flag setter result (1); registered by main for the "Germany" token at 0x48F6B0/0x48F6BA
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (1 call in `window level1 Germany` at caller 0x48F455 with empty suffix, EAX 1, then F300("Germany") EAX 8 and F2B0(1) -> [0x5C8C20]="Germany", [0x5C8BDC]=1); registration alone never dispatches this handler; candidate runtime unobserved until main wiring; BN catalog alias is wrong
std::int32_t NATIVE_DECL AppMain_GermanyCommand(char* suffix)
{
    NATIVE_LOG_ENTER("AppMain_GermanyCommand", "0x0048F930");
    (void)suffix;
    (void)AppContext_SetLanguage("Germany");
    return AppContext_SetGermanVersion(1);
}

// Native: AppMain_USACommand; retail VA 0x0048F950
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05c-20261023T223344Z/per-native/0048f950.md; BN catalog alias CombatState_ProcessHardcoreGerman is wrong
// Behavior/CFG: 12 bytes: `push "USA" (0x5382C8); call 0x48F300; pop ecx; retn`; discards the suffix pointer, sets the language string to "USA" and returns the copied length (4); registered by main for the "USA" token at 0x48F6BF/0x48F6C9
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (1 call in `window level1 USA` at caller 0x48F455 with empty suffix, EAX 4, then F300("USA") EAX 4 -> [0x5C8C20]="USA"); registration alone never dispatches this handler; candidate runtime unobserved until main wiring; BN catalog alias is wrong
std::int32_t NATIVE_DECL AppMain_USACommand(char* suffix)
{
    NATIVE_LOG_ENTER("AppMain_USACommand", "0x0048F950");
    (void)suffix;
    return AppContext_SetLanguage("USA");
}

// Native: AppMain_HardcoreGermanCommand; retail VA 0x0048F960
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05d-20261023T225957Z/per-native/48f960.md; BN catalog alias Input_FormatDIErrString is wrong
// Behavior/CFG: 9 bytes `push 0; call 0x48F2B0; pop ecx; retn` (bytes 6a 00 e8 49 f9 ff ff 59 c3; r2 rel32 target verified); discards the suffix pointer, sets the German-version flag to 0 (inverse of the Germany handler, no language write) and returns the setter result (0); main registers it at 0x48F7AF/0x48F7B4 and parses at 0x48F7BE, the last parser registration in main
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (1 call in `window level1 HardcoreGerman` at caller 0x48F455 with empty suffix, EAX 0, then F2B0(0) write observed with no delta because the flag was already 0; plain `window level1` calls the parser at 0x48F7BE but never dispatches this handler without the token); candidate runtime unobserved until main wiring
std::int32_t NATIVE_DECL AppMain_HardcoreGermanCommand(char* suffix)
{
    NATIVE_LOG_ENTER("AppMain_HardcoreGermanCommand", "0x0048F960");
    (void)suffix;
    return AppContext_SetGermanVersion(0);
}

} // extern "C"
