# External symbol table: igi-re-names.tsv

**Source:** `https://github.com/slickdaddyy/IGI-RE/blob/main/symbols/names.tsv`, fetched 2026-10-25,
raw length 285048 bytes, **7068 symbols**, columns `VA \t Name \t source \t kind`.

**This is EXTERNAL, UNTRUSTED DATA.** It is a naming *candidate* source, not evidence about the
binary. Per the naming rule an unverified native keeps its address-suffixed placeholder, so these
names belong in provenance **notes** and in task descriptions, not in function names until each
native's own evidence supports the rename.

**Distribution:** source `manual` 6956, `igipatch` 112. Kind: `entry` 5391, `data` 1565,
`contain` 112.

## What it resolves

| set | resolved |
|---|---|
| the 68 frontier symbols | **66** (only `TerrainLightMap_Init`, `TextureModifier_Init` absent - both already tree names) |
| tree files carrying a placeholder name | **515 of 519** |

## Corroboration before use

Four bodies were read and compared with their table name. All four matched, and three are
independent corroborations because the body **registers the string the name implies**:

| VA | table name | body evidence |
|---|---|---|
| `0x004EDCE0` | `PhysicsObjType_Open` | registers the string `"DefinePhysicsObjType"` |
| `0x0052CA70` | `PushButton_Init` | registers the string `"PushButton"` |
| `0x004F1E30` | `ParamEnum_InitTypes` | registers `EnumInt32`, `EnumReal32`, `EnumString16/32/256` |
| `0x004B9BE0` | `Expr_Free` | is **recursive** - `call 0x4b9be0` inside its own body, the shape of a tree free |

A name that matches the literal string a routine registers is not a coincidence. That is why this
table is being used at all - and it is still not verification of any individual identity.

## What it must not be used for

- **Not** a bulk rename of the 515 tree files. Each rename needs that native's own evidence.
- **Not** a substitute for reading the body. A plausible name on an unread body is the
  "name is metadata about the file, not evidence about the file" failure.
- **Not** a source of arity, calling convention or behaviour. It carries names only.
