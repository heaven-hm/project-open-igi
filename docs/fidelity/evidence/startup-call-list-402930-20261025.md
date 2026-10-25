# The startup call list at `0x00402930`: 155 calls, 139 still missing

Measured 2026-10-25 from retail `igi.exe` (SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`), image base
`0x00400000`. `0x00402930` is 780 bytes of bare `call rel32` followed by `jmp 0x004CEF30` - 155 calls and no other control flow.
The tree currently implements it as a **fail-closed stub** in `core/class_registration.cpp`, which is why no candidate window
appears: startup aborts there before any of these run.

Method: `r2 -q -c "pd 156 @ 0x402930"` on the image, extracting every `call` target; then matching each target against the
provenance header (`// Native: ...; retail VA 0x...`) of every `engine/src/retail/**/*.cpp`. Match by **content**, never by
filename.

## Result

- **155 call sites, 154 distinct targets** (one target is called twice), **15 present, 139 missing.**
- The 15 present include `0x00407D60`, `0x0040FE30`, `0x00413DF0`, `0x00402FF0`, `0x00406450`, `0x00407290` and the other
  already-transcribed positions.

## The gap by address band

| Band | Missing |
| --- | --- |
| `0x400000`-`0x40FFFF` | 7 |
| `0x410000`-`0x41FFFF` | 13 |
| `0x420000`-`0x42FFFF` | 24 |
| `0x430000`-`0x43FFFF` | 24 |
| `0x440000`-`0x44FFFF` | 16 |
| `0x450000`-`0x45FFFF` | 8 |
| `0x460000`-`0x46FFFF` | 23 |
| `0x470000`-`0x47FFFF` | 13 |
| `0x480000`-`0x48FFFF` | 9 |
| `0x4C0000`-`0x4CFFFF` | 1 |
| `0x500000`-`0x50FFFF` | 1 |

Split for the two worker lanes, by the write scopes already in force:

- **`0x00400000`-`0x0045FFFF`: 92 functions** (`handler-abi-worker`), plus `0x004Cxxxx` (1) and `0x0050xxxx` (1).
- **`0x00460000`-`0x0048FFFF`: 45 functions** (`chain-worker`).

## Why this is the critical path and not a worklist of equal items

Every one of the 155 calls executes during startup, in this order, before the game reaches the level load. A missing callee is
therefore not "one more native" - it is the reason the candidate has no window at all. Nothing downstream of `0x00402930` can be
observed at runtime until all 139 exist, which is also why the honest ceiling on every native transcribed so far remains
`BUILD_VERIFIED` and not `RUNTIME_TESTED`.

## The ordered call list

| # | target | state |
| --- | --- | --- |
| 1 | `0x401fc0` | MISSING |
| 2 | `0x402f50` | MISSING |
| 3 | `0x402ff0` | MISSING |
| 4 | `0x406450` | MISSING |
| 5 | `0x407290` | MISSING |
| 6 | `0x407d60` | MISSING |
| 7 | `0x40fe30` | MISSING |
| 8 | `0x413df0` | MISSING |
| 9 | `0x415070` | present |
| 10 | `0x415530` | present |
| 11 | `0x4170a0` | MISSING |
| 12 | `0x4190e0` | MISSING |
| 13 | `0x419760` | present |
| 14 | `0x4199f0` | MISSING |
| 15 | `0x41a1e0` | MISSING |
| 16 | `0x41aaf0` | MISSING |
| 17 | `0x41b310` | MISSING |
| 18 | `0x41b8e0` | MISSING |
| 19 | `0x41c700` | MISSING |
| 20 | `0x41d080` | MISSING |
| 21 | `0x41dc50` | MISSING |
| 22 | `0x41e690` | MISSING |
| 23 | `0x41f340` | MISSING |
| 24 | `0x420300` | MISSING |
| 25 | `0x420cc0` | MISSING |
| 26 | `0x421430` | MISSING |
| 27 | `0x421750` | MISSING |
| 28 | `0x4223d0` | MISSING |
| 29 | `0x422b40` | MISSING |
| 30 | `0x4230a0` | MISSING |
| 31 | `0x423fc0` | MISSING |
| 32 | `0x424b90` | MISSING |
| 33 | `0x424e80` | MISSING |
| 34 | `0x425c80` | MISSING |
| 35 | `0x426630` | MISSING |
| 36 | `0x426d90` | MISSING |
| 37 | `0x4272a0` | MISSING |
| 38 | `0x427830` | MISSING |
| 39 | `0x427d90` | MISSING |
| 40 | `0x427ff0` | MISSING |
| 41 | `0x42acc0` | MISSING |
| 42 | `0x42beb0` | MISSING |
| 43 | `0x42d2b0` | MISSING |
| 44 | `0x42dac0` | MISSING |
| 45 | `0x42dcf0` | MISSING |
| 46 | `0x42fe40` | MISSING |
| 47 | `0x42ff00` | MISSING |
| 48 | `0x4310f0` | MISSING |
| 49 | `0x4330f0` | MISSING |
| 50 | `0x4351e0` | MISSING |
| 51 | `0x435870` | MISSING |
| 52 | `0x435d90` | MISSING |
| 53 | `0x4361d0` | MISSING |
| 54 | `0x436780` | MISSING |
| 55 | `0x436b10` | MISSING |
| 56 | `0x437510` | MISSING |
| 57 | `0x438390` | MISSING |
| 58 | `0x4388b0` | MISSING |
| 59 | `0x438df0` | MISSING |
| 60 | `0x4397c0` | MISSING |
| 61 | `0x43a840` | MISSING |
| 62 | `0x43ad80` | MISSING |
| 63 | `0x43b2f0` | MISSING |
| 64 | `0x43b850` | MISSING |
| 65 | `0x43bfd0` | MISSING |
| 66 | `0x43c6d0` | MISSING |
| 67 | `0x43cc70` | MISSING |
| 68 | `0x43e4f0` | MISSING |
| 69 | `0x43e8d0` | MISSING |
| 70 | `0x43eff0` | MISSING |
| 71 | `0x43f900` | MISSING |
| 72 | `0x4403e0` | MISSING |
| 73 | `0x440710` | MISSING |
| 74 | `0x440c20` | MISSING |
| 75 | `0x440fd0` | MISSING |
| 76 | `0x443680` | MISSING |
| 77 | `0x443ed0` | MISSING |
| 78 | `0x444bf0` | MISSING |
| 79 | `0x4464c0` | MISSING |
| 80 | `0x447db0` | MISSING |
| 81 | `0x4485e0` | MISSING |
| 82 | `0x448d30` | MISSING |
| 83 | `0x449360` | MISSING |
| 84 | `0x449970` | MISSING |
| 85 | `0x449d30` | MISSING |
| 86 | `0x44c130` | MISSING |
| 87 | `0x44c8d0` | MISSING |
| 88 | `0x4533b0` | MISSING |
| 89 | `0x453550` | MISSING |
| 90 | `0x454970` | MISSING |
| 91 | `0x456ad0` | MISSING |
| 92 | `0x459880` | MISSING |
| 93 | `0x45cab0` | MISSING |
| 94 | `0x45db70` | MISSING |
| 95 | `0x45e450` | MISSING |
| 96 | `0x464260` | MISSING |
| 97 | `0x465a10` | MISSING |
| 98 | `0x465bc0` | MISSING |
| 99 | `0x4663d0` | MISSING |
| 100 | `0x466e40` | MISSING |
| 101 | `0x466f70` | MISSING |
| 102 | `0x467700` | MISSING |
| 103 | `0x4682f0` | MISSING |
| 104 | `0x4688b0` | MISSING |
| 105 | `0x468e20` | MISSING |
| 106 | `0x4696b0` | MISSING |
| 107 | `0x46ab60` | MISSING |
| 108 | `0x46b510` | MISSING |
| 109 | `0x46b7f0` | MISSING |
| 110 | `0x46ba60` | MISSING |
| 111 | `0x46c0c0` | MISSING |
| 112 | `0x46cb70` | MISSING |
| 113 | `0x46d420` | MISSING |
| 114 | `0x46db40` | MISSING |
| 115 | `0x46dd00` | MISSING |
| 116 | `0x46df10` | MISSING |
| 117 | `0x46e370` | MISSING |
| 118 | `0x46f2b0` | MISSING |
| 119 | `0x470410` | present |
| 120 | `0x470610` | present |
| 121 | `0x470fa0` | MISSING |
| 122 | `0x472670` | present |
| 123 | `0x472a70` | MISSING |
| 124 | `0x472d00` | MISSING |
| 125 | `0x4733f0` | MISSING |
| 126 | `0x4739d0` | MISSING |
| 127 | `0x473ed0` | MISSING |
| 128 | `0x475380` | present |
| 129 | `0x475ac0` | MISSING |
| 130 | `0x476110` | present |
| 131 | `0x476730` | present |
| 132 | `0x477220` | present |
| 133 | `0x477e30` | MISSING |
| 134 | `0x47b4b0` | MISSING |
| 135 | `0x47cab0` | MISSING |
| 136 | `0x47cf20` | MISSING |
| 137 | `0x47cf70` | MISSING |
| 138 | `0x47e700` | MISSING |
| 139 | `0x4801b0` | MISSING |
| 140 | `0x4813e0` | MISSING |
| 141 | `0x4825c0` | MISSING |
| 142 | `0x484f30` | MISSING |
| 143 | `0x4851b0` | MISSING |
| 144 | `0x485330` | present |
| 145 | `0x485a40` | MISSING |
| 146 | `0x486900` | present |
| 147 | `0x4872c0` | MISSING |
| 148 | `0x487be0` | MISSING |
| 149 | `0x488620` | present |
| 150 | `0x488800` | present |
| 151 | `0x488bb0` | MISSING |
| 152 | `0x489560` | present |
| 153 | `0x4c1780` | MISSING |
| 154 | `0x4c1780` | MISSING |
| 155 | `0x508f80` | MISSING |
