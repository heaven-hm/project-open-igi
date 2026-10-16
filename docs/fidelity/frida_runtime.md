# RE-Agent and Frida runtime verification

REA CLI: `/Users/haseeb-mir/Documents/Code/rea/scripts/rea.mjs`.
Installed Frida CLI and Python module: version `17.24.0`.
Python interpreter used by REA:
`/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python`.
The default `python3` cannot import this pipx-isolated module.

The available REA `autonomy-trace-frida` adapter currently accepts host-architecture
Mach-O targets. It does not establish x86 Windows guest interception under
CrossOver. IGIProbe remains necessary for that guest's call and memory captures.

The owned native `retail_random_generator_test` has an optional
`--wait-for-instrumentation` stdin gate. Normal CTest execution does not wait.
REA first validates the selected binary's preferred base and instruction prefix,
attaches, and then releases the gate. The `igi_retail_random_draw` hook captured
all eight return values matching the original x86 RNG checkpoints.

Successful evidence:
`ev_a6b6bcd8bf06b7f3cffa9ab7f8de6d21ad5ef43e91c7b89741f6b421050dc913`.
Raw evidence: `/tmp/igi-frida-rng-gadget-gated-evidence.json`.
Configuration: `/tmp/igi-frida-rng-gadget-config.json`.
The trace reports `runtime-observed`, eight calls, no output truncation, owned
child reaped with exit code zero, detached session, and removed private Gadget.

The existing hash-verified Gadget is version `17.23.3`, used with the installed
`17.24.0` client. No dependency was downloaded. Gadget SHA-256:
`448b967b786da64a1f87786a25f23bb4dc34f157b7e3b8b3b5cc13a23136beda`.
It is loaded privately into the authored test and is not distributed with the game.

Retained failed experiments:

- Direct attachment was denied by macOS process access:
  `ev_52833c6fb1154960b5172a57ca744978c7dd629111c34431780eb59c47c73323`.
- The first Gadget attempt used an ungated short-lived test, which exited before
  attachment: `ev_792d055b6d0829e0a37439ab5d1a659b2e3d2448765f9b582dd29359eb5a8111`.

This validates Frida instrumentation of the tested macOS primitive. It is not
full-game retail runtime verification and does not promote any native catalog
entry to an accepted live-IGI status.
