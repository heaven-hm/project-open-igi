# Paired runtime evidence: `0x00498480 Display_ProbeDirectXVersion` and the WinMain DX-tier gate

Date: 2026-10-25. Host: macOS, CrossOver wine (`CX_BOTTLE=open-igi`). Images:
retail `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`;
candidate `igi1.exe` built from `mvp/level1-integration`, 14,249,775 bytes after commit `1cbd7daf`.

This is the first paired runtime observation of a native that this session linked into the candidate: it caught a transcription bug that
structural review had already declared byte-exact.

## Commands (identical arguments and working directory for both)

```
cd /Users/haseeb-mir/Downloads/IGI
CX_BOTTLE=open-igi IGI1MUTEX=1 \
  /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
  <igi.exe | build-win32-no-verify-gate/igi1.exe> window level1
```

Window enumeration and capture used Quartz `CGWindowListCopyWindowInfo` filtered by owner name, then
`screencapture -x -o -l<kCGWindowNumber>` so only the game's own window is recorded (never the full screen).
Tesseract OCR was used to read dialog text, because the screenshot cannot be inspected as an image in this session's model.

## Observation 1 - before the GUID correction (candidate only)

Candidate window list after 14 s:

```
id=12339 name='Error' bounds={'X': 692.0, 'Height': 110.0, 'Y': 438.0, 'Width': 295.0}
id=12338 name=''      bounds={'X': 0.0,   'Height': 500.0, 'Y': 550.0, 'Width': 500.0}
```

OCR of `id=12339`:

```
Error
This application requires Direct version 7.0 or
greater to run.
OK
```

Screenshot: `candidate-before-guid-fix-error-dialog.png` (19,794 bytes, SHA-256 prefix `d89612e6a3ace510`). The missing `X` in
"Direct version" is OCR noise; the retail format string at `0x00541E68` is
`This application requires DirectX version %d.%d or greater to run.`

That dialog is the retail `< 0x700` path at `0x0048F6F6`-`0x0048F71F`, so the candidate's probe returned a tier below `0x700`.

## Observation 2 - retail under the same wine, same level

Retail window list after 14 s:

```
id=12371 name='IGI' bounds={'X': 328.0, 'Height': 668.0, 'Y': 177.0, 'Width': 1024.0}
id=12370 name=''    bounds={'X': 0.0,   'Height': 500.0, 'Y': 550.0, 'Width': 500.0}
```

Retail reached its main window, i.e. its own `0x00498480` returned a tier `>= 0x700` in this environment. Screenshot:
`retail-level1-window.png` (29,622 bytes, SHA-256 prefix `66e4480570754aba`; the window shows rendered 3D content, so OCR finds no text).

**Divergence:** same wine bottle, same arguments, retail passes the gate while the candidate fails it.

## Cause - two GUIDs transcribed with `Data2`/`Data3` byte-swapped

| retail address | retail image bytes | correct fields | wrong fields first written |
| --- | --- | --- | --- |
| `0x00534A60` IID_IDirectDraw2 | `e0 f3 a6 b3 43 2b cf 11 a2 de 00 aa 00 b9 33 56` | Data2 `0x2B43`, Data3 `0x11CF` | `0x432B`, `0xCF11` |
| `0x00534A40` IID_IDirectDrawSurface3 | `00 4e 04 da b2 69 d0 11 a1 d5 00 aa 00 b8 df bb` | Data2 `0x69B2`, Data3 `0x11D0` | `0xB269`, `0x11D0` |

Both `QueryInterface` calls therefore failed, so the ladder stopped before `0x500`/`0x700` and WinMain took the requirement path.
The transcription's own provenance block had claimed the GUIDs were read byte-exactly from `.rdata`; they were not - the listing had been read in
GUID text order instead of decoding the little-endian words in memory order. The four other GUIDs were already correct
(`0x534A30`, `0x534A50`, `0x5348F0`, `0x5348E0`).

## Observation 3 - after the fix

Candidate after the correction: no requirement dialog is created, and the process reaches `QTask_RegisterClasses` (`0x00402930`), where the
transcription guard aborts the startup pass:

```
terminate called after throwing an instance of 'std::logic_error'
  what():  QTask_RegisterClasses (0x00402930): the retail function at this address is the startup init sequence, 155 ordered
  init/open/load calls ending in a tail jump to 0x004CEF30, not a class registrar; it is not transcribed, 146 of its 154 distinct
  callees have no engine implementation, and it stops at AI graph
```

So in this environment the candidate now passes the DX gate exactly where retail does, and stops at the next known gap rather than at a bug.

## Status and limits

- `0x00498480`: `RUNTIME_TESTED` for the tier outcome under CrossOver wine, paired with retail. The COM internals
  (`QueryInterface`/`CreateSurface`/`CoCreateInstance` return values per stage) were **not** traced; the tier value is observed indirectly
  through the gate outcome, not printed. A diagnostic line exists in the file but only on the full non-early-return path, so it did not fire here.
- The gate itself (`0x0048F6E2` call site, `< 0x700` branch) is exercised in both outcomes above: failing branch before the fix, passing
  branch after it.
- Wine's DirectDraw support is not retail Windows DirectDraw. This evidence shows behaviour under one documented runtime, not parity on
  original hardware.
- Timing: the dialog appears between 4 s and 8 s after launch; the abort path is likewise reached within the same window.
