# Lightmap stack entry: bounded verified scope

Native 0x004F82B0, TerrainLightMap_Init_2. The engine source, selected Binary Ninja analysis, Radare2 graph/bytes, independent source review and typed paired Frida fixtures agree for nonaliasing valid contexts with mode bytes 0/1, enabled values 0/1/-1 and stack counts 0/19. All 24 input combinations compare return value/category, full 384-byte context, stack counter, 20 DWORD stack entries and mode bytes. Fixtures execute before the primary game thread resumes, restore and verify globals, then retail receives Escape. The source-bound clean-built x86 candidate is staged beside retail assets.

This scope excludes aliases, invalid pointers, negative/overflow counters, broader stack storage, concurrent access and natural scene/game-loop integration. It does not verify terrain rendering or the MVP. Evidence is in docs/fidelity/evidence/lightmap-20261021/.
