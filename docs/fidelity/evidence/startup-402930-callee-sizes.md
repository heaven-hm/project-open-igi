# `0x00402930` callee sizes in retail call order

Artifact: retail `igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Method: for each distinct callee of `0x00402930`, `radare2 6.2.0` `af`/`afij` at the exact VA over the retail image. Sizes are the r2 function extents,
which match the inventory's boundaries for the functions spot-checked, but a function whose tail r2 merges with a neighbour can read large - treat the
buckets as planning data, not as exact instruction counts.

## Summary

- **154 distinct callees**; sizes recovered for 154. Total 67560 bytes in r2 extents.
- <= 200 B: **36** call-order entries; 201-800 B: **101**; > 800 B: **17**.
- Callees with an engine implementation today: **8** (`0x00489560`, `0x00415530`, `0x004C1780`, `0x00401FC0`, `0x0042FE40`, `0x0042DAC0`, `0x00424B90`, `0x00419760`).

## The first wall is the AI graph, and it is not cheap

The sequence's first call is `0x00407D60`-style cheap later, but the call order starts at the AI graph registrar:

| order | callee | size | blocks | instrs |
| --- | --- | --- | --- | --- |
| 1 | `0x00508F80` | 787 | 2 | 198 |
| 2 | `0x00489560` | 104 | 1 | 23 |
| 3 | `0x00407D60` | 117 | 3 | 31 |
| 4 | `0x00488BB0` | 580 | 1 | 171 |
| 5 | `0x00402FF0` | 1639 | 15 | 449 |
| 6 | `0x00415530` | 670 | 1 | 168 |

First 6 callees together are about **3897 bytes**. `0x00508F80` is 787 B/198 instrs and its first callee `0x005183D0` reports 4985 B over
90 instructions - a large extent with few instructions, i.e. a jump-table or data-bearing block that r2 merged - so the AI graph is a subsystem wave, not
a one-pass transcription. The fail-closed guard stops at whichever registrar is untranscribed first, so nothing after `0x00508F80` becomes observable
until that cluster lands.

## Full table, in retail call order

| # | call site | callee | size B | blocks | instrs | status |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `0x00402930` | `0x00508F80` | 787 | 2 | 198 | absent |
| 2 | `0x00402935` | `0x00489560` | 104 | 1 | 23 | engine: engine/src/retail/core/native_489560.cpp |
| 3 | `0x0040293A` | `0x00407D60` | 117 | 3 | 31 | absent |
| 4 | `0x0040293F` | `0x00488BB0` | 580 | 1 | 171 | absent |
| 5 | `0x00402944` | `0x00402FF0` | 1639 | 15 | 449 | absent |
| 6 | `0x00402949` | `0x00415530` | 670 | 1 | 168 | engine: engine/src/retail/core/game_flow.cpp |
| 7 | `0x0040294E` | `0x004C1780` | 1 | 1 | 1 | engine: engine/src/retail/core/native_4c1780.cpp |
| 8 | `0x00402953` | `0x00401FC0` | 166 | 1 | 45 | engine: engine/src/retail/core/flow.cpp |
| 9 | `0x00402958` | `0x00488800` | 376 | 1 | 109 | absent |
| 10 | `0x0040295D` | `0x00488620` | 202 | 1 | 56 | absent |
| 11 | `0x00402962` | `0x00487BE0` | 421 | 1 | 115 | absent |
| 12 | `0x00402967` | `0x004872C0` | 476 | 1 | 132 | absent |
| 13 | `0x0040296C` | `0x00415070` | 110 | 1 | 30 | absent |
| 14 | `0x00402971` | `0x00486900` | 171 | 1 | 46 | absent |
| 15 | `0x00402976` | `0x00485A40` | 133 | 1 | 36 | absent |
| 16 | `0x0040297B` | `0x00485330` | 571 | 1 | 166 | absent |
| 17 | `0x00402980` | `0x004851B0` | 111 | 1 | 31 | absent |
| 18 | `0x00402985` | `0x00484F30` | 575 | 2 | 17 | absent |
| 19 | `0x0040298A` | `0x004825C0` | 131 | 3 | 34 | absent |
| 20 | `0x0040298F` | `0x004813E0` | 923 | 1 | 256 | absent |
| 21 | `0x00402994` | `0x004801B0` | 615 | 1 | 164 | absent |
| 22 | `0x00402999` | `0x0047E700` | 129 | 1 | 34 | absent |
| 23 | `0x0040299E` | `0x0047CF70` | 227 | 1 | 63 | absent |
| 24 | `0x004029A3` | `0x0047CF20` | 41 | 1 | 10 | absent |
| 25 | `0x004029A8` | `0x0047CAB0` | 607 | 2 | 14 | absent |
| 26 | `0x004029AD` | `0x00413DF0` | 408 | 1 | 99 | absent |
| 27 | `0x004029B2` | `0x00407290` | 2235 | 1 | 703 | absent |
| 28 | `0x004029B7` | `0x0047B4B0` | 172 | 1 | 45 | absent |
| 29 | `0x004029BC` | `0x00477E30` | 1110 | 1 | 295 | absent |
| 30 | `0x004029C1` | `0x00477220` | 991 | 1 | 267 | absent |
| 31 | `0x004029C6` | `0x00476730` | 366 | 1 | 95 | absent |
| 32 | `0x004029CB` | `0x00476110` | 150 | 1 | 39 | absent |
| 33 | `0x004029D0` | `0x00475AC0` | 333 | 1 | 91 | absent |
| 34 | `0x004029D5` | `0x00475380` | 222 | 1 | 59 | absent |
| 35 | `0x004029DA` | `0x00473ED0` | 695 | 1 | 194 | absent |
| 36 | `0x004029DF` | `0x004739D0` | 222 | 1 | 59 | absent |
| 37 | `0x004029E4` | `0x004733F0` | 222 | 1 | 59 | absent |
| 38 | `0x004029E9` | `0x00472D00` | 267 | 1 | 68 | absent |
| 39 | `0x004029EE` | `0x00472A70` | 104 | 1 | 27 | absent |
| 40 | `0x004029F3` | `0x00472670` | 376 | 1 | 99 | absent |
| 41 | `0x004029F8` | `0x00470FA0` | 372 | 1 | 101 | absent |
| 42 | `0x004029FD` | `0x00470610` | 330 | 1 | 87 | absent |
| 43 | `0x00402A02` | `0x00470410` | 229 | 1 | 61 | absent |
| 44 | `0x00402A07` | `0x0046F2B0` | 215 | 1 | 57 | absent |
| 45 | `0x00402A0C` | `0x0046E370` | 576 | 1 | 165 | absent |
| 46 | `0x00402A11` | `0x0046DF10` | 81 | 1 | 22 | absent |
| 47 | `0x00402A16` | `0x0046DD00` | 190 | 1 | 50 | absent |
| 48 | `0x00402A1B` | `0x0046DB40` | 224 | 1 | 67 | absent |
| 49 | `0x00402A20` | `0x0046D420` | 462 | 1 | 130 | absent |
| 50 | `0x00402A25` | `0x0046CB70` | 425 | 1 | 118 | absent |
| 51 | `0x00402A2A` | `0x0046C0C0` | 486 | 1 | 134 | absent |
| 52 | `0x00402A2F` | `0x0046BA60` | 167 | 1 | 45 | absent |
| 53 | `0x00402A34` | `0x0046B7F0` | 140 | 1 | 38 | absent |
| 54 | `0x00402A39` | `0x0046B510` | 140 | 1 | 38 | absent |
| 55 | `0x00402A3E` | `0x0046AB60` | 126 | 1 | 34 | absent |
| 56 | `0x00402A43` | `0x004696B0` | 141 | 1 | 37 | absent |
| 57 | `0x00402A48` | `0x00468E20` | 111 | 1 | 31 | absent |
| 58 | `0x00402A4D` | `0x004688B0` | 111 | 1 | 31 | absent |
| 59 | `0x00402A52` | `0x004682F0` | 498 | 1 | 147 | absent |
| 60 | `0x00402A57` | `0x00467700` | 255 | 1 | 67 | absent |
| 61 | `0x00402A5C` | `0x00466F70` | 1080 | 1 | 342 | absent |
| 62 | `0x00402A61` | `0x00466E40` | 203 | 1 | 61 | absent |
| 63 | `0x00402A66` | `0x004663D0` | 162 | 1 | 42 | absent |
| 64 | `0x00402A6B` | `0x00465BC0` | 263 | 1 | 72 | absent |
| 65 | `0x00402A70` | `0x00465A10` | 174 | 1 | 47 | absent |
| 66 | `0x00402A75` | `0x00464260` | 310 | 1 | 83 | absent |
| 67 | `0x00402A7A` | `0x0045E450` | 1666 | 5 | 447 | absent |
| 68 | `0x00402A7F` | `0x0040FE30` | 575 | 1 | 152 | absent |
| 69 | `0x00402A84` | `0x0045DB70` | 309 | 1 | 90 | absent |
| 70 | `0x00402A89` | `0x0045CAB0` | 402 | 1 | 109 | absent |
| 71 | `0x00402A8E` | `0x00459880` | 402 | 1 | 109 | absent |
| 72 | `0x00402A93` | `0x00456AD0` | 402 | 1 | 109 | absent |
| 73 | `0x00402A98` | `0x00454970` | 402 | 1 | 109 | absent |
| 74 | `0x00402A9D` | `0x00453550` | 215 | 4 | 54 | absent |
| 75 | `0x00402AA2` | `0x004533B0` | 183 | 1 | 56 | absent |
| 76 | `0x00402AA7` | `0x0044C8D0` | 972 | 15 | 235 | absent |
| 77 | `0x00402AAC` | `0x00406450` | 1796 | 1 | 527 | absent |
| 78 | `0x00402AB1` | `0x0044C130` | 392 | 1 | 118 | absent |
| 79 | `0x00402AB6` | `0x00449D30` | 275 | 3 | 69 | absent |
| 80 | `0x00402ABB` | `0x00449970` | 270 | 1 | 71 | absent |
| 81 | `0x00402AC0` | `0x00449360` | 361 | 1 | 102 | absent |
| 82 | `0x00402AC5` | `0x00448D30` | 243 | 1 | 64 | absent |
| 83 | `0x00402ACA` | `0x004485E0` | 382 | 1 | 106 | absent |
| 84 | `0x00402ACF` | `0x00447DB0` | 570 | 1 | 157 | absent |
| 85 | `0x00402AD4` | `0x004464C0` | 1072 | 3 | 297 | absent |
| 86 | `0x00402AD9` | `0x00444BF0` | 805 | 1 | 232 | absent |
| 87 | `0x00402ADE` | `0x00443ED0` | 639 | 1 | 175 | absent |
| 88 | `0x00402AE3` | `0x00443680` | 433 | 1 | 118 | absent |
| 89 | `0x00402AE8` | `0x00440FD0` | 1515 | 1 | 437 | absent |
| 90 | `0x00402AED` | `0x00440C20` | 262 | 1 | 71 | absent |
| 91 | `0x00402AF2` | `0x00440710` | 300 | 1 | 83 | absent |
| 92 | `0x00402AF7` | `0x004403E0` | 222 | 1 | 59 | absent |
| 93 | `0x00402AFC` | `0x0043F900` | 468 | 1 | 130 | absent |
| 94 | `0x00402B01` | `0x0043EFF0` | 499 | 1 | 139 | absent |
| 95 | `0x00402B06` | `0x0043E8D0` | 461 | 1 | 133 | absent |
| 96 | `0x00402B0B` | `0x0043E4F0` | 142 | 1 | 38 | absent |
| 97 | `0x00402B10` | `0x0043CC70` | 925 | 5 | 267 | absent |
| 98 | `0x00402B15` | `0x0043C6D0` | 497 | 1 | 147 | absent |
| 99 | `0x00402B1A` | `0x0043BFD0` | 215 | 1 | 57 | absent |
| 100 | `0x00402B1F` | `0x0043B850` | 714 | 1 | 196 | absent |
| 101 | `0x00402B24` | `0x0043B2F0` | 414 | 1 | 119 | absent |
| 102 | `0x00402B29` | `0x0043AD80` | 410 | 3 | 121 | absent |
| 103 | `0x00402B2E` | `0x0043A840` | 783 | 1 | 202 | absent |
| 104 | `0x00402B33` | `0x004397C0` | 653 | 5 | 185 | absent |
| 105 | `0x00402B38` | `0x00438DF0` | 671 | 1 | 200 | absent |
| 106 | `0x00402B3D` | `0x004388B0` | 234 | 1 | 66 | absent |
| 107 | `0x00402B42` | `0x00438390` | 163 | 1 | 42 | absent |
| 108 | `0x00402B47` | `0x00437510` | 1471 | 1 | 370 | absent |
| 109 | `0x00402B4C` | `0x00436B10` | 322 | 1 | 102 | absent |
| 110 | `0x00402B51` | `0x00436780` | 454 | 1 | 123 | absent |
| 111 | `0x00402B56` | `0x004361D0` | 238 | 1 | 62 | absent |
| 112 | `0x00402B5B` | `0x00435D90` | 190 | 1 | 50 | absent |
| 113 | `0x00402B60` | `0x00435870` | 190 | 1 | 50 | absent |
| 114 | `0x00402B65` | `0x004351E0` | 190 | 1 | 50 | absent |
| 115 | `0x00402B6A` | `0x004330F0` | 761 | 1 | 218 | absent |
| 116 | `0x00402B74` | `0x004310F0` | 1096 | 1 | 312 | absent |
| 117 | `0x00402B79` | `0x0042FF00` | 243 | 1 | 64 | absent |
| 118 | `0x00402B7E` | `0x0042FE40` | 51 | 1 | 15 | engine: engine/src/retail/core/native_42fe40.cpp |
| 119 | `0x00402B83` | `0x0042DCF0` | 1132 | 1 | 318 | absent |
| 120 | `0x00402B88` | `0x0042DAC0` | 51 | 1 | 15 | engine: engine/src/retail/core/native_42dac0.cpp |
| 121 | `0x00402B8D` | `0x0042D2B0` | 390 | 1 | 104 | absent |
| 122 | `0x00402B92` | `0x0042BEB0` | 236 | 1 | 62 | absent |
| 123 | `0x00402B97` | `0x0042ACC0` | 870 | 1 | 246 | absent |
| 124 | `0x00402B9C` | `0x00427FF0` | 631 | 1 | 171 | absent |
| 125 | `0x00402BA1` | `0x00427D90` | 410 | 1 | 114 | absent |
| 126 | `0x00402BA6` | `0x00427830` | 267 | 1 | 71 | absent |
| 127 | `0x00402BAB` | `0x004272A0` | 267 | 1 | 71 | absent |
| 128 | `0x00402BB0` | `0x00426D90` | 243 | 1 | 64 | absent |
| 129 | `0x00402BB5` | `0x00426630` | 267 | 1 | 71 | absent |
| 130 | `0x00402BBA` | `0x00425C80` | 263 | 1 | 78 | absent |
| 131 | `0x00402BBF` | `0x00424E80` | 457 | 1 | 135 | absent |
| 132 | `0x00402BC4` | `0x004170A0` | 445 | 1 | 128 | absent |
| 133 | `0x00402BC9` | `0x00424B90` | 121 | 1 | 33 | engine: engine/src/retail/core/native_424b90.cpp |
| 134 | `0x00402BCE` | `0x00423FC0` | 500 | 1 | 126 | absent |
| 135 | `0x00402BD3` | `0x004230A0` | 312 | 1 | 83 | absent |
| 136 | `0x00402BD8` | `0x00422B40` | 335 | 1 | 89 | absent |
| 137 | `0x00402BDD` | `0x004223D0` | 535 | 1 | 159 | absent |
| 138 | `0x00402BE2` | `0x00421750` | 596 | 1 | 173 | absent |
| 139 | `0x00402BE7` | `0x00421430` | 223 | 1 | 61 | absent |
| 140 | `0x00402BEC` | `0x00420CC0` | 450 | 1 | 128 | absent |
| 141 | `0x00402BF1` | `0x00420300` | 497 | 1 | 147 | absent |
| 142 | `0x00402BF6` | `0x0041F340` | 596 | 1 | 173 | absent |
| 143 | `0x00402BFB` | `0x0041E690` | 558 | 1 | 161 | absent |
| 144 | `0x00402C00` | `0x0041DC50` | 371 | 1 | 101 | absent |
| 145 | `0x00402C05` | `0x0041D080` | 520 | 1 | 149 | absent |
| 146 | `0x00402C0A` | `0x0041C700` | 460 | 1 | 132 | absent |
| 147 | `0x00402C0F` | `0x0041B8E0` | 559 | 1 | 162 | absent |
| 148 | `0x00402C14` | `0x0041B310` | 383 | 1 | 111 | absent |
| 149 | `0x00402C19` | `0x0041AAF0` | 421 | 1 | 124 | absent |
| 150 | `0x00402C1E` | `0x0041A1E0` | 389 | 1 | 108 | absent |
| 151 | `0x00402C23` | `0x004199F0` | 496 | 1 | 139 | absent |
| 152 | `0x00402C28` | `0x00419760` | 137 | 1 | 38 | engine: engine/src/retail/core/native_419760.cpp |
| 153 | `0x00402C2D` | `0x004190E0` | 269 | 1 | 78 | absent |
| 154 | `0x00402C32` | `0x00402F50` | 156 | 7 | 78 | absent |
