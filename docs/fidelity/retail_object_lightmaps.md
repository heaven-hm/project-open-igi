# Retail object lightmap ownership

## Retail evidence

Target: `/Users/haseeb-mir/Downloads/IGI/igi.exe`  
SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`  
Provider: Ghidra 12.1.2 through REA

- REA evidence `ev_6417a233e171ac6f8e479719a033cba5dfaffa9dfd97406af4138730d52ce869` analyzes `0x00526330`; the retail type initializer registers `LightmapInfo` and its task fields.
- REA evidence `ev_3642b356a13373d86cbbed973ed88ae81ba571cab7f185119a2f691429612778` analyzes `0x004d0b30`; the renderer registers `DrawLightmapMesh` alongside the rigid, bone, and spline mesh modes.
- Level 1 `objects.qvm` nests each `LightmapInfo` under a `Building` task. The set name is the final argument of that 13-argument task call; vanilla C# identifies that field as `Filename` through `WorldScene.LightmapFilenameParameter`.
- Level 1 `lightmaps/lightmaps.res` contains 1,308 OLM entries across 22 sets; each of the 22 QVM owners names a set present in that archive.

## C++ implementation and checks

`QvmTaskTree` reconstructs parent-child links from the initializer's retained call references. It preserves argument order, rejects invalid or multiply-parented task references, and reports each lightmap owner's ordered subtree. `MissionAssets` indexes the Level 1 OLM archive and decodes pages to mission textures.

```sh
cmake --build /tmp/project-open-igi-fog-build --target mvp_lightmap_task_tree_probe mvp_olm_probe -j2
ctest --test-dir /tmp/project-open-igi-fog-build --output-on-failure -R 'mvp_(lightmap_task_tree|original_lightmap_task_tree|object_lightmap_format|original_object_lightmap_format)'
```

The tests validate QVM hierarchy, owner-to-archive set correspondence, OLM structure and page decoding. They do not yet assign per-model lightmap indices through LOD and recursive MEF attachments, bind those pages to placed geometry, or render `DrawLightmapMesh` behavior. Object lightmap rendering is therefore still incomplete and has no paired retail frame comparison.
