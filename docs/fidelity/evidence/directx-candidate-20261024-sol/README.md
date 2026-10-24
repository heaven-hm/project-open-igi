# 0x00498480 candidate: diagnostic successful-path match, acceptance blocked

This follows the missing WinMain call at retail 0x0048F6E2. It replaces the
non-compiling `Display_DinputDll` catalog copies with one provisional x86 native,
`Native_00498480`. Original source symbols/local names cannot be recovered from
these stripped instructions. Capability/platform describe the observed DWORD
roles; SDK COM interface names are established by exact GUID bytes. No guessed
camera name or uniformly BOOL return type is retained.

Static packet: `../directx-probe-20261024-sol/`. Binary Ninja is the only selected
decompiler; Radare2 independently recovers the same 38 retail blocks (score 1.0),
and angr recovers the exact entry without unresolved jumps. This score compares
two retail graphs, not candidate behavior. No Ghidra analysis was used.

## Build and actual game-process comparison

`diagnostic-receipt.json` binds the clean x86 build to the compiler, configured
target, source/header inputs, actual objects and SDK dependencies. The receipt
was validated before and after capture. Candidate SHA-256:
`9e38a3a69437ca8e2ec741001ecb6c7e5e74932144f215ad9e2edbeff64a4079`.
Candidate symbol VA 0x00466EB0; bounded extent 0x700 to the next text symbol.
Retail SHA-256 is `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

The private CMake overlay removes the production main from this diagnostic
target and compiles `diagnostic-main.cpp` instead. Its only flow addition is the
actual capability call at the recovered startup position. It deliberately has
no invented replacement for the unresolved parent <0x700 error branch, whose
retail sprintf/exit callees still need transcription. This copy and build are
diagnostic infrastructure under build/, not a second production entry point.
Root CMake and engine sources are unchanged by this increment.

`retail.json[l]` then `candidate.json[l]` record suspended spawn, hooks installed
before resume, cwd Downloads/IGI, `window level1`, the same probe SHA-256 and
15-second observation windows. The shared lease prevents overlapping captures;
the first owned process was confirmed gone before launching the second.
Neither an installed old candidate nor another agent's process was used.

`comparison.json` passes eight diagnostic checks. Both sides have exactly one
native entry/return and 46 ordered import/COM observations. Every typed value
matches: version data, DLL/procedure names and DLL handle roles, GUIDs, complete
108-byte descriptor, COM object identity relationships, HRESULT/reference-count
returns, initialized output states and final EAX. Capability advances through
0x100, 0x200, 0x300, 0x500, 0x600, 0x601, 0x700; platform is 2; EAX is 1.
Owned DDRAW/DINPUT module reference counts both finish at zero. Code locations
are excluded from cross-image equality; no typed observation is dropped.

Candidate startup terminates after this native. Its later RPC summary therefore
fails with `script has been destroyed`; this is retained, not hidden. The probe
emits a native-complete terminal marker immediately after the observed return,
binding its full 48-element sequence and identity/module counts. That marker
and complete raw stream justify this native-only observation comparison. They
do not validate later WinMain, and do not change the fail-closed full-startup
comparator. Both owned processes were confirmed gone.

## Source review and explicit gaps

The candidate preserves the odd FreeLibrary(NULL) branch; raw pointer EAX on the
pre-NT4 path; raw register after void debug APIs; exact debug strings; NT4's
GetProcAddress -> FreeLibrary -> pointer test order; unloading both DLLs before
Release on the missing-input-export branch; no added Surface3/4 releases; and
no CoUninitialize on failed music creation. Descriptor zeroing remains 27 DWORD
stores rather than an added helper call. Main discards EAX and checks the output.

This is the author's source review, not an independent critic's approval.
Failure injection/other OS paths, output aliasing and machine-register behavior
on those paths have no paired runtime proof. The probe observes output state at
call boundaries, not every intermediate write. No authorized runtime_contract,
strict behavioral acceptance or production intake is claimed. The manifest
stays BUILD_VERIFIED and the native's name stays provisional.

The required `--changed --threshold VERIFIED --cfg-threshold 0.85
--require-semantic-evidence --require-behavioral-tests` gate blocks the staged
native. Its raw report/log are retained here. The earlier pre-stage report saw
zero natives because the new catalog source was ignored: its exit zero is an
empty-scope diagnostic, explicitly not acceptance. Staging the exact source
made the gate check 0x00498480 and block. Infrastructure self-check: 12/12,
`native_verified=False`. No native has been promoted to make a build pass.

Reproduce using the root scripts and the retained overlay/startup copy:

```sh
env PYTHONDONTWRITEBYTECODE=1 '/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python' scripts/tester/capture_directx_pair.py \
  --candidate build-directx-candidate-sol/igi1.exe \
  --receipt build-directx-candidate-sol/diagnostic-receipt.json \
  --out build/runtime-traces/directx-new-unique-run
```

Rebuild with `scripts/candidate_build_receipt.py` if any bound source changes.
Never substitute this diagnostic capture for the reviewed intake workflow.
