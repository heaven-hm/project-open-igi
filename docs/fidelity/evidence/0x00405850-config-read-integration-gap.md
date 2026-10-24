# Config_Read integration gap — 2026-10-21

Retail image SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Selected decompiler: Binary Ninja MCP, active PE/x86 view_1. The 74 bytes returned
by Binary Ninja match the Radare2 listing byte for byte. Both recover three
blocks and two branch edges at `0x00405850`. angr independently recovered the
exact entry; its retained report is `0x00405850-config-read-angr.json`.

The GameNatives source preserves the observed branch/call ordering:

- Call `Qvm_Load` (`0x004B80B0`) with the supplied path.
- On successful load, call `Qvm_Execute` (`0x004B85B0`), then `Qvm_Cleanup`
  (`0x004B83D0`) with the same QVM object.
- On failed load, call `Config_SaveGraphicOptions` (`0x00405300`).
- Both paths then call `Config_SaveSoundOptions` (`0x004058A0`),
  `Config_NoD3DDriversWithHard` (`0x00405980`), and return
  `Config_Language_2` (`0x00405B30`).

The misleading analyst names must not define their behavior: Binary Ninja's
`Config_SaveSoundOptions` body reconciles binding rows with the default table,
whereas `Config_Language_2` registers the language namespace and calls music/SFX
volume consumers. Omitting those calls would change Config_Read's behavior.

There is no engine Config_Read entry point and no engine Qvm_Execute entry point.
The current `retail_config.cpp` parses a subset of constant QVM calls; it is not
this native interpreter chain. Existing Qvm_Load/Qvm_Cleanup also retain
unresolved boundaries, as documented in their source. Engine integration needs
the actual dependency closure, including script execution and configuration
state; routing this native to the subset parser would not resolve the gap.

A binary-bound `ANALYZED` manifest record now binds the reference candidate.
It explicitly records that it is not an engine implementation. The exact user
command was rerun: it no longer reports a missing record, and correctly blocks
on provisional status, missing semantic acceptance and missing differential
runner. No runtime behavior, engine linkage or full configuration parity is
claimed. Paired Frida captures must compare the same configuration/script and
its typed state changes before acceptance.
