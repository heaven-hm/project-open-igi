#!/usr/bin/env python3
"""Binary Ninja MCP Client for Project Open-IGI.

Connects to the Binary Ninja MCP server running at http://127.0.0.1:24642/mcp
with authorization disabled. Handles session management (MCP-Session-Id)
and provides quick commands for decompilation, disassembly, callers, and callees.
"""

import argparse
import json
import os
import sys
import urllib.error
import urllib.request

DEFAULT_BN_MCP_URL = "http://127.0.0.1:24642/mcp"
SESSION_FILE = "/tmp/bn_mcp_session_id.txt"


def get_cached_session() -> str:
    if os.path.exists(SESSION_FILE):
        try:
            with open(SESSION_FILE, "r") as f:
                return f.read().strip()
        except Exception:
            pass
    return ""


def save_cached_session(session_id: str):
    try:
        with open(SESSION_FILE, "w") as f:
            f.write(session_id)
    except Exception:
        pass


def init_session(url: str = DEFAULT_BN_MCP_URL) -> str:
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
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
    )
    with urllib.request.urlopen(req, timeout=15) as resp:
        session_id = resp.headers.get("MCP-Session-Id", "")
        if session_id:
            save_cached_session(session_id)
        return session_id


def call_mcp(method: str, params: dict, url: str = DEFAULT_BN_MCP_URL, retry: bool = True) -> dict:
    session_id = get_cached_session()
    if not session_id and method != "initialize":
        session_id = init_session(url)

    headers = {"Content-Type": "application/json"}
    if session_id:
        headers["MCP-Session-Id"] = session_id

    payload = {
        "jsonrpc": "2.0",
        "id": 1,
        "method": method,
        "params": params,
    }
    req = urllib.request.Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers=headers,
    )
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            new_sess = resp.headers.get("MCP-Session-Id")
            if new_sess:
                save_cached_session(new_sess)
            data = json.loads(resp.read().decode("utf-8"))
            if "error" in data:
                raise RuntimeError(f"MCP Error: {data['error']}")
            return data.get("result", {})
    except urllib.error.HTTPError as e:
        if e.code == 400 and retry and method != "initialize":
            # Session likely expired or missing; re-initialize and retry once
            new_sess = init_session(url)
            return call_mcp(method, params, url, retry=False)
        raise ConnectionError(f"Binary Ninja MCP HTTP Error {e.code}: {e.read().decode('utf-8', errors='replace')}") from e
    except urllib.error.URLError as e:
        raise ConnectionError(f"Could not connect to Binary Ninja MCP at {url}: {e}") from e


def call_tool(tool_name: str, arguments: dict, url: str = DEFAULT_BN_MCP_URL) -> str:
    result = call_mcp("tools/call", {"name": tool_name, "arguments": arguments}, url=url)
    content = result.get("content", [])
    outputs = []
    for item in content:
        if isinstance(item, dict) and "text" in item:
            outputs.append(item["text"])
        else:
            outputs.append(str(item))
    return "\n".join(outputs)


def ensure_active_view(url: str = DEFAULT_BN_MCP_URL):
    try:
        call_mcp("tools/call", {"name": "bn_binary_view_set_active", "arguments": {"binaryView": "view_1"}}, url=url)
    except Exception:
        pass


def main() -> int:
    parser = argparse.ArgumentParser(description="Binary Ninja MCP Client")
    parser.add_argument("--url", default=DEFAULT_BN_MCP_URL, help="Binary Ninja MCP URL")
    subparsers = parser.add_subparsers(dest="command", required=True)

    # list-tools
    subparsers.add_parser("list-tools", help="List all available MCP tools")

    # decompile
    p_dec = subparsers.add_parser("decompile", help="Decompile a function")
    p_dec.add_argument("function", help="Function address or name (e.g. 0x004C6670 or CreateTerrainMaterial)")

    # disasm
    p_dis = subparsers.add_parser("disasm", help="Disassemble a function")
    p_dis.add_argument("function", help="Function address or name (e.g. 0x004C6670)")

    # callers
    p_callers = subparsers.add_parser("callers", help="List callers of a function")
    p_callers.add_argument("function", help="Function address or name")

    # callees
    p_callees = subparsers.add_parser("callees", help="List callees of a function")
    p_callees.add_argument("function", help="Function address or name")

    # call
    p_call = subparsers.add_parser("call", help="Call arbitrary tool")
    p_call.add_argument("tool", help="Tool name")
    p_call.add_argument("args", nargs="?", default="{}", help="JSON string arguments")

    args = parser.parse_args()

    try:
        if args.command == "list-tools":
            res = call_mcp("tools/list", {}, url=args.url)
            tools = res.get("tools", [])
            print(f"Binary Ninja MCP has {len(tools)} tools:")
            for t in tools:
                print(f"  - {t['name']}: {t.get('description', '')}")
            return 0

        ensure_active_view(url=args.url)

        if args.command == "decompile":
            output = call_tool("bn_function_decompile", {"function": args.function}, url=args.url)
            print(output)
            return 0

        if args.command == "disasm":
            output = call_tool("bn_function_disassembly", {"function": args.function}, url=args.url)
            print(output)
            return 0

        if args.command == "callers":
            output = call_tool("bn_function_callers", {"function": args.function}, url=args.url)
            print(output)
            return 0

        if args.command == "callees":
            output = call_tool("bn_function_callees", {"function": args.function}, url=args.url)
            print(output)
            return 0

        if args.command == "call":
            tool_args = json.loads(args.args)
            output = call_tool(args.tool, tool_args, url=args.url)
            print(output)
            return 0

    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
