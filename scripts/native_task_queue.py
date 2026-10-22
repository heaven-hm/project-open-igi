#!/usr/bin/env python3
"""Persistent native task queue for autonomous reverse-engineering agents (fail closed).

Capability implemented here: central task scheduling primitives.

  - persistent JSON task state
  - atomic claim (one active lease per native; duplicate execution prevention)
  - lease heartbeat
  - release
  - crash recovery (expired leases return task to queued)
  - status updates / listing

This module schedules and tracks task state only; it does not attest runtime parity and does
not execute natives itself. Acceptance authority remains scripts/native_gate.py and
scripts/advanced_reverse_engineering_pipeline.py (fail closed).

Design constraints honored here:
  - one native is worked by one agent at a time (single active lease)
  - crash recovery via lease expiry
  - nothing is marked verified without the strict gate / evidence pipeline
"""
from __future__ import annotations

import argparse
import fcntl
import json
import os
import tempfile
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

SCHEMA_VERSION = 1
KIND = "igi-native-task-queue"
STATUSES = ("queued", "claimed", "in_progress", "passed", "failed", "deferred", "escalated", "done")
ACTIVE_STATUSES = ("claimed", "in_progress")
TERMINAL_STATUSES = ("passed", "done")


def now_epoch() -> float:
    return time.time()


def iso_utc(epoch: float) -> str:
    return datetime.fromtimestamp(epoch, timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def new_task(address: str, name: str = "", dependencies: list[str] | None = None) -> dict:
    return {
        "address": address,
        "name": name,
        "status": "queued",
        "agent": None,
        "model": None,
        "lease_id": None,
        "lease_expires_at": None,
        "started_at": None,
        "updated_at": now_epoch(),
        "attempts": 0,
        "last_error": None,
        "dependencies": list(dependencies or []),
        "verification": {},
        "git_commit": None,
    }


def load_state(path: Path) -> dict:
    tasks: dict[str, dict] = {}
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        data = {}
    raw = data.get("tasks") if isinstance(data, dict) else None
    if isinstance(raw, dict):
        for address, task in raw.items():
            if isinstance(task, dict) and "address" in task:
                tasks[address] = task
    return {"schema_version": SCHEMA_VERSION, "kind": KIND, "tasks": tasks}


def save_state(path: Path, state: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, tmp = tempfile.mkstemp(dir=str(path.parent), prefix=path.name + ".", suffix=".tmp")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as handle:
            json.dump(state, handle, indent=2)
            handle.write("\n")
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(tmp, path)
    except OSError:
        try:
            os.unlink(tmp)
        except OSError:
            pass
        raise


def _lock(path: Path):
    path.parent.mkdir(parents=True, exist_ok=True)
    handle = path.with_name(path.name + ".lock").open("a+")
    fcntl.flock(handle.fileno(), fcntl.LOCK_EX)
    return handle


def _unlock(handle) -> None:
    try:
        fcntl.flock(handle.fileno(), fcntl.LOCK_UN)
    finally:
        handle.close()


def _task_or_fail(tasks: dict, address: str) -> tuple[dict | None, dict]:
    task = tasks.get(address)
    if task is None:
        return None, {"ok": False, "result": "failed", "error": f"unknown native task: {address}", "task": None}
    return task, {"ok": True, "result": "ok", "task": task}


def op_enqueue(path: Path, address: str, name: str = "", dependencies: list[str] | None = None) -> dict:
    handle = _lock(path)
    try:
        state = load_state(path)
        tasks = state["tasks"]
        task = tasks.get(address)
        created = task is None
        if created:
            task = new_task(address, name, dependencies)
            tasks[address] = task
            save_state(path, state)
        return {"ok": True, "result": "enqueued" if created else "already_enqueued", "task": task}
    finally:
        _unlock(handle)


def op_claim(path: Path, address: str, agent: str, model: str | None = None, lease_seconds: float = 1800.0) -> dict:
    handle = _lock(path)
    try:
        state = load_state(path)
        tasks = state["tasks"]
        task, outcome = _task_or_fail(tasks, address)
        if task is None:
            return outcome
        status = task.get("status")
        now = now_epoch()
        lease_expires = task.get("lease_expires_at")
        lease_active = isinstance(lease_expires, (int, float)) and now < float(lease_expires)
        if status in TERMINAL_STATUSES:
            return {"ok": False, "result": "failed", "error": f"native task already completed: {address}", "task": task}
        if status == "queued" or (status == "claimed" and not lease_active) or (status == "in_progress" and not lease_active):
            task["status"] = "claimed"
            task["agent"] = agent
            task["model"] = model or task.get("model")
            task["lease_id"] = f"lease-{address}-{int(now)}-{agent}"
            task["lease_expires_at"] = now + lease_seconds
            task["started_at"] = task.get("started_at") or now
            task["updated_at"] = now
            task["attempts"] = int(task.get("attempts") or 0) + 1
            task["last_error"] = None
            tasks[address] = task
            save_state(path, state)
            return {"ok": True, "result": "claimed", "task": task}
        if status == "claimed" and lease_active and task.get("agent") == agent:
            return {"ok": True, "result": "already_claimed", "task": task}
        return {"ok": False, "result": "failed", "error": f"native task {address} is not claimable (status={status})", "task": task}
    finally:
        _unlock(handle)


def op_heartbeat(path: Path, address: str, agent: str, lease_seconds: float = 1800.0) -> dict:
    handle = _lock(path)
    try:
        state = load_state(path)
        tasks = state["tasks"]
        task, outcome = _task_or_fail(tasks, address)
        if task is None:
            return outcome
        now = now_epoch()
        if task.get("status") not in ACTIVE_STATUSES:
            return {"ok": False, "result": "failed", "error": f"native task {address} is not active", "task": task}
        if task.get("agent") != agent:
            return {"ok": False, "result": "failed", "error": f"native task {address} lease held by {task.get('agent')}", "task": task}
        lease_expires = task.get("lease_expires_at")
        if not isinstance(lease_expires, (int, float)) or now > float(lease_expires):
            return {"ok": False, "result": "failed", "error": f"native task {address} lease expired", "task": task}
        task["lease_expires_at"] = now + lease_seconds
        task["updated_at"] = now
        save_state(path, state)
        return {"ok": True, "result": "heartbeated", "task": task}
    finally:
        _unlock(handle)


def op_release(path: Path, address: str, agent: str, status: str = "failed", error: str | None = None) -> dict:
    handle = _lock(path)
    try:
        state = load_state(path)
        tasks = state["tasks"]
        task, outcome = _task_or_fail(tasks, address)
        if task is None:
            return outcome
        if task.get("status") not in ACTIVE_STATUSES:
            return {"ok": False, "result": "failed", "error": f"native task {address} is not active", "task": task}
        if task.get("agent") != agent:
            return {"ok": False, "result": "failed", "error": f"native task {address} lease held by {task.get('agent')}", "task": task}
        task["status"] = status
        task["lease_id"] = None
        task["lease_expires_at"] = None
        task["updated_at"] = now_epoch()
        if error:
            task["last_error"] = error
        tasks[address] = task
        save_state(path, state)
        return {"ok": True, "result": "released", "task": task}
    finally:
        _unlock(handle)


def op_recover(path: Path, now: float | None = None) -> dict:
    handle = _lock(path)
    try:
        state = load_state(path)
        tasks = state["tasks"]
        recovered: list[str] = []
        changed = False
        now = now_epoch() if now is None else float(now)
        for address, task in tasks.items():
            if task.get("status") in ACTIVE_STATUSES:
                lease_expires = task.get("lease_expires_at")
                if isinstance(lease_expires, (int, float)) and now > float(lease_expires) and task.get("agent"):
                    task["status"] = "queued"
                    task["agent"] = None
                    task["lease_id"] = None
                    task["lease_expires_at"] = None
                    task["updated_at"] = now
                    recovered.append(address)
                    changed = True
        if changed:
            save_state(path, state)
        return {"ok": True, "result": "recovered", "recovered": recovered, "task_count": len(tasks)}
    finally:
        _unlock(handle)


def op_update(path: Path, address: str, status: str, agent: str | None = None, error: str | None = None) -> dict:
    handle = _lock(path)
    try:
        state = load_state(path)
        tasks = state["tasks"]
        task, outcome = _task_or_fail(tasks, address)
        if task is None:
            return outcome
        task["status"] = status
        if agent:
            task["agent"] = agent
        if error:
            task["last_error"] = error
        task["updated_at"] = now_epoch()
        tasks[address] = task
        save_state(path, state)
        return {"ok": True, "result": "updated", "task": task}
    finally:
        _unlock(handle)


def op_list(path: Path, address: str | None = None, status: str | None = None) -> dict:
    state = load_state(path)
    tasks = state["tasks"]
    addresses = [address] if address else sorted(tasks)
    selected = {a: tasks[a] for a in addresses if a in tasks}
    if status is not None:
        selected = {a: t for a, t in selected.items() if t.get("status") == status}
    return {"ok": True, "result": "listed", "task_count": len(selected), "tasks": selected}


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--state", type=Path, default=Path("build/native-task-queue.json"))
    sub = parser.add_subparsers(dest="command", required=True)

    claim = sub.add_parser("claim", help="Atomically claim a native task for one agent.")
    claim.add_argument("--address", required=True)
    claim.add_argument("--agent", required=True)
    claim.add_argument("--model", default=None)
    claim.add_argument("--lease-seconds", type=float, default=1800.0)

    enq = sub.add_parser("enqueue", help="Enqueue (or re-enqueue idempotently) a native task.")
    enq.add_argument("--address", required=True)
    enq.add_argument("--name", default="")

    hb = sub.add_parser("heartbeat", help="Extend the lease of an actively held task.")
    hb.add_argument("--address", required=True)
    hb.add_argument("--agent", required=True)
    hb.add_argument("--lease-seconds", type=float, default=1800.0)

    rel = sub.add_parser("release", help="Release an active task lease (e.g. to failed/deferred).")
    rel.add_argument("--address", required=True)
    rel.add_argument("--agent", required=True)
    rel.add_argument("--status", default="failed")
    rel.add_argument("--error", default=None)

    rec = sub.add_parser("recover", help="Recover crashed agents by releasing expired leases.")
    rec.add_argument("--now", type=float, default=None)

    upd = sub.add_parser("update", help="Update task status/error fields.")
    upd.add_argument("--address", required=True)
    upd.add_argument("--status", required=True)
    upd.add_argument("--agent", default=None)
    upd.add_argument("--error", default=None)

    lst = sub.add_parser("list")
    lst.add_argument("--address", default=None)
    lst.add_argument("--status", default=None)
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    path = args.state
    if args.command == "list":
        payload = op_list(path, args.address, args.status)
    elif args.command == "enqueue":
        payload = op_enqueue(path, args.address, args.name)
    elif args.command == "claim":
        payload = op_claim(path, args.address, args.agent, args.model, args.lease_seconds)
    elif args.command == "heartbeat":
        payload = op_heartbeat(path, args.address, args.agent, args.lease_seconds)
    elif args.command == "release":
        payload = op_release(path, args.address, args.agent, args.status, args.error)
    elif args.command == "recover":
        payload = op_recover(path, args.now)
    elif args.command == "update":
        payload = op_update(path, args.address, args.status, args.agent, args.error)
    else:
        payload = {"ok": False, "result": "failed", "error": f"unknown command: {args.command}"}
    print(json.dumps(payload, indent=2))
    return 0 if payload.get("ok") else 1


if __name__ == "__main__":
    raise SystemExit(main())
