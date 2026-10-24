// Native: DirectInput_ErrorText (placeholder until VERIFIED); retail VA 0x0048FA80-0x0048FBFD (381 bytes, 59 blocks, BN alias DIERR_GENERIC).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP decompile/disassembly + radare2 6.2.0 bytes + angr CFGFast; jump table 0x0048FC00 and the 25 DIERR_* literals are image .rdata; DINPUT-LAYER packet §3.2.
// Behavior/CFG: HRESULT -> DIERR_* literal in retail branch order: 0x80040209 first, then the hr<=0x80040202 switch, then the (hr+0x7FFBFDFD)<=5 range switch, then the second switch (0x8004020A/0x80070002/...), default "Unknown error".
// Status/limits: TRANSCRIBED; no callees, data-only mapping; strings byte-identical to the retail literals. Runtime UNOBSERVED.
#include <cstdint>

extern "C" {

char const* DirectInput_ErrorText(std::int32_t hr)
{
    // 0x0048FA80: cmp eax,0x80040209; je -> DIERR_UNPLUGGED.
    if (hr == 0x80040209)
    {
        return "DIERR_UNPLUGGED";
    }
    // 0x0048FA8D: cmp eax,0x80040202; jg 0x0048FAF0.
    if (hr <= 0x80040202)
    {
        switch (hr)
        {
        case static_cast<std::int32_t>(0x80004001U): return "DIERR_UNSUPPORTED";
        case static_cast<std::int32_t>(0x80004002U): return "DIERR_NOINTERFACE";
        case static_cast<std::int32_t>(0x80004005U): return "DIERR_GENERIC";
        case static_cast<std::int32_t>(0x80040110U): return "DIERR_NOAGGREGATION";
        case static_cast<std::int32_t>(0x80040154U): return "DIERR_DEVICENOTREG";
        case static_cast<std::int32_t>(0x80040200U): return "DIERR_INSUFFICIENTPRIVS";
        case static_cast<std::int32_t>(0x80040201U): return "DIERR_DEVICEFULL";
        case static_cast<std::int32_t>(0x80040202U): return "DIERR_MOREDATA";
        }
    }
    // 0x0048FAF0: lea ecx,[eax+0x7FFBFDFD]; cmp ecx,5; ja 0x0048FB50.
    else if (static_cast<std::uint32_t>(hr) + 0x7FFBFDFDU <= 5U)
    {
        switch (hr)
        {
        case static_cast<std::int32_t>(0x80040203U): return "DIERR_NOTDOWNLOADED";
        case static_cast<std::int32_t>(0x80040204U): return "DIERR_HASEFFECTS";
        case static_cast<std::int32_t>(0x80040205U): return "DIERR_NOTEXCLUSIVEACQUIRED";
        case static_cast<std::int32_t>(0x80040206U): return "DIERR_INCOMPLETEEFFECT";
        case static_cast<std::int32_t>(0x80040207U): return "DIERR_NOTBUFFERED";
        case static_cast<std::int32_t>(0x80040208U): return "DIERR_EFFECTPLAYING";
        }
    }
    // 0x0048FB50: second switch over the remaining literals; default 0x0048FBEA "Unknown error".
    switch (hr)
    {
    case static_cast<std::int32_t>(0x8004020AU): return "DIERR_REPORTFULL";
    case static_cast<std::int32_t>(0x80070002U): return "DIERR_NOTFOUND";
    case static_cast<std::int32_t>(0x80070005U): return "DIERR_OTHERAPPHASPRIO";
    case static_cast<std::int32_t>(0x8007000CU): return "DIERR_NOTACQUIRED";
    case static_cast<std::int32_t>(0x8007000EU): return "DIERR_OUTOFMEMORY";
    case static_cast<std::int32_t>(0x80070015U): return "DIERR_NOTINITIALIZED";
    case static_cast<std::int32_t>(0x8007001EU): return "DIERR_INPUTLOST";
    case static_cast<std::int32_t>(0x80070057U): return "DIERR_INVALIDPARAM";
    case static_cast<std::int32_t>(0x80070077U): return "DIERR_BADDRIVERVER";
    case static_cast<std::int32_t>(0x800700AAU): return "DIERR_ACQUIRED";
    case static_cast<std::int32_t>(0x8007047EU): return "DIERR_OLDDIRECTINPUTVERSION";
    case static_cast<std::int32_t>(0x80070481U): return "DIERR_BETADIRECTINPUTVERSION";
    case static_cast<std::int32_t>(0x800704DFU): return "DIERR_ALREADYINITIALIZED";
    }
    return "Unknown error";
}

} // extern "C"
