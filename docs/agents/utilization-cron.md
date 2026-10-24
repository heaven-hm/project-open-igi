# Utilization cron: keep the team at its allowed ceiling

Owner instruction (2026-10-25): a cron job must check how many agents are running and how many model requests are being sent per
minute, and when either is below target, raise it: **agents >= 3** and **RPM = 30**.

## The numbers are a ceiling, not a floor to exceed

The binding project rule is *at most 3 active agents total (captain + 2 worker slots)* and *a shared 30 model requests per rolling
minute including retries*. So "increase to 30 RPM" means **run at the ceiling**, never above it: the cron raises utilization when the
team is idling, and must not push past 3 agents or 30 requests/minute, because that is what the owner rule forbids.

## Measurement

`scripts/agent_utilization.py` reads the harness' own journals under `~/.dsh/sessions/*/*/session.v4.jsonl.zstd` and counts
`step/start` events in a rolling window - one step is one model request. It reports:

- `rpm` - requests per minute in the window
- `active_journals` - journals that produced steps in the window (one per active agent)
- `recent_agents` - agent names seen in inbox events
- `below_target` - `{agents, rpm}` booleans against 3 / 30

`list_agents` stays authoritative for who is actually running; the script is the request-rate meter that the harness does not otherwise
expose.

## Cron

DSH schedule `Check agents and RPM targets` fires every 120 seconds and prompts the Lead to:

1. run `python3 scripts/agent_utilization.py`,
2. call `list_agents`,
3. if `active agents < 3` - resume or dispatch worker tasks that are safe to run in parallel (disjoint write scopes), never a third
   worker slot,
4. if `rpm < 30` - start the next independent step (dispatch a research/verification task, run a build or a capture) so the pipeline is
   not idle,
5. if both are at/over the ceiling - do nothing and report "at ceiling"; never exceed it.

A cron tick that finds nothing to do must say so plainly rather than invent work.
