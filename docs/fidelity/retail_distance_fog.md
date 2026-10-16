# Retail distance fog

## Implemented behavior

The renderer previously used an unverified linear fade beginning at 140 metres,
ending at 1,040 metres, and tinting every world surface gray. The scene loader now
reads the lowest-time `GlobalLightKeyframe` from the decoded `objects.qvm` call
tree. It stores separate terrain and object fog values. Terrain triangles use
the terrain band; static models, doors, and actors use the object band. The
first-person weapon remains unfogged.

The retail calculation observed in `igi.exe` is:

```text
published_density = clamp(authored_density * 0.0005, 0, 16384)
visibility         = exp(-view_depth_game_units * 0.001 * published_density)
output              = visibility * surface + (1 - visibility) * fog_colour
```

This engine stores world distance in metres and uses 4096 game units per metre,
so the loader stores `published_density * 0.001 * 4096` as density per metre.
The software rasterizer evaluates visibility at each clipped vertex and
interpolates those factors across the triangle. This follows the retail
per-vertex fog path. It does not claim pixel parity with Direct3D 7: there is no
paired retail frame capture in this verification.

The current C++ mesh path has no object fog-category field. It uses category 1
for nonterrain objects. Level 1's authored object categories 1 through 4 share
the same fog values, so this limitation does not change the Level 1 values.
Other levels and keyframe interpolation/night-vision changes are not covered by
this implementation slice.

## Retail evidence

Target: `/Users/haseeb-mir/Downloads/IGI/igi.exe`, SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

- REA/Ghidra procedure `0x527520`, evidence
  `ev_a760501c6bab41b5c6758b097bbc17a815e61a8683cd4cb8864e8cfb028d470c`,
  passes the selected keyframe density multiplied by `0.0005` to the fog state
  setter.
- REA/Ghidra procedure `0x50FD20`, evidence
  `ev_eccf676188683fd8d82c7b0c2c862fcbf759fecc6cdf0e236bf89f3611c8a4a0`,
  stores the fog colour and density and clamps each colour channel to `[0, 1]`.
- REA/Ghidra procedure `0x512C30`, evidence
  `ev_0a3ed618a50f5bf3ccf6e67ed6347d1025e3ff08c538fb5f81601bdc29b95333`,
  computes the exponential visibility factor from view depth, the `0.001`
  distance scale, and the published fog density, then places the factor in the
  vertex specular alpha.
- `missions/location0/level1/objects.qsc` declares the fog band parameter
  ordering and Level 1's lowest keyframe at time 12. Terrain and object fog
  colors are `(0.576923, 0.538462, 0.480769)` with authored density `1.89`.
  This decoded script is corroborating asset data; the binary analysis above is
  the evidence for the runtime formula.

## Verification

On 2026-10-17, AppleClang 16 Release builds of `mvp_renderer_probe` and
`mvp_real_scene_probe` succeeded in `/tmp/project-open-igi-fog-build`.

- `mvp_renderer_depth_clipping` passed synthetic terrain-fog and object-fog
  tint assertions and confirmed serial/parallel render equality.
- `mvp_original_level_gameplay` passed against the installed Level 1 assets.
  The probe asserts both Level 1 fog colors and the derived density
  `0.00387072` per metre. The runtime log selected keyframe time `12.000000`
  and reported density `0.003871` per metre for both bands.
- Clang static analyzer checks (`clang-analyzer-*`) passed on
  `mission_scene.cpp` and `scene_renderer.cpp` with a CMake compilation
  database.

These checks establish that the C++ loader and renderer consume the known Level
1 values and apply both fog bands consistently. A matched CrossOver capture,
retail memory observation of the selected keyframe at runtime, and a pixel-level
comparison against the retail Direct3D 7 frame are still outstanding. Full game
parity remains unverified.
