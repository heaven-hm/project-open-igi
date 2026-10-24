#!/usr/bin/env python3
# Verification gate (AGENTS.md 10.5/10.6 + verification-before-completion).
# Blocks non-verified native transcriptions from being compiled/built.
# A native .cpp/.hpp must carry the 10.7 five-line provenance comment with a
# `Status/limits:` line whose status ranks >= the threshold.
# Default threshold = STRUCTURALLY_VERIFIED: a native must be structurally
# verified against igi.exe before it is allowed to compile/run.
import argparse, re, sys, subprocess, pathlib

RANK = ["ANALYZED", "TRANSCRIBED", "STRUCTURALLY_VERIFIED", "BUILD_VERIFIED",
        "RUNTIME_TESTED", "VERIFIED", "VERIFIED_WITH_DOCUMENTED_SCOPE"]
STATUS_RE = re.compile(r'(?:Status\s*/\s*limits|Status)\s*[::\-]?\s*([A-Z][A-Z_]+)', re.I)


def parse_status(head):
    for ln in head:
        m = STATUS_RE.search(ln)
        if m:
            return m.group(1).upper()
    return None


def rank(s):
    return RANK.index(s) if s in RANK else -1


def staged_files(root):
    r = subprocess.run(
        ["git", "-C", str(root), "diff", "--cached", "--name-only", "--diff-filter=ACM",
         "--", "*.cpp", "*.hpp"],
        capture_output=True, text=True)
    NAT = ("engine/src/retail", "scripts/GameNatives")
    out = []
    for p in r.stdout.split():
        if not p.strip():
            continue
        if pathlib.Path(p).suffix != ".cpp":       # gate native FUNCTIONS, not .hpp scaffolding
            continue
        if not any(n in p for n in NAT):           # native transcription trees only
            continue
        out.append(pathlib.Path(root) / p)
    return out


def tree_files(tree):
    return [p for p in pathlib.Path(tree).rglob("*.cpp")] + \
           [p for p in pathlib.Path(tree).rglob("*.hpp")]


def main():
    ap = argparse.ArgumentParser(description="Enforce igi.exe verification gate on native transcriptions.")
    ap.add_argument("--root", default=".")
    ap.add_argument("--staged", action="store_true",
                    help="Gate only files staged in git (non-breaking for existing code).")
    ap.add_argument("--tree", help="Gate every native under this tree.")
    ap.add_argument("--threshold", default="STRUCTURALLY_VERIFIED",
                    choices=RANK, help="Minimum verification status required to build.")
    args = ap.parse_args()
    thr = rank(args.threshold)

    files = staged_files(args.root) if args.staged else tree_files(args.tree) if args.tree else []
    files = [f for f in files if f.is_file()]

    blocked = []
    for f in files:
        head = f.read_text(errors="replace").splitlines()[:8]
        st = parse_status(head)
        if st is None or rank(st) < thr:
            blocked.append((str(f), st or "MISSING_STATUS"))

    if blocked:
        print(f"native_gate: BLOCKED {len(blocked)} non-verified native(s) "
              f"(threshold={args.threshold}):")
        for f, s in blocked[:80]:
            print(f"  - {s:32s} {f}")
        if len(blocked) > 80:
            print(f"  ... and {len(blocked)-80} more")
        print("\nFix: add the 5-line provenance comment (10.7) with a "
              "'Status/limits:' line >= STRUCTURALLY_VERIFIED plus dual-engine "
              "Ghidra+r2 evidence. See scripts/GameNatives/igi_*.hpp scaffolding.")
        return 1
    print(f"native_gate: OK — {len(files)} native(s) >= {args.threshold}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
