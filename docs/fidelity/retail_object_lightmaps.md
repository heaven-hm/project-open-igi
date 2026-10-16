# Retail object lightmap ownership

## Retail evidence

Target: `/Users/haseeb-mir/Downloads/IGI/igi.exe`  
SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`  
Provider: Ghidra 12.1.2 through REA

- REA evidence `ev_6417a233e171ac6f8e479719a033cba5dfaffa9dfd97406af4138730d52ce869` analyzes `0x00526330`; the retail type initializer registers `LightmapInfo` and its task fields.
- REA evidence `ev_3642b356a13373d86cbbed973ed88ae81ba571cab7f185119a2f691429612778` analyzes `0x004d0b30`; the renderer registers `DrawLightmapMesh` alongside rigid, bone, and spline mesh modes.
- Current-session REA evidence `ev_c9abc09233cfa58c7dba5ca902f428f09d991dfb9e5d4fe50f7fe03e86ab14a3` analyzes retail `0x004dfa50`. Its body walks attachment records, looks up each attachment name in the magic-object table, and requests the corresponding model.
- Current-session REA evidence `ev_7eac61924e19862e83e4c6f283c368a4f1bbb02962e2a639b68a0bfeec80773c` analyzes `0x004cec10`: it tries the requested model and, when that lookup fails and the name is nonempty, retries the model named `missing`.
- Current-session REA evidence `ev_9553fbe173ffe8c40da0d488f1562a730505e8805c8833f08b9bbedb66c7873c` analyzes `0x004ced50`. The recovered routine increments a model-name suffix and caps its loop at five entries. Its surrounding routine-specific context is retained; this observation alone does not establish that every model type uses this chain.
- Current-session REA evidence `ev_cc94d6bd014d57d44f42d5b94c24f7b040958adcd2cbe135d62270aabcfee43f` analyzes `0x004dc050`; it emits one list entry per LOD only when the resolved model has MEF type 3, then recurses through the task's child list.
- Current-session REA evidence `ev_9b45c7bf0fff795239af29a8e83147df24717b5ce4d4ea74e2f2565620fcc758` analyzes `0x004dc190`; it applies the type-3 gate, emits per-LOD entries, visits its engine-owned child list, and recurses into task children.
- Radare2 independently recognizes `0x004dc050` as a 305-byte, 13-block function, `0x004dc190` as 352 bytes / 16 blocks, and attachment expansion `0x004dfa50` as 3,508 bytes / 68 blocks. These boundary and size checks do not constitute a CFG parity score or proof that the C++ layout is equivalent.
- Vanilla C# `WorldObject` and `LodModelChain` provide the readable reference for depth-first ordering, ordinary attachments before sector attachments, and shared indexing across multiple roots.
- Level 1 `objects.qvm` nests each `LightmapInfo` under a `Building` task. The set name is the final argument of that 13-argument task call; vanilla C# identifies it as `Filename`.
- Level 1 `lightmaps/lightmaps.res` contains 1,308 OLM entries across 22 sets; each of the 22 QVM owners names a set present in that archive.

## C++ implementation and checks

`QvmTaskTree` reconstructs parent-child links from retained QVM call references. It preserves argument order, rejects invalid or multiply-parented task references, and reports each lightmap owner's ordered subtree. `MissionAssets` indexes the Level 1 OLM archive and decodes pages to mission textures.

`MissionAssets::model_lightmap_layout` now builds a depth-first layout for one or more root models. It follows up to five name-incremented LODs, resolves ATTA names through `DefineMagicObj`, substitutes the `missing` model when a mapped child model is absent, and visits non-sector children before children whose MEF has a nonempty `TROP` chunk. A list of roots shares one lightmap index sequence, matching the C# `BuildAll` call shape.

The Level 1 asset probe checks `435_01_1` begins at index zero, verifies its available LOD names and consecutive indices, includes its QVM-resolved attachment, continues indices across repeated roots, and numerically verifies the attached triangle's ATTA position and orientation. It now reconstructs parent links from `QvmInitializer` call references, visits all 22 `LightmapInfo`-to-`Building` owners, and compares every renderable MEF/LOD slot with its OLM entry and page count. On the current retail assets, all 22 owners and 794 renderable MEF/OLM slots pass. The probe also validates all 1,308 archive entries.

This is asset/index verification. The slots and OLM pages are still not bound to placed scene triangles or sampled by the C++ renderer, and no paired retail frame comparison has been made. The complete Level 1 gameplay MVP remains unverified.

```sh
cmake --build /tmp/project-open-igi-fog-build --target mvp_olm_probe -j4
/tmp/project-open-igi-fog-build/engine/mvp_olm_probe /Users/haseeb-mir/Downloads/IGI
ctest --test-dir /tmp/project-open-igi-fog-build --output-on-failure -R 'mvp_(lightmap_task_tree|original_lightmap_task_tree|object_lightmap_format|original_object_lightmap_format)'
```

Complete placement binding, OLM sampling, `DrawLightmapMesh` rendering, and paired retail frame comparison remain outstanding.
