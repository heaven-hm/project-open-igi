# Startup comparator correction — replay of real historical captures

The archived `historical-comparison.json` reported PREFIX_CANDIDATE_MATCH with
56 matches and no mismatches. Its comparator removed the missing DirectX probe,
FPU call and substituted helper from the call stream, collapsed two retail
GetLastError calls to one, and counted declared differences as matches.
That result cannot establish the full startup-prefix fidelity required by AGENTS.

The corrected `scripts/tester/compare_main_prefix_candidate.py` compares complete
ordered call sequences and every GetLastError return without those exclusions.
It also checks both image identities, both retained probe hashes, hook/script
failures, a complete final summary and overflow status. Missing returns do not
establish an exception or its cause. Profile notes remain historical metadata and
authorize no missing-call exemption. Only actual MATCH rows count as matches.

The only testing-build mutex difference remains explicitly recorded; process-local
pointer/handle values still require their stated relations rather than raw address
equality. No new engine code or acceptance exception was introduced.

The same real raw captures/profile/probes are retained here. `replayed-comparison.json`
now reports PREFIX_CANDIDATE_BLOCKED: the candidate final summary was unavailable
after its script was destroyed. It also retains the full call-order discrepancies
and reports the first differing call as retail's second GetLastError versus
candidate CoInitialize. This historical candidate predates the WinMain correction;
the replay tests the checker, not the current candidate or the gameplay MVP.

Replay from this checkout:

```sh
python3 scripts/tester/compare_main_prefix_candidate.py \
  --profile docs/fidelity/evidence/startup-comparator-20261024-sol/candidate-profile.json \
  --retail docs/fidelity/evidence/startup-comparator-20261024-sol/retail-main.json \
  --candidate docs/fidelity/evidence/startup-comparator-20261024-sol/candidate-main.json \
  --probe-root docs/fidelity/evidence/startup-comparator-20261024-sol/probes \
  --out build/replayed-startup-prefix.json
```

Expected exit status: 1. The probe-root option only relocates archived files for
replay; their hashes must still match the raw captures. No artificial passing
fixture or native status edit is used. Infrastructure self-check remains 12/12,
with native_verified=false. Broad prefix value/ABI acceptance still needs current
paired captures and independent review; this correction does not supply them.
