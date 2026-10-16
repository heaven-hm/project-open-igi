# 📋 Changelog

All notable changes to Project Open I.G.I. will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-beta.1] - 2026-10-16

First release of the Project Open I.G.I. native engine — a from-scratch C++20
implementation reverse-engineered with Binary Ninja and Ghidra from the retail
`igi.exe`, based on the project-igi-natives research (4000+ cataloged natives).

### Working in this release (playable demo from `igi.app`)

- **Player** — movement, locomotion, collision.
- **AI** — Level 1 enemy sensing, patrol, and combat.
- **Weapons** — selection, aiming, firing, recoil, reload.
- **Graphs** — retail nav-graph loading and routes.
- **Terrain** — level terrain rendering.
- **Textures** — retail texture decoding.
- **MEF models** — model parsing and skeletal rendering.

None of these are complete, but together they are enough to play a short demo
from `igi.app`.

### Not working yet

- Main Menu
- Magic Objects
- Sound
- Movies
- Cutscenes

### Platform

- macOS `igi.app` (requires SDL2, e.g. `brew install sdl2`).
- Windows and Linux builds are the same codebase; backends are not yet
  published as binaries.
