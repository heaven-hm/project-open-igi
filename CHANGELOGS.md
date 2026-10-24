# Changelog

Versions record real, evidence-backed milestones. A version exists only when the
candidate does something the previous one could not, and every entry lists how it
was achieved and how many natives the manifest holds at that release.

## v0.0.1 — 2026-10-24

**Achieved**
- Retail entry point transcribed and linked: `WinMain` at retail `0x0048F530`, the only entry in the tree.
- Candidate startup prefix reproduces retail: mutex/duplicate path, `CoInitialize`, `GetForegroundWindow`, window class `"Innerloop"` (WNDCLASSA style 3, proc `0x48F970`, menu==class `"Innerloop"`), the 16 startup setters in retail order, four parser registrations, `CreateWindowExA` style `0x37C80000`, 640x480 at X=520/Y=285, `ShowCursor(0)`, `ShowWindow(10)`, `StoreWindowHandle` → `[0x5C8BC4]`.
- B3 transcribed: WinProc handler-chain container init (`0x48F674-0x48F6A6`), plus the 12-byte `{next,+prev_link,+handler}` entry layout the retail `HeapList`/`WindowProcedure` use.
- Candidate stops at the documented registrar boundary (`QTask_RegisterClasses 0x402930`, AI-graph registrar not yet transcribed) — no gameplay yet, no masking.

**How**
- Matching decompilation of retail `igi.exe` (SHA-256 `dea34892…`) with Binary Ninja (single decompiler), Radare2 CFG and angr exact-entry recovery; provenance blocks on every native.
- Paired Frida prefix probes: retail `PREFIX_RETAIL_MATCH 178/179`; candidate comparison found the one real divergence (handler chain, all zero) which B3 now implements.
- Chain instrumentation limits declared, not hidden: `0x402930` 152/156 rows, `0x5183D0` 66/74, verdict `RETAIL_CHAIN_PARTIAL` (never a pass).

**Natives at this release** (manifest)
- 998 records: 132 VERIFIED, 18 VERIFIED_WITH_DOCUMENTED_SCOPE, 12 RUNTIME_TESTED, 815 STRUCTURALLY_VERIFIED, 26 TRANSCRIBED, rest ANALYZED/INCONCLUSIVE.
- EU catalog (BlankNameES, human-verified): 55; HeavenHM catalog (human-verified): 78 unique addresses — attributed in the provenance blocks.

**Candidate binary**: `igi1.exe` SHA-256 `30c39039b1948475def85283960d9775686b7204d1dd1ec6cb17f6e140614d9b` (built by `build_no_verify.sh`, deployed to `/Users/haseeb-mir/Downloads/IGI`).
**Screenshot**: `screenshots/v0.0.1-candidate-startup.png` (candidate startup window under Wine from the igi1 bottle).
**Limits**: registrar chain untranscribed, so the process still terminates at `QTask_CreateTask`/`QTask_RegisterClasses`; DX gate `0x498480` and `0x498460` self-check path open; no gameplay claimed.
