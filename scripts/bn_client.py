#!/usr/bin/env python3
"""Persistent-session Binary Ninja MCP client.

Spawning a process per call costs ~250ms; holding one session alive costs ~1ms.
This matters because the GameNatives corpus is 5734 addresses.
"""

import json
import urllib.error
import urllib.request

URL = "http://127.0.0.1:24642/mcp"
SESSION_FILE = "/tmp/bn_sid.txt"


class BN:
    def __init__(self, url: str = URL):
        self.url = url
        self.sid = self._init()
        self._id = 0

    def _init(self) -> str:
        req = urllib.request.Request(
            self.url,
            data=json.dumps({
                "jsonrpc": "2.0", "id": 1, "method": "initialize",
                "params": {"protocolVersion": "2024-11-05", "capabilities": {},
                           "clientInfo": {"name": "bn-bulk", "version": "1.0"}},
            }).encode(),
            headers={"Content-Type": "application/json"},
        )
        with urllib.request.urlopen(req, timeout=30) as resp:
            sid = resp.headers.get("MCP-Session-Id", "")
        try:
            with open(SESSION_FILE, "w") as f:
                f.write(sid)
        except OSError:
            pass
        note = urllib.request.Request(
            self.url,
            data=json.dumps({"jsonrpc": "2.0",
                             "method": "notifications/initialized"}).encode(),
            headers={"Content-Type": "application/json", "MCP-Session-Id": sid},
        )
        urllib.request.urlopen(note, timeout=30).read()
        return sid

    def _post(self, payload, timeout=300):
        req = urllib.request.Request(
            self.url, data=json.dumps(payload).encode(),
            headers={"Content-Type": "application/json", "MCP-Session-Id": self.sid})
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            return json.loads(resp.read().decode())

    def call(self, tool: str, args: dict, timeout=300) -> str:
        self._id += 1
        res = self._post({"jsonrpc": "2.0", "id": self._id, "method": "tools/call",
                          "params": {"name": tool, "arguments": args}}, timeout)
        if "error" in res:
            return ""
        out = []
        for item in res.get("result", {}).get("content", []):
            out.append(item.get("text", ""))
        return "\n".join(out)

    def call_json(self, tool: str, args: dict, timeout=300):
        """Call a tool and parse the JSON payload it returns as text content."""
        self._id += 1
        res = self._post({"jsonrpc": "2.0", "id": self._id, "method": "tools/call",
                          "params": {"name": tool, "arguments": args}}, timeout)
        result = res.get("result", {})
        for item in result.get("content", []):
            text = item.get("text", "")
            if text.lstrip().startswith("{"):
                try:
                    return json.loads(text)
                except json.JSONDecodeError:
                    continue
        return result

    # -- convenience -----------------------------------------------------
    @staticmethod
    def header(text: str, key: str):
        for ln in text.splitlines():
            if ln.startswith(key + ":"):
                return ln.split(":", 1)[1].strip().strip('"').strip(",")
        return None

    def info(self, addr: str) -> dict:
        t = self.call("bn_function_info", {"function": addr})
        proto = ""
        raw = self.call_json("bn_function_prototype_get", {"function": addr})
        try:
            proto = raw["prototype"]["definition"]
        except (KeyError, TypeError):
            pass
        return {
            "name": self.header(t, "name"),
            "start": self.header(t, "function"),
            "blocks": int(self.header(
                self.call("bn_function_basic_blocks", {"function": addr}),
                "count") or 0),
            "callers": int(self.header(
                self.call("bn_function_callers", {"function": addr}),
                "count") or 0),
            "callees": int(self.header(
                self.call("bn_function_callees", {"function": addr}),
                "count") or 0),
            "prototype": proto,
        }

    def rename(self, addr: str, new_name: str) -> tuple[bool, str]:
        self._id += 1
        res = self._post({"jsonrpc": "2.0", "id": self._id, "method": "tools/call",
                          "params": {"name": "bn_symbol_rename",
                                     "arguments": {"symbol": addr, "newName": new_name,
                                                   "type": "FunctionSymbol"}}})
        if "error" in res:
            return False, json.dumps(res["error"])[:200]
        r = res.get("result", {})
        return not r.get("isError", False), (r.get("content") or [{}])[0].get("text", "")[:200]
