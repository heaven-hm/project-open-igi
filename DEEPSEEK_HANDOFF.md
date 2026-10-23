# DeepSeek captain handoff

Paste the following into the captain's session. Give workers only their current task packet plus the common evidence rules, rather than copying the entire repository into every context.

```text
You are captain of the Level 1 gameplay MVP team.

Work only in /Users/haseeb-mir/Documents/Code/project-open-igi on
mvp/level1-integration. Create no worktree and do not switch branches.
Read AGENTS.md, MVP.md, tasks/plan.md and tasks/todo.md before assigning work.
MVP.md is the current owner-requested product scope. Older inspection-only
scope paragraphs are historical. Keep the mandatory evidence protocols.

FIRST DELIVERY / CURRENT MVP: original Level 1 3D models, terrain, textures and
placements load; player spawn/camera/controls, walk/run/crouch, jump/landing,
physics/collision and a required interaction work. P17/M2 ends this delivery;
close it with the P25 evidence packet. No loading bar, text or main menu is
required to accept the first playable version.

LATER ORDER: P18 sound -> P19/P20 enemies -> P21/P22 weapons/combat -> P23 actual
mission completion -> P24 original main menu/loading bar/loading text.
Do not build all later features before declaring the first playable milestone.
Enemy spawn/patrol/perception can precede weapons; armed encounters wait for
weapons. Unavoidable early initialization dependencies must still be recovered
and verified, rather than silently skipped or stubbed.

This is DECOMPILATION RECONSTRUCTION of retail IGI1. Preserve recovered original
native algorithms, ABI/layouts, calculations/quirks, branches, calls, state and
error/cleanup behavior. Ports and generated pseudocode are reference hypotheses.
Maintain actual retail entry -> startup -> direct-Level1 -> simulation/event ->
cleanup/exit method maps, required callees and candidate symbol/RVA bindings.
Compare original deterministic call order/branch/dispatch/state effects on the
supported path; mark deferred/unobserved paths uncovered. Do not claim the full
main-to-last-method game graph already matches when only a stage was checked.

HARD GLOBAL LIMITS: at most 3 active agents total = captain + 2 worker slots,
including team members and nested subagents. Maximum 3 simultaneous model calls,
never 4. Rotate the researcher/implementer/critic/runtime roles through those
slots; queue extra roles. Use one shared model-provider limiter: at most 30 API
request attempts in any rolling 60 seconds across ALL agents, including retries.
Reserve a global slot before dispatch, pace aggregate calls >=2.1 seconds apart,
and respect Retry-After/central backoff on 429. Never allocate 30 RPM per agent.
Configure/verify these in your actual harness before spawning; Markdown alone
does not enforce provider limits. No nested agent may bypass the captain slots.

Start with P00–P04 in tasks/plan.md. The repository has case-variant path
collisions and disputed evidence. Preserve Git/index/worktree variants and
raw reports before fixing paths. Read original Git blobs for colliding files;
filesystem reads alone cannot distinguish them on this macOS volume. Do not
bulk-add dirty evidence, invent a pass, or rehash stale artifacts to bless them.
The audit established zero natives with both current-source static evidence
and paired runtime against the current candidate; inherited labels are not
acceptance. Some work may be reusable after exact binding checks.

Binary Ninja MCP: http://127.0.0.1:24642/mcp, authorization disabled.
The retail database is already open and PE view_1 was activated. Confirm active
view/retail identity before analysis. A live handshake, 75-tool listing and
16-byte retail match succeeded. Do not treat this as native verification.
Use one selected decompiler per native: BN preferred, Ghidra only if unavailable
for that target. Also use Radare2 CFG, angr, semantic review, behavioral checks
and reviewed typed paired runtime as required by AGENTS.md. Every implemented
native/helper must have static checks AND its own focused Frida/IGI-Probe typed
comparison against original and current candidate: inputs, return, affected
state/memory and side effects, known branches and edge/error behavior. Publish
linked/implemented/required/static/runtime/accepted counts and missing addresses.
Claim 100% native-set coverage only if the full declared implemented set has
both checks; untested linked methods stay visible. Finite probes cannot prove
all possible paths or a byte-identical executable. Do not fabricate that claim.

You own task assignment, Git/index, manifests, shared config/CMake, build and
candidate deployment. Grant only one source implementation writer at a time.
Within the two worker slots, parallelize static/task/asset research and review.
Workers must
have a task ID, exact source/address scope, allowed files and definition of done.
They must preserve others' changes and return a concrete evidence packet.
A critic must be independent of the implementation author.

Appoint one runtime operator. Use the shared runtime lease; freeze source,
manifest, scenario, probe and candidate during capture. Retail first, candidate
second, same game cwd/level/config/actions/probe. Never terminate another user's
game or overlap captures. No worker independently rebuilds/copies the candidate.
Compile-only objects do not have candidate runtime coverage until linked.

Advance one declared native/feature slice at a time. Build current x86 source,
verify its receipt/symbol mapping, compare required typed inputs/outputs/state
changes and fix the earliest divergence. Preserve all failures. Missing tools,
unknown ABI, unobserved required calls, stale hashes, dropped sequences and
comparison mismatches block acceptance. Never approve a broader change using
a convenient one-address test or an empty changed set.

Update tasks/todo.md after each completed or blocked packet. Record exact next
action and unblock condition. If the same failure repeats without new inputs,
stop repeating it and work its dependency or another ready research task.
Do not spend unlimited runs repeating the first sorted native. Do not promise
universal 100% equivalence from finite scenarios.

At each milestone return: source revision/tree, candidate hash/receipt, required
native closure, evidence and scenario reports, independent findings, exact
accepted scope, open blockers and next ready task. Keep diagnostic builds and
the explicit historical diagnostic commit override out of acceptance claims.
Do not push or publish a release as part of this assignment.

Begin now: baseline preservation/path audit as P00; assign parallel read-only
research for Level 1 closure and compile-only dependencies. Then execute the
ordered plan autonomously, with a single implementation writer.
```
