# WinMain's second game-native call: StoreInstanceHandle

Seen in retail: the call at `0x0048F5FA`, immediately after StoreCommandLine,
reaches `0x0048F0D0`. The complete ten-byte body is
`8b442404 a3c08b5c00 c3`: copy the cdecl word into `0x005C8BC0` and EAX.
BN is the selected decompiler. BN/r2 agree on one block and no edges (score 1.0);
angr independently recovers the exact entry without unresolved jumps. Their
retail-graph agreement alone is not a candidate semantic proof.

The existing candidate already implements that load/store/return. This increment
changes verification tooling and evidence, not engine code. The old runner's
source filename and symbol mapping were stale. It now resolves the actual
StoreInstanceHandle/InstanceHandle symbols from the freshly compiled x86 image.
The eleven existing word fixtures are unchanged. Controlled calls do not resume
main; a separate pre-resume hook observes the actual natural WinMain call.

`acceptance.json` records VERIFIED_WITH_DOCUMENTED_SCOPE on source SHA-256
`0cd758f2ad54db1e376f379e730f907a4c5dfb986d519302642cb948d41fdc08` and candidate
`e8caac38b98906eeacdf7fdf5818b51622ea3388107bae083c674cc31c7c6bd7`.
The fresh build, natural pair, controlled pair, strict focused gate, typed
comparison and runtime checker all passed. The gate ran another live paired
11/11 replay. A tampered recorded return was rejected by the comparator.
The tested executable is retained locally under build/accepted-native-images/.

Natural observations compare module-base instance relations, within-process
argument/store/return aliases, the initial null global and sampled neighboring
bytes. No pointed-to instance-handle data is dereferenced. Controlled observations
compare every ordered input word, return and global delta, and require state
restoration. Neither pointer addresses across processes nor only EAX/counts are
used as parity evidence. The native has no window calls: later window creation
or window HINSTANCE relations are outside this declared scope.

The previous manifest entry, including its older window observations and stale
hashes, is preserved unchanged as prior-record.json. It is not rebound to this
build. The first attempted fresh build was rejected because concurrent source
edits invalidated its compiler-input snapshot; that failure is retained too.

leanrea was installed at the owner's request and used for supplementary focused
PE/objdump inspection of a disposable retail copy. The exact skill digest,
commands and seen/inferred/unknown claims are in leanrea-use.json. This does not
replace BN/r2/angr, semantic review or paired Frida evidence. The runtime-unknown
claim in that earlier inspection was subsequently resolved for the scope above.

The conservative engine provenance comment was not edited; its lower status is
a gate warning. Independent human/agent critique was unavailable. Compiler CRT
startup and Windows SDK implementations remain outside game-native scope.
There is no universal proof, full WinMain/startup acceptance, deployment, or
playable Level 1 claim. The later game-code sequence at 0x00402930 remains
incomplete. files.json binds retained bytes; gzip logs preserve raw line endings.
