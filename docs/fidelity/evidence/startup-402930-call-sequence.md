# Startup registration sequence `0x00402930` and its neighbour `0x00402C40`

Artifact: retail Project IGI `igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`.

## Correction: this document's first version measured two functions as one

An earlier revision of this file claimed `0x00402930` contains **309 calls over 303 distinct callees** and that the in-candidate guard's figures
("155 ordered init/open/load calls ending in a tail jump to `0x004CEF30`") were wrong. **That claim was wrong, and the guard was right.** The
mistake: the measurement ran from `0x00402930` to the next *manifest* native `0x00402F50` (`Config_YmbeAfp`), which spans **two separate
functions**. The true extent of `0x00402930` ends at `0x00402C3B`:

```
0x00402C32  e8 19 03 00 00   call 0x00402F50      (Config_YmbeAfp, last of 155 calls)
0x00402C37  e9 f4 c2 0c 00   jmp  0x004CEF30      (tail transfer, exactly as the guard says)
0x00402C3C..0x00402C3F        nop padding
```

Measured independently with `objdump -d --start-address=0x402930 --stop-address=0x402C3B`: **155 `call` instructions**, matching the guard.
The second function starts at `0x00402C40` and is a separate, previously unrecorded sequence.

## `0x00402930` - 155 calls, the sequence the candidate stops at

- call sites: **155**, distinct callees: **154**, with an implementation file: **22**, absent: **132**
- the guard's "134 absent" is the same finding within counting-method noise (this count calls a callee implemented only when its manifest record
  names an engine file that exists; `Config_YmbeAfp` has only a `scripts/GameNatives` candidate, so it counts as absent here)
- ends with the `Config_YmbeAfp` call and the tail `jmp 0x004CEF30`

| # | call site | target | manifest name | status |
| --- | --- | --- | --- | --- |
| 1 | `0x00402930` | `0x00508F80` | - | absent |
| 2 | `0x00402935` | `0x00489560` | AI_Detection_Init | implemented |
| 3 | `0x0040293A` | `0x00407D60` | HumanPlayer_Setactivematerial | implemented |
| 4 | `0x0040293F` | `0x00488BB0` | - | absent |
| 5 | `0x00402944` | `0x00402FF0` | Config_FillPlayerProfileListBox_2 | implemented |
| 6 | `0x00402949` | `0x00415530` | Game_Init | implemented |
| 7 | `0x0040294E` | `0x004C1780` | AnimSound_LoadParameters | implemented |
| 8 | `0x00402953` | `0x00401FC0` | Flow_Init | implemented |
| 9 | `0x00402958` | `0x00488800` | - | absent |
| 10 | `0x0040295D` | `0x00488620` | - | absent |
| 11 | `0x00402962` | `0x00487BE0` | - | absent |
| 12 | `0x00402967` | `0x004872C0` | - | absent |
| 13 | `0x0040296C` | `0x00415070` | - | absent |
| 14 | `0x00402971` | `0x00486900` | - | absent |
| 15 | `0x00402976` | `0x00485A40` | - | absent |
| 16 | `0x0040297B` | `0x00485330` | - | absent |
| 17 | `0x00402980` | `0x004851B0` | - | absent |
| 18 | `0x00402985` | `0x00484F30` | - | absent |
| 19 | `0x0040298A` | `0x004825C0` | HumanCamera_Init | implemented |
| 20 | `0x0040298F` | `0x004813E0` | - | absent |
| 21 | `0x00402994` | `0x004801B0` | - | absent |
| 22 | `0x00402999` | `0x0047E700` | - | absent |
| 23 | `0x0040299E` | `0x0047CF70` | - | absent |
| 24 | `0x004029A3` | `0x0047CF20` | - | absent |
| 25 | `0x004029A8` | `0x0047CAB0` | AmmoTypeOpen | implemented |
| 26 | `0x004029AD` | `0x00413DF0` | - | absent |
| 27 | `0x004029B2` | `0x00407290` | - | absent |
| 28 | `0x004029B7` | `0x0047B4B0` | - | absent |
| 29 | `0x004029BC` | `0x00477E30` | - | absent |
| 30 | `0x004029C1` | `0x00477220` | - | absent |
| 31 | `0x004029C6` | `0x00476730` | - | absent |
| 32 | `0x004029CB` | `0x00476110` | - | absent |
| 33 | `0x004029D0` | `0x00475AC0` | - | absent |
| 34 | `0x004029D5` | `0x00475380` | - | absent |
| 35 | `0x004029DA` | `0x00473ED0` | - | absent |
| 36 | `0x004029DF` | `0x004739D0` | - | absent |
| 37 | `0x004029E4` | `0x004733F0` | - | absent |
| 38 | `0x004029E9` | `0x00472D00` | - | absent |
| 39 | `0x004029EE` | `0x00472A70` | - | absent |
| 40 | `0x004029F3` | `0x00472670` | - | absent |
| 41 | `0x004029F8` | `0x00470FA0` | - | absent |
| 42 | `0x004029FD` | `0x00470610` | - | absent |
| 43 | `0x00402A02` | `0x00470410` | - | absent |
| 44 | `0x00402A07` | `0x0046F2B0` | - | absent |
| 45 | `0x00402A0C` | `0x0046E370` | - | absent |
| 46 | `0x00402A11` | `0x0046DF10` | - | absent |
| 47 | `0x00402A16` | `0x0046DD00` | - | absent |
| 48 | `0x00402A1B` | `0x0046DB40` | - | absent |
| 49 | `0x00402A20` | `0x0046D420` | - | absent |
| 50 | `0x00402A25` | `0x0046CB70` | - | absent |
| 51 | `0x00402A2A` | `0x0046C0C0` | - | absent |
| 52 | `0x00402A2F` | `0x0046BA60` | - | absent |
| 53 | `0x00402A34` | `0x0046B7F0` | - | absent |
| 54 | `0x00402A39` | `0x0046B510` | - | absent |
| 55 | `0x00402A3E` | `0x0046AB60` | - | absent |
| 56 | `0x00402A43` | `0x004696B0` | - | absent |
| 57 | `0x00402A48` | `0x00468E20` | - | absent |
| 58 | `0x00402A4D` | `0x004688B0` | - | absent |
| 59 | `0x00402A52` | `0x004682F0` | - | absent |
| 60 | `0x00402A57` | `0x00467700` | - | absent |
| 61 | `0x00402A5C` | `0x00466F70` | - | absent |
| 62 | `0x00402A61` | `0x00466E40` | - | absent |
| 63 | `0x00402A66` | `0x004663D0` | - | absent |
| 64 | `0x00402A6B` | `0x00465BC0` | - | absent |
| 65 | `0x00402A70` | `0x00465A10` | HumanAttach_Init | implemented |
| 66 | `0x00402A75` | `0x00464260` | HumanView_Init | implemented |
| 67 | `0x00402A7A` | `0x0045E450` | Human_Init | implemented |
| 68 | `0x00402A7F` | `0x0040FE30` | HumanPlayer_Init | implemented |
| 69 | `0x00402A84` | `0x0045DB70` | HumanPlayerInput_Init | implemented |
| 70 | `0x00402A89` | `0x0045CAB0` | HumanSoldier_Init | implemented |
| 71 | `0x00402A8E` | `0x00459880` | HumanSoldierPistol_Init | implemented |
| 72 | `0x00402A93` | `0x00456AD0` | HumanSoldierRPG_Init | implemented |
| 73 | `0x00402A98` | `0x00454970` | HumanSoldierFemale_Init | implemented |
| 74 | `0x00402A9D` | `0x00453550` | - | absent |
| 75 | `0x00402AA2` | `0x004533B0` | - | absent |
| 76 | `0x00402AA7` | `0x0044C8D0` | HumanAI_Init | implemented |
| 77 | `0x00402AAC` | `0x00406450` | HumanAIConfig_Init | implemented |
| 78 | `0x00402AB1` | `0x0044C130` | - | absent |
| 79 | `0x00402AB6` | `0x00449D30` | - | absent |
| 80 | `0x00402ABB` | `0x00449970` | - | absent |
| 81 | `0x00402AC0` | `0x00449360` | - | absent |
| 82 | `0x00402AC5` | `0x00448D30` | - | absent |
| 83 | `0x00402ACA` | `0x004485E0` | - | absent |
| 84 | `0x00402ACF` | `0x00447DB0` | - | absent |
| 85 | `0x00402AD4` | `0x004464C0` | - | absent |
| 86 | `0x00402AD9` | `0x00444BF0` | - | absent |
| 87 | `0x00402ADE` | `0x00443ED0` | - | absent |
| 88 | `0x00402AE3` | `0x00443680` | - | absent |
| 89 | `0x00402AE8` | `0x00440FD0` | - | absent |
| 90 | `0x00402AED` | `0x00440C20` | - | absent |
| 91 | `0x00402AF2` | `0x00440710` | - | absent |
| 92 | `0x00402AF7` | `0x004403E0` | - | absent |
| 93 | `0x00402AFC` | `0x0043F900` | - | absent |
| 94 | `0x00402B01` | `0x0043EFF0` | - | absent |
| 95 | `0x00402B06` | `0x0043E8D0` | - | absent |
| 96 | `0x00402B0B` | `0x0043E4F0` | - | absent |
| 97 | `0x00402B10` | `0x0043CC70` | - | absent |
| 98 | `0x00402B15` | `0x0043C6D0` | - | absent |
| 99 | `0x00402B1A` | `0x0043BFD0` | - | absent |
| 100 | `0x00402B1F` | `0x0043B850` | - | absent |
| 101 | `0x00402B24` | `0x0043B2F0` | - | absent |
| 102 | `0x00402B29` | `0x0043AD80` | - | absent |
| 103 | `0x00402B2E` | `0x0043A840` | - | absent |
| 104 | `0x00402B33` | `0x004397C0` | - | absent |
| 105 | `0x00402B38` | `0x00438DF0` | - | absent |
| 106 | `0x00402B3D` | `0x004388B0` | - | absent |
| 107 | `0x00402B42` | `0x00438390` | - | absent |
| 108 | `0x00402B47` | `0x00437510` | - | absent |
| 109 | `0x00402B4C` | `0x00436B10` | - | absent |
| 110 | `0x00402B51` | `0x00436780` | - | absent |
| 111 | `0x00402B56` | `0x004361D0` | - | absent |
| 112 | `0x00402B5B` | `0x00435D90` | - | absent |
| 113 | `0x00402B60` | `0x00435870` | - | absent |
| 114 | `0x00402B65` | `0x004351E0` | - | absent |
| 115 | `0x00402B6A` | `0x004330F0` | - | absent |
| 116 | `0x00402B6F` | `0x004C1780` | AnimSound_LoadParameters | implemented |
| 117 | `0x00402B74` | `0x004310F0` | - | absent |
| 118 | `0x00402B79` | `0x0042FF00` | - | absent |
| 119 | `0x00402B7E` | `0x0042FE40` | - | absent |
| 120 | `0x00402B83` | `0x0042DCF0` | - | absent |
| 121 | `0x00402B88` | `0x0042DAC0` | - | absent |
| 122 | `0x00402B8D` | `0x0042D2B0` | - | absent |
| 123 | `0x00402B92` | `0x0042BEB0` | - | absent |
| 124 | `0x00402B97` | `0x0042ACC0` | - | absent |
| 125 | `0x00402B9C` | `0x00427FF0` | - | absent |
| 126 | `0x00402BA1` | `0x00427D90` | - | absent |
| 127 | `0x00402BA6` | `0x00427830` | - | absent |
| 128 | `0x00402BAB` | `0x004272A0` | - | absent |
| 129 | `0x00402BB0` | `0x00426D90` | - | absent |
| 130 | `0x00402BB5` | `0x00426630` | - | absent |
| 131 | `0x00402BBA` | `0x00425C80` | - | absent |
| 132 | `0x00402BBF` | `0x00424E80` | - | absent |
| 133 | `0x00402BC4` | `0x004170A0` | - | absent |
| 134 | `0x00402BC9` | `0x00424B90` | Cursor_Init_424B90 | implemented |
| 135 | `0x00402BCE` | `0x00423FC0` | - | absent |
| 136 | `0x00402BD3` | `0x004230A0` | - | absent |
| 137 | `0x00402BD8` | `0x00422B40` | ModelViewBox_Init | implemented |
| 138 | `0x00402BDD` | `0x004223D0` | - | absent |
| 139 | `0x00402BE2` | `0x00421750` | - | absent |
| 140 | `0x00402BE7` | `0x00421430` | - | absent |
| 141 | `0x00402BEC` | `0x00420CC0` | - | absent |
| 142 | `0x00402BF1` | `0x00420300` | - | absent |
| 143 | `0x00402BF6` | `0x0041F340` | - | absent |
| 144 | `0x00402BFB` | `0x0041E690` | - | absent |
| 145 | `0x00402C00` | `0x0041DC50` | - | absent |
| 146 | `0x00402C05` | `0x0041D080` | - | absent |
| 147 | `0x00402C0A` | `0x0041C700` | - | absent |
| 148 | `0x00402C0F` | `0x0041B8E0` | - | absent |
| 149 | `0x00402C14` | `0x0041B310` | - | absent |
| 150 | `0x00402C19` | `0x0041AAF0` | - | absent |
| 151 | `0x00402C1E` | `0x0041A1E0` | - | absent |
| 152 | `0x00402C23` | `0x004199F0` | - | absent |
| 153 | `0x00402C28` | `0x00419760` | - | absent |
| 154 | `0x00402C2D` | `0x004190E0` | - | absent |
| 155 | `0x00402C32` | `0x00402F50` | Config_YmbeAfp | implemented |

## `0x00402C40` - a second, unrecorded 154-call sequence

Not in the manifest, no entry in the work order, and `axt 0x402C40` finds no direct caller. Its first calls are `0x004C1780`, then `0x004192B0`,
`0x004199B0` - the small teardown-shaped routines that call `0x00401A20` (see the registrar pattern evidence). It is plausibly a companion
register/unregister pass, but that is a hypothesis, not a finding: it has no name and no recorded call site.

- call sites: **154**, distinct callees: **150**, with an implementation file: **5**, absent: **145**

| # | call site | target | manifest name | status |
| --- | --- | --- | --- | --- |
| 1 | `0x00402C40` | `0x004C1780` | AnimSound_LoadParameters | implemented |
| 2 | `0x00402C45` | `0x004192B0` | - | absent |
| 3 | `0x00402C4A` | `0x004199B0` | - | absent |
| 4 | `0x00402C4F` | `0x0041A1A0` | - | absent |
| 5 | `0x00402C54` | `0x0041AAC0` | - | absent |
| 6 | `0x00402C59` | `0x0041B260` | - | absent |
| 7 | `0x00402C5E` | `0x0041B840` | - | absent |
| 8 | `0x00402C63` | `0x0041C660` | - | absent |
| 9 | `0x00402C68` | `0x0041D050` | - | absent |
| 10 | `0x00402C6D` | `0x0041DBF0` | - | absent |
| 11 | `0x00402C72` | `0x0041E660` | - | absent |
| 12 | `0x00402C77` | `0x0041F2A0` | - | absent |
| 13 | `0x00402C7C` | `0x004201B0` | - | absent |
| 14 | `0x00402C81` | `0x00420C90` | - | absent |
| 15 | `0x00402C86` | `0x004213E0` | - | absent |
| 16 | `0x00402C8B` | `0x00421700` | - | absent |
| 17 | `0x00402C90` | `0x004223A0` | - | absent |
| 18 | `0x00402C95` | `0x00422B10` | - | absent |
| 19 | `0x00402C9A` | `0x00423050` | - | absent |
| 20 | `0x00402C9F` | `0x00423F90` | - | absent |
| 21 | `0x00402CA4` | `0x00424790` | - | absent |
| 22 | `0x00402CA9` | `0x00424E10` | - | absent |
| 23 | `0x00402CAE` | `0x00418AC0` | - | absent |
| 24 | `0x00402CB3` | `0x00425580` | - | absent |
| 25 | `0x00402CB8` | `0x00426610` | - | absent |
| 26 | `0x00402CBD` | `0x00426D60` | - | absent |
| 27 | `0x00402CC2` | `0x00427280` | - | absent |
| 28 | `0x00402CC7` | `0x00427810` | - | absent |
| 29 | `0x00402CCC` | `0x00427D80` | - | absent |
| 30 | `0x00402CD1` | `0x00427FB0` | - | absent |
| 31 | `0x00402CD6` | `0x00429A00` | - | absent |
| 32 | `0x00402CDB` | `0x0042BC00` | - | absent |
| 33 | `0x00402CE0` | `0x0042C4E0` | - | absent |
| 34 | `0x00402CE5` | `0x0042DAA0` | - | absent |
| 35 | `0x00402CEA` | `0x00430940` | - | absent |
| 36 | `0x00402CEF` | `0x004C1780` | AnimSound_LoadParameters | implemented |
| 37 | `0x00402CF4` | `0x0042FDA0` | - | absent |
| 38 | `0x00402CF9` | `0x004C1780` | AnimSound_LoadParameters | implemented |
| 39 | `0x00402CFE` | `0x00433010` | - | absent |
| 40 | `0x00402D03` | `0x004C1780` | AnimSound_LoadParameters | implemented |
| 41 | `0x00402D08` | `0x00434DF0` | - | absent |
| 42 | `0x00402D0D` | `0x00435450` | - | absent |
| 43 | `0x00402D12` | `0x00435C30` | - | absent |
| 44 | `0x00402D17` | `0x004360B0` | - | absent |
| 45 | `0x00402D1C` | `0x00436770` | - | absent |
| 46 | `0x00402D21` | `0x00436AF0` | - | absent |
| 47 | `0x00402D26` | `0x004374E0` | - | absent |
| 48 | `0x00402D2B` | `0x00438370` | - | absent |
| 49 | `0x00402D30` | `0x00438890` | - | absent |
| 50 | `0x00402D35` | `0x00438C10` | - | absent |
| 51 | `0x00402D3A` | `0x004397A0` | - | absent |
| 52 | `0x00402D3F` | `0x0043A810` | - | absent |
| 53 | `0x00402D44` | `0x0043AD60` | - | absent |
| 54 | `0x00402D49` | `0x0043B2D0` | - | absent |
| 55 | `0x00402D4E` | `0x0043B830` | - | absent |
| 56 | `0x00402D53` | `0x0043BE60` | - | absent |
| 57 | `0x00402D58` | `0x0043C380` | - | absent |
| 58 | `0x00402D5D` | `0x0043CC50` | - | absent |
| 59 | `0x00402D62` | `0x0043E4C0` | - | absent |
| 60 | `0x00402D67` | `0x0043E8C0` | - | absent |
| 61 | `0x00402D6C` | `0x0043EEF0` | - | absent |
| 62 | `0x00402D71` | `0x0043F800` | - | absent |
| 63 | `0x00402D76` | `0x00440250` | - | absent |
| 64 | `0x00402D7B` | `0x00440700` | - | absent |
| 65 | `0x00402D80` | `0x00440BE0` | - | absent |
| 66 | `0x00402D85` | `0x00440FB0` | - | absent |
| 67 | `0x00402D8A` | `0x00443610` | - | absent |
| 68 | `0x00402D8F` | `0x00443E60` | - | absent |
| 69 | `0x00402D94` | `0x00444B00` | - | absent |
| 70 | `0x00402D99` | `0x00445DC0` | - | absent |
| 71 | `0x00402D9E` | `0x00447D00` | - | absent |
| 72 | `0x00402DA3` | `0x00448530` | - | absent |
| 73 | `0x00402DA8` | `0x00448B50` | - | absent |
| 74 | `0x00402DAD` | `0x00449290` | - | absent |
| 75 | `0x00402DB2` | `0x004497D0` | - | absent |
| 76 | `0x00402DB7` | `0x00449D10` | - | absent |
| 77 | `0x00402DBC` | `0x0044A1C0` | - | absent |
| 78 | `0x00402DC1` | `0x0044C810` | - | absent |
| 79 | `0x00402DC6` | `0x00465BB0` | HumanAttach_OnEvent | implemented |
| 80 | `0x00402DCB` | `0x00464600` | - | absent |
| 81 | `0x00402DD0` | `0x00455960` | - | absent |
| 82 | `0x00402DD5` | `0x004577E0` | - | absent |
| 83 | `0x00402DDA` | `0x0045A7C0` | - | absent |
| 84 | `0x00402DDF` | `0x0045D580` | - | absent |
| 85 | `0x00402DE4` | `0x0045E040` | - | absent |
| 86 | `0x00402DE9` | `0x00413770` | HumanPlayer_UpdateAnimation_9 | implemented |
| 87 | `0x00402DEE` | `0x00460B20` | - | absent |
| 88 | `0x00402DF3` | `0x00406D90` | - | absent |
| 89 | `0x00402DF8` | `0x00450830` | - | absent |
| 90 | `0x00402DFD` | `0x00453520` | - | absent |
| 91 | `0x00402E02` | `0x00453710` | - | absent |
| 92 | `0x00402E07` | `0x00466210` | - | absent |
| 93 | `0x00402E0C` | `0x00466E10` | - | absent |
| 94 | `0x00402E11` | `0x00466F40` | - | absent |
| 95 | `0x00402E16` | `0x00467570` | - | absent |
| 96 | `0x00402E1B` | `0x00467D30` | - | absent |
| 97 | `0x00402E20` | `0x00468870` | - | absent |
| 98 | `0x00402E25` | `0x00468DA0` | - | absent |
| 99 | `0x00402E2A` | `0x00469420` | - | absent |
| 100 | `0x00402E2F` | `0x0046AAD0` | - | absent |
| 101 | `0x00402E34` | `0x0046AF90` | - | absent |
| 102 | `0x00402E39` | `0x0046B650` | - | absent |
| 103 | `0x00402E3E` | `0x0046B960` | - | absent |
| 104 | `0x00402E43` | `0x0046C010` | - | absent |
| 105 | `0x00402E48` | `0x0046C950` | - | absent |
| 106 | `0x00402E4D` | `0x0046D3E0` | - | absent |
| 107 | `0x00402E52` | `0x0046DB10` | - | absent |
| 108 | `0x00402E57` | `0x0046DCE0` | - | absent |
| 109 | `0x00402E5C` | `0x0046DF00` | - | absent |
| 110 | `0x00402E61` | `0x0046E200` | - | absent |
| 111 | `0x00402E66` | `0x0046EAA0` | - | absent |
| 112 | `0x00402E6B` | `0x004703F0` | - | absent |
| 113 | `0x00402E70` | `0x00470600` | - | absent |
| 114 | `0x00402E75` | `0x00470EB0` | - | absent |
| 115 | `0x00402E7A` | `0x004725D0` | - | absent |
| 116 | `0x00402E7F` | `0x00472A50` | - | absent |
| 117 | `0x00402E84` | `0x00472CF0` | - | absent |
| 118 | `0x00402E89` | `0x00473980` | - | absent |
| 119 | `0x00402E8E` | `0x00473D20` | - | absent |
| 120 | `0x00402E93` | `0x004733E0` | - | absent |
| 121 | `0x00402E98` | `0x004752E0` | - | absent |
| 122 | `0x00402E9D` | `0x00475AB0` | - | absent |
| 123 | `0x00402EA2` | `0x00476000` | - | absent |
| 124 | `0x00402EA7` | `0x00476660` | - | absent |
| 125 | `0x00402EAC` | `0x00477010` | - | absent |
| 126 | `0x00402EB1` | `0x00477BC0` | - | absent |
| 127 | `0x00402EB6` | `0x0047C4A0` | - | absent |
| 128 | `0x00402EBB` | `0x0047AA40` | - | absent |
| 129 | `0x00402EC0` | `0x00407D00` | - | absent |
| 130 | `0x00402EC5` | `0x00414810` | - | absent |
| 131 | `0x00402ECA` | `0x0047CCE0` | - | absent |
| 132 | `0x00402ECF` | `0x0047CF50` | - | absent |
| 133 | `0x00402ED4` | `0x0047E6B0` | - | absent |
| 134 | `0x00402ED9` | `0x00482560` | - | absent |
| 135 | `0x00402EDE` | `0x0047E970` | - | absent |
| 136 | `0x00402EE3` | `0x00480FA0` | - | absent |
| 137 | `0x00402EE8` | `0x00484CF0` | - | absent |
| 138 | `0x00402EED` | `0x00485140` | - | absent |
| 139 | `0x00402EF2` | `0x004852F0` | - | absent |
| 140 | `0x00402EF7` | `0x00485950` | - | absent |
| 141 | `0x00402EFC` | `0x00486610` | - | absent |
| 142 | `0x00402F01` | `0x00487130` | - | absent |
| 143 | `0x00402F06` | `0x00415480` | - | absent |
| 144 | `0x00402F0B` | `0x00487BC0` | - | absent |
| 145 | `0x00402F10` | `0x004885F0` | - | absent |
| 146 | `0x00402F15` | `0x004887E0` | - | absent |
| 147 | `0x00402F1A` | `0x00488B90` | - | absent |
| 148 | `0x00402F1F` | `0x004027E0` | - | absent |
| 149 | `0x00402F24` | `0x004C1780` | AnimSound_LoadParameters | implemented |
| 150 | `0x00402F29` | `0x004166C0` | - | absent |
| 151 | `0x00402F2E` | `0x00405830` | Config_Language | implemented |
| 152 | `0x00402F33` | `0x00489430` | - | absent |
| 153 | `0x00402F38` | `0x004082B0` | HumanPlayer_LoadParameters_2 | implemented |
| 154 | `0x00402F3D` | `0x004895D0` | - | absent |
