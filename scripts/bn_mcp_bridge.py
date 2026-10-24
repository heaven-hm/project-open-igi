#!/usr/bin/env python3
"""Binary Ninja Stdio-to-HTTP MCP Bridge.

Bridges standard input/output (stdio JSON-RPC) from MCP hosts to
Binary Ninja's HTTP MCP server running at http://127.0.0.1:24642/mcp.
Automatically captures and forwards MCP-Session-Id across requests.
"""

import json
import sys
import urllib.error
import urllib.request

BN_URL = "http://127.0.0.1:24642/mcp"


def main():
    session_id = None

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            headers = {"Content-Type": "application/json"}
            if session_id:
                headers["MCP-Session-Id"] = session_id

            req = urllib.request.Request(
                BN_URL,
                data=line.encode("utf-8"),
                headers=headers,
            )
            with urllib.request.urlopen(req, timeout=60) as resp:
                new_session = resp.headers.get("MCP-Session-Id")
                if new_session:
                    session_id = new_session

                res_data = resp.read().decode("utf-8")
                sys.stdout.write(res_data + "\n")
                sys.stdout.flush()
        except urllib.error.HTTPError as e:
            err_body = e.read().decode("utf-8", errors="replace")
            msg_id = None
            try:
                msg = json.loads(line)
                msg_id = msg.get("id")
            except Exception:
                pass
            err_resp = {
                "jsonrpc": "2.0",
                "id": msg_id,
                "error": {"code": -32603, "message": f"BN MCP HTTP {e.code}: {err_body}"},
            }
            sys.stdout.write(json.dumps(err_resp) + "\n")
            sys.stdout.flush()
        except Exception as e:
            msg_id = None
            try:
                msg = json.loads(line)
                msg_id = msg.get("id")
            except Exception:
                pass
            err_resp = {
                "jsonrpc": "2.0",
                "id": msg_id,
                "error": {"code": -32603, "message": f"Bridge error: {e}"},
            }
            sys.stdout.write(json.dumps(err_resp) + "\n")
            sys.stdout.flush()


if __name__ == "__main__":
    main()
