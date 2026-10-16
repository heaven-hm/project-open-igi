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
