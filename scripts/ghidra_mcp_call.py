#!/usr/bin/env python3
"""Call a Ghidra Headless MCP tool and print the result.

The Ghidra Headless MCP server speaks line-delimited JSON-RPC over a raw TCP
socket, not the HTTP transport the Binary Ninja MCP uses, so it needs its own
client. The server is started with:

    GHIDRA_INSTALL_DIR=/opt/homebrew/opt/ghidra/libexec \\
        python3 -m ghidra_headless_mcp --transport tcp --host 127.0.0.1 --port 8771

    python3 scripts/ghidra_mcp_call.py tools/list
    python3 scripts/ghidra_mcp_call.py decompile_function '{"address": "0x004D67A0"}'
"""

import argparse
import json
import socket
import sys

GHIDRA_URL = ("127.0.0.1", 8771)


def call(method: str, params: dict, request_id: int = 1) -> dict:
    with socket.create_connection(GHIDRA_URL, timeout=180) as connection:
        message = {"jsonrpc": "2.0", "id": request_id, "method": method}
        if params:
            message["params"] = params
        connection.sendall((json.dumps(message) + "\n").encode("utf-8"))
        buffer = b""
        while not buffer.endswith(b"\n"):
            chunk = connection.recv(65536)
            if not chunk:
                break
            buffer += chunk
    if not buffer.strip():
        raise SystemExit("the Ghidra MCP returned nothing")
    return json.loads(buffer.decode("utf-8").strip())


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("method", help="the JSON-RPC method, for example tools/call")
    parser.add_argument("params", nargs="?", default="{}", help="the JSON parameters")
    arguments = parser.parse_args()
    params = json.loads(arguments.params) if arguments.params else {}
    response = call(arguments.method, params)
    if "error" in response:
        print(json.dumps(response["error"], indent=1))
        return 1
    print(json.dumps(response.get("result", response), indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
