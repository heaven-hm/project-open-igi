# 0x00402930 startup chain: the ordered work order for making the candidate boot

The guard in `engine/src/retail/core/class_registration.cpp` reports this function's real nature and is correct: retail 0x00402930 is the
startup init sequence, not a class registrar. Its own message (155 ordered calls, tail jump to 0x004CEF30, 134 callees absent) matches
the measurement below, so the candidate fails closed on purpose until the chain exists.

Measured from the retail disassembly, function extent 0x00402930..0x00402930+780:

- call sites: **155**
- callees in CRT ranges (supplied by the toolchain, not transcribed): **1**
- callees already transcribed: **3**
- callees missing: **151**

## Ordered chain

The order is retail call order. Arguments are the literal constants pushed in the six instructions before each call, when they are
literals; `-` means the argument is computed rather than immediate and has to be read from the block.

| # | call site | callee | status | literal args |
| --- | --- | --- | --- | --- |
| 1 | `0x402930` | `0x508F80` | missing | - |
| 2 | `0x402935` | `0x489560` | implemented | - |
| 3 | `0x40293A` | `0x407D60` | missing | - |
| 4 | `0x40293F` | `0x488BB0` | missing | - |
| 5 | `0x402944` | `0x402FF0` | missing | - |
| 6 | `0x402949` | `0x415530` | missing | - |
| 7 | `0x40294E` | `0x4C1780` | implemented | - |
| 8 | `0x402953` | `0x401FC0` | CRT | - |
| 9 | `0x402958` | `0x488800` | missing | - |
| 10 | `0x40295D` | `0x488620` | missing | - |
| 11 | `0x402962` | `0x487BE0` | missing | - |
| 12 | `0x402967` | `0x4872C0` | missing | - |
| 13 | `0x40296C` | `0x415070` | missing | - |
| 14 | `0x402971` | `0x486900` | missing | - |
| 15 | `0x402976` | `0x485A40` | missing | - |
| 16 | `0x40297B` | `0x485330` | missing | - |
| 17 | `0x402980` | `0x4851B0` | missing | - |
| 18 | `0x402985` | `0x484F30` | missing | - |
| 19 | `0x40298A` | `0x4825C0` | missing | - |
| 20 | `0x40298F` | `0x4813E0` | missing | - |
| 21 | `0x402994` | `0x4801B0` | missing | - |
| 22 | `0x402999` | `0x47E700` | missing | - |
| 23 | `0x40299E` | `0x47CF70` | missing | - |
| 24 | `0x4029A3` | `0x47CF20` | missing | - |
| 25 | `0x4029A8` | `0x47CAB0` | missing | - |
| 26 | `0x4029AD` | `0x413DF0` | missing | - |
| 27 | `0x4029B2` | `0x407290` | missing | - |
| 28 | `0x4029B7` | `0x47B4B0` | missing | - |
| 29 | `0x4029BC` | `0x477E30` | missing | - |
| 30 | `0x4029C1` | `0x477220` | missing | - |
| 31 | `0x4029C6` | `0x476730` | missing | - |
| 32 | `0x4029CB` | `0x476110` | missing | - |
| 33 | `0x4029D0` | `0x475AC0` | missing | - |
| 34 | `0x4029D5` | `0x475380` | missing | - |
| 35 | `0x4029DA` | `0x473ED0` | missing | - |
| 36 | `0x4029DF` | `0x4739D0` | missing | - |
| 37 | `0x4029E4` | `0x4733F0` | missing | - |
| 38 | `0x4029E9` | `0x472D00` | missing | - |
| 39 | `0x4029EE` | `0x472A70` | missing | - |
| 40 | `0x4029F3` | `0x472670` | missing | - |
| 41 | `0x4029F8` | `0x470FA0` | missing | - |
| 42 | `0x4029FD` | `0x470610` | missing | - |
| 43 | `0x402A02` | `0x470410` | missing | - |
| 44 | `0x402A07` | `0x46F2B0` | missing | - |
| 45 | `0x402A0C` | `0x46E370` | missing | - |
| 46 | `0x402A11` | `0x46DF10` | missing | - |
| 47 | `0x402A16` | `0x46DD00` | missing | - |
| 48 | `0x402A1B` | `0x46DB40` | missing | - |
| 49 | `0x402A20` | `0x46D420` | missing | - |
| 50 | `0x402A25` | `0x46CB70` | missing | - |
| 51 | `0x402A2A` | `0x46C0C0` | missing | - |
| 52 | `0x402A2F` | `0x46BA60` | missing | - |
| 53 | `0x402A34` | `0x46B7F0` | missing | - |
| 54 | `0x402A39` | `0x46B510` | missing | - |
| 55 | `0x402A3E` | `0x46AB60` | missing | - |
| 56 | `0x402A43` | `0x4696B0` | missing | - |
| 57 | `0x402A48` | `0x468E20` | missing | - |
| 58 | `0x402A4D` | `0x4688B0` | missing | - |
| 59 | `0x402A52` | `0x4682F0` | missing | - |
| 60 | `0x402A57` | `0x467700` | missing | - |
| 61 | `0x402A5C` | `0x466F70` | missing | - |
| 62 | `0x402A61` | `0x466E40` | missing | - |
| 63 | `0x402A66` | `0x4663D0` | missing | - |
| 64 | `0x402A6B` | `0x465BC0` | missing | - |
| 65 | `0x402A70` | `0x465A10` | missing | - |
| 66 | `0x402A75` | `0x464260` | missing | - |
| 67 | `0x402A7A` | `0x45E450` | missing | - |
| 68 | `0x402A7F` | `0x40FE30` | missing | - |
| 69 | `0x402A84` | `0x45DB70` | missing | - |
| 70 | `0x402A89` | `0x45CAB0` | missing | - |
| 71 | `0x402A8E` | `0x459880` | missing | - |
| 72 | `0x402A93` | `0x456AD0` | missing | - |
| 73 | `0x402A98` | `0x454970` | missing | - |
| 74 | `0x402A9D` | `0x453550` | missing | - |
| 75 | `0x402AA2` | `0x4533B0` | missing | - |
| 76 | `0x402AA7` | `0x44C8D0` | missing | - |
| 77 | `0x402AAC` | `0x406450` | missing | - |
| 78 | `0x402AB1` | `0x44C130` | missing | - |
| 79 | `0x402AB6` | `0x449D30` | missing | - |
| 80 | `0x402ABB` | `0x449970` | missing | - |
| 81 | `0x402AC0` | `0x449360` | missing | - |
| 82 | `0x402AC5` | `0x448D30` | missing | - |
| 83 | `0x402ACA` | `0x4485E0` | missing | - |
| 84 | `0x402ACF` | `0x447DB0` | missing | - |
| 85 | `0x402AD4` | `0x4464C0` | missing | - |
| 86 | `0x402AD9` | `0x444BF0` | missing | - |
| 87 | `0x402ADE` | `0x443ED0` | missing | - |
| 88 | `0x402AE3` | `0x443680` | missing | - |
| 89 | `0x402AE8` | `0x440FD0` | missing | - |
| 90 | `0x402AED` | `0x440C20` | missing | - |
| 91 | `0x402AF2` | `0x440710` | missing | - |
| 92 | `0x402AF7` | `0x4403E0` | missing | - |
| 93 | `0x402AFC` | `0x43F900` | missing | - |
| 94 | `0x402B01` | `0x43EFF0` | missing | - |
| 95 | `0x402B06` | `0x43E8D0` | missing | - |
| 96 | `0x402B0B` | `0x43E4F0` | missing | - |
| 97 | `0x402B10` | `0x43CC70` | missing | - |
| 98 | `0x402B15` | `0x43C6D0` | missing | - |
| 99 | `0x402B1A` | `0x43BFD0` | missing | - |
| 100 | `0x402B1F` | `0x43B850` | missing | - |
| 101 | `0x402B24` | `0x43B2F0` | missing | - |
| 102 | `0x402B29` | `0x43AD80` | missing | - |
| 103 | `0x402B2E` | `0x43A840` | missing | - |
| 104 | `0x402B33` | `0x4397C0` | missing | - |
| 105 | `0x402B38` | `0x438DF0` | missing | - |
| 106 | `0x402B3D` | `0x4388B0` | missing | - |
| 107 | `0x402B42` | `0x438390` | missing | - |
| 108 | `0x402B47` | `0x437510` | missing | - |
| 109 | `0x402B4C` | `0x436B10` | missing | - |
| 110 | `0x402B51` | `0x436780` | missing | - |
| 111 | `0x402B56` | `0x4361D0` | missing | - |
| 112 | `0x402B5B` | `0x435D90` | missing | - |
| 113 | `0x402B60` | `0x435870` | missing | - |
| 114 | `0x402B65` | `0x4351E0` | missing | - |
| 115 | `0x402B6A` | `0x4330F0` | missing | - |
| 116 | `0x402B6F` | `0x4C1780` | implemented | - |
| 117 | `0x402B74` | `0x4310F0` | missing | - |
| 118 | `0x402B79` | `0x42FF00` | missing | - |
| 119 | `0x402B7E` | `0x42FE40` | missing | - |
| 120 | `0x402B83` | `0x42DCF0` | missing | - |
| 121 | `0x402B88` | `0x42DAC0` | missing | - |
| 122 | `0x402B8D` | `0x42D2B0` | missing | - |
| 123 | `0x402B92` | `0x42BEB0` | missing | - |
| 124 | `0x402B97` | `0x42ACC0` | missing | - |
| 125 | `0x402B9C` | `0x427FF0` | missing | - |
| 126 | `0x402BA1` | `0x427D90` | missing | - |
| 127 | `0x402BA6` | `0x427830` | missing | - |
| 128 | `0x402BAB` | `0x4272A0` | missing | - |
| 129 | `0x402BB0` | `0x426D90` | missing | - |
| 130 | `0x402BB5` | `0x426630` | missing | - |
| 131 | `0x402BBA` | `0x425C80` | missing | - |
| 132 | `0x402BBF` | `0x424E80` | missing | - |
| 133 | `0x402BC4` | `0x4170A0` | missing | - |
| 134 | `0x402BC9` | `0x424B90` | missing | - |
| 135 | `0x402BCE` | `0x423FC0` | missing | - |
| 136 | `0x402BD3` | `0x4230A0` | missing | - |
| 137 | `0x402BD8` | `0x422B40` | missing | - |
| 138 | `0x402BDD` | `0x4223D0` | missing | - |
| 139 | `0x402BE2` | `0x421750` | missing | - |
| 140 | `0x402BE7` | `0x421430` | missing | - |
| 141 | `0x402BEC` | `0x420CC0` | missing | - |
| 142 | `0x402BF1` | `0x420300` | missing | - |
| 143 | `0x402BF6` | `0x41F340` | missing | - |
| 144 | `0x402BFB` | `0x41E690` | missing | - |
| 145 | `0x402C00` | `0x41DC50` | missing | - |
| 146 | `0x402C05` | `0x41D080` | missing | - |
| 147 | `0x402C0A` | `0x41C700` | missing | - |
| 148 | `0x402C0F` | `0x41B8E0` | missing | - |
| 149 | `0x402C14` | `0x41B310` | missing | - |
| 150 | `0x402C19` | `0x41AAF0` | missing | - |
| 151 | `0x402C1E` | `0x41A1E0` | missing | - |
| 152 | `0x402C23` | `0x4199F0` | missing | - |
| 153 | `0x402C28` | `0x419760` | missing | - |
| 154 | `0x402C2D` | `0x4190E0` | missing | - |
| 155 | `0x402C32` | `0x402F50` | missing | - |

## How to use this

Work top to bottom. For each missing callee: read its body, close its own closure first, transcribe it with the five-line provenance
block, review it, probe it in retail, then run the gate for that one file. When all 155 call sites have a transcribed target, 0x00402930
itself is a straight-line sequence of calls and becomes the last transcription in this chain - not the first.

The tail jump to 0x004CEF30 is the entry into the main frame loop. That is the point at which the candidate stops being a bootstrap and
starts being a game, so it is also the point where Level 1 becomes reachable rather than theoretical.

## What must not be done

Stubbing, reordering, or skipping a call to reach the tail jump faster would produce a binary that boots and is not the retail program.
The guard exists because that shortcut would look like progress. Every row above is either transcribed or still missing; nothing is
unknown-but-assumed.
