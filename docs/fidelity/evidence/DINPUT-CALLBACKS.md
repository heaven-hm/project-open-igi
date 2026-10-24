# DINPUT CALLBACKS: joypad enumeration, joypad init, and the two window procedures

Retail `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`; PE32 base 0x400000. Read-only verification: no engine/manifest edits, no build, no launch. Companion `dinput-callbacks.json` carries the raw bytes/disassembly/tables used here.

File offsets: every image-backed byte used below satisfies `file offset = VA - 0x400000` (verified against the PE section map: `.text` raw 0x1000 VA 0x401000, `.rdata` raw 0x133000 VA 0x533000, `.data` raw 0x136000 VA 0x536000). `.data` is writable (`0xC0000040`), `.rdata` is read-only (`0x40000040`) - this matters because 0x509A40 mutates the axis-pair table, and that table is in `.data` (`0x0054E8A8`, file offset `0x0014E8A8`), not in `.rdata`.

## 1. 0x509A40 - EnumDevicesA callback (629 B, 22 blocks, `__stdcall`, `ret 8`)

- Extent `0x00509A40-0x00509CB4` (629 bytes). ABI: one used argument, the `LPCDIDEVICEINSTANCEA`; `ret 8` pops two stack arguments (device instance, pvRef); the pvRef argument is never read.
- Full machine-code bytes:

```
83ec3c8b4424405783c00433ff578b08894c24088b50048954240c8b4808894c
24108b500ca1f0b4a700895424148b1538b6a7008b088d1495e0b2a700528d54
240c5250ff510c85c00f8c170200008b0d38b6a70053568b048de0b2a7008d0c
8de0b4a7005168c04953008b1050ff128b1538b6a7008bf08b0495e0b2a70050
8b08ff51083bf77d0f6898e95400e8dd5cfaff83c404ebfe8b1538b6a7006838
4c53008b0495e0b4a700508b08ff512c85c07d2e8b1538b6a7008b0495e0b4a7
008b0850ff51088b1538b6a7005eb8010000005b893c95e0b4a7005f83c43cc2
0800a138b6a7006a058b3485e0b4a7008b1ee86955f8ff5056ff533485c07d2d
8b0d38b6a7008b048de0b4a700508b10ff5208a138b6a7005e5b893c85e0b4a7
00b8010000005f83c43cc20800bb10000000c744243018000000895c2434c744
243c01000000c744244080ffffffc744244480000000beace854008b1538b6a7
008b4efc894c24388b0495e0b2a7008d542430526a048b0850ff51183bc7740d
3d020007800f85ca000000893e83c60881feece854007cc3a138b6a7008d5424
1cc744241c14000000895c24208b0485e0b2a700897c2424897c2428c744242c
010000008b08526a0950ff5118a138b6a7008d54241c897c2428c744242c1027
00008b0485e0b2a700526a06508b08ff5118a13cb6a700c74424280100000089
44242cbea8e85400397e0474248b1538b6a7008b0e894c24248b0495e0b2a700
8d54241c526a058b0850ff511885c0752483c60881fee8e854007ccca138b6a7
005e405ba338b6a700b8010000005f83c43cc20800a138b6a7008b0485e0b4a7
00e95bfeffff687ce95400e8005bfaff83c404ebfe
```

- Full disassembly with instruction boundaries (radare2 6.2.0, bytes column is the encoding):

```
0x00509a40      83ec3c         sub esp, 0x3c
0x00509a43      8b442440       mov eax, dword [esp + 0x40]
0x00509a47      57             push edi
0x00509a48      83c004         add eax, 4
0x00509a4b      33ff           xor edi, edi
0x00509a4d      57             push edi
0x00509a4e      8b08           mov ecx, dword [eax]
0x00509a50      894c2408       mov dword [esp + 8], ecx
0x00509a54      8b5004         mov edx, dword [eax + 4]
0x00509a57      8954240c       mov dword [esp + 0xc], edx
0x00509a5b      8b4808         mov ecx, dword [eax + 8]
0x00509a5e      894c2410       mov dword [esp + 0x10], ecx
0x00509a62      8b500c         mov edx, dword [eax + 0xc]
0x00509a65      a1f0b4a700     mov eax, dword [0xa7b4f0]   ; [0xa7b4f0:4]=0
0x00509a6a      89542414       mov dword [esp + 0x14], edx
0x00509a6e      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509a74      8b08           mov ecx, dword [eax]
0x00509a76      8d1495e0b2..   lea edx, [edx*4 + 0xa7b2e0]
0x00509a7d      52             push edx
0x00509a7e      8d54240c       lea edx, [esp + 0xc]
0x00509a82      52             push edx
0x00509a83      50             push eax
0x00509a84      ff510c         call dword [ecx + 0xc]      ; 12
0x00509a87      85c0           test eax, eax
┌─< 0x00509a89      0f8c17020000   jl 0x509ca6
│   0x00509a8f      8b0d38b6a700   mov ecx, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509a95      53             push ebx
│   0x00509a96      56             push esi
│   0x00509a97      8b048de0b2..   mov eax, dword [ecx*4 + 0xa7b2e0]
│   0x00509a9e      8d0c8de0b4..   lea ecx, [ecx*4 + 0xa7b4e0]
│   0x00509aa5      51             push ecx
│   0x00509aa6      68c0495300     push 0x5349c0
│   0x00509aab      8b10           mov edx, dword [eax]
│   0x00509aad      50             push eax
│   0x00509aae      ff12           call dword [edx]
│   0x00509ab0      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509ab6      8bf0           mov esi, eax
│   0x00509ab8      8b0495e0b2..   mov eax, dword [edx*4 + 0xa7b2e0]
│   0x00509abf      50             push eax
│   0x00509ac0      8b08           mov ecx, dword [eax]
│   0x00509ac2      ff5108         call dword [ecx + 8]        ; 8
│   0x00509ac5      3bf7           cmp esi, edi
┌──< 0x00509ac7      7d0f           jge 0x509ad8
││   0x00509ac9      6898e95400     push str.Failed_to_obtain_interface. ; 0x54e998 ; "Failed to obtain interface."
││   0x00509ace      e8dd5cfaff     call 0x4af7b0
││   0x00509ad3      83c404         add esp, 4
───> 0x00509ad6      ebfe           jmp 0x509ad6
└──> 0x00509ad8      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509ade      68384c5300     push 0x534c38               ; '8LS'
│   0x00509ae3      8b0495e0b4..   mov eax, dword [edx*4 + 0xa7b4e0]
│   0x00509aea      50             push eax
│   0x00509aeb      8b08           mov ecx, dword [eax]
│   0x00509aed      ff512c         call dword [ecx + 0x2c]     ; 44
│   0x00509af0      85c0           test eax, eax
┌──< 0x00509af2      7d2e           jge 0x509b22
││   0x00509af4      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
││   0x00509afa      8b0495e0b4..   mov eax, dword [edx*4 + 0xa7b4e0]
││   0x00509b01      8b08           mov ecx, dword [eax]
││   0x00509b03      50             push eax
││   0x00509b04      ff5108         call dword [ecx + 8]        ; 8
││   0x00509b07      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
││   0x00509b0d      5e             pop esi
││   0x00509b0e      b801000000     mov eax, 1
││   0x00509b13      5b             pop ebx
││   0x00509b14      893c95e0b4..   mov dword [edx*4 + 0xa7b4e0], edi ; [0xa7b4e0:4]=0
││   0x00509b1b      5f             pop edi
││   0x00509b1c      83c43c         add esp, 0x3c
││   0x00509b1f      c20800         ret 8
└──> 0x00509b22      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509b27      6a05           push 5                      ; 5
│   0x00509b29      8b3485e0b4..   mov esi, dword [eax*4 + 0xa7b4e0]
│   0x00509b30      8b1e           mov ebx, dword [esi]
│   0x00509b32      e86955f8ff     call 0x48f0a0
│   0x00509b37      50             push eax
│   0x00509b38      56             push esi
│   0x00509b39      ff5334         call dword [ebx + 0x34]     ; 52
│   0x00509b3c      85c0           test eax, eax
┌──< 0x00509b3e      7d2d           jge 0x509b6d
││   0x00509b40      8b0d38b6a700   mov ecx, dword [0xa7b638]   ; [0xa7b638:4]=0
││   0x00509b46      8b048de0b4..   mov eax, dword [ecx*4 + 0xa7b4e0]
││   0x00509b4d      50             push eax
││   0x00509b4e      8b10           mov edx, dword [eax]
││   0x00509b50      ff5208         call dword [edx + 8]        ; 8
││   0x00509b53      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
││   0x00509b58      5e             pop esi
││   0x00509b59      5b             pop ebx
││   0x00509b5a      893c85e0b4..   mov dword [eax*4 + 0xa7b4e0], edi ; [0xa7b4e0:4]=0
││   0x00509b61      b801000000     mov eax, 1
││   0x00509b66      5f             pop edi
││   0x00509b67      83c43c         add esp, 0x3c
││   0x00509b6a      c20800         ret 8
└──> 0x00509b6d      bb10000000     mov ebx, 0x10               ; 16
│   0x00509b72      c744243018..   mov dword [esp + 0x30], 0x18 ; [0x18:4]=-1 ; 24
│   0x00509b7a      895c2434       mov dword [esp + 0x34], ebx
│   0x00509b7e      c744243c01..   mov dword [esp + 0x3c], 1
│   0x00509b86      c744244080..   mov dword [esp + 0x40], 0xffffff80 ; [0xffffff80:4]=-1 ; 4294967168
│   0x00509b8e      c744244480..   mov dword [esp + 0x44], 0x80 ; [0x80:4]=-1 ; 128
│   0x00509b96      beace85400     mov esi, 0x54e8ac
│   0x00509b9b      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509ba1      8b4efc         mov ecx, dword [esi - 4]
│   0x00509ba4      894c2438       mov dword [esp + 0x38], ecx
│   0x00509ba8      8b0495e0b2..   mov eax, dword [edx*4 + 0xa7b2e0]
│   0x00509baf      8d542430       lea edx, [esp + 0x30]
│   0x00509bb3      52             push edx
│   0x00509bb4      6a04           push 4                      ; 4
│   0x00509bb6      8b08           mov ecx, dword [eax]
│   0x00509bb8      50             push eax
│   0x00509bb9      ff5118         call dword [ecx + 0x18]     ; 24
│   0x00509bbc      3bc7           cmp eax, edi
│   0x00509bbe      740d           je 0x509bcd
│   0x00509bc0      3d02000780     cmp eax, 0x80070002
│   0x00509bc5      0f85ca000000   jne 0x509c95
│   0x00509bcb      893e           mov dword [esi], edi
│   0x00509bcd      83c608         add esi, 8
│   0x00509bd0      81feece85400   cmp esi, 0x54e8ec           ; "d not get capabilities for one or more joysticks ."
│   0x00509bd6      7cc3           jl 0x509b9b
│   0x00509bd8      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509bdd      8d54241c       lea edx, [esp + 0x1c]
│   0x00509be1      c744241c14..   mov dword [esp + 0x1c], 0x14 ; [0x14:4]=-1 ; 20
│   0x00509be9      895c2420       mov dword [esp + 0x20], ebx
│   0x00509bed      8b0485e0b2..   mov eax, dword [eax*4 + 0xa7b2e0]
│   0x00509bf4      897c2424       mov dword [esp + 0x24], edi
│   0x00509bf8      897c2428       mov dword [esp + 0x28], edi
│   0x00509bfc      c744242c01..   mov dword [esp + 0x2c], 1
│   0x00509c04      8b08           mov ecx, dword [eax]
│   0x00509c06      52             push edx
│   0x00509c07      6a09           push 9                      ; 9
│   0x00509c09      50             push eax
│   0x00509c0a      ff5118         call dword [ecx + 0x18]     ; 24
│   0x00509c0d      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509c12      8d54241c       lea edx, [esp + 0x1c]
│   0x00509c16      897c2428       mov dword [esp + 0x28], edi
│   0x00509c1a      c744242c10..   mov dword [esp + 0x2c], 0x2710 ; '\x10\''
│                                                              ; [0x2710:4]=-1
│   0x00509c22      8b0485e0b2..   mov eax, dword [eax*4 + 0xa7b2e0]
│   0x00509c29      52             push edx
│   0x00509c2a      6a06           push 6                      ; 6
│   0x00509c2c      50             push eax
│   0x00509c2d      8b08           mov ecx, dword [eax]
│   0x00509c2f      ff5118         call dword [ecx + 0x18]     ; 24
│   0x00509c32      a13cb6a700     mov eax, dword [0xa7b63c]   ; [0xa7b63c:4]=0
│   0x00509c37      c744242801..   mov dword [esp + 0x28], 1
│   0x00509c3f      8944242c       mov dword [esp + 0x2c], eax
│   0x00509c43      bea8e85400     mov esi, 0x54e8a8
│   0x00509c48      397e04         cmp dword [esi + 4], edi
│   0x00509c4b      7424           je 0x509c71
│   0x00509c4d      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509c53      8b0e           mov ecx, dword [esi]
│   0x00509c55      894c2424       mov dword [esp + 0x24], ecx
│   0x00509c59      8b0495e0b2..   mov eax, dword [edx*4 + 0xa7b2e0]
│   0x00509c60      8d54241c       lea edx, [esp + 0x1c]
│   0x00509c64      52             push edx
│   0x00509c65      6a05           push 5                      ; 5
│   0x00509c67      8b08           mov ecx, dword [eax]
│   0x00509c69      50             push eax
│   0x00509c6a      ff5118         call dword [ecx + 0x18]     ; 24
│   0x00509c6d      85c0           test eax, eax
│   0x00509c6f      7524           jne 0x509c95
│   0x00509c71      83c608         add esi, 8
│   0x00509c74      81fee8e85400   cmp esi, str.Could_not_get_capabilities_for_one_or_more_joysticks_. ; 0x54e8e8 ; "Could not get capabilities for one or more joysticks ."
│   0x00509c7a      7ccc           jl 0x509c48
│   0x00509c7c      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509c81      5e             pop esi
│   0x00509c82      40             inc eax
│   0x00509c83      5b             pop ebx
│   0x00509c84      a338b6a700     mov dword [0xa7b638], eax   ; [0xa7b638:4]=0
│   0x00509c89      b801000000     mov eax, 1
│   0x00509c8e      5f             pop edi
│   0x00509c8f      83c43c         add esp, 0x3c
│   0x00509c92      c20800         ret 8
│   0x00509c95      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
│   0x00509c9a      8b0485e0b4..   mov eax, dword [eax*4 + 0xa7b4e0]
│   0x00509ca1      e95bfeffff     jmp 0x509b01
└─> 0x00509ca6      687ce95400     push str.Failed_to_create_device. ; 0x54e97c ; "Failed to create device."
0x00509cab      e8005bfaff     call 0x4af7b0
0x00509cb0      83c404         add esp, 4
0x00509cb3      ebfe           jmp 0x509cb3
0x00509cb5      90             nop
0x00509cb6      90             nop
0x00509cb7      90             nop
0x00509cb8      90             nop
0x00509cb9      90             nop
0x00509cba      90             nop
0x00509cbb      90             nop
0x00509cbc      90             nop
0x00509cbd      90             nop
0x00509cbe      90             nop
0x00509cbf      90             nop
0x00509cc0      8b442404       mov eax, dword [esp + 4]
0x00509cc4      8b0c85e0b4..   mov ecx, dword [eax*4 + 0xa7b4e0]
0x00509ccb      85c9           test ecx, ecx
0x00509ccd      7417           je 0x509ce6
0x00509ccf      8b0485e0b2..   mov eax, dword [eax*4 + 0xa7b2e0]
0x00509cd6      50             push eax
0x00509cd7      8b08           mov ecx, dword [eax]
0x00509cd9      ff511c         call dword [ecx + 0x1c]     ; 28
0x00509cdc      33d2           xor edx, edx
0x00509cde      85c0           test eax, eax
0x00509ce0      0f9dc2         setge dl
0x00509ce3      8bc2           mov eax, edx
0x00509ce5      c3             ret
0x00509ce6      33c0           xor eax, eax
0x00509ce8      c3             ret
0x00509ce9      90             nop
0x00509cea      90             nop
0x00509ceb      90             nop
0x00509cec      90             nop
0x00509ced      90             nop
0x00509cee      90             nop
0x00509cef      90             nop
0x00509cf0      83ec08         sub esp, 8
0x00509cf3      53             push ebx
0x00509cf4      55             push ebp
0x00509cf5      8b2d38b6a700   mov ebp, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509cfb      33db           xor ebx, ebx
0x00509cfd      56             push esi
0x00509cfe      3beb           cmp ebp, ebx
0x00509d00      57             push edi
0x00509d01      895c2414       mov dword [esp + 0x14], ebx
0x00509d05      0f8e80000000   jle 0x509d8b
0x00509d0b      c7442410e0..   mov dword [esp + 0x10], 0xa7b4e0 ; [0xa7b4e0:4]=0
0x00509d13      8b442410       mov eax, dword [esp + 0x10]
0x00509d17      8b00           mov eax, dword [eax]
0x00509d19      85c0           test eax, eax
0x00509d1b      744d           je 0x509d6a
0x00509d1d      8dabf0b2a700   lea ebp, [ebx + 0xa7b2f0]
0x00509d23      8dbbf8b4a700   lea edi, [ebx + 0xa7b4f8]
0x00509d29      b914000000     mov ecx, 0x14               ; 20
0x00509d2e      8bf5           mov esi, ebp
0x00509d30      f3a5           rep movsd dword es:[edi], dword [esi]
0x00509d32      8b08           mov ecx, dword [eax]
0x00509d34      50             push eax
0x00509d35      ff5164         call dword [ecx + 0x64]     ; 100
0x00509d38      8b542410       mov edx, dword [esp + 0x10]
0x00509d3c      55             push ebp
0x00509d3d      6a50           push 0x50                   ; 'P' ; 80
0x00509d3f      8b02           mov eax, dword [edx]
0x00509d41      50             push eax
0x00509d42      8b08           mov ecx, dword [eax]
0x00509d44      ff5124         call dword [ecx + 0x24]     ; 36
0x00509d47      85c0           test eax, eax
0x00509d49      7d1f           jge 0x509d6a
0x00509d4b      3d1e000780     cmp eax, 0x8007001e         ; '\x1e'
0x00509d50      750d           jne 0x509d5f
0x00509d52      8b542414       mov edx, dword [esp + 0x14]
0x00509d56      52             push edx
0x00509d57      e864ffffff     call 0x509cc0
0x00509d5c      83c404         add esp, 4
0x00509d5f      b914000000     mov ecx, 0x14               ; 20
0x00509d64      33c0           xor eax, eax
0x00509d66      8bfd           mov edi, ebp
0x00509d68      f3ab           rep stosd dword es:[edi], eax
0x00509d6a      8b442414       mov eax, dword [esp + 0x14]
0x00509d6e      8b742410       mov esi, dword [esp + 0x10]
0x00509d72      8b2d38b6a700   mov ebp, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509d78      40             inc eax
0x00509d79      83c604         add esi, 4
0x00509d7c      83c350         add ebx, 0x50               ; 80
0x00509d7f      3bc5           cmp eax, ebp
0x00509d81      89442414       mov dword [esp + 0x14], eax
0x00509d85      89742410       mov dword [esp + 0x10], esi
0x00509d89      7c88           jl 0x509d13
0x00509d8b      85ed           test ebp, ebp
0x00509d8d      0f8ef1000000   jle 0x509e84
0x00509d93      bff0b2a700     mov edi, 0xa7b2f0
0x00509d98      ba20aabc00     mov edx, 0xbcaa20
0x00509d9d      8b02           mov eax, dword [edx]
0x00509d9f      33f6           xor esi, esi
0x00509da1      894204         mov dword [edx + 4], eax
0x00509da4      c70200000000   mov dword [edx], 0
0x00509daa      8b1a           mov ebx, dword [edx]
0x00509dac      8a4c3730       mov cl, byte [edi + esi + 0x30]
0x00509db0      33c0           xor eax, eax
0x00509db2      80e180         and cl, 0x80                ; 128
0x00509db5      80f980         cmp cl, 0x80                ; 128
0x00509db8      8d4e04         lea ecx, [esi + 4]
0x00509dbb      0f94c0         sete al
0x00509dbe      d3e0           shl eax, cl
0x00509dc0      0bd8           or ebx, eax
0x00509dc2      46             inc esi
0x00509dc3      83fe20         cmp esi, 0x20               ; 32
0x00509dc6      7ce4           jl 0x509dac
0x00509dc8      891a           mov dword [edx], ebx
0x00509dca      8a42e4         mov al, byte [edx - 0x1c]
0x00509dcd      84c0           test al, al
0x00509dcf      7429           je 0x509dfa
0x00509dd1      8b07           mov eax, dword [edi]
0x00509dd3      85c0           test eax, eax
0x00509dd5      7d06           jge 0x509ddd
0x00509dd7      8bc3           mov eax, ebx
0x00509dd9      0c01           or al, 1
0x00509ddb      eb06           jmp 0x509de3
0x00509ddd      7e06           jle 0x509de5
0x00509ddf      8b02           mov eax, dword [edx]
0x00509de1      0c02           or al, 2
0x00509de3      8902           mov dword [edx], eax
0x00509de5      8b4704         mov eax, dword [edi + 4]
0x00509de8      85c0           test eax, eax
0x00509dea      7d06           jge 0x509df2
0x00509dec      8b02           mov eax, dword [edx]
0x00509dee      0c04           or al, 4
0x00509df0      eb06           jmp 0x509df8
0x00509df2      7e06           jle 0x509dfa
0x00509df4      8b02           mov eax, dword [edx]
0x00509df6      0c08           or al, 8
0x00509df8      8902           mov dword [edx], eax
0x00509dfa      8a42e5         mov al, byte [edx - 0x1b]
0x00509dfd      84c0           test al, al
0x00509dff      740f           je 0x509e10
0x00509e01      833a00         cmp dword [edx], 0
0x00509e04      7504           jne 0x509e0a
0x00509e06      c642e500       mov byte [edx - 0x1b], 0
0x00509e0a      c70200000000   mov dword [edx], 0
0x00509e10      db07           fild dword [edi]
0x00509e12      8b4f14         mov ecx, dword [edi + 0x14]
0x00509e15      894c2414       mov dword [esp + 0x14], ecx
0x00509e19      dc0db04b5300   fmul qword [0x534bb0]       ; [0x534bb0:8]=0x3f80000000000000
0x00509e1f      d95ae8         fstp dword [edx - 0x18]
0x00509e22      db4704         fild dword [edi + 4]
0x00509e25      dc0db04b5300   fmul qword [0x534bb0]       ; [0x534bb0:8]=0x3f80000000000000
0x00509e2b      d95aec         fstp dword [edx - 0x14]
0x00509e2e      db442414       fild dword [esp + 0x14]
0x00509e32      d81dec335300   fcomp dword [0x5333ec]
0x00509e38      dfe0           fnstsw ax
0x00509e3a      f6c441         test ah, 0x41               ; 65
0x00509e3d      7516           jne 0x509e55
0x00509e3f      db442414       fild dword [esp + 0x14]
0x00509e43      c742f80000..   mov dword [edx - 8], 0
0x00509e4a      dc0db04b5300   fmul qword [0x534bb0]       ; [0x534bb0:8]=0x3f80000000000000
0x00509e50      d95afc         fstp dword [edx - 4]
0x00509e53      eb16           jmp 0x509e6b
0x00509e55      db442414       fild dword [esp + 0x14]
0x00509e59      dc0db04b5300   fmul qword [0x534bb0]       ; [0x534bb0:8]=0x3f80000000000000
0x00509e5f      d9e0           fchs
0x00509e61      d95af8         fstp dword [edx - 8]
0x00509e64      c742fc0000..   mov dword [edx - 4], 0
0x00509e6b      db4718         fild dword [edi + 0x18]
0x00509e6e      83c750         add edi, 0x50               ; 80
0x00509e71      83c228         add edx, 0x28               ; 40
0x00509e74      4d             dec ebp
0x00509e75      d95ac8         fstp dword [edx - 0x38]
0x00509e78      db47cc         fild dword [edi - 0x34]
0x00509e7b      d95acc         fstp dword [edx - 0x34]
0x00509e7e      0f8519ffffff   jne 0x509d9d
0x00509e84      5f             pop edi
0x00509e85      5e             pop esi
0x00509e86      5d             pop ebp
0x00509e87      5b             pop ebx
0x00509e88      83c408         add esp, 8
0x00509e8b      c3             ret
0x00509e8c      90             nop
0x00509e8d      90             nop
0x00509e8e      90             nop
0x00509e8f      90             nop
0x00509e90      a1f0b4a700     mov eax, dword [0xa7b4f0]   ; [0xa7b4f0:4]=0
0x00509e95      85c0           test eax, eax
0x00509e97      745c           je 0x509ef5
0x00509e99      8b0d38b6a700   mov ecx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509e9f      57             push edi
0x00509ea0      33ff           xor edi, edi
0x00509ea2      85c9           test ecx, ecx
0x00509ea4      7e33           jle 0x509ed9
0x00509ea6      56             push esi
0x00509ea7      bee0b4a700     mov esi, 0xa7b4e0
0x00509eac      8b06           mov eax, dword [esi]
0x00509eae      85c0           test eax, eax
0x00509eb0      7414           je 0x509ec6
0x00509eb2      8b08           mov ecx, dword [eax]
0x00509eb4      50             push eax
0x00509eb5      ff5120         call dword [ecx + 0x20]     ; 32
0x00509eb8      8b06           mov eax, dword [esi]
0x00509eba      50             push eax
0x00509ebb      8b10           mov edx, dword [eax]
0x00509ebd      ff5208         call dword [edx + 8]        ; 8
0x00509ec0      c70600000000   mov dword [esi], 0
0x00509ec6      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509ecb      47             inc edi
0x00509ecc      83c604         add esi, 4
0x00509ecf      3bf8           cmp edi, eax
0x00509ed1      7cd9           jl 0x509eac
0x00509ed3      a1f0b4a700     mov eax, dword [0xa7b4f0]   ; [0xa7b4f0:4]=0
0x00509ed8      5e             pop esi
0x00509ed9      8b08           mov ecx, dword [eax]
0x00509edb      50             push eax
0x00509edc      ff5108         call dword [ecx + 8]        ; 8
0x00509edf      c705f0b4a7..   mov dword [0xa7b4f0], 0     ; [0xa7b4f0:4]=0
0x00509ee9      c70538b6a7..   mov dword [0xa7b638], 0     ; [0xa7b638:4]=0
0x00509ef3      5f             pop edi
0x00509ef4      c3             ret
0x00509ef5      c70538b6a7..   mov dword [0xa7b638], 0     ; [0xa7b638:4]=0
0x00509eff      c3             ret
0x00509f00      8b4c2404       mov ecx, dword [esp + 4]
0x00509f04      8b148de0b4..   mov edx, dword [ecx*4 + 0xa7b4e0]
0x00509f0b      8d0489         lea eax, [ecx + ecx*4]
0x00509f0e      c1e003         shl eax, 3
0x00509f11      85d2           test edx, edx
0x00509f13      0f95c2         setne dl
0x00509f16      889000aabc00   mov byte [eax + 0xbcaa00], dl ; [0xbcaa00:1]=0
0x00509f1c      b201           mov dl, 1
0x00509f1e      889001aabc00   mov byte [eax + 0xbcaa01], dl ; [0xbcaa01:1]=0
0x00509f24      889003aabc00   mov byte [eax + 0xbcaa03], dl ; [0xbcaa03:1]=0
0x00509f2a      889004aabc00   mov byte [eax + 0xbcaa04], dl ; [0xbcaa04:1]=0
0x00509f30      8d1489         lea edx, [ecx + ecx*4]
0x00509f33      8d0c51         lea ecx, [ecx + edx*2]
0x00509f36      8a148d40b4..   mov dl, byte [ecx*4 + 0xa7b440]
0x00509f3d      889007aabc00   mov byte [eax + 0xbcaa07], dl ; [0xbcaa07:1]=0
0x00509f43      c3             ret
0x00509f44      90             nop
0x00509f45      90             nop
0x00509f46      90             nop
0x00509f47      90             nop
0x00509f48      90             nop
0x00509f49      90             nop
0x00509f4a      90             nop
0x00509f4b      90             nop
0x00509f4c      90             nop
0x00509f4d      90             nop
0x00509f4e      90             nop
0x00509f4f      90             nop
0x00509f50      56             push esi
0x00509f51      e82a72faff     call 0x4b1180
0x00509f56      8bf0           mov esi, eax
0x00509f58      68509f5000     push 0x509f50
0x00509f5d      56             push esi
0x00509f5e      e87d7cfaff     call 0x4b1be0
0x00509f63      6880174c00     push 0x4c1780
0x00509f68      56             push esi
0x00509f69      e8927cfaff     call 0x4b1c00
0x00509f6e      6870a35000     push 0x50a370
0x00509f73      56             push esi
0x00509f74      e8a77cfaff     call 0x4b1c20
0x00509f79      6830a45000     push 0x50a430
0x00509f7e      56             push esi
0x00509f7f      e8bc7cfaff     call 0x4b1c40
0x00509f84      6850a45000     push 0x50a450
0x00509f89      56             push esi
0x00509f8a      e8d17cfaff     call 0x4b1c60
0x00509f8f      6890a45000     push 0x50a490
0x00509f94      56             push esi
0x00509f95      e8e67cfaff     call 0x4b1c80
0x00509f9a      68d0a45000     push 0x50a4d0
0x00509f9f      56             push esi
0x00509fa0      e8fb7cfaff     call 0x4b1ca0
0x00509fa5      6850a55000     push 0x50a550
0x00509faa      56             push esi
0x00509fab      e8107dfaff     call 0x4b1cc0
0x00509fb0      83c440         add esp, 0x40
0x00509fb3      6810a55000     push 0x50a510
0x00509fb8      56             push esi
0x00509fb9      e8227dfaff     call 0x4b1ce0
0x00509fbe      6870a55000     push 0x50a570
0x00509fc3      56             push esi
0x00509fc4      e8777dfaff     call 0x4b1d40
0x00509fc9      68a0a55000     push 0x50a5a0
0x00509fce      56             push esi
0x00509fcf      e84c7dfaff     call 0x4b1d20
0x00509fd4      68b0a55000     push 0x50a5b0
0x00509fd9      56             push esi
0x00509fda      e8217dfaff     call 0x4b1d00
0x00509fdf      68c0a55000     push 0x50a5c0
0x00509fe4      56             push esi
0x00509fe5      e8767dfaff     call 0x4b1d60
0x00509fea      6850a05000     push 0x50a050
0x00509fef      56             push esi
0x00509ff0      e8ab7dfaff     call 0x4b1da0
0x00509ff5      68d0a55000     push 0x50a5d0
0x00509ffa      56             push esi
0x00509ffb      e8c07dfaff     call 0x4b1dc0
0x0050a000      6840a05000     push 0x50a040
0x0050a005      56             push esi
0x0050a006      e8757dfaff     call 0x4b1d80
0x0050a00b      8b442448       mov eax, dword [esp + 0x48]
0x0050a00f      83c440         add esp, 0x40
0x0050a012      50             push eax
0x0050a013      56             push esi
0x0050a014      e87772faff     call 0x4b1290
0x0050a019      83c408         add esp, 8
0x0050a01c      b8e8b6a700     mov eax, 0xa7b6e8
0x0050a021      c70000000000   mov dword [eax], 0
0x0050a027      05ac000000     add eax, 0xac               ; 172
0x0050a02c      3da8c1a700     cmp eax, 0xa7c1a8
0x0050a031      7cee           jl 0x50a021
0x0050a033      8bc6           mov eax, esi
0x0050a035      5e             pop esi
0x0050a036      c3             ret
0x0050a037      90             nop
0x0050a038      90             nop
0x0050a039      90             nop
0x0050a03a      90             nop
0x0050a03b      90             nop
0x0050a03c      90             nop
0x0050a03d      90             nop
0x0050a03e      90             nop
0x0050a03f      90             nop
0x0050a040      8b442408       mov eax, dword [esp + 8]
0x0050a044      50             push eax
0x0050a045      e81dcc0000     call 0x516c67
0x0050a04a      59             pop ecx
0x0050a04b      c3             ret
0x0050a04c      90             nop
0x0050a04d      90             nop
0x0050a04e      90             nop
0x0050a04f      90             nop
0x0050a050      81ec58030000   sub esp, 0x358
0x0050a056      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a059      33c0           xor eax, eax
0x0050a05b      8d94245402..   lea edx, [esp + 0x254]
0x0050a062      53             push ebx
0x0050a063      55             push ebp
0x0050a064      8bac246803..   mov ebp, dword [esp + 0x368]
0x0050a06b      56             push esi
0x0050a06c      57             push edi
0x0050a06d      8bfd           mov edi, ebp
0x0050a06f      f2ae           repne scasb al, byte es:[edi]
0x0050a071      f7d1           not ecx
0x0050a073      2bf9           sub edi, ecx
0x0050a075      8bc1           mov eax, ecx
0x0050a077      8bf7           mov esi, edi
0x0050a079      8bfa           mov edi, edx
0x0050a07b      c1e902         shr ecx, 2
0x0050a07e      f3a5           rep movsd dword es:[edi], dword [esi]
0x0050a080      8bc8           mov ecx, eax
0x0050a082      33c0           xor eax, eax
0x0050a084      83e103         and ecx, 3
0x0050a087      f3a4           rep movsb byte es:[edi], byte [esi]
0x0050a089      8bfd           mov edi, ebp
0x0050a08b      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a08e      f2ae           repne scasb al, byte es:[edi]
0x0050a090      f7d1           not ecx
0x0050a092      49             dec ecx
0x0050a093      807c29ff2f     cmp byte [ecx + ebp - 1], 0x2f ; '/'
0x0050a098      7438           je 0x50a0d2
0x0050a09a      bfb8e95400     mov edi, 0x54e9b8           ; "/"
0x0050a09f      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a0a2      33c0           xor eax, eax
0x0050a0a4      8d94246402..   lea edx, [esp + 0x264]
0x0050a0ab      f2ae           repne scasb al, byte es:[edi]
0x0050a0ad      f7d1           not ecx
0x0050a0af      2bf9           sub edi, ecx
0x0050a0b1      8bf7           mov esi, edi
0x0050a0b3      8bd9           mov ebx, ecx
0x0050a0b5      8bfa           mov edi, edx
0x0050a0b7      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a0ba      f2ae           repne scasb al, byte es:[edi]
0x0050a0bc      8bcb           mov ecx, ebx
0x0050a0be      4f             dec edi
0x0050a0bf      c1e902         shr ecx, 2
0x0050a0c2      f3a5           rep movsd dword es:[edi], dword [esi]
0x0050a0c4      8bcb           mov ecx, ebx
0x0050a0c6      bbb8e95400     mov ebx, 0x54e9b8           ; "/"
0x0050a0cb      83e103         and ecx, 3
0x0050a0ce      f3a4           rep movsb byte es:[edi], byte [esi]
0x0050a0d0      eb05           jmp 0x50a0d7
0x0050a0d2      bb747c5600     mov ebx, 0x567c74           ; 't|V'
0x0050a0d7      bfb4e95400     mov edi, 0x54e9b4           ; "*.*"
0x0050a0dc      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a0df      33c0           xor eax, eax
0x0050a0e1      8d94246402..   lea edx, [esp + 0x264]
0x0050a0e8      f2ae           repne scasb al, byte es:[edi]
0x0050a0ea      f7d1           not ecx
0x0050a0ec      2bf9           sub edi, ecx
0x0050a0ee      8bf7           mov esi, edi
0x0050a0f0      8bfa           mov edi, edx
0x0050a0f2      8bd1           mov edx, ecx
0x0050a0f4      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a0f7      f2ae           repne scasb al, byte es:[edi]
0x0050a0f9      8bca           mov ecx, edx
0x0050a0fb      4f             dec edi
0x0050a0fc      c1e902         shr ecx, 2
0x0050a0ff      f3a5           rep movsd dword es:[edi], dword [esi]
0x0050a101      8bca           mov ecx, edx
0x0050a103      8d84242401..   lea eax, [esp + 0x124]
0x0050a10a      83e103         and ecx, 3
0x0050a10d      50             push eax
0x0050a10e      f3a4           rep movsb byte es:[edi], byte [esi]
0x0050a110      8d8c246802..   lea ecx, [esp + 0x268]
0x0050a117      51             push ecx
0x0050a118      ff1548315300   call dword [sym.imp.KERNEL32.dll_FindFirstFileA] ; 0x533148 ; HANDLE FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
0x0050a11e      8d542420       lea edx, [esp + 0x20]
0x0050a122      83f8ff         cmp eax, 0xffffffff
0x0050a125      89442410       mov dword [esp + 0x10], eax
0x0050a129      89542414       mov dword [esp + 0x14], edx
0x0050a12d      0f8427020000   je 0x50a35a
0x0050a133      8b84246c03..   mov eax, dword [esp + 0x36c]
0x0050a13a      50             push eax
0x0050a13b      e8a07cfaff     call 0x4b1de0
0x0050a140      8bf8           mov edi, eax
0x0050a142      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a145      33c0           xor eax, eax
0x0050a147      83c404         add esp, 4
0x0050a14a      f2ae           repne scasb al, byte es:[edi]
0x0050a14c      f7d1           not ecx
0x0050a14e      2bf9           sub edi, ecx
0x0050a150      8d542420       lea edx, [esp + 0x20]
0x0050a154      8bc1           mov eax, ecx
0x0050a156      8bf7           mov esi, edi
0x0050a158      c1e902         shr ecx, 2
0x0050a15b      8bfa           mov edi, edx
0x0050a15d      8d542420       lea edx, [esp + 0x20]
0x0050a161      f3a5           rep movsd dword es:[edi], dword [esi]
0x0050a163      8bc8           mov ecx, eax
0x0050a165      33c0           xor eax, eax
0x0050a167      83e103         and ecx, 3
0x0050a16a      f3a4           rep movsb byte es:[edi], byte [esi]
0x0050a16c      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a16f      8bfd           mov edi, ebp
0x0050a171      f2ae           repne scasb al, byte es:[edi]
0x0050a173      f7d1           not ecx
0x0050a175      2bf9           sub edi, ecx
0x0050a177      8bf7           mov esi, edi
0x0050a179      8bfa           mov edi, edx
0x0050a17b      8bd1           mov edx, ecx
0x0050a17d      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a180      f2ae           repne scasb al, byte es:[edi]
0x0050a182      8bca           mov ecx, edx
0x0050a184      4f             dec edi
0x0050a185      c1e902         shr ecx, 2
0x0050a188      f3a5           rep movsd dword es:[edi], dword [esi]
0x0050a18a      8bca           mov ecx, edx
0x0050a18c      8d542420       lea edx, [esp + 0x20]
0x0050a190      83e103         and ecx, 3
0x0050a193      f3a4           rep movsb byte es:[edi], byte [esi]
0x0050a195      8bfb           mov edi, ebx
0x0050a197      83c9ff         or ecx, 0xffffffff          ; -1
0x0050a19a      f2ae           repne scasb al, byte es:[edi]
0x0050a19c      f7d1           not ecx
0x0050a19e      2bf9           sub edi, ecx
0x0050a1a0      8bf7           mov esi, edi
0x0050a1a2      8bfa           mov edi, edx
0x0050a1a4      8bd1           mov edx, ecx
```

### 1.1 DIDEVICEINSTANCEA fields it reads

- At `0x509A43`: `eax = dword [esp+0x40]` = argument 1; `0x509A48: add eax, 4`; then four dword copies from `eax+0`, `eax+4`, `eax+8`, `eax+0xC` into the 16-byte stack local. The only field read is **`guidInstance`, at byte offset +4, 16 bytes**. It never reads `dwSize` (+0) or `dwDevType` (+8).
- The guid passed to `CreateDevice` is the address of that 16-byte stack copy (`lea edx,[esp+0xC]` at 0x509A7E after the `push edi`), not the instance pointer itself.

### 1.2 Call sequence with exact argument order

1. `push edi` (edi = 0) - punkOuter for CreateDevice (0x509A4D).
2. Copy guidInstance 16 bytes to stack (0x509A4E-0x509A6A).
3. `mov eax,[0x00A7B4F0]` (IDirectInputA*), `edx = [0x00A7B638]`, `lea edx,[edx*4+0x00A7B2E0]`, `push edx` (ppDevice slot), `lea edx,[esp+0xC]` (guid copy), `push edx`, `push eax` (this), `call [ecx+0x0C]` = **CreateDevice(this, &guid, &g_A7B2E0[count], 0)**. Return < 0 goes to 0x509CA6.
4. `ecx=[0x00A7B638]`; `eax=[ecx*4+0x00A7B2E0]`; `lea ecx,[ecx*4+0x00A7B4E0]`; `push ecx` (ppv), `push 0x005349C0` (riid), `push eax` (device), `call [edx]` = **QueryInterface(device, IID 0x5349C0, &g_A7B4E0[count])** (vtable slot 0).
5. `eax=[0x00A7B638]; eax=[eax*4+0x00A7B2E0]; push eax; call [ecx+8]` = **Release(raw device)** - always runs, before the QueryInterface result is tested.
6. If QueryInterface result < 0: `Debug_Error("Failed to obtain interface.")` then `jmp $` (0x509AD6).
7. `push 0x00534C38` (DIDATAFORMAT), `eax=[0x00A7B4E0[count]]`, `push eax`, `call [ecx+0x2C]` = **SetDataFormat(device, &format 0x534C38)**. If < 0: `Release(device)`; `g_A7B4E0[count] = 0`; `return 1` (0x509B04-0x509B1F).
8. `push 5`, `call 0x0048F0A0` (GetWindowHandle), `push eax`, `esi=[0x00A7B4E0[count]]`, `push esi`, `call [ebx+0x34]` = **SetCooperativeLevel(device, hWnd, 5)**. If < 0: `Release(device)`; `g_A7B4E0[count] = 0`; `return 1` (0x509B40-0x509B6A).
9. DIPROP_RANGE loop over the 8 (offset, value) pairs at `0x0054E8A8`: struct `{0x18,0x10, dwObj=pair_offset, dwHow=1, lMin=0xFFFFFF80, lMax=0x00000080}`; `push &struct; push 4; push [0xA7B2E0[count]]; call [ecx+0x18]` = **SetProperty(prop 4, device=g_A7B2E0[count], &range)**. On return 0 continue; on return `0x80070002` write 0 to the pair value slot (`mov [esi],edi`, i.e. table[2*i+1]=0) and continue; on any other return jump to the release/null/return-1 path.
10. DIPROPDWORD `{0x14,0x10,0,0,1}` + **SetProperty(prop 9, device=g_A7B2E0[count], &pdw)** (return value ignored).
11. Same struct with dwData=`0x2710` + **SetProperty(prop 6, device=g_A7B2E0[count], &pdw)** (return value ignored).
12. Deadzone loop over the same 8 pairs: skip pairs whose value slot is 0; struct `{0x14,0x10, dwObj=pair_offset, dwHow=1, dwData=dword [0x00A7B63C]}` + **SetProperty(prop 5, device=g_A7B2E0[count], &pdw)**; any nonzero return on a non-skipped pair goes to the release/null/return-1 path (0x509C95 -> 0x509B01).
13. `++g_A7B638`; `return 1` (`ret 8`). Note steps 9-12 use the **raw `g_A7B2E0[count]`** pointer, while steps 7-8 use the QueryInterface result in **`g_A7B4E0[count]`**.

Failure-path order, exactly as blocks fall:

- CreateDevice < 0: 0x509A89 -> 0x509CA6 `Debug_Error("Failed to create device.")` -> `jmp $` at 0x509CB3. No release, no slot writes, no return.
- QueryInterface < 0 (after raw Release): 0x509AC7 -> 0x509AC9 push string -> 0x509ACE call Debug_Error -> `jmp $` at 0x509AD6. Raw device was released; `g_A7B4E0[count]` may hold the failed result (not cleared).
- SetDataFormat < 0: Release(QI device) -> `g_A7B4E0[count]=0` -> return 1. `g_A7B2E0[count]` is not cleared.
- SetCooperativeLevel < 0: Release(QI device) -> `g_A7B4E0[count]=0` -> return 1.
- DIPROP_RANGE return != 0 and != 0x80070002: 0x509C95 loads `g_A7B4E0[count]` then jumps to 0x509B01 = Release -> `g_A7B4E0[count]=0` -> return 1.
- DIPROP_DEADZONE return != 0 on a non-skipped pair: same jump to 0x509C95 -> same release/null/return 1.

### 1.3 Literal strings used by this native (byte-exact)

| VA | file offset | hex bytes | text |
|---|---|---|---|
| `0x54e97c` | `0x14e97c` | `4661696c656420746f20637265617465206465766963652e` | "Failed to create device." |
| `0x54e998` | `0x14e998` | `4661696c656420746f206f627461696e20696e746572666163652e` | "Failed to obtain interface." |

Both are `.data` image strings (VA >= 0x536000). They are referenced by `push imm32` at 0x509CAA and 0x509AC9.

### 1.4 Axis-pair table 0x0054E8A8..0x0054E8E7 (`.data`, writable, image-init)

- File offset `0x0014E8A8`; 64 bytes; 8 pairs of (dword axis offset, dword value=1):

```
0c000000010000001000000001000000140000000100000000000000010000000400000001000000080000000100000018000000010000001c00000001000000
```

- Decoded: `(0x0C,1) (0x10,1) (0x14,1) (0x00,1) (0x04,1) (0x08,1) (0x18,1) (0x1C,1)`; the next image byte at `0x0054E8E8` is the first byte of the string "Could not get capabilities for one or more joysticks ." (that is why the loop bounds compare against 0x0054E8EC / 0x0054E8E8).
- The RANGE loop consumes only the pair offsets (first dword of each pair). The value dword is a support flag: it is cleared to 0 when RANGE returns `0x80070002`, and the DEADZONE loop then skips those axes. Because the writes target `.data`, a port must keep this table in writable storage with exactly these initial bytes.

### 1.5 DIPROP struct layouts used

- RANGE (prop 4): `{ DWORD dwSize=0x18; DWORD dwHeaderSize=0x10; DWORD dwObj=axis_offset; DWORD dwHow=1; LONG lMin=0xFFFFFF80; LONG lMax=0x00000080; }` - 0x18 bytes.
- DWORD (props 9, 6, 5): `{ DWORD dwSize=0x14; DWORD dwHeaderSize=0x10; DWORD dwObj; DWORD dwHow; DWORD dwData; }` - 0x14 bytes. Prop 9: dwObj=0, dwHow=0, dwData=1. Prop 6: dwObj=0, dwHow=0, dwData=0x2710. Prop 5: dwObj=axis_offset, dwHow=1, dwData=`[0x00A7B63C]` (loaded at 0x509C32).
- Numeric ids are authoritative; against dinput.h these are 4=DIPROP_RANGE, 9=DIPROP_AUTOCENTER, 6=DIPROP_SATURATION, 5=DIPROP_DEADZONE.

### 1.6 Paste-ready body

```cpp
// Native: Joypad_EnumDevicesCallback_509A40 (placeholder until VERIFIED; BN alias Physics_SolveConstraints_9 is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509A40-0x00509CB4.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 6.2.0 bytes/disassembly; 629 bytes, 22 blocks, __stdcall with ret 8.
// Behavior/CFG: EnumDevicesA callback; copies lpddi guidInstance (+4, 16 bytes), CreateDevice on g_A7B4F0 into g_A7B2E0[count],
//           QueryInterface(IID 0x005349C0) into g_A7B4E0[count] and Releases the raw device, SetDataFormat(0x00534C38),
//           SetCooperativeLevel(hWnd,5), DIPROP_RANGE over the 8 pairs at 0x0054E8A8 (clearing a pair on 0x80070002),
//           DIPROP_AUTOCENTER=1, DIPROP_SATURATION=0x2710, DIPROP_DEADZONE=dword[0x00A7B63C] skipping cleared pairs,
//           then ++g_A7B638 and return 1. Device failures release + null the g_A7B4E0 slot and return 1;
//           CreateDevice/QueryInterface failures call Debug_Error and spin forever.
// Status/limits: TRANSCRIBED; deps 0x0048F0A0, 0x004AF7B0 and the landed runtime-data block; image tables 0x005349C0/0x00534C38/0x0054E8A8 must exist once. Runtime UNOBSERVED.
#include "igi/retail/core/native_0048f0e0.hpp"
#include "igi/retail/core/retail_error.hpp"
#include <cstdint>

extern "C" {

// Declared once by the landed runtime-data block:
void* g_a7b4f0;
std::int32_t g_a7b638;
void* g_a7b2e0[16];
void* g_a7b4e0[16];
std::int32_t g_a7b63c;

// Image data: the only three constants this native needs (byte-exact, file offset = VA - 0x400000).
static std::uint8_t const g_iid_idirectinputdevice_5349c0[16] = {
    0x82, 0xE6, 0x44, 0x59, 0x2E, 0xC9, 0xCF, 0x11, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
static std::uint8_t const g_format_joystick_534c38[0x18] = {
    0x18, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static std::int32_t g_axis_pairs_54e8a8[16] = {
    0x0C, 1, 0x10, 1, 0x14, 1, 0x00, 1, 0x04, 1, 0x08, 1, 0x18, 1, 0x1C, 1}; // writable: values are cleared below

std::int32_t __stdcall Joypad_EnumDevicesCallback_509A40(void* device_instance, void* unused)
{
    (void)unused; // retail never reads the pvRef argument
    std::uint32_t guid[4];
    std::uint32_t const* src = reinterpret_cast<std::uint32_t const*>(static_cast<std::uint8_t const*>(device_instance) + 4);
    guid[0] = src[0]; // 0x00509A4E: dword copy of lpddi->guidInstance (4 x dword)
    guid[1] = src[1];
    guid[2] = src[2];
    guid[3] = src[3];

    std::int32_t count = g_a7b638;
    void* const io = g_a7b4f0;
    void* const* const io_vtable = *reinterpret_cast<void* const* const*>(io);
    typedef std::int32_t (__stdcall* CreateDeviceFn)(void*, void const*, void**, void*);
    std::int32_t const create_hr = reinterpret_cast<CreateDeviceFn>(io_vtable[3])(io, guid, &g_a7b2e0[count], 0); // [ecx+0xC]
    if (create_hr < 0)
    {
        Debug_Error("Failed to create device."); // 0x00509CAA
        for (;;)
        {
        }
    }

    count = g_a7b638;
    void* const raw_device = g_a7b2e0[count];
    void* const* const raw_vtable = *reinterpret_cast<void* const* const*>(raw_device);
    typedef std::int32_t (__stdcall* QueryInterfaceFn)(void*, void const*, void**);
    typedef std::int32_t (__stdcall* ReleaseFn)(void*);
    std::int32_t const qi_hr = reinterpret_cast<QueryInterfaceFn>(raw_vtable[0])(raw_device, g_iid_idirectinputdevice_5349c0, &g_a7b4e0[count]); // [edx]
    reinterpret_cast<ReleaseFn>(raw_vtable[2])(raw_device); // 0x00509AC2: release raw device before testing the QI result
    if (qi_hr < 0)
    {
        Debug_Error("Failed to obtain interface."); // 0x00509AC9
        for (;;)
        {
        }
    }

    count = g_a7b638;
    void* const device = g_a7b4e0[count];
    void* const* const device_vtable = *reinterpret_cast<void* const* const*>(device);
    typedef std::int32_t (__stdcall* SetDataFormatFn)(void*, void const*);
    if (reinterpret_cast<SetDataFormatFn>(device_vtable[0x2C / 4])(device, g_format_joystick_534c38) < 0) // [ecx+0x2C]
    {
        reinterpret_cast<ReleaseFn>(device_vtable[2])(device); // 0x00509B04
        g_a7b4e0[g_a7b638] = 0;                              // 0x00509B14 (0x509B14 is inside this native)
        return 1;
    }

    count = g_a7b638;
    void* const coop_device = g_a7b4e0[count];
    void* const* const coop_vtable = *reinterpret_cast<void* const* const*>(coop_device);
    typedef std::int32_t (__stdcall* SetCooperativeLevelFn)(void*, void*, std::uint32_t);
    if (reinterpret_cast<SetCooperativeLevelFn>(coop_vtable[0x34 / 4])(coop_device, GetWindowHandle_48F0A0(), 5) < 0) // [ebx+0x34]
    {
        reinterpret_cast<ReleaseFn>(coop_vtable[2])(coop_device); // 0x00509B50
        g_a7b4e0[g_a7b638] = 0;                                 // 0x00509B5A
        return 1;
    }

    typedef std::int32_t (__stdcall* SetPropertyFn)(void*, std::uint32_t, void const*);
    std::uint32_t range[6];
    range[0] = 0x18;      // dwSize
    range[1] = 0x10;      // dwHeaderSize
    range[3] = 1;         // dwHow = DIPH_BYOFFSET
    range[4] = 0xFFFFFF80; // lMin -128
    range[5] = 0x00000080; // lMax  128
    for (std::int32_t pair = 0; pair < 8; ++pair)
    {
        range[2] = static_cast<std::uint32_t>(g_axis_pairs_54e8a8[pair * 2]); // dwObj = axis offset
        count = g_a7b638;
        void* const prop_device = g_a7b2e0[count]; // property calls use the raw device slot
        void* const* const prop_vtable = *reinterpret_cast<void* const* const*>(prop_device);
        std::int32_t const prop_hr = reinterpret_cast<SetPropertyFn>(prop_vtable[0x18 / 4])(prop_device, 4, range); // [ecx+0x18]
        if (prop_hr != 0)
        {
            if (prop_hr != 0x80070002) // DIERR_NOTFOUND is expected for axes without range support
            {
                count = g_a7b638;
                void* const fail_device = g_a7b4e0[count];
                void* const* const fail_vtable = *reinterpret_cast<void* const* const*>(fail_device);
                reinterpret_cast<ReleaseFn>(fail_vtable[2])(fail_device); // 0x509C95 -> 0x509B01
                g_a7b4e0[g_a7b638] = 0;
                return 1;
            }
            g_axis_pairs_54e8a8[pair * 2 + 1] = 0; // 0x00509BCB: clear the support flag in the writable table
        }
    }

    std::uint32_t pdw[5];
    pdw[0] = 0x14; // dwSize
    pdw[1] = 0x10; // dwHeaderSize
    pdw[2] = 0;    // dwObj
    pdw[3] = 0;    // dwHow = DIPH_DEVICE
    pdw[4] = 1;    // dwData (DIPROP_AUTOCENTER)
    count = g_a7b638;
    void* const ac_device = g_a7b2e0[count];
    reinterpret_cast<SetPropertyFn>((*reinterpret_cast<void* const* const*>(ac_device))[0x18 / 4])(ac_device, 9, pdw); // [ecx+0x18], result ignored

    pdw[4] = 0x2710; // DIPROP_SATURATION = 10000
    count = g_a7b638;
    void* const sat_device = g_a7b2e0[count];
    reinterpret_cast<SetPropertyFn>((*reinterpret_cast<void* const* const*>(sat_device))[0x18 / 4])(sat_device, 6, pdw); // result ignored

    pdw[3] = 1;              // dwHow = DIPH_BYOFFSET
    pdw[4] = static_cast<std::uint32_t>(g_a7b63c); // dwData = [0x00A7B63C]
    for (std::int32_t pair = 0; pair < 8; ++pair)
    {
        if (g_axis_pairs_54e8a8[pair * 2 + 1] == 0) // skip axes whose RANGE was not found
        {
            continue;
        }
        pdw[2] = static_cast<std::uint32_t>(g_axis_pairs_54e8a8[pair * 2]);
        count = g_a7b638;
        void* const dz_device = g_a7b2e0[count];
        void* const* const dz_vtable = *reinterpret_cast<void* const* const*>(dz_device);
        if (reinterpret_cast<SetPropertyFn>(dz_vtable[0x18 / 4])(dz_device, 5, pdw) != 0) // [ecx+0x18]
        {
            count = g_a7b638;
            void* const fail_device = g_a7b4e0[count];
            void* const* const fail_vtable = *reinterpret_cast<void* const* const*>(fail_device);
            reinterpret_cast<ReleaseFn>(fail_vtable[2])(fail_device);
            g_a7b4e0[g_a7b638] = 0;
            return 1;
        }
    }

    g_a7b638 = g_a7b638 + 1; // 0x00509C84
    return 1;
}

} // extern "C"
```

## 2. 0x509980 - joypad create + enumerate (185 B, 14 blocks, `__cdecl`, no arguments)

- Extent `0x00509980-0x00509A38` (185 bytes). Return value: `g_A7B638` (device count) on the success path, 0 when EnumDevices installed nothing; error paths never return (Debug_Error + `jmp $`).
- Full bytes:

```
5633f65668f0b4a7006800070000893538b6a700e8f756f8ff50e8a15dfaff85
c0740f6850e95400e8035efaff83c404ebfea1f0b4a70053576a018b08566840
9a50006a0450ff5110a138b6a7003bc674457e43bbe0b4a700bf30b4a70056e8
dc02000083c40485c074308b03c7072c00000057508b10ff520c85c0752c56e8
fc040000a138b6a70083c4044683c72c83c3043bf07cc75f5b5ec36820e95400
e88b5dfaff83c404ebfe68e8e85400e87c5dfaff83c404ebfe
```

- Full disassembly:

```
0x00509980      56             push esi
0x00509981      33f6           xor esi, esi
0x00509983      56             push esi
0x00509984      68f0b4a700     push 0xa7b4f0
0x00509989      6800070000     push 0x700                  ; 1792
0x0050998e      893538b6a700   mov dword [0xa7b638], esi   ; [0xa7b638:4]=0
0x00509994      e8f756f8ff     call 0x48f090
0x00509999      50             push eax
0x0050999a      e8a15dfaff     call 0x4af740
0x0050999f      85c0           test eax, eax
┌─< 0x005099a1      740f           je 0x5099b2
│   0x005099a3      6850e95400     push str.Could_not_create_DirectInput_joypad_object. ; 0x54e950 ; "Could not create DirectInput joypad object."
│   0x005099a8      e8035efaff     call 0x4af7b0
│   0x005099ad      83c404         add esp, 4
──> 0x005099b0      ebfe           jmp 0x5099b0
└─> 0x005099b2      a1f0b4a700     mov eax, dword [0xa7b4f0]   ; [0xa7b4f0:4]=0
0x005099b7      53             push ebx
0x005099b8      57             push edi
0x005099b9      6a01           push 1                      ; 1
0x005099bb      8b08           mov ecx, dword [eax]
0x005099bd      56             push esi
0x005099be      68409a5000     push 0x509a40
0x005099c3      6a04           push 4                      ; 4
0x005099c5      50             push eax
0x005099c6      ff5110         call dword [ecx + 0x10]     ; 16
0x005099c9      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
0x005099ce      3bc6           cmp eax, esi
┌─< 0x005099d0      7445           je 0x509a17
┌──< 0x005099d2      7e43           jle 0x509a17
││   0x005099d4      bbe0b4a700     mov ebx, 0xa7b4e0
││   0x005099d9      bf30b4a700     mov edi, 0xa7b430
┌───> 0x005099de      56             push esi
╎││   0x005099df      e8dc020000     call 0x509cc0
╎││   0x005099e4      83c404         add esp, 4
╎││   0x005099e7      85c0           test eax, eax
┌────< 0x005099e9      7430           je 0x509a1b
│╎││   0x005099eb      8b03           mov eax, dword [ebx]
│╎││   0x005099ed      c7072c000000   mov dword [edi], 0x2c       ; ','
│╎││                                                              ; [0x2c:4]=-1 ; 44
│╎││   0x005099f3      57             push edi
│╎││   0x005099f4      50             push eax
│╎││   0x005099f5      8b10           mov edx, dword [eax]
│╎││   0x005099f7      ff520c         call dword [edx + 0xc]      ; 12
│╎││   0x005099fa      85c0           test eax, eax
┌─────< 0x005099fc      752c           jne 0x509a2a
││╎││   0x005099fe      56             push esi
││╎││   0x005099ff      e8fc040000     call 0x509f00
││╎││   0x00509a04      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
││╎││   0x00509a09      83c404         add esp, 4
││╎││   0x00509a0c      46             inc esi
││╎││   0x00509a0d      83c72c         add edi, 0x2c               ; 44
││╎││   0x00509a10      83c304         add ebx, 4
││╎││   0x00509a13      3bf0           cmp esi, eax
││└───< 0x00509a15      7cc7           jl 0x5099de
││ └└─> 0x00509a17      5f             pop edi
││      0x00509a18      5b             pop ebx
││      0x00509a19      5e             pop esi
││      0x00509a1a      c3             ret
│└────> 0x00509a1b      6820e95400     push str.Could_not_acquire_one_or_more_joystick_devices. ; 0x54e920 ; "Could not acquire one or more joystick devices."
│       0x00509a20      e88b5dfaff     call 0x4af7b0
│       0x00509a25      83c404         add esp, 4
│    ─> 0x00509a28      ebfe           jmp 0x509a28
└─────> 0x00509a2a      68e8e85400     push str.Could_not_get_capabilities_for_one_or_more_joysticks_. ; 0x54e8e8 ; "Could not get capabilities for one or more joysticks ."
0x00509a2f      e87c5dfaff     call 0x4af7b0
0x00509a34      83c404         add esp, 4
─> 0x00509a37      ebfe           jmp 0x509a37
0x00509a39      90             nop
0x00509a3a      90             nop
0x00509a3b      90             nop
0x00509a3c      90             nop
0x00509a3d      90             nop
0x00509a3e      90             nop
0x00509a3f      90             nop
0x00509a40      83ec3c         sub esp, 0x3c
0x00509a43      8b442440       mov eax, dword [esp + 0x40]
0x00509a47      57             push edi
0x00509a48      83c004         add eax, 4
0x00509a4b      33ff           xor edi, edi
0x00509a4d      57             push edi
0x00509a4e      8b08           mov ecx, dword [eax]
0x00509a50      894c2408       mov dword [esp + 8], ecx
0x00509a54      8b5004         mov edx, dword [eax + 4]
0x00509a57      8954240c       mov dword [esp + 0xc], edx
0x00509a5b      8b4808         mov ecx, dword [eax + 8]
0x00509a5e      894c2410       mov dword [esp + 0x10], ecx
0x00509a62      8b500c         mov edx, dword [eax + 0xc]
0x00509a65      a1f0b4a700     mov eax, dword [0xa7b4f0]   ; [0xa7b4f0:4]=0
0x00509a6a      89542414       mov dword [esp + 0x14], edx
0x00509a6e      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509a74      8b08           mov ecx, dword [eax]
0x00509a76      8d1495e0b2..   lea edx, [edx*4 + 0xa7b2e0]
0x00509a7d      52             push edx
0x00509a7e      8d54240c       lea edx, [esp + 0xc]
0x00509a82      52             push edx
0x00509a83      50             push eax
0x00509a84      ff510c         call dword [ecx + 0xc]      ; 12
0x00509a87      85c0           test eax, eax
0x00509a89      0f8c17020000   jl 0x509ca6
0x00509a8f      8b0d38b6a700   mov ecx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509a95      53             push ebx
0x00509a96      56             push esi
0x00509a97      8b048de0b2..   mov eax, dword [ecx*4 + 0xa7b2e0]
0x00509a9e      8d0c8de0b4..   lea ecx, [ecx*4 + 0xa7b4e0]
0x00509aa5      51             push ecx
0x00509aa6      68c0495300     push 0x5349c0
0x00509aab      8b10           mov edx, dword [eax]
0x00509aad      50             push eax
0x00509aae      ff12           call dword [edx]
0x00509ab0      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509ab6      8bf0           mov esi, eax
0x00509ab8      8b0495e0b2..   mov eax, dword [edx*4 + 0xa7b2e0]
0x00509abf      50             push eax
0x00509ac0      8b08           mov ecx, dword [eax]
0x00509ac2      ff5108         call dword [ecx + 8]        ; 8
0x00509ac5      3bf7           cmp esi, edi
0x00509ac7      7d0f           jge 0x509ad8
0x00509ac9      6898e95400     push str.Failed_to_obtain_interface. ; 0x54e998 ; "Failed to obtain interface."
0x00509ace      e8dd5cfaff     call 0x4af7b0
0x00509ad3      83c404         add esp, 4
0x00509ad6      ebfe           jmp 0x509ad6
0x00509ad8      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509ade      68384c5300     push 0x534c38               ; '8LS'
0x00509ae3      8b0495e0b4..   mov eax, dword [edx*4 + 0xa7b4e0]
0x00509aea      50             push eax
0x00509aeb      8b08           mov ecx, dword [eax]
0x00509aed      ff512c         call dword [ecx + 0x2c]     ; 44
0x00509af0      85c0           test eax, eax
0x00509af2      7d2e           jge 0x509b22
0x00509af4      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509afa      8b0495e0b4..   mov eax, dword [edx*4 + 0xa7b4e0]
0x00509b01      8b08           mov ecx, dword [eax]
0x00509b03      50             push eax
0x00509b04      ff5108         call dword [ecx + 8]        ; 8
0x00509b07      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509b0d      5e             pop esi
0x00509b0e      b801000000     mov eax, 1
0x00509b13      5b             pop ebx
0x00509b14      893c95e0b4..   mov dword [edx*4 + 0xa7b4e0], edi ; [0xa7b4e0:4]=0
0x00509b1b      5f             pop edi
0x00509b1c      83c43c         add esp, 0x3c
0x00509b1f      c20800         ret 8
0x00509b22      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509b27      6a05           push 5                      ; 5
0x00509b29      8b3485e0b4..   mov esi, dword [eax*4 + 0xa7b4e0]
0x00509b30      8b1e           mov ebx, dword [esi]
0x00509b32      e86955f8ff     call 0x48f0a0
0x00509b37      50             push eax
0x00509b38      56             push esi
0x00509b39      ff5334         call dword [ebx + 0x34]     ; 52
0x00509b3c      85c0           test eax, eax
0x00509b3e      7d2d           jge 0x509b6d
0x00509b40      8b0d38b6a700   mov ecx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509b46      8b048de0b4..   mov eax, dword [ecx*4 + 0xa7b4e0]
0x00509b4d      50             push eax
0x00509b4e      8b10           mov edx, dword [eax]
0x00509b50      ff5208         call dword [edx + 8]        ; 8
0x00509b53      a138b6a700     mov eax, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509b58      5e             pop esi
0x00509b59      5b             pop ebx
0x00509b5a      893c85e0b4..   mov dword [eax*4 + 0xa7b4e0], edi ; [0xa7b4e0:4]=0
0x00509b61      b801000000     mov eax, 1
0x00509b66      5f             pop edi
0x00509b67      83c43c         add esp, 0x3c
0x00509b6a      c20800         ret 8
0x00509b6d      bb10000000     mov ebx, 0x10               ; 16
0x00509b72      c744243018..   mov dword [esp + 0x30], 0x18 ; [0x18:4]=-1 ; 24
0x00509b7a      895c2434       mov dword [esp + 0x34], ebx
0x00509b7e      c744243c01..   mov dword [esp + 0x3c], 1
0x00509b86      c744244080..   mov dword [esp + 0x40], 0xffffff80 ; [0xffffff80:4]=-1 ; 4294967168
0x00509b8e      c744244480..   mov dword [esp + 0x44], 0x80 ; [0x80:4]=-1 ; 128
0x00509b96      beace85400     mov esi, 0x54e8ac
0x00509b9b      8b1538b6a700   mov edx, dword [0xa7b638]   ; [0xa7b638:4]=0
0x00509ba1      8b4efc         mov ecx, dword [esi - 4]
0x00509ba4      894c2438       mov dword [esp + 0x38], ecx
0x00509ba8      8b0495e0b2..   mov eax, dword [edx*4 + 0xa7b2e0]
0x00509baf      8d542430       lea edx, [esp + 0x30]
0x00509bb3      52             push edx
0x00509bb4      6a04           push 4                      ; 4
0x00509bb6      8b08           mov ecx, dword [eax]
0x00509bb8      50             push eax
0x00509bb9      ff5118         call dword [ecx + 0x18]     ; 24
0x00509bbc      3bc7           cmp eax, edi
0x00509bbe      740d           je 0x509bcd
0x00509bc0      3d02000780     cmp eax, 0x80070002
0x00509bc5      0f85ca000000   jne 0x509c95
0x00509bcb      893e           mov dword [esi], edi
```

Exact call sequence:

1. `g_A7B638 = 0`. `push 0` (punkOuter), `push &g_A7B4F0`, `push 0x700`, `push GetInstanceHandle_48F090()` -> `DirectInputCreateA(hinstance, 0x700, &g_A7B4F0, 0)` via thunk 0x4AF740. Nonzero return: `Debug_Error("Could not create DirectInput joypad object.")` + `jmp $` (0x5099A1 -> 0x5099A3 -> 0x5099B0).
2. `EnumDevices`: `push 1` (DIEDFL_ATTACHEDONLY), `push 0` (pvRef), `push 0x00509A40` (callback), `push 4` (DI8DEVTYPE_JOYSTICK), `push g_A7B4F0` -> `call [ecx+0x10]`. Return value ignored.
3. `eax = g_A7B638`; if `0`: return 0. Loop `index = 0; index < g_A7B638; ++index`:
   - `Joypad_IsAcquired_509CC0(index)`; if 0 -> `Debug_Error("Could not acquire one or more joystick devices.")` + `jmp $` (0x509A1B).
   - `caps = &g_A7B430 + index*0x2C`; `*caps = 0x2C`; `GetCapabilities(g_A7B4E0[index], caps)` via `call [edx+0x0C]`; nonzero -> `Debug_Error("Could not get capabilities for one or more joysticks .")` + `jmp $` (0x509A2A).
   - `Joypad_InitActionFlags_509F00(index)`; then `index++`, `caps += 0x2C`, `device_slot += 4`; loop while `index < g_A7B638`.
4. Return `g_A7B638` (count).

Literal strings (byte-exact) - `.data` image:

| VA | file offset | hex | text |
|---|---|---|---|
| `0x54e950` | `0x14e950` | `436f756c64206e6f742063726561746520446972656374496e707574206a6f79706164206f626a6563742e` | "Could not create DirectInput joypad object." |
| `0x54e920` | `0x14e920` | `436f756c64206e6f742061637175697265206f6e65206f72206d6f7265206a6f79737469636b20646576696365732e` | "Could not acquire one or more joystick devices." |
| `0x54e8e8` | `0x14e8e8` | `436f756c64206e6f7420676574206361706162696c697469657320666f72206f6e65206f72206d6f7265206a6f79737469636b73202e` | "Could not get capabilities for one or more joysticks ." |

Paste-ready body:

```cpp
// Native: Joypad_Init_509980 (placeholder until VERIFIED; BN alias Physics_CouldNotCreateDirect is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509980-0x00509A38.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 6.2.0 bytes/disassembly; 185 bytes, 14 blocks, no arguments.
// Behavior/CFG: g_A7B638=0; DirectInputCreateA(hinst,0x700,&g_A7B4F0,0); EnumDevices(4, 0x509A40, 0, 1); if no devices return 0;
//           per device: 0x509CC0 acquire check, GetCapabilities into g_A7B430+index*0x2C with dwSize=0x2C, 0x509F00; return count.
//           Every failure calls Debug_Error and spins forever.
// Status/limits: TRANSCRIBED; deps 0x0048F090, 0x004AF740, 0x004AF7B0, 0x00509A40, 0x00509CC0, 0x00509F00. Runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/input/native_48f090.hpp"
#include <cstdint>

extern "C" {

void* g_a7b4f0;
std::int32_t g_a7b638;
void* g_a7b4e0[16];
std::uint8_t g_a7b430[16 * 0x2C];

extern "C" __declspec(dllimport) std::int32_t __stdcall DirectInputCreateA(
    void* hinst, std::uint32_t version, void** out, void* outer);

extern "C" std::int32_t __stdcall Joypad_EnumDevicesCallback_509A40(void* device_instance, void* unused);
extern "C" std::int32_t NATIVE_DECL Joypad_IsAcquired_509CC0(std::int32_t index);
extern "C" void NATIVE_DECL Joypad_InitActionFlags_509F00(std::int32_t index);

std::int32_t NATIVE_DECL Joypad_Init_509980()
{
    g_a7b638 = 0;
    std::int32_t const create_hr = DirectInputCreateA(
        GetInstanceHandle_48F090(), 0x700, &g_a7b4f0, nullptr); // 0x0050999A thunk 0x004AF740
    if (create_hr != 0)
    {
        Debug_Error("Could not create DirectInput joypad object."); // 0x005099A3
        for (;;)
        {
        }
    }

    void* const io = g_a7b4f0;
    void* const* const io_vtable = *reinterpret_cast<void* const* const*>(io);
    typedef std::int32_t (__stdcall* EnumDevicesFn)(void*, std::uint32_t, void*, void*, std::uint32_t);
    reinterpret_cast<EnumDevicesFn>(io_vtable[0x10 / 4])(
        io, 4, reinterpret_cast<void*>(&Joypad_EnumDevicesCallback_509A40), nullptr, 1); // [ecx+0x10] at 0x005099C6

    std::int32_t count = g_a7b638;
    if (count == 0)
    {
        return count; // retail returns eax = g_A7B638 = 0
    }

    std::int32_t index = 0;
    std::uint8_t* caps = g_a7b430;
    do
    {
        if (Joypad_IsAcquired_509CC0(index) == 0) // 0x005099DF
        {
            Debug_Error("Could not acquire one or more joystick devices."); // 0x00509A1B
            for (;;)
            {
            }
        }
        *reinterpret_cast<std::uint32_t*>(caps) = 0x2C; // 0x005099ED: DIDEVCAPS.dwSize
        void* const device = g_a7b4e0[index];
        void* const* const device_vtable = *reinterpret_cast<void* const* const*>(device);
        typedef std::int32_t (__stdcall* GetCapabilitiesFn)(void*, void*);
        if (reinterpret_cast<GetCapabilitiesFn>(device_vtable[0x0C / 4])(device, caps) != 0) // [edx+0x0C] at 0x005099F7
        {
            Debug_Error("Could not get capabilities for one or more joysticks ."); // 0x00509A2A
            for (;;)
            {
            }
        }
        Joypad_InitActionFlags_509F00(index); // 0x005099FF
        count = g_a7b638;
        ++index;
        caps += 0x2C;
    } while (index < count);

    return g_a7b638; // 0x00509A1A ret, eax = count
}

} // extern "C"
```

## 3. Window procedures

Both are registered through `WinProc_RegisterHandler_48F490` and are invoked by the transcriber for 0x48F970 with `(hwnd, msg, wParam, lParam)` on the stack; both ignore the hwnd argument, read `msg` at `[esp+8]`, `wParam` at `[esp+0xC]`, `lParam` at `[esp+0x10]`, and both return 1 (0 would end WinProc dispatch).

### 3.1 0x490120 - keyboard window procedure (83 B, 5 blocks)

- Extent `0x00490120-0x00490172`; message jump table `0x00490174` (6 dwords): `0x00490135, 0x00490154, 0x0049016D, 0x0049016D, 0x00490135, 0x00490154`.
- Cases (index = msg - 0x100):
  - `0x0100 WM_KEYDOWN` and `0x0104 WM_SYSKEYDOWN` -> `byte [0x00C28E80 + value] = 0x80`, where `value = 0x00490190(wParam, lParam)` (the VK/scan-code mapping native).
  - `0x0101 WM_KEYUP` and `0x0105 WM_SYSKEYUP` -> same index, `byte [0x00C28E80 + value] = 0`.
  - `0x0102` and `0x0103` (WM_CHAR / WM_SYSCHAR) -> jump to 0x49016D, no state change.
  - All other messages -> 0x49016D, return 1.
- Full bytes:

```
8b4424080500ffffff83f805773fff2485740149008b4c24108b54240c5152e8
4c000000c680808ec2008083c408b801000000c38b4424108b4c240c5051e82d
00000083c408c680808ec20000b801000000c3
```

- Full disassembly:

```
0x00490120      8b442408       mov eax, dword [esp + 8]
0x00490124      0500ffffff     add eax, 0xffffff00         ; 4294967040
0x00490129      83f805         cmp eax, 5                  ; 5
┌─< 0x0049012c      773f           ja 0x49016d
│   0x0049012e      ff24857401..   jmp dword [eax*4 + 0x490174]
│   0x00490135      8b4c2410       mov ecx, dword [esp + 0x10]
│   0x00490139      8b54240c       mov edx, dword [esp + 0xc]
│   0x0049013d      51             push ecx
│   0x0049013e      52             push edx
│   0x0049013f      e84c000000     call 0x490190
│   0x00490144      c680808ec2..   mov byte [eax + 0xc28e80], 0x80 ; [0xc28e80:1]=0
│   0x0049014b      83c408         add esp, 8
│   0x0049014e      b801000000     mov eax, 1
│   0x00490153      c3             ret
│   0x00490154      8b442410       mov eax, dword [esp + 0x10]
│   0x00490158      8b4c240c       mov ecx, dword [esp + 0xc]
│   0x0049015c      50             push eax
│   0x0049015d      51             push ecx
│   0x0049015e      e82d000000     call 0x490190
│   0x00490163      83c408         add esp, 8
│   0x00490166      c680808ec2..   mov byte [eax + 0xc28e80], 0 ; [0xc28e80:1]=0
└─> 0x0049016d      b801000000     mov eax, 1
0x00490172      c3             ret
0x00490173      90             nop
0x00490174      3501490054     xor eax, 0x54004901
0x00490179      014900         add dword [ecx], ecx
0x0049017c      6d             insd dword es:[edi], dx
0x0049017d      014900         add dword [ecx], ecx
0x00490180      6d             insd dword es:[edi], dx
0x00490181      014900         add dword [ecx], ecx
0x00490184      3501490054     xor eax, 0x54004901
0x00490189      014900         add dword [ecx], ecx
0x0049018c      90             nop
0x0049018d      90             nop
0x0049018e      90             nop
0x0049018f      90             nop
0x00490190      8b442404       mov eax, dword [esp + 4]
0x00490194      83c0ef         add eax, 0xffffffef
0x00490197      83f817         cmp eax, 0x17               ; 23
┌─< 0x0049019a      774c           ja 0x4901e8
│   0x0049019c      33c9           xor ecx, ecx
│   0x0049019e      8a8814024900   mov cl, byte [eax + 0x490214]
│   0x004901a4      ff248df401..   jmp dword [ecx*4 + 0x4901f4]
│   0x004901ab      b8d1000000     mov eax, 0xd1               ; 209
│   0x004901b0      c3             ret
│   0x004901b1      b8c9000000     mov eax, 0xc9               ; 201
│   0x004901b6      c3             ret
│   0x004901b7      b8c8000000     mov eax, 0xc8               ; 200
│   0x004901bc      c3             ret
│   0x004901bd      b8d0000000     mov eax, 0xd0               ; 208
│   0x004901c2      c3             ret
│   0x004901c3      b8cb000000     mov eax, 0xcb               ; 203
│   0x004901c8      c3             ret
│   0x004901c9      b8cd000000     mov eax, 0xcd               ; 205
│   0x004901ce      c3             ret
│   0x004901cf      8b4c2408       mov ecx, dword [esp + 8]
│   0x004901d3      8bc1           mov eax, ecx
│   0x004901d5      81e100007f00   and ecx, 0x7f0000
│   0x004901db      d1f8           sar eax, 1
│   0x004901dd      2500008000     and eax, 0x800000
│   0x004901e2      0bc1           or eax, ecx
│   0x004901e4      c1f810         sar eax, 0x10
│   0x004901e7      c3             ret
└─> 0x004901e8      8b442408       mov eax, dword [esp + 8]
0x004901ec      c1f810         sar eax, 0x10
0x004901ef      83e07f         and eax, 0x7f               ; 127
0x004901f2      c3             ret
0x004901f3      90             nop
0x004901f4      cf             iretd
0x004901f5      014900         add dword [ecx], ecx
0x004901f8      b101           mov cl, 1
0x004901fa      49             dec ecx
0x004901fb      00ab014900c3   add byte [ebx - 0x3cffb6ff], ch
0x00490201      014900         add dword [ecx], ecx
0x00490204      b701           mov bh, 1
0x00490206      49             dec ecx
0x00490207      00c9           add cl, cl
0x00490209      014900         add dword [ecx], ecx
0x0049020c      bd014900e8     mov ebp, 0xe8004901
0x00490211      014900         add dword [ecx], ecx
0x00490214      0000           add byte [eax], al
0x00490216      07             pop es
0x00490217      07             pop es
```

- Paste-ready body (needs `0x490190` transcribed and `0xC28E80` declared as a 256-byte zero-fill runtime array):

```cpp
// Native: WindowProc_Keyboard_490120 (placeholder until VERIFIED; BN alias DisplayRender_InvokeOpenDevice is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00490120-0x00490172.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 6.2.0 bytes/disassembly; 83 bytes, 5 blocks.
// Behavior/CFG: WM_KEYDOWN/WM_SYSKEYDOWN set byte [0x00C28E80 + 0x490190(wParam,lParam)] = 0x80;
//           WM_KEYUP/WM_SYSKEYUP set it to 0; WM_CHAR/WM_SYSCHAR and every other message return 1 unchanged.
// Status/limits: TRANSCRIBED; deps 0x00490190 only (plus the 0xC28E80 array). The hWnd argument is unused. Runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::uint8_t g_c28e80[0x100]; // 0x00C28E80 keyboard state array (zero-fill runtime data)

extern "C" std::int32_t NATIVE_DECL Keyboard_VkToScanIndex_490190(std::int32_t vk, std::int32_t lparam);

std::int32_t NATIVE_DECL WindowProc_Keyboard_490120(
    void* hwnd, std::uint32_t message, std::uint32_t wparam, std::uint32_t lparam)
{
    (void)hwnd; // retail never reads the window handle in this procedure
    if (message - 0x100 <= 5)
    {
        switch (message)
        {
        case 0x100: // WM_KEYDOWN
        case 0x104: // WM_SYSKEYDOWN
        {
            std::int32_t const index = Keyboard_VkToScanIndex_490190(
                static_cast<std::int32_t>(wparam), static_cast<std::int32_t>(lparam));
            g_c28e80[index] = 0x80; // 0x00490144
            break;
        }
        case 0x101: // WM_KEYUP
        case 0x105: // WM_SYSKEYUP
        {
            std::int32_t const index = Keyboard_VkToScanIndex_490190(
                static_cast<std::int32_t>(wparam), static_cast<std::int32_t>(lparam));
            g_c28e80[index] = 0; // 0x00490166
            break;
        }
        }
    }
    return 1;
}

} // extern "C"
```

### 3.2 0x490190 - VK/scan-code to keyboard-state index (99 B, 10 blocks)

- Extent `0x00490190-0x004901F2`; jump table `0x004901F4` (8 dwords): `0x004901CF, 0x004901B1, 0x004901AB, 0x004901C3, 0x004901B7, 0x004901C9, 0x004901BD, 0x004901E8`; selector byte table `0x00490214` (0x18 bytes): `00 00 07 07 07 07 07 07 07 07 07 07 07 07 07 07 01 02 07 07 03 04 05 06`.
- Behaviour: if `vk - 0x11 <= 0x17` then `index = table[vk - 0x11]` and dispatch through the 8-entry jump table: 0 -> `(sar(lParam,1) & 0x800000 | lParam & 0x7F0000) >> 16`; 1 -> `0xC9`; 2 -> `0xD1`; 3 -> `0xCB`; 4 -> `0xC8`; 5 -> `0xCD`; 6 -> `0xD0`; 7 -> `(sar(lParam,16) & 0x7F)`. If vk is outside 0x11..0x28 -> `(sar(lParam,16) & 0x7F)`.
- Table meaning: VK 0x11 (CONTROL) and 0x12 (MENU) select the lParam scan-code path; VK 0x21->0xC9, 0x22->0xD1, 0x25->0xCB, 0x26->0xC8, 0x27->0xCD, 0x28->0xD0; the remaining VKs in range use the scan-code path.
- Full bytes:

```
8b44240483c0ef83f817774c33c98a8814024900ff248df4014900b8d1000000
c3b8c9000000c3b8c8000000c3b8d0000000c3b8cb000000c3b8cd000000c38b
4c24088bc181e100007f00d1f825000080000bc1c1f810c38b442408c1f81083
e07fc3
```

- Full disassembly:

```
0x00490190      8b442404       mov eax, dword [esp + 4]
0x00490194      83c0ef         add eax, 0xffffffef
0x00490197      83f817         cmp eax, 0x17               ; 23
┌─< 0x0049019a      774c           ja 0x4901e8
│   0x0049019c      33c9           xor ecx, ecx
│   0x0049019e      8a8814024900   mov cl, byte [eax + 0x490214]
│   0x004901a4      ff248df401..   jmp dword [ecx*4 + 0x4901f4]
│   0x004901ab      b8d1000000     mov eax, 0xd1               ; 209
│   0x004901b0      c3             ret
│   0x004901b1      b8c9000000     mov eax, 0xc9               ; 201
│   0x004901b6      c3             ret
│   0x004901b7      b8c8000000     mov eax, 0xc8               ; 200
│   0x004901bc      c3             ret
│   0x004901bd      b8d0000000     mov eax, 0xd0               ; 208
│   0x004901c2      c3             ret
│   0x004901c3      b8cb000000     mov eax, 0xcb               ; 203
│   0x004901c8      c3             ret
│   0x004901c9      b8cd000000     mov eax, 0xcd               ; 205
│   0x004901ce      c3             ret
│   0x004901cf      8b4c2408       mov ecx, dword [esp + 8]
│   0x004901d3      8bc1           mov eax, ecx
│   0x004901d5      81e100007f00   and ecx, 0x7f0000
│   0x004901db      d1f8           sar eax, 1
│   0x004901dd      2500008000     and eax, 0x800000
│   0x004901e2      0bc1           or eax, ecx
│   0x004901e4      c1f810         sar eax, 0x10
│   0x004901e7      c3             ret
└─> 0x004901e8      8b442408       mov eax, dword [esp + 8]
0x004901ec      c1f810         sar eax, 0x10
0x004901ef      83e07f         and eax, 0x7f               ; 127
0x004901f2      c3             ret
0x004901f3      90             nop
0x004901f4      cf             iretd
0x004901f5      014900         add dword [ecx], ecx
0x004901f8      b101           mov cl, 1
0x004901fa      49             dec ecx
0x004901fb      00ab014900c3   add byte [ebx - 0x3cffb6ff], ch
0x00490201      014900         add dword [ecx], ecx
0x00490204      b701           mov bh, 1
0x00490206      49             dec ecx
0x00490207      00c9           add cl, cl
0x00490209      014900         add dword [ecx], ecx
0x0049020c      bd014900e8     mov ebp, 0xe8004901
0x00490211      014900         add dword [ecx], ecx
0x00490214      0000           add byte [eax], al
0x00490216      07             pop es
0x00490217      07             pop es
0x00490218      07             pop es
0x00490219      07             pop es
0x0049021a      07             pop es
0x0049021b      07             pop es
0x0049021c      07             pop es
0x0049021d      07             pop es
0x0049021e      07             pop es
0x0049021f      07             pop es
0x00490220      07             pop es
0x00490221      07             pop es
0x00490222      07             pop es
0x00490223      07             pop es
0x00490224      0102           add dword [edx], eax
0x00490226      07             pop es
0x00490227      07             pop es
0x00490228      0304050690..   add eax, dword [eax - 0x6f6f6ffa]
0x0049022f      90             nop
0x00490230      a1308e5c00     mov eax, dword [0x5c8e30]   ; [0x5c8e30:4]=0
0x00490235      56             push esi
0x00490236      85c0           test eax, eax
0x00490238      57             push edi
┌─< 0x00490239      751d           jne 0x490258
│   0x0049023b      a1248e5c00     mov eax, dword [0x5c8e24]   ; [0x5c8e24:4]=0
│   0x00490240      50             push eax
│   0x00490241      8b08           mov ecx, dword [eax]
│   0x00490243      ff511c         call dword [ecx + 0x1c]     ; 28
│   0x00490246      85c0           test eax, eax
┌──< 0x00490248      0f8590000000   jne 0x4902de
││   0x0049024e      c705308e5c..   mov dword [0x5c8e30], 1     ; [0x5c8e30:4]=0
│└─> 0x00490258      b940000000     mov ecx, 0x40               ; '@' ; 64
│    0x0049025d      be628cc200     mov esi, 0xc28c62
│    0x00490262      bf628dc200     mov edi, 0xc28d62
│    0x00490267      f3a5           rep movsd dword es:[edi], dword [esi]
│    0x00490269      a0608cc200     mov al, byte [0xc28c60]     ; [0xc28c60:1]=0
│    0x0049026e      84c0           test al, al
│┌─< 0x00490270      755e           jne 0x4902d0
││   0x00490272      e8f9efffff     call 0x48f270
││   0x00490277      85c0           test eax, eax
┌───< 0x00490279      7414           je 0x49028f
│││   0x0049027b      b940000000     mov ecx, 0x40               ; '@' ; 64
│││   0x00490280      be808ec200     mov esi, 0xc28e80
│││   0x00490285      bf628cc200     mov edi, 0xc28c62
│││   0x0049028a      f3a5           rep movsd dword es:[edi], dword [esi]
│││   0x0049028c      5f             pop edi
│││   0x0049028d      5e             pop esi
│││   0x0049028e      c3             ret
└───> 0x0049028f      a1248e5c00     mov eax, dword [0x5c8e24]   ; [0x5c8e24:4]=0
││   0x00490294      68628cc200     push 0xc28c62
││   0x00490299      6800010000     push pe_nt_image_headers32  ; 0x100
││   0x0049029e      50             push eax
││   0x0049029f      8b10           mov edx, dword [eax]
││   0x004902a1      ff5224         call dword [edx + 0x24]     ; 36
```

- Paste-ready body:

```cpp
// Native: Keyboard_VkToScanIndex_490190 (placeholder until VERIFIED; BN alias Display_OpenDevice is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00490190-0x004901F2.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 6.2.0 bytes/disassembly; 99 bytes, 10 blocks, tables 0x004901F4/0x00490214.
// Behavior/CFG: VK (0x11..0x28) -> keyboard-state index via the 0x00490214 selector table and the 0x004901F4 jump table;
//           every other VK returns (sar(lParam,16) & 0x7F). The lParam scan-code path is used by VK_CONTROL/VK_MENU and selector 7.
// Status/limits: TRANSCRIBED; no callees, no writable data. Runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::int32_t NATIVE_DECL Keyboard_VkToScanIndex_490190(std::int32_t vk, std::int32_t lparam)
{
    if (static_cast<std::uint32_t>(vk - 0x11) <= 0x17U)
    {
        static std::uint8_t const selector[0x18] = {
            0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
            0x01, 0x02, 0x07, 0x07, 0x03, 0x04, 0x05, 0x06}; // 0x00490214
        switch (selector[vk - 0x11])
        {
        case 0: // 0x004901CF
            return static_cast<std::int32_t>(((lparam >> 1) & 0x800000) | (lparam & 0x7F0000)) >> 0x10;
        case 1: return 0xC9; // 0x004901B1
        case 2: return 0xD1; // 0x004901AB
        case 3: return 0xCB; // 0x004901C3
        case 4: return 0xC8; // 0x004901B7
        case 5: return 0xCD; // 0x004901C9
        case 6: return 0xD0; // 0x004901BD
        }
    }
    return (lparam >> 0x10) & 0x7F; // 0x004901E8 / out-of-range fallthrough
}

} // extern "C"
```

### 3.3 0x48FED0 - mouse window procedure (168 B, 10 blocks)

- Extent `0x0048FED0-0x0048FF77`; message jump table at `0x0048FF78` (10 dwords): `0x48FF18, 0x48FF48, 0x48FF72, 0x48FF28, 0x48FF58, 0x48FF72, 0x48FF38, 0x48FF68, 0x48FF72, 0x48FEE9`.
- Cases: `0x0201 WM_LBUTTONDOWN` -> `g_5C8E10=1`; `0x0202 WM_LBUTTONUP` -> `g_5C8E10=0`; `0x0204 WM_RBUTTONDOWN` -> `g_5C8E0C=1`; `0x0205 WM_RBUTTONUP` -> `g_5C8E0C=0`; `0x0207 WM_MBUTTONDOWN` -> `g_5C8E18=1`; `0x0208 WM_MBUTTONUP` -> `g_5C8E18=0`; `0x0203`, `0x0206`, `0x0209` -> return 1 only; `0x020A WM_MOUSEWHEEL` -> `g_5C8E08 += (int16)(wParam >> 16) / 120` (retail computes it with the 0x88888889 magic division, `sar edx,6` after correction = truncating divide by 120); everything else -> return 1.
- Full bytes:

```
8b44240805fffdffff83f8090f8790000000ff248578ff48008b4c240cb88988
8888c1e9100fbfc9f7e903d1c1fa068bc2c1e81f03d0a1088e5c0003c2a3088e
5c00b801000000c3c705108e5c0001000000b801000000c3c7050c8e5c000100
0000b801000000c3c705188e5c0001000000b801000000c3c705108e5c000000
0000b801000000c3c7050c8e5c0000000000b801000000c3c705188e5c000000
0000b801000000c3
```

- Full disassembly:

```
0x0048fed0      8b442408       mov eax, dword [esp + 8]
0x0048fed4      05fffdffff     add eax, 0xfffffdff         ; 4294966783
0x0048fed9      83f809         cmp eax, 9                  ; 9
┌─< 0x0048fedc      0f8790000000   ja 0x48ff72
│   0x0048fee2      ff248578ff..   jmp dword [eax*4 + 0x48ff78]
│   0x0048fee9      8b4c240c       mov ecx, dword [esp + 0xc]
│   0x0048feed      b889888888     mov eax, 0x88888889
│   0x0048fef2      c1e910         shr ecx, 0x10
│   0x0048fef5      0fbfc9         movsx ecx, cx
│   0x0048fef8      f7e9           imul ecx
│   0x0048fefa      03d1           add edx, ecx
│   0x0048fefc      c1fa06         sar edx, 6
│   0x0048feff      8bc2           mov eax, edx
│   0x0048ff01      c1e81f         shr eax, 0x1f
│   0x0048ff04      03d0           add edx, eax
│   0x0048ff06      a1088e5c00     mov eax, dword [0x5c8e08]   ; [0x5c8e08:4]=0
│   0x0048ff0b      03c2           add eax, edx
│   0x0048ff0d      a3088e5c00     mov dword [0x5c8e08], eax   ; [0x5c8e08:4]=0
│   0x0048ff12      b801000000     mov eax, 1
│   0x0048ff17      c3             ret
│   0x0048ff18      c705108e5c..   mov dword [0x5c8e10], 1     ; [0x5c8e10:4]=0
│   0x0048ff22      b801000000     mov eax, 1
│   0x0048ff27      c3             ret
│   0x0048ff28      c7050c8e5c..   mov dword [0x5c8e0c], 1     ; [0x5c8e0c:4]=0
│   0x0048ff32      b801000000     mov eax, 1
│   0x0048ff37      c3             ret
│   0x0048ff38      c705188e5c..   mov dword [0x5c8e18], 1     ; [0x5c8e18:4]=0
│   0x0048ff42      b801000000     mov eax, 1
│   0x0048ff47      c3             ret
│   0x0048ff48      c705108e5c..   mov dword [0x5c8e10], 0     ; [0x5c8e10:4]=0
│   0x0048ff52      b801000000     mov eax, 1
│   0x0048ff57      c3             ret
│   0x0048ff58      c7050c8e5c..   mov dword [0x5c8e0c], 0     ; [0x5c8e0c:4]=0
│   0x0048ff62      b801000000     mov eax, 1
│   0x0048ff67      c3             ret
│   0x0048ff68      c705188e5c..   mov dword [0x5c8e18], 0     ; [0x5c8e18:4]=0
└─> 0x0048ff72      b801000000     mov eax, 1
0x0048ff77      c3             ret
0x0048ff78      18ff           sbb bh, bh
0x0048ff7a      48             dec eax
0x0048ff7b      0048ff         add byte [eax - 1], cl
0x0048ff7e      48             dec eax
0x0048ff7f      0072ff         add byte [edx - 1], dh
0x0048ff82      48             dec eax
0x0048ff83      0028           add byte [eax], ch
0x0048ff85      ff4800         dec dword [eax]
0x0048ff88      58             pop eax
0x0048ff89      ff4800         dec dword [eax]
┌─< 0x0048ff8c      72ff           jb 0x48ff8d
0x0048ff8e      48             dec eax
0x0048ff8f      0038           add byte [eax], bh
0x0048ff91      ff4800         dec dword [eax]
0x0048ff94      68ff480072     push 0x720048ff
0x0048ff99      ff4800         dec dword [eax]
┌─< 0x0048ff9c      e9fe48008b     jmp 0x8b49489f
│   0x0048ffa1      0d148e5c00     or eax, 0x5c8e14
│   0x0048ffa6      85c9           test ecx, ecx
┌──< 0x0048ffa8      743a           je 0x48ffe4
││   0x0048ffaa      a11c8e5c00     mov eax, dword [0x5c8e1c]   ; [0x5c8e1c:4]=0
││   0x0048ffaf      85c0           test eax, eax
┌───< 0x0048ffb1      7421           je 0x48ffd4
│││   0x0048ffb3      8b08           mov ecx, dword [eax]
│││   0x0048ffb5      50             push eax
│││   0x0048ffb6      ff5120         call dword [ecx + 0x20]     ; 32
│││   0x0048ffb9      a11c8e5c00     mov eax, dword [0x5c8e1c]   ; [0x5c8e1c:4]=0
│││   0x0048ffbe      50             push eax
│││   0x0048ffbf      8b10           mov edx, dword [eax]
│││   0x0048ffc1      ff5208         call dword [edx + 8]        ; 8
│││   0x0048ffc4      8b0d148e5c00   mov ecx, dword [0x5c8e14]   ; [0x5c8e14:4]=0
│││   0x0048ffca      c7051c8e5c..   mov dword [0x5c8e1c], 0     ; [0x5c8e1c:4]=0
└───> 0x0048ffd4      8b01           mov eax, dword [ecx]
││   0x0048ffd6      51             push ecx
││   0x0048ffd7      ff5008         call dword [eax + 8]        ; 8
││   0x0048ffda      c705148e5c..   mov dword [0x5c8e14], 0     ; [0x5c8e14:4]=0
└──> 0x0048ffe4      68d0fe4800     push 0x48fed0
│   0x0048ffe9      e802f5ffff     call 0x48f4f0
│   0x0048ffee      59             pop ecx
│   0x0048ffef      c3             ret
│   0x0048fff0      83ec10         sub esp, 0x10
│   0x0048fff3      a100495300     mov eax, dword [0x534900]   ; [0x534900:4]=0x6f1d2b61 ; "a+\x1do\xa0\xd5\xcf\x11\xbf\xc7DEST"
│   0x0048fff8      8b0d04495300   mov ecx, dword [0x534904]   ; [0x534904:4]=0x11cfd5a0
│   0x0048fffe      8b1508495300   mov edx, dword [0x534908]   ; [0x534908:4]=0x4544c7bf
│   0x00490004      89442400       mov dword [esp], eax
│   0x00490008      a10c495300     mov eax, dword [0x53490c]   ; [0x53490c:4]=0x5453
│   0x0049000d      6a00           push 0
│   0x0049000f      68288e5c00     push 0x5c8e28
│   0x00490014      6800070000     push 0x700                  ; 1792
│   0x00490019      894c2410       mov dword [esp + 0x10], ecx
│   0x0049001d      89542414       mov dword [esp + 0x14], edx
│   0x00490021      89442418       mov dword [esp + 0x18], eax
│   0x00490025      c705288e5c..   mov dword [0x5c8e28], 0     ; [0x5c8e28:4]=0
│   0x0049002f      c705248e5c..   mov dword [0x5c8e24], 0     ; [0x5c8e24:4]=0
│   0x00490039      e852f0ffff     call 0x48f090
│   0x0049003e      50             push eax
│   0x0049003f      e8fcf60100     call 0x4af740
│   0x00490044      85c0           test eax, eax
│   0x00490046      740f           je 0x490057
│   0x00490048      6874225400     push str.Could_not_create_DirectInput_keyboard_object_ ; 0x542274 ; "Could not create DirectInput keyboard object!"
│   0x0049004d      e85ef70100     call 0x4af7b0
│   0x00490052      83c404         add esp, 4
│   0x00490055      ebfe           jmp 0x490055
│   0x00490057      a1288e5c00     mov eax, dword [0x5c8e28]   ; [0x5c8e28:4]=0
│   0x0049005c      6a00           push 0
│   0x0049005e      8d542404       lea edx, [esp + 4]
│   0x00490062      68248e5c00     push 0x5c8e24
│   0x00490067      8b08           mov ecx, dword [eax]
│   0x00490069      52             push edx
│   0x0049006a      50             push eax
│   0x0049006b      ff510c         call dword [ecx + 0xc]      ; 12
│   0x0049006e      85c0           test eax, eax
│   0x00490070      740f           je 0x490081
│   0x00490072      6844225400     push str.Could_not_create_DirectInput_device_keyboard_ ; 0x542244 ; "Could not create DirectInput device keyboard!"
│   0x00490077      e834f70100     call 0x4af7b0
│   0x0049007c      83c404         add esp, 4
│   0x0049007f      ebfe           jmp 0x49007f
│   0x00490081      a1248e5c00     mov eax, dword [0x5c8e24]   ; [0x5c8e24:4]=0
│   0x00490086      68c8485300     push 0x5348c8
│   0x0049008b      50             push eax
│   0x0049008c      8b08           mov ecx, dword [eax]
│   0x0049008e      ff512c         call dword [ecx + 0x2c]     ; 44
│   0x00490091      85c0           test eax, eax
│   0x00490093      740f           je 0x4900a4
│   0x00490095      681c225400     push str.Could_not_set_data_format_for_keyboard_ ; 0x54221c ; "Could not set data format for keyboard!"
│   0x0049009a      e811f70100     call 0x4af7b0
│   0x0049009f      83c404         add esp, 4
│   0x004900a2      ebfe           jmp 0x4900a2
│   0x004900a4      8b15248e5c00   mov edx, dword [0x5c8e24]   ; [0x5c8e24:4]=0
│   0x004900aa      57             push edi
│   0x004900ab      6a06           push 6                      ; 6
│   0x004900ad      8b3a           mov edi, dword [edx]
│   0x004900af      e8ecefffff     call 0x48f0a0
│   0x004900b4      50             push eax
│   0x004900b5      a1248e5c00     mov eax, dword [0x5c8e24]   ; [0x5c8e24:4]=0
│   0x004900ba      50             push eax
│   0x004900bb      ff5734         call dword [edi + 0x34]     ; 52
│   0x004900be      85c0           test eax, eax
│   0x004900c0      740f           je 0x4900d1
│   0x004900c2      686c215400     push str.Could_not_set_cooperative_level_for_keyboard_ ; 0x54216c ; "Could not set cooperative level for keyboard!"
│   0x004900c7      e8e4f60100     call 0x4af7b0
│   0x004900cc      83c404         add esp, 4
│   0x004900cf      ebfe           jmp 0x4900cf
│   0x004900d1      a1248e5c00     mov eax, dword [0x5c8e24]   ; [0x5c8e24:4]=0
│   0x004900d6      50             push eax
│   0x004900d7      8b08           mov ecx, dword [eax]
│   0x004900d9      ff511c         call dword [ecx + 0x1c]     ; 28
│   0x004900dc      85c0           test eax, eax
│   0x004900de      750a           jne 0x4900ea
│   0x004900e0      c705308e5c..   mov dword [0x5c8e30], 1     ; [0x5c8e30:4]=0
│   0x004900ea      e851020000     call 0x490340
│   0x004900ef      6a00           push 0
│   0x004900f1      e85a020000     call 0x490350
│   0x004900f6      b940000000     mov ecx, 0x40               ; '@' ; 64
│   0x004900fb      33c0           xor eax, eax
│   0x004900fd      bf808ec200     mov edi, 0xc28e80
│   0x00490102      6820014900     push 0x490120
│   0x00490107      f3ab           rep stosd dword es:[edi], eax
│   0x00490109      e882f3ffff     call 0x48f490
│   0x0049010e      83c408         add esp, 8
│   0x00490111      5f             pop edi
│   0x00490112      83c410         add esp, 0x10
│   0x00490115      c3             ret
│   0x00490116      90             nop
│   0x00490117      90             nop
│   0x00490118      90             nop
│   0x00490119      90             nop
│   0x0049011a      90             nop
│   0x0049011b      90             nop
│   0x0049011c      90             nop
```

- Paste-ready body:

```cpp
// Native: WindowProc_Mouse_48FED0 (placeholder until VERIFIED; BN alias Input_InitKeyboard is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0048FED0-0x0048FF77.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 6.2.0 bytes/disassembly; 168 bytes, 10 blocks.
// Behavior/CFG: mouse button and wheel messages update g_5C8E10 (left), g_5C8E0C (right), g_5C8E18 (middle) and g_5C8E08 (wheel/120);
//           every branch returns 1. The hWnd and lParam arguments are unused.
// Status/limits: TRANSCRIBED; no callees; deps are the runtime-data globals 0x5C8E08/0C/10/18. Runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::int32_t g_5c8e08; // wheel accumulator (zero-fill runtime data)
std::int32_t g_5c8e0c; // right button (zero-fill runtime data)
std::int32_t g_5c8e10; // left button (zero-fill runtime data)
std::int32_t g_5c8e18; // middle button (zero-fill runtime data)

std::int32_t NATIVE_DECL WindowProc_Mouse_48FED0(
    void* hwnd, std::uint32_t message, std::uint32_t wparam, std::uint32_t lparam)
{
    (void)hwnd;
    (void)lparam;
    if (message - 0x201 <= 9)
    {
        switch (message)
        {
        case 0x201: g_5c8e10 = 1; break; // 0x0048FF18: WM_LBUTTONDOWN
        case 0x202: g_5c8e10 = 0; break; // 0x0048FF48: WM_LBUTTONUP
        case 0x204: g_5c8e0c = 1; break; // 0x0048FF28: WM_RBUTTONDOWN
        case 0x205: g_5c8e0c = 0; break; // 0x0048FF58: WM_RBUTTONUP
        case 0x207: g_5c8e18 = 1; break; // 0x0048FF38: WM_MBUTTONDOWN
        case 0x208: g_5c8e18 = 0; break; // 0x0048FF68: WM_MBUTTONUP
        case 0x20A: // 0x0048FEE9: WM_MOUSEWHEEL; wParam high word is the signed wheel delta
            g_5c8e08 = g_5c8e08 + static_cast<std::int16_t>(static_cast<std::uint16_t>(wparam >> 0x10)) / 120;
            break;
        }
    }
    return 1; // 0x0048FF72/0x0048FF77
}

} // extern "C"
```

## 4. Dependency status (grep-verified in the current tree)

| dependency | evidence |
|---|---|
| `0x0048F090 GetInstanceHandle_48F090` | `engine/src/retail/input/native_48f090.cpp` - real native, returns `InstanceHandle` |
| `0x0048F0A0 GetWindowHandle_48F0A0` | `engine/src/retail/input/native_48f0a0.cpp` - real native, returns `g_window_handle` |
| `0x0048F490 WinProc_RegisterHandler_48F490` | `engine/src/retail/core/native_48f490.cpp` - TRANSCRIBED |
| `0x004AF7B0 Debug_Error` | `engine/src/retail/core/retail_error.cpp` + `retail_error.hpp` - real native |
| `0x00509CC0 Joypad_IsAcquired_509CC0` | `engine/src/retail/input/native_509cc0.cpp` |
| `0x00509E90 Joypad_ReleaseAll_509E90` | `engine/src/retail/input/native_509e90.cpp` |
| `0x00509F00 Joypad_InitActionFlags_509F00` | `engine/src/retail/input/native_509f00.cpp` |
| `0x004AF740 DirectInputCreateA thunk` | landed in the prior batch (verify its own native file exists) |
| runtime globals `0xA7B4F0/0xA7B2E0/0xA7B4E0/0xA7B638/0xA7B430/0xA7B63C/0xBCAA00/0xA7B440` | landed in the prior batch per lead; if not, declare once as zero-fill runtime data |
| `0xC28E80` (256-byte keyboard state) | used by 0x48FFF0 and 0x490120; declare once as zero-fill runtime data |
| `0x5C8E08/0C/10/18` | used by 0x48FED0; declare once as zero-fill runtime data (the prior batch landed only 0x5C8E1C and 0xC28F80-81) |

Still missing (this batch): `0x00509A40`, `0x00509980`, `0x00490120`, `0x00490190`, `0x0048FED0`; plus the image constants `0x005349C0` (16-byte IID), `0x00534C38` (0x18-byte DIDATAFORMAT) and `0x0054E8A8` (mutable 64-byte axis table) if the prior batch did not land them under their own image-data natives.

## 5. Landing notes

1. `0x509A40` and `0x509980` are a pair: landing 0x509A40 first is required because 0x509980 pushes its address into EnumDevices; 0x509980 also needs the already-landed 0x509CC0/0x509F00.
2. `0x490120` needs `0x490190`; they are two separate retail VAs and must be two separate natives (one implementation per VA), not one merged switch.
3. `0x48FED0` needs only the four runtime globals and the engine WinProc register; it is independent of the keyboard path.
4. The writable axis table means the port must not mark it `const`: retail clears the value dword on `DIERR_NOTFOUND` (0x80070002) and the deadzone pass reads those cleared flags.
5. `0x509A40` writes `0` into `g_A7B4E0[count]` on every device-level failure but still returns 1, so `EnumDevices` keeps enumerating; only CreateDevice/QueryInterface failures are fatal.

