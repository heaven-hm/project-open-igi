# Windows x86 DirectDraw 7 baseline — 2026-10-19

Status: **build verified; CrossOver runtime verification pending**. This checkpoint does not establish a successful DirectDraw window startup or visual/gameplay parity.

## Build artifact

- Repository: `/Users/haseeb-mir/Documents/Code/project-open-igi`
- CMake build tree: `build-win32` (`Unix Makefiles`)
- Toolchain: `i686-w64-mingw32-g++`; CMake identified the target as Windows and the x86 compiler check passed.
- Build command: `cmake --build build-win32 --target open-igi -j4`
- Result: `open-igi` target built successfully.
- Artifact: `build-win32/engine/platform/windows/open-igi.exe`
- SHA-256: `b318726b27ee7de45672b514eeff525ca6af91178b695fb03e0fd691dfc50e74`
- PE inspection: PE32 / `pei-i386`, Windows GUI subsystem, imports `ddraw.dll`.

## Tests and gates

- Host build and CTest: **37/37 passed** using `cmake --build build -j4` and `ctest --test-dir build --output-on-failure`.
- Windows cross-build CTest: no tests are registered in `build-win32`; this is not counted as a test pass.
- `./build.sh`: production gate blocked after angr recovered 91/91 exact entries. The gate reported missing or stale binary-bound semantic/behavioral evidence; no production release acceptance is claimed.
- `git diff --check`: passed at checkpoint time.

## Runtime verification still required

- CrossOver bottle: `open-igi` (configured as Windows 10). CrossOver version and launch logs have not yet been captured for this artifact.
- Required command and working directory: launch `/Users/haseeb-mir/Downloads/IGI/open-igi.exe window level1` from `/Users/haseeb-mir/Downloads/IGI`.
- Retail `igi` is currently open in CrossOver. Do not launch OpenIGI alongside it because both use `IGIMUTEX`.
- Frida server process/listener was observed by `lsof` on `127.0.0.1:27042`, but the shell-side Frida check could not connect. No native trace was captured.
- No OpenIGI game-window screenshot, application log, CrossOver launch stdout/stderr, or paired retail/OpenIGI run is included yet.

## Next acceptance evidence

After the retail process is closed, launch the artifact with the command above, confirm the DirectDraw 7 window appears, record CrossOver version and process output, capture the actual game window, and retain the application log. Then rerun the runtime check without launching retail and OpenIGI concurrently.
