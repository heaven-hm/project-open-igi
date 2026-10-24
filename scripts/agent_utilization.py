#!/usr/bin/env python3
"""Measure runtime utilization against the project's hard ceiling (3 agents, 30 model requests per rolling minute).

Reports, for a rolling window:
  - rpm            model requests in the window, counted from the harness' own `step/start` events in the
                   session journals (one step === one model request)
  - recent_agents  agent names seen in the window (`agent.name` / `data.agent` on any event)
  - sessions       which journals were read

It only measures; it never changes anything. The Lead acts on the numbers: below target it raises concurrency,
above the ceiling it must not (the ceiling is an owner rule, not a target to exceed).
"""
import argparse
import glob
import json
import os
import subprocess
import time

WORKSPACES = os.path.expanduser('~/.dsh/sessions')


def journals():
    for path in glob.glob(os.path.join(WORKSPACES, '*', '*', 'session.v4.jsonl*.zstd')):
        yield path


def tail_events(path, max_lines=4000):
    """Last `max_lines` events of a zstd journal; older lines are irrelevant to a one-minute window."""
    try:
        out = subprocess.run(['zstd', '-dc', path], capture_output=True, timeout=120)
    except (OSError, subprocess.TimeoutExpired):
        return []
    lines = out.stdout.decode('utf-8', 'replace').splitlines()[-max_lines:]
    events = []
    for line in lines:
        try:
            events.append(json.loads(line))
        except json.JSONDecodeError:
            continue
    return events


def collect(window_seconds):
    now_ms = int(time.time() * 1000)
    start_ms = now_ms - window_seconds * 1000
    steps, agents, sessions = 0, set(), []
    active_journals = 0
    for path in journals():
        events = tail_events(path)
        if not events:
            continue
        in_window = [e for e in events if isinstance(e.get('time'), int) and e['time'] >= start_ms]
        if not in_window:
            continue
        sessions.append(os.path.basename(os.path.dirname(path)))
        window_steps = sum(1 for e in in_window if e.get('type') == 'step/start')
        steps += window_steps
        if window_steps:
            active_journals += 1
        for event in in_window:
            data = event.get('data') or {}
            for key in ('agent', 'agentName', 'target'):
                value = data.get(key)
                if isinstance(value, str) and value:
                    agents.add(value)
            if event.get('type') == 'agent/inbox/spliced':
                for key in ('from', 'sender', 'owner'):
                    value = data.get(key)
                    if isinstance(value, str) and value:
                        agents.add(value)
    rpm = round(steps * 60.0 / window_seconds, 1)
    return {
        'active_journals': active_journals,
        'window_seconds': window_seconds,
        'rpm': rpm,
        'steps_in_window': steps,
        'recent_agents': sorted(agents),
        'sessions_read': sessions,
        'targets': {'agents': 3, 'rpm': 30},
        'below_target': {'agents': len(agents) < 3, 'rpm': rpm < 30},
    }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--window', type=int, default=60)
    parser.add_argument('--json', action='store_true')
    args = parser.parse_args()
    report = collect(args.window)
    print(json.dumps(report, indent=1) if args.json else
          f"rpm={report['rpm']} steps={report['steps_in_window']} journals={report['active_journals']} agents={len(report['recent_agents'])} "
          f"{report['recent_agents']} sessions={len(report['sessions_read'])} "
          f"below_target={report['below_target']}")


if __name__ == '__main__':
    main()
