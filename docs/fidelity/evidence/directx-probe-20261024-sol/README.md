# Required startup capability probe — ANALYZED, no candidate acceptance

Retail native: 0x00498480, called only at WinMain 0x0048F6E2. It precedes window
creation. This packet advances the ordered startup investigation after the
mutex-prefix correction, rather than skipping the missing capability check.

`capture.json` retains the selected Binary Ninja analysis, Radare2 graph and angr
recovery, bound to retail SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
BN/r2 each recover 38 blocks with graph similarity 1.0; angr recovers the exact
entry without unresolved jumps. The extent is 0x00498480..0x004988CA (1098 bytes).
No Ghidra analysis was used. This is structural evidence, not a candidate score.

## Complete retail observation

Primary runtime evidence: `retail-com-state.json[l]`, captured with the exact
archived `probe-com-state.js` (its SHA-256 matches the report). Retail was spawned
suspended with game cwd and `window level1`, under the shared runtime lease.
The bounded 15-second run observed one entry, one return, 46 ordered import/COM
events (48 observations including native entry/return), seven process-local interface identities and 16 distinct hooked COM
method addresses. There were no script failures or pending native calls at the
final summary; the owned process exited during cleanup.

Observed capability states at typed call boundaries:
`0x100 -> 0x200 -> 0x300 -> 0x500 -> 0x600 -> 0x601 -> 0x700`.
Final output DWORDs were capability 0x700 and platform 2; EAX was 1. Fields not
yet observed as initialized are recorded as null/unknown, never normalized to
an invented initial value. Interface identities are local ordinals; raw pointers
must not be compared across processes.

The actual sequence includes DirectDrawCreate, QueryInterface for IDirectDraw2,
Release, SetCooperativeLevel(NULL, 8), CreateSurface, queries for
IDirectDrawSurface3/4, surface/DirectDraw releases, CoInitialize(NULL),
CoCreateInstance(CLSID_DirectMusic, NULL, 1, IID_IDirectMusic), the music release,
CoUninitialize, DirectDrawCreateEx for IDirectDraw7 and its release. Import events
retain DLL/procedure strings, HRESULT/BOOL results and typed GUID/descriptor bytes.
The surface descriptor is 108 bytes, zero-filled except size 108, flags 1 and
caps 0x200, matching the retail stores at 0x498719..0x49872F.

GetVersionExA reported platform 2, major 6, minor 2, build 9200 in this CrossOver
environment. These are API observations, not a statement about the host OS.

## Evidence that must guide the transcription

DirectMusic is part of this required pre-window native. Deferring gameplay audio
does not authorize deleting the COM call. The source contains unusual cleanup
orderings: preserve them. The successful retail trace does not release interface
identities 4 and 5 returned by the surface queries; do not add cleanup for style.

The existing camera/display catalog aliases and BOOL prototype are hypotheses.
The body returns different raw EAX sources on different branches, including a
pointer and calls to the void OutputDebugStringA API. Its only caller discards
EAX and reads the two output DWORDs. Original source names and full return-type
semantics are not established; use an address placeholder until verified.

The GUID bytes in `guid-observations.json` match the local MinGW SDK declarations
in ddraw.h and dmusicc.h. This resolves the interface IDs; SDK spelling does not
prove an original game function/variable name.

## Earlier attempts and remaining work

The three-second imports-only run ended inside CoCreateInstance and is incomplete.
The subsequent imports-only 15-second run returned. Those reports are historical
observations; that early probe version was superseded. `retail-com.json[l]` plus
`probe-com.js` retain the next complete import/COM capture. The primary state
capture adds the required ordered output-state observations.

Candidate implementation/linkage and paired capture are still absent; no native
was added to engine/ by this packet. Error paths, NT4/older-OS behavior, partial
library/interface failure, independent semantic critique and an authorized paired
runtime contract remain required. This packet does not mark the native VERIFIED
or make Level 1 playable.
