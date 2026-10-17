# Retail \`GunPickup_Action\` Distance Gate

## Retail binary evidence

Target: \`/Users/haseeb-mir/Downloads/IGI/igi.exe\`

SHA-256: \`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01\`

The retail function at \`0x0046D230\` is 424 bytes and has 18 basic blocks in
both REA/Ghidra 12.1.2 (\`ev_f437d0dd853eed68d11e64601f2b62a0770665611b1529b0f5d354fe2e9ee22d\`)
and radare2 (\`fcn.0046d230\`, 424 bytes, 18 basic blocks, 120 instructions).
The independent providers agree on its observed extent and CFG.

REA pseudocode and assembly establish this pickup gate:

1. Read the three axis values at pickup offsets \`+0x98\`, \`+0x9C\`, and \`+0xA0\`.
2. Multiply their maximum by the value returned from \`0x004D0950(pickup + 0x6C)\`.
3. Clamp that result upward to \`2867.2\` game units.
4. Compute the Euclidean distance between the pickup position and human position.
5. Collect only when that distance is strictly below the computed limit and the
   absolute vertical difference is strictly below \`4096.0\` units.

The helper at \`0x004D0950\` is 31 bytes/four blocks in both REA
(\`ev_9ca1c6ac8982866ca180a9496a32d9c5e4ab6d8dd8d03a83cc49b119762bd329\`)
and radare2. Its machine code returns \`1.0\` when the passed pointer or its first
pointer is null; otherwise it reads a float through offsets \`+0x0C\`, \`+0x0C\`,
and \`+0x78\`.

\`GunPickup_LoadParameters\` at \`0x0046CD20\` initializes the three axis values to
\`1.0\`; REA observed this in a 228-byte body (\`ev_e5279dd7b727753c7868fd17f0eddb1e5282b95425bdfff2448fef4fcd0fa474\`).
The C++ simulation stores those initial values and expresses positions in
metres, converting position deltas by the retail \`4096 units/metre\` scale.

## C++ port and verification

The distance gate is implemented as
\`igi::retail_natives::GunPickup::GunPickup_Action\` and is used for automatic
weapon pickup. The interaction-key/3-metre fallback was removed. Tests cover
the strict range boundary, full 3D distance, vertical limit, scaled radius, and
non-finite input handling.

Validated on this change:

- \`gun_pickup_action_test\` — passed.
- \`mvp_navigation_inventory_probe\` — passed.
- CMake compiled both targets with the project warning policy enabled.

## Remaining work before claiming a full native port

The simulation currently supplies a default parent scale of \`1.0\`. The retail
helper's non-null pointer chain has not yet been mapped to the C++ task/model
transform, so scaled pickup instances are not proven equivalent. The retail
task callback-table dispatch, picked-state field, and localized
\`PICKEDUPWEAPON%s\` status-message path also remain to be ported and verified.
This evidence therefore verifies the retail distance predicate and its C++
regression, not the complete \`GunPickup_Action\` native or retail runtime parity.
