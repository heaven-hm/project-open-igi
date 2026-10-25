# Automated per-native fidelity audit

`scripts/native_fidelity_audit.py` replaces hand verification. Run it, read the flags; do not
inspect natives by eye.

    python3 scripts/native_fidelity_audit.py --json docs/fidelity/evidence/fidelity-audit-all.json
    python3 scripts/native_fidelity_audit.py --va 0x004A5BB0
    python3 scripts/native_fidelity_audit.py --handlers-only

## What it detects automatically

1. **True function extent** by reachable-set traversal, not a linear sweep. A sweep that stops at
   the first `ret` reports 30 instructions for `0x004A5BB0`, which has three epilogues and is
   really 66.
2. **CFG match** by canonicalised graph comparison. Retail and candidate live in different
   address spaces (`0x004A5BB0` vs `0x00000000`), so comparing block ADDRESSES always yields
   jaccard 0; blocks are renumbered by a deterministic DFS from the entry first.
3. **Divergence classification**, so a compiler artifact is not reported as a defect:
   `MATCH`, `EPILOGUE_MERGE`, `TAIL_CALL_PRESERVED`, `DIVERGENT`.
4. **The data-as-code defect.** For every declared symbol the candidate address-takes, it decodes
   the retail bytes and reports whether retail ever emits an `E8` to that address. This is the
   defect that invented 55 phantom link failures for the AI-action handlers.
5. **Ambiguous extents**, reported with both readings rather than guessed.

## The tail-jump rule, and why it reports ambiguity

The rule started as "a forward unconditional `jmp` whose target is not a conditional-branch
target is a tail call". Four independently measured cases show that is wrong:

| case | truth | naive rule | why the naive rule fails |
|---|---|---|---|
| `0x004A5BB0` -> `0x004AA0A6` | tail call | correct | |
| `0x004C7560` -> `0x004C759C` | **internal loop jump** | wrong (20 vs 46) | jmp targets `0x004C759C`, the `jne` at `0x004C75D7` targets `0x004C7598` - four bytes apart |
| `0x005183D0` -> `0x00519710` | tail call | correct | |
| `0x00508F80` -> `0x00508FA0` | tail call, 32 bytes ahead | correct | |

Every simple discriminator was tested and each one fails on at least one case:

- target is a conditional-branch target -> fails on `0x004C7560` (4 bytes apart)
- target unreachable when this jmp is blocked -> fails on `0x004C7560` (blocking disconnects the loop)
- target is a manifest function entry -> fails on `0x005183D0` (`0x00519710` is not one)
- target is an `E8` call target -> fails on all four
- distance between site and target -> fails (32 bytes real tail call, 60 bytes internal jump)

**This is the function-boundary problem and it is not decidable from local byte evidence.** The
tool therefore does not decide: it applies a default and records `ambiguous` with the alternative
size, so only flagged cases need corroboration from the caller set, the target's own `ret`, or a
tree/manifest record. Recording the gap is required; guessing is not permitted.

Validated on all four known cases: three defaults correct, and the one wrong default is flagged.

## Corrected by flow-analyst, verified independently here

The `0x004C7560` counterexample is theirs. It was re-verified byte-level before being adopted:
`0x004c7596` is `jmp 0x4c759c` and `0x004c75d7` is `jne 0x4c7598`.
