# Level 1 playable demo plan

The full scope and mandatory order are defined in MVP.md and plan.md.

1. Build and verify the Windows x86 DirectDraw 7 baseline.
2. Render Level 1 3D models at transforms produced by the retail OBJECTS.QVM path.
3. Render Level 1 terrain with its original textures.
4. Verify every object's placement/material/texture, capture screenshots, and wait for owner review.
5. Implement the QVM/task loop, player, physics and world interaction, AI/navigation, health/damage, weapons, gameplay animations/effects, and sound in the order in plan.md.
6. Build, run focused tests, capture a game-window screenshot for each visible increment, create small local commits, and provide each checkpoint for manual testing.

The target is Windows x86 with DirectDraw 7 / Direct3D 7. CrossOver on macOS is only the local Windows runtime test environment. The repository does not yet have a playable Level 1 scene; see the current-state audit in plan.md.
