// Native: DirectInput_ErrorText_48FA80 (placeholder until VERIFIED); retail VA 0x0048FA80-0x0048FBFD (381 bytes, 59 blocks, BN alias DIERR_GENERIC).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP decompile/disassembly + radare2 6.2.0 bytes + angr CFGFast; jump table 0x0048FC00 and the 25 DIERR_* literals are image .rdata; DINPUT-LAYER packet §3.2.
// Behavior/CFG: HRESULT -> DIERR_* literal in retail branch order: 0x80040209 first, then the hr<=0x80040202 switch, then the (hr+0x7FFBFDFD)<=5 range switch, then the second switch (0x8004020A/0x80070002/...), default "Unknown error".
// Status/limits: TRANSCRIBED; no callees, data-only mapping; strings byte-identical to the retail literals. Runtime UNOBSERVED.
#include <cstdint>

extern "C" {

char const* DirectInput_ErrorText_48FA80(std::int32_t hr)
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
        case 0x80004001: return "DIERR_UNSUPPORTED";
        case 0x80004002: return "DIERR_NOINTERFACE";
        case 0x80004005: return "DIERR_GENERIC";
        case 0x80040110: return "DIERR_NOAGGREGATION";
        case 0x80040154: return "DIERR_DEVICENOTREG";
        case 0x80040200: return "DIERR_INSUFFICIENTPRIVS";
        case 0x80040201: return "DIERR_DEVICEFULL";
        case 0x80040202: return "DIERR_MOREDATA";
        }
    }
    // 0x0048FAF0: lea ecx,[eax+0x7FFBFDFD]; cmp ecx,5; ja 0x0048FB50.
    else if (static_cast<std::uint32_t>(hr) + 0x7FFBFDFDU <= 5U)
    {
        switch (hr)
        {
        case 0x80040203: return "DIERR_NOTDOWNLOADED";
        case 0x80040204: return "DIERR_HASEFFECTS";
        case 0x80040205: return "DIERR_NOTEXCLUSIVEACQUIRED";
        case 0x80040206: return "DIERR_INCOMPLETEEFFECT";
        case 0x80040207: return "DIERR_NOTBUFFERED";
        case 0x80040208: return "DIERR_EFFECTPLAYING";
        }
    }
    // 0x0048FB50: second switch over the remaining literals; default 0x0048FBEA "Unknown error".
    switch (hr)
    {
    case 0x8004020A: return "DIERR_REPORTFULL";
    case 0x80070002: return "DIERR_NOTFOUND";
    case 0x80070005: return "DIERR_OTHERAPPHASPRIO";
    case 0x8007000C: return "DIERR_NOTACQUIRED";
    case 0x8007000E: return "DIERR_OUTOFMEMORY";
    case 0x80070015: return "DIERR_NOTINITIALIZED";
    case 0x8007001E: return "DIERR_INPUTLOST";
    case 0x80070057: return "DIERR_INVALIDPARAM";
    case 0x80070077: return "DIERR_BADDRIVERVER";
    case 0x800700AA: return "DIERR_ACQUIRED";
    case 0x8007047E: return "DIERR_OLDDIRECTINPUTVERSION";
    case 0x80070481: return "DIERR_BETADIRECTINPUTVERSION";
    case 0x800704DF: return "DIERR_ALREADYINITIALIZED";
    }
    return "Unknown error";
}

} // extern "C"
