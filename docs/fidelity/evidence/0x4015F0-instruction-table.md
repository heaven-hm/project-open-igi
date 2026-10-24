# 0x004015F0 instruction table (function-scoped, 391 bytes, 0x004015F0-0x00401777)

Source: radare2 6.2.0 `af` + `pdf` on retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.

Rows: 88. Function byte extent reported by radare2: 391. Mean bytes per instruction: 4.44 - checked against the extent, which is the check the round-184 table failed.

| addr | bytes | instruction |
| --- | --- | --- |
| 0x004015f0 | `8b15587c5600` | mov edx, dword [0x567c58]   ; [0x567c58:4]=0 |
| 0x004015f6 | `53` | push ebx |
| 0x004015f7 | `4a` | dec edx |
| 0x004015f8 | `55` | push ebp |
| 0x004015f9 | `8915587c5600` | mov dword [0x567c58], edx   ; [0x567c58:4]=0 |
| 0x004015ff | `8b15e0a7af00` | mov edx, dword [0xafa7e0]   ; [0xafa7e0:4]=0 |
| 0x00401605 | `33c0` | xor eax, eax |
| 0x00401607 | `56` | push esi |
| 0x00401608 | `8b742410` | mov esi, dword [arg_10h] |
| 0x0040160c | `57` | push edi |
| 0x0040160d | `85d2` | test edx, edx |
| 0x00401611 | `b9e0a6af00` | mov ecx, 0xafa6e0 |
| 0x00401626 | `85d2` | test edx, edx |
| 0x0040162a | `8b0a` | mov ecx, dword [edx] |
| 0x0040162c | `f7d9` | neg ecx |
| 0x0040162e | `1bc9` | sbb ecx, ecx |
| 0x00401630 | `23ca` | and ecx, edx |
| 0x0040163f | `668b461c` | mov ax, word [esi + 0x1c] |
| 0x00401643 | `` | 8b0485e07c..   mov eax, dword [eax*4 + 0xa97ce0] |
| 0x0040164a | `85c0` | test eax, eax |
| 0x0040164e | `56` | push esi |
| 0x0040164f | `ffd0` | call eax |
| 0x00401651 | `83c404` | add esp, 4 |
| 0x00401657 | `8b7804` | mov edi, dword [eax + 4] |
| 0x0040165a | `85ff` | test edi, edi |
| 0x00401672 | `668b4e1c` | mov cx, word [esi + 0x1c] |
| 0x00401676 | `` | 8b048de076..   mov eax, dword [ecx*4 + 0xa976e0] |
| 0x0040167d | `85c0` | test eax, eax |
| 0x00401681 | `56` | push esi |
| 0x00401682 | `ffd0` | call eax |
| 0x00401684 | `83c404` | add esp, 4 |
| 0x0040168a | `85c0` | test eax, eax |
| 0x0040168e | `a1407c5600` | mov eax, dword [0x567c40]   ; [0x567c40:4]=0 |
| 0x00401693 | `33c9` | xor ecx, ecx |
| 0x00401695 | `85c0` | test eax, eax |
| 0x00401697 | `8bd0` | mov edx, eax |
| 0x004016c5 | `e806ffffff` | call fcn.004015d0 |
| 0x004016ca | `8a461e` | mov al, byte [esi + 0x1e] |
| 0x004016cd | `83c404` | add esp, 4 |
| 0x004016d0 | `84c0` | test al, al |
| 0x004016d8 | `33c0` | xor eax, eax |
| 0x004016da | `668b461c` | mov ax, word [esi + 0x1c] |
| 0x004016de | `8d0440` | lea eax, [eax + eax*2] |
| 0x004016e1 | `c1e004` | shl eax, 4 |
| 0x004016e4 | `8b88005faf00` | mov ecx, dword [eax + 0xaf5f00] |
| 0x004016ea | `8ba8f45eaf00` | mov ebp, dword [eax + 0xaf5ef4] |
| 0x004016f0 | `4d` | dec ebp |
| 0x004016f1 | `8b3c31` | mov edi, dword [ecx + esi] |
| 0x004016f4 | `89a8f45eaf00` | mov dword [eax + 0xaf5ef4], ebp ; [0xaf5ef4:4]=0 |
| 0x004016fa | `33c0` | xor eax, eax |
| 0x004016fc | `668b461c` | mov ax, word [esi + 0x1c] |
| 0x00401700 | `8d0c40` | lea ecx, [eax + eax*2] |
| 0x00401703 | `c1e104` | shl ecx, 4 |
| 0x00401706 | `8b81f45eaf00` | mov eax, dword [ecx + 0xaf5ef4] |
| 0x0040170c | `3bf8` | cmp edi, eax |
| 0x00401710 | `8b91f05eaf00` | mov edx, dword [ecx + 0xaf5ef0] |
| 0x00401716 | `8b99ec5eaf00` | mov ebx, dword [ecx + 0xaf5eec] |
| 0x0040171c | `8b0482` | mov eax, dword [edx + eax*4] |
| 0x0040171f | `8b2cba` | mov ebp, dword [edx + edi*4] |
| 0x00401722 | `` | 0faf81fc5e..   imul eax, dword [ecx + 0xaf5efc] |
| 0x00401729 | `8b89005faf00` | mov ecx, dword [ecx + 0xaf5f00] |
| 0x0040172f | `03c3` | add eax, ebx |
| 0x00401731 | `8b1c01` | mov ebx, dword [ecx + eax] |
| 0x00401734 | `8b0c9a` | mov ecx, dword [edx + ebx*4] |
| 0x00401737 | `890cba` | mov dword [edx + edi*4], ecx |
| 0x0040173a | `33c9` | xor ecx, ecx |
| 0x0040173c | `668b4e1c` | mov cx, word [esi + 0x1c] |
| 0x00401740 | `8d1449` | lea edx, [ecx + ecx*2] |
| 0x00401743 | `c1e204` | shl edx, 4 |
| 0x00401746 | `8b8af05eaf00` | mov ecx, dword [edx + 0xaf5ef0] |
| 0x0040174c | `892c99` | mov dword [ecx + ebx*4], ebp |
| 0x0040174f | `33c9` | xor ecx, ecx |
| 0x00401751 | `668b4e1c` | mov cx, word [esi + 0x1c] |
| 0x00401755 | `8d1449` | lea edx, [ecx + ecx*2] |
| 0x00401758 | `c1e204` | shl edx, 4 |
| 0x0040175b | `8b8a005faf00` | mov ecx, dword [edx + 0xaf5f00] |
| 0x00401761 | `893c01` | mov dword [ecx + eax], edi |
| 0x00401764 | `5f` | pop edi |
| 0x00401765 | `5e` | pop esi |
| 0x00401766 | `5d` | pop ebp |
| 0x00401767 | `5b` | pop ebx |
| 0x00401768 | `c3` | ret |
| 0x0040176a | `e8a1f50a00` | call fcn.004b0d10 |
| 0x0040176f | `83c404` | add esp, 4 |
| 0x00401773 | `5e` | pop esi |
| 0x00401774 | `5d` | pop ebp |
| 0x00401775 | `5b` | pop ebx |
| 0x00401776 | `c3` | ret |
