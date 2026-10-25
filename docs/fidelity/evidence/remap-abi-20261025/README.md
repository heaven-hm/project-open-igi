# GOInRemap ABI observations — 0x004037C0

Retail-only Level 1 capture; no candidate acceptance. The retail executable and
installed configuration hashes were unchanged before/after capture. See
`observation.json` for identities, probe/action hashes and artifact hashes.
`retail.jsonl` contains raw messages; `retail.json` records launch and cleanup.

The selected Binary Ninja, Radare2 and angr evidence is retained in
`../current-config-20261025/remap-static.json` (CFG 1.0, exact entry). This probe
adds runtime evidence for the interface already established by those instructions:

- 0x004037CA reads the third entry stack argument, forwarded as argument 1 of
  0x004B8A20. All 81 calls made three ordered reader calls with indices 0, 1, 2;
  every reader frame pointer equaled that third argument.
- Each table write matched the corresponding reader result, with the third word
  masked by 0x3FFFFFFF. For example, the first control value was 1073741854 before
  masking and 30 afterwards. The per-profile counter increased exactly once.
- 0x0040382E loads the first entry stack argument as output destination. All 81
  returns aliased that pointer. Output words 0, 2 and 3 were zero, and word 4 was
  the relocated retail address 0x00567C74.
- Words 1 and 5 are copied from local stack slots that this function does not
  initialize. In this run they were 65841756 and 31132186 in all 81 outputs.
  These are raw observations, **not constants or cross-process comparison keys**.
  A faithful treatment needs the caller/stack and return-value layout recovered;
  forcing zero is not justified by the retail instructions.

The probe only reads the reviewed 24-byte output region and remap table words;
it compares frame pointer identity without dereferencing the unknown frame.
It does not mutate game state. Both the owned retail process and private Frida
server were terminated, with the server listener confirmed closed.

## Candidate correction requirements

The source hash is recorded in `observation.json`. At that revision,
`engine/src/retail/config/input_remap.cpp` takes one argument, directly indexes a
fabricated frame view, returns a private static buffer and zeroes the two copied
stack words. Its `VERIFIED` comment is unsupported for this implementation.
The header also incorrectly describes the return as the masked third argument.

Before accepting a replacement, recover the caller's output/value layout and the
actual script dispatcher ABI; retain the unused second argument slot; use the
retail argument-reader native and preserve its call ordering; preserve output
ownership and the real global layout; then compile and compare the linked
candidate with a reviewed typed probe. The current custom config decoder's
one-argument call must not be used to justify retaining the wrong native ABI.
No engine changes were made in this research packet while the existing engine
writer was active. This packet is not an alternate implementation.

Active-profile selection, effective movement/input dispatch, error paths and
candidate behavior remain unobserved. These checks cover the installed config's
81 calls only and do not verify the complete native or gameplay.
