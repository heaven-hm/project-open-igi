# 0x00498480: version-branch diagnostic comparisons

This extends the earlier successful-path packet without changing the catalog
native or adding an engine native. The same probe is used for both images in
each scenario. Retail always runs first; owned-process exit is confirmed before
the candidate starts. Cwd, Level 1/window arguments and observation steps match.
The explicit test overrides live entirely in scripts, never engine flow.

The newly built x86 candidate SHA-256 is
`416215e8c2143bc602d34879525fe4bafdd3cc1cbc4cb83da7ee735947ac518a`.
The native source remains `de8cc4f52b7ffbc01061d717f3e5d27e3e80e40f9d1232ef6c69856e36422bf9`.
`diagnostic-receipt.json` and `build.log` retain its actual compiler inputs.
Every completed comparison validated that receipt before and after capture.

| Scenario | Controlled change | Native outputs and return | Result |
|---|---|---|---|
| none | None | capability 0x700, platform 2, EAX 1 | 48 ordered observations match |
| version-api-zero | Real GetVersionExA runs, then its EAX is overridden to zero | capability 0, platform 0, EAX 0 | 4 observations match |
| version-major-3 | Record real version, then set platform 2 and major 3 | capability 0, platform 2; EAX equals the first output pointer | 4 observations match |
| version-platform-1 | Record real version, then set platform 1 | capability 0x700, platform 1, EAX 1 | 48 observations match |
| version-major-4 | Record real version, then set platform 2 and major 4 | Raw observations retained | Rejected: source freshness changed before final receipt validation |

Version substitutions select branches tested by retail instructions, not guessed
game constants. They are controlled tests on CrossOver, not runs on actual older
Windows installations. The real API return/version and the effective override
are retained. No API is skipped; in the failure case its original structure
writes still occur. This tests the native's result-zero branch, not the OS's
real failure behavior or its GetLastError contract.

The major-3 branch demonstrates why BOOL was wrong. Retail 0x00498503..516 returns
the first output address. Raw EAX values remain in both captures; comparison uses
the observed pointer relationship rather than equating process addresses.
All ordered typed API/COM data and initialized output states remain compared.
Terminal markers bind event count, module references, object identities and the
number of injected changes. Other DLL/COM failure branches are still uncovered.

The final major-4 pair rejects because another agent edited the compiled
`engine/src/retail/core/task_types.cpp`. Both raw sides remain, but no comparison
report/pass was manufactured after the receipt failed. `prior-nt4/` retains a
previous completed match (10 observations) against the older candidate
`9e38a3a69437ca8e2ec741001ecb6c7e5e74932144f215ad9e2edbeff64a4079`;
its build receipt is in `../directx-candidate-20261024-sol/`. It is supplementary
evidence for that image, not a pass for the new image or current shared tree.

Source changes after a completed run do not erase the captured facts, but they
do invalidate whole-current-tree acceptance. No native status is promoted here.
The candidate still lacks the parent's DirectX-failure formatting/exit flow;
later startup can terminate and lose RPC access. A completed native terminal
marker covers only this native. Independent review, a reviewed runtime contract,
strict intake and actual Level 1 gameplay remain open.

Reproduce a single scenario with `scripts/tester/capture_directx_pair.py
--scenario <name>`, the rebuilt candidate/receipt, and a unique output directory.
Retain rejected captures; never weaken freshness checks to reuse a stale build.
