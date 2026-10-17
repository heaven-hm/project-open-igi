# Developer controls checkpoint — 2026-10-17

Windows x86 release build and focused tests passed. The directed-window end-to-end run tested
normal and devmode scenes separately. F9/F10/F12 did nothing in normal mode. In devmode, F9
logged its player-only request, F12 toggled off/on, and F10 loaded Level 2 and presented a second
3D frame. Key-repeat suppression was tested in the focused test. Level 14 wrapping remains
unverified by runtime; it must not be described as a tested retail transition.

F9 does **not** implement working gameplay cheats yet. The diagnostic scene has no connected player
health, ammunition or clip simulation; its flags are requests only. No enemy state is modified.
Developer tooling is an explicit owner-requested exception, not a transcribed game native.

A Level 1 back-buffer image from this binary replaces the Level 1 image in the README's four-image
grid. It shows models/textures, not terrain or a playable mission. Other grid images remain earlier
Task 1 captures. No new terrain screenshot or terrain completion is claimed.

Raw execution record:

```json
{
  "binary_sha256": "127d50f73494fe5133cdccf9f5e2fa7dd1fcae05c940e2561f9c0056619b8cd0",
  "results": [
    {
      "devmode": false,
      "result": "passed"
    },
    {
      "devmode": true,
      "result": "passed"
    }
  ],
  "f9_limit": "request only; player gameplay not connected",
  "capture": "/Users/haseeb-mir/Documents/Code/project-igi-natives/scratch/developer-controls/level1-developer-build.bmp"
}
```

Changes remain uncommitted pending owner manual verification per Agents.md.

## Focused Windows regression results

Nine additional PE32 executables passed under CrossOver: render data, diagnostic log records,
debug command-line parsing, MEF, MTP, compiled QVM, RES, terrain texture metadata and ATTA
accessors. Together with the developer-controls test, ten focused test executables passed.
These are fixture/parser tests, not terrain rendering or full retail behavior comparisons.
Normal `IGIMUTEX` remains enabled; the model-scene runtime checks ran sequentially.
