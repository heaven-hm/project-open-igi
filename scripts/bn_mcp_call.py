#!/usr/bin/env python3
"""Call a Binary Ninja MCP tool over HTTP and print the text result."""

import argparse
import json
import sys
import urllib.request

BN_URL = "http://127.0.0.1:24642/mcp"
SID_FILE = "/tmp/bn_sid.txt"


def session() -> str:
    try:
        with open(SID_FILE) as f:
            sid = f.read().strip()
        if sid:
            return sid
    except OSError:
        pass
    return init()


def init() -> str:
    payload = {
        "jsonrpc": "2.0",
        "id": 1,
        "method": "initialize",
        "params": {
            "protocolVersion": "2024-11-05",
            "capabilities": {},
            "clientInfo": {"name": "bn-cli", "version": "1.0.0"},
        },
    }
    req = urllib.request.Request(
        BN_URL,
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json"},
    )
    with urllib.request.urlopen(req, timeout=20) as resp:
        sid = resp.headers.get("MCP-Session-Id", "")
    if sid:
        with open(SID_FILE, "w") as f:
            f.write(sid)
    notify = urllib.request.Request(
        BN_URL,
        data=json.dumps({"jsonrpc": "2.0", "method": "notifications/initialized"}).encode(),
        headers={"Content-Type": "application/json", "MCP-Session-Id": sid},
    )
    urllib.request.urlopen(notify, timeout=20).read()
    return sid


def call(name: str, args: dict, sid: str) -> dict:
    payload = {
        "jsonrpc": "2.0",
        "id": 2,
        "method": "tools/call",
        "params": {"name": name, "arguments": args},
    }
    req = urllib.request.Request(
        BN_URL,
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json", "MCP-Session-Id": sid},
    )
    with urllib.request.urlopen(req, timeout=300) as resp:
        return json.loads(resp.read().decode())


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("tool")
    ap.add_argument("args", nargs="?", default="{}")
    a = ap.parse_args()

    sid = session()
    try:
        res = call(a.tool, json.loads(a.args), sid)
    except Exception:
        sid = init()
        res = call(a.tool, json.loads(a.args), sid)

    if "error" in res:
        print(json.dumps(res["error"], indent=1))
        return 1
    for item in res.get("result", {}).get("content", []):
        print(item.get("text", ""))
    if res.get("result", {}).get("isError"):
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
