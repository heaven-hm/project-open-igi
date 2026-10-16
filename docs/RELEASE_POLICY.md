# Branch and release policy

## Branch roles

- `main` is the stable branch. Stable changes enter it after review and validation.
- `develop` is the experimental integration branch. Feature work is integrated here for testing before it is considered stable.
- Feature branches hold isolated work and are never direct release sources.
- This repository currently has a `main` branch. Do not create a parallel `master` branch.

## Version tags

- Experimental prereleases use `vMAJOR.MINOR.PATCH-alpha.N`, `-beta.N`, `-rc.N`, or `-experimental.N`, for example `v0.1.0-alpha.1`. The tagged commit must be reachable from `develop`.
- Stable releases use `vMAJOR.MINOR.PATCH`, for example `v0.1.0`. The tagged commit must be reachable from `main`.
- The release workflow rejects malformed tags, feature-only commits, and prerelease tags that are not on `develop`.

## Current release contents

The GitHub workflow creates source-only GitHub releases. CMake builds the gameplay library, diagnostic probes, and a non-playable macOS `igi.app` shell; it does not build a playable game. Release notes state this limitation. Do not attach retail game assets or executables. Add platform app artifacts only after the corresponding build and gameplay verification exist.

The older `SDK Expansion Release` workflow publishes the separate legacy Win32 native DLL package from `v*-sdk-expansion` tags. It is not the Project Open I.G.I. game release workflow.
