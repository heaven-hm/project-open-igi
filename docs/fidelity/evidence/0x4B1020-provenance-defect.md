# 0x004B1020 provenance defect in the landed `alias_resolve.cpp` (recorded 2026-10-25)

Found while landing 0x004B12F0, the routine that populates the alias tables. Recorded here because AGENTS.md §10.13(2)/§10.14(9)
require a change that adds logic absent from `igi.exe` to be removed rather than kept for convenience, and because the fix
needs a faithful body before the old file can be deleted in the same change.

## The defect

`engine/src/retail/qfile/alias_resolve.cpp` is labelled `// 0x004B1020`, but it resolves aliases against port-owned storage:

- an anonymous-namespace `DeviceEntry device_table[device_entry_count]` and `char resolved_alias[0x100]`;
- helpers `QFile_GetDeviceTable()` and `QFile_GetResolvedAlias()` that expose that storage;
- a match against `entry.name` / `entry.alias` fields with `std::strncmp` and a `std::strchr`-based `;` split.

Retail 0x004B1020 does none of that. From the bytes at 0x004B1020-0x004B109F (and the continuation at 0x004B10BB):

- it walks the 32 records at 0x00942330 in 0x94-byte strides (`mov esi,0x00942330`, `add esi,0x94`, `cmp esi,0x009435B0`),
  testing the handle word at record+0x90;
- it compares with a call at 0x004B1053 (0x004AAD10) rather than an inlined `strncmp`;
- on a match it copies the record's target string (record+0x10, the table at 0x00942340) into the caller's destination with
  `rep movsd`/`rep movsb` (0x004B1072-0x004B1093) and then copies the caller's alias into the fixed buffer at 0x00943A18
  (0x004B10A4-0x004B10B3);
- on the no-match path it indexes 0x00942340 by the record index and walks it with the constant 0x3B (';') through
  0x004AA0A0, decrementing a depth counter.

So the landed file's storage, its helpers and its matching/`;`-splitting logic are port inventions, not a transcription. It is
also the reader that 0x004B12F0's writes should reach, so the alias registration path is currently disconnected from a faithful
reader.

## Consequence and plan

- The file is **not** a transcription of 0x004B1020 and must not be counted as one; its manifest row and status need the same
  re-derivation treatment as any other native.
- Fix order (one change, per §10.15(16)): derive the full 0x004B1020 body against the retail tables, land it, then delete the
  port storage and the two helpers, and update the call sites in `native_4b1420.cpp` plus the manifest in the same commit.
- Until that lands, this is an open fidelity defect, not a documented divergence: it changes behaviour rather than only codegen.
