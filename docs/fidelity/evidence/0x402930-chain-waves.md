# The 0x00402930 startup chain: what blocks it, measured

0x00402930 is the project-owned fail-closed boundary: it is the retail startup init sequence, 155 ordered
init/open/load calls ending in a tail jump to 0x004CEF30, and the candidate cannot run past it. Until the chain is
transcribed the candidate executes no game native at all, so no paired retail/candidate runtime comparison can succeed
and no native can be promoted to VERIFIED on both-sides evidence.

Chain calls: 155 (154 distinct). In catalog: 22. Missing: 133.

Landability is measured per native's own body, not by distance in the call graph: a chain entry is landable when every
callee and every code-address immediate its body uses is already transcribed (AGENTS.md section 21.7).

## Landable now with no new dependency: 1

- 0x453550: 68 instructions, 214 bytes

## One to four missing dependencies (the next wave): 21

- 0x488620: 56 instructions, 201 bytes; missing calls ['0x4f0ff0']; missing code immediates ['0x4886f0', '0x488770', '0x488780']
- 0x415070: 30 instructions, 109 bytes; missing calls []; missing code immediates ['0x4150e0', '0x415380', '0x415450']
- 0x486900: 46 instructions, 170 bytes; missing calls []; missing code immediates ['0x4869b0', '0x486cd0', '0x486d70', '0x486f30']
- 0x485A40: 36 instructions, 132 bytes; missing calls []; missing code immediates ['0x485b20', '0x485b40', '0x486010', '0x486020']
- 0x4851B0: 31 instructions, 110 bytes; missing calls []; missing code immediates ['0x424280', '0x485220']
- 0x484F30: 160 instructions, 527 bytes; missing calls ['0x485170', '0x4b8a80']; missing code immediates []
- 0x47E700: 34 instructions, 128 bytes; missing calls ['0x416930', '0x4169d0']; missing code immediates ['0x47e790', '0x47e820']
- 0x47CF20: 10 instructions, 40 bytes; missing calls ['0x4f1cd0', '0x4f1db0']; missing code immediates []
- 0x413DF0: 99 instructions, 407 bytes; missing calls ['0x4f1cd0', '0x4f1db0']; missing code immediates ['0x413f90']
- 0x472A70: 27 instructions, 103 bytes; missing calls ['0x460c10', '0x477bf0']; missing code immediates ['0x472ae0', '0x472ca0']
- 0x46DF10: 22 instructions, 80 bytes; missing calls ['0x4f45f0']; missing code immediates ['0x46df70']
- 0x46AB60: 34 instructions, 125 bytes; missing calls []; missing code immediates ['0x46abe0', '0x46ac30', '0x46ae20', '0x46ae30']
- 0x468E20: 31 instructions, 110 bytes; missing calls []; missing code immediates ['0x468e90', '0x469110']
- 0x4688B0: 31 instructions, 110 bytes; missing calls []; missing code immediates ['0x468920', '0x468bc0']
- 0x4533B0: 56 instructions, 182 bytes; missing calls []; missing code immediates ['0x453470', '0x453480', '0x453490']
- 0x449D30: 69 instructions, 274 bytes; missing calls ['0x4b4720', '0x4b4770']; missing code immediates ['0x449e50', '0x44a1a0']
- 0x438390: 42 instructions, 162 bytes; missing calls ['0x4169d0']; missing code immediates ['0x438440', '0x4387b0', '0x46fb50']
- 0x436B10: 102 instructions, 321 bytes; missing calls []; missing code immediates ['0x436c60', '0x436d10', '0x4374c0', '0x4374d0']
- 0x425C80: 78 instructions, 262 bytes; missing calls []; missing code immediates ['0x425d90', '0x425de0', '0x426590', '0x4265a0']
- 0x424B90: 33 instructions, 120 bytes; missing calls []; missing code immediates ['0x424c10', '0x424c40', '0x424ca0']

## Everything else: 111 natives, each needing five or more dependencies.

## Consequence for the work order

Working the chain in retail order is correct but not sufficient: most entries reference handler addresses and helper
natives that are themselves untranscribed, so the chain must be filled by dependency waves - a native whose body only
uses already-landed callees and already-landed code addresses first, then the natives that reference it. The
one-per-round habit therefore has to be one *wave* per round, not one chain index per round, or the boundary never
clears.
