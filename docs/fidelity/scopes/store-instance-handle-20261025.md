# StoreInstanceHandle — declared single-native verification scope

Retail `0x0048F0D0`, called by WinMain at `0x0048F5FA` immediately after
StoreCommandLine. Retail SHA-256:
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

The body is `8b442404 a3c08b5c00 c3`: load the cdecl argument word, store it to
`0x005C8BC0`, return the identical EAX word. There are no callees, branches,
argument dereferences, arithmetic or floating-point operations. The candidate
is `_StoreInstanceHandle`, storing into its mapped `_InstanceHandle` symbol.
Its compiled ten-byte instruction sequence must match after mapping the store
operand to that symbol. Original private identifiers and typedefs are not
recovered; the existing name describes the effect and supported caller role.

Acceptance requires the strict focused gate with BN, r2 and exact-entry angr,
a source-bound semantic review, a fresh Windows x86 image/receipt, and sequential
retail/candidate Frida captures. Both games use the same probe within each
comparison, original game-data working directory and `window level1` arguments.

Controlled replay covers the existing eleven opaque 32-bit input words,
including zero, sign-bit boundaries and all-one bits. Every ordered input,
return, destination before/after value and restoration is checked. These values
are not treated as valid handles or pointers and are never dereferenced. A
separate natural capture must observe the actual WinMain call, establish its
argument equals that process's module base, compare within-process store/return
aliases, and check the sampled adjacent bytes are unchanged. Pointer addresses
are not compared across processes. The body does not read command-line strings,
QVM/configuration, assets, or any pointed-to handle data.

The controlled probe is `scripts/frida_app_startup_store_5c8bc0_probe.js`;
the natural probe is `scripts/tester/frida_store_instance_handle_natural.js`.
The natural capture does not test the later window's HINSTANCE or window
creation, because neither is an effect of this three-instruction native. Older
window observations remain historical evidence with their original limits.

The proposed scope document does not grant acceptance. The focused gate and
complete typed differential checker must pass before promotion. Acceptance is
limited to this source/image and declared observations, not universal proof,
full WinMain/startup parity, rendering, movement or playable Level 1. Compiler
CRT startup and Windows SDK code are outside game-native transcription scope.
The later untranscribed startup sequence at 0x00402930 remains a separate blocker.
