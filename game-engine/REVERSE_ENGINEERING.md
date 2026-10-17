# Retail Reverse-Engineering Evidence

This directory is a new engine implementation. It does not link to the legacy engine, the editor, the converter, or the C# project. Those projects and the native catalogs may be used to locate candidate behavior, but a routine enters this tree only with evidence from the retail executable.

## Binary under analysis

- Artifact: `/Users/haseeb-mir/Downloads/IGI/igi.exe`
- Format: PE32 x86
- SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`

## First verified slice: audio string routines

| Retail VA | Local symbol | Ghidra evidence | Radare2 cross-check | Observed behavior |
| --- | --- | --- | --- | --- |
| `0x00497390` | `FUN_00497390` | pseudocode `ev_3014beff32e4ee2bed84ac8cdbc8932fa0b358a1c35982e4d90781999805c2e2`; assembly `ev_328f1efa85f78fa027c70ac148cd6fb976d4e7ec0ea45911d2d95b8dca80243e` | 124 bytes, 14 basic blocks, 58 instructions, cdecl | Clears output; logs empty/dotless input through `FUN_004af810`; finds the first dot and scans backward for `/`, `\`, or `:`; copies the filename segment before that dot. If no separator is found, returns with output empty and does not log. |
| `0x00497410` | `FUN_00497410` | pseudocode `ev_3014beff32e4ee2bed84ac8cdbc8932fa0b358a1c35982e4d90781999805c2e2`; assembly `ev_43574c2bf4aa848470f93b52598b9a61346825bc4a4fa719eb1b266257c815e9` | 63 bytes, 6 basic blocks, 24 instructions, cdecl | Uppercases ASCII `a` through `z` in place and returns a 32-bit weighted sum. Weight starts at 1, advances by `0x13d`, and is masked to 16 bits per byte. |
| `0x00497450` | `FUN_00497450` | pseudocode `ev_3014beff32e4ee2bed84ac8cdbc8932fa0b358a1c35982e4d90781999805c2e2`; assembly `ev_07f85bcd1948d3a7ac236b967ecf33db7823bbc17fe0fe2664d2beca3617e218` | 76 bytes, 6 basic blocks, 30 instructions, cdecl | Uppercases ASCII `a` through `z` in place. The assembly additionally computes a modulo-101 rolling remainder using weights starting at 1 and advancing by `0x13e`; Ghidra's pseudocode declares the function `void` and omits that computation. The C++ slice records and tests the observed register result, while the return-value ABI remains unresolved. |

## Layouts, globals, and platform boundary

This slice requires no game object layouts or engine globals. `FUN_00497390` calls the retail logger at `0x004af810`; that logger remains an unresolved native dependency and is only stubbed by the focused test executable. The project has no third-party runtime dependencies. Windows x86 calling-convention annotations are retained, but a Windows x86 build and a CrossOver runtime comparison have not yet been performed.

The retail filename routine has no output-capacity argument. The C++ function preserves that pointer-based ABI shape, so callers must provide the same valid writable capacity the retail callers provide. The reverse-engineering record has identified caller output buffers of different sizes; boundary conditions and exact maximum lengths still need to be verified before broader use.

## Verification status

- Static: Ghidra and Radare2 independently agree on the three function sizes and basic-block counts. Ghidra assembly was inspected for the lowercase comparisons, hash recurrences, and return-register discrepancy.
- Unit: local tests cover basename extraction, all three separators, invalid-name logging, embedded NUL behavior, ASCII mutation, and a sample result for each observed hash computation.
- Runtime comparison with retail IGI: not yet performed. No full engine or parity claim is made.
