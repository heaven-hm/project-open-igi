# Agents.md — Ground Rules for Project Open IGI

This file is binding for every human and AI contributor. Read it before writing any code.

---

## 1. The one rule

**This project is not a reimplementation. It is a transcription of `igi.exe`.**

We do not design behaviour. We do not write "equivalent" logic. We find the retail method, copy it, and
call it properly. If a method exists in `igi.exe`, the port must contain that method — same name, same
address, same body — and the rest of the code must call it.

> If it is not in `igi.exe`, it does not go in the engine.

---

## 2. What "copy the method" means

A method is considered ported only when **all** of these are true:

1. **Same name** as the retail method (e.g. `Camera_UpdateTransform`, `DynObj_LocalModelsSMef`).
2. **Same address** recorded in the comment (`VA 0x004A0110`) and confirmed with
   `bn_function_info` / Radare2.
3. **Same control flow** — the same branches, loops, early returns.
4. **Same constants and struct offsets** — `0x28`, `0x14`, `0x3DCCCCCD`, FourCC dwords, etc.
5. **Same call order** to the same callees (which must themselves be ported).
6. **No invented helper** standing in for a retail method.

Renaming a method to something descriptive is allowed. Replacing its body with different logic is not.

---

## 3. When a retail method is not self-contained

Retail methods are not standalone. They read engine globals, call helpers, and operate on runtime
state. That is **not** a licence to write your own version.

**Correct procedure:**

1. Decompile the method.
2. List every callee and every global it touches.
3. Port those first, in dependency order, under their own retail names.
4. Then paste the method body and call it.

**Worked example — the camera:**

`Camera_UpdateTransform` (`VA 0x004A0110`) decompiles to eight calls:

```
Sys_PushStack_19(g_bcad7c, g_7205b8);
Sys_PushStack_19(g_bcadac, g_723508);
Sys_PushStack_19(g_bcadac, g_7205bc);
Sys_PushStack_19(g_bcade8, g_723504);
Sys_PushStack_19(g_bcadac, g_7addd4);
Sys_PushStack_19(g_bcade8, g_7205b0);
Sys_PushStack_19(g_bcad7c, g_7ac09c);
Sys_PushStack_19(g_bcadac, g_7234b8);
```

`Sys_PushStack_19` is at `VA 0x004B7450`. The eight globals are matrix slots. So the order is:

1. Port `Sys_PushStack_19` (0x004B7450) and the matrix state it manipulates.
2. Port the camera context fields (`TransContext`, `+0x28` camera position = `0x00BCAB08/10/18`).
3. Port `Transcontext_SetActiveTransContext` (0x00497E70) — already done.
4. Then paste `Camera_UpdateTransform` unchanged and call it from the render loop.

**Never** replace that chain with a hand-written `lookAt()`.

---

## 4. Why this rule exists

The hand-written camera basis in `retail_model_scene.cpp` produced:

- a **mirrored scene** — `right = up × forward` gave `(-1, 0, 0)` for a north-facing camera;
- an **inverted free camera** — forward and strafe axes did not match the world convention.

Neither bug can exist in a method transcribed from `igi.exe`, because retail fixed those conventions once.
Every line we invent is a line that can be wrong. Every line we copy is a line that is already right.

---

## 5. Forbidden

- Hand-rolled matrices, projections, look-at bases, Euler order, quaternion code.
- Guessed constants, offsets, units, or scales.
- "Close enough", "reasonable", "standard FPS values".
- Placeholders, stubs, or TODO bodies presented as implemented.
- Inventing a new method to do what a retail method already does.
- Re-implementing retail behaviour under a new name.

If the retail method is unknown: **stop and reverse-engineer it**. Do not guess.

---

## 6. Verification before a method counts as ported

1. `bn_function_info <address>` returns the expected name (or the mismatch is documented).
2. The ported body matches the decompilation branch for branch, constant for constant.
3. A focused test exists for it.
4. The caller uses it — a ported method that nothing calls is not integrated.
5. Runtime comparison against retail where observable (`igi-probe`, CrossOver).

Catalog names are working labels, not symbols: `igi.pdb` does not exist. The **address** is the fact.

---

## 7. Current state (honest)

Only two modules are literal transcriptions:

| Module | Retail source |
|---|---|
| `retail/core/app_context.cpp` | `App_ContextSetDebugged` 0x48F1A0, `…DebugtextState` 0x48F1E0, `…LightmapsUsed` 0x48F240, `…TerrainLightmapsUsed` 0x48F260 + getters |
| `retail/render/trans_context.cpp` | `Transcontext_SetActiveTransContext` 0x00497E70 |

Everything else in the render path is still hand-written scaffolding and must be replaced:

| To port | Address |
|---|---|
| `Camera_UpdateTransform` | `0x004A0110` |
| `Camera_SetFieldOfView` | `0x004A0340` |
| `Camera_UpdateTransform_2` | `0x004A26C0` |
| `Camera_ApplyWideScreen` | `0x004A23E0` |
| `Camera_UpdateViewCone` | `0x004A2490` |
| `Camera_ResetMatrix` | `0x004A2600` |
| `Sys_PushStack_19` (matrix stack) | `0x004B7450` |
| `DisplayRender_ProcessTransContextPrim` | `0x00497970` |
| `Direct3DRender_DrawRigidMesh` | `0x0049E006` region |
| `DisplayRender_Processmodelsnotconvert` | `0x0049DF60` |
| `Mesh_SkinBones` | `0x0049F700` |
| Terrain loaders (`.HMP/.CTR/.BIT/.LMP`) | `0x004C5F00`, `0x004C6240`, `0x004C6760`, `0x004D14E0` |

`retail_model_scene.cpp` is a diagnostic harness, not a port. It exists to prove data loads; it is not
the deliverable and must not grow new hand-written logic.

---

## 8. Checklist for every change

- [ ] The method exists in `igi.exe`; address recorded and verified.
- [ ] Body copied from the decompilation — no invented logic.
- [ ] Every callee is ported or explicitly listed as missing.
- [ ] No hand-written replacement anywhere in the path.
- [ ] Test added; build green; nothing claimed that was not run.
- [ ] Nothing committed or pushed without manual verification.
