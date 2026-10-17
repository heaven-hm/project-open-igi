# Level 1 renderer performance correction

2026-10-16. The manually delivered `9a3c71f` app was a Debug build.
The CPU rasterizer was profiled after the owner reported severe lag.

## Controlled measurements

Original assets, Level 1, high geometry/texture settings, 1024x768, 12 rendered
headless benchmark frames per run, fixed 30 Hz simulation, warm shared asset
cache. Three sequential runs of each variant, without overlapping benchmarks:

| Variant | Render-loop seconds, three runs | Median FPS |
| --- | --- | ---: |
| Debug serial | 4.508440, 4.505974, 4.492575 | 2.66 |
| Release serial | 0.613186, 0.640900, 0.616202 | 19.47 |
| Release parallel bands | 0.333768, 0.330304, 0.328937 | 36.33 |

Release optimization alone improves this measurement 7.31 times. Four raster
bands add a measured 1.87 times improvement over Release serial. This is a
headless rendering measurement, not a comparison with retail FPS or a promise
of this rate in every camera position/window size.

An owned Release process was sampled for three seconds with macOS `sample`.
Most main-thread samples were in SceneRenderer rendering/rasterization.
Raw report: `/tmp/igi-release-cpu-sample.txt`.

## Rendering-only change

The default renderer uses up to four standard C++ workers. Each owns its pixel
and depth buffers, preserves the complete triangle order, and rasterizes a
disjoint horizontal band. Bands are copied into the final framebuffer after
all workers complete. Scene and simulation state are read-only during rendering.
Exceptions from workers propagate through futures to the existing application
error handler. A one-worker constructor retains the serial execution path.
No geometry density, texture filtering, lighting formula, animation, AI tick
or simulation timestep was reduced to obtain the improvement.

## Correctness evidence

At the same fixed Level 1 simulation tick, serial-before and parallel-after
BMP files are byte-identical, SHA-256:
`4a8689e3bbe08cfe48870a8e48edab9d6abb1d140d3cc9f3757ef9162c87c955`.
State CSV files are also byte-identical, SHA-256:
`2d67ab655acbedfb468f38314efc8d6ec85e4c445b1510a6244f89a9ae12b3ed`.
Artifacts and run logs: `/tmp/igi-lag-comparison/`.

The renderer regression now compares every serial/parallel pixel and visible
triangle count at an odd 641x481 viewport with three bands, including depth,
near clipping and firing effects. Missing texture errors still propagate.
The Release renderer regression passes. Full functional/sanitizer suites
must be rerun for this renderer change; earlier 19-test and sanitizer results
belong to the preceding gameplay checkpoint, not this implementation.

The current backend is still a CPU renderer. This improves its measured speed;
it does not provide hardware rendering or establish complete retail visual parity.

## Retail-derived terrain LOD correction

2026-10-17. The C++ Ultra preset previously selected terrain octree depth 20 with
hand-written distance bands. The C# terrain traversal reference sets a maximum
exponent of 16 and chooses refinement with a camera-relative integer distance
metric. Its presets separate the retail settings (Low 96, Medium 129, Vanilla
163) from the C# Enhanced, High, and Ultra values (220, 320, 450). C# Vanilla
derives its distance from field of view; this C++ Vanilla value uses the retail
4:3 base and is not yet field-of-view scaled. Terrain node bounds are retained
individually for per-frame view culling. Initial terrain selection still uses the
player spawn; it is not yet re-traversed as the player moves.

### Evidence and limits

- REA/Ghidra 12.1.2 analyzed the retail `igi.exe` with SHA-256
  `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
- `0x004D14E0` (`FUN_004d14e0`) is the retail terrain traversal, body
  `0x004D14E0–0x004D2534`; REA evidence `ev_ef3464c485f0f03db0ae5d1e6fd2053f23d41c1c38dc821578c4daa1d1bb7394`
  and pseudocode `ev_cb0e338a7f00cdbf3f554a26db3af0089569a279c7f9d2fa0a4f9dfdf6fb0b1d`.
- `0x004D2540` initializes the traversal from camera state and the view-distance
  setting; evidence `ev_599770868c131906fe3957702683ec5f8f0f15198ff8b404bfec8680a2c1ce10`.
- `0x00512C30` is the retail terrain render path that consumes visible terrain
  chunks; evidence `ev_3cdab5ee9745c912b8c6b7a4a708b85b51acacdb788bcbbfed9568ce4e95a370`.
- The reference implementation is
  `open-igi/src/OpenIGI.Game/World/TerrainTraversal.cs`; the player ground query
  uses exponent 16 in `TerrainLeafQuery.cs`. These C# files support the porting
  details; REA observations independently establish the retail binary functions.

### Level 1 Ultra measurement

Same Release binary configuration, original Level 1 assets, warm cache, 1280x960,
three headless benchmark frames, and Ultra's default anisotropy:

| Revision | Terrain triangles loaded | Terrain triangles submitted | Chunks culled | Mean FPS |
| --- | ---: | ---: | ---: | ---: |
| Depth-20 distance bands | 5,655,922 | 1,501,893 | 3,981 / 5,420 | 3.01 |
| Depth-16 retail-derived metric | 33,780 | 8,698 | 3,036 / 4,100 | 7.29 |

The benchmark improved 2.42 times. This is not a comparison against retail FPS
or a proof of pixel parity. The full CTest suite had 27 functional tests pass;
`retail_engine_bit_exact` remains unverified because the required retail and port
capture files are not configured. The renderer pixel-equality probe passes for
the culling-on/off synthetic scene.
