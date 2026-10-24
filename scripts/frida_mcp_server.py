#!/usr/bin/env python3
"""
Frida MCP Server
Exposes live runtime dynamic binary instrumentation (DBI) tools via Model Context Protocol.
Enables AI agents to query memory, trace functions, enumerate modules, and execute Frida scripts
against live processes (such as open-igi.exe inside CrossOver/Wine).
"""

import asyncio
import json
import sys
import frida
from mcp.server.mcpserver import MCPServer

mcp = MCPServer(
    name="frida-mcp",
    description="Frida Dynamic Binary Instrumentation (DBI) Runtime MCP Server"
)

DEFAULT_HOST = "127.0.0.1:27042"


def _get_device(host: str = DEFAULT_HOST):
    manager = frida.get_device_manager()
    return manager.add_remote_device(host)


@mcp.tool()
async def frida_list_processes(host: str = DEFAULT_HOST) -> str:
    """List running processes on the target Frida server (e.g. inside Wine / CrossOver or local).
    
    Args:
        host: Host:port of the Frida server (default: 127.0.0.1:27042).
    """
    try:
        device = _get_device(host)
        processes = device.enumerate_processes()
        lines = [f"{'PID':>8}  {'NAME'}"]
        lines.append("-" * 35)
        for p in processes:
            lines.append(f"{p.pid:>8}  {p.name}")
        return "\n".join(lines)
    except Exception as e:
        return f"Error listing processes on {host}: {str(e)}"


@mcp.tool()
async def frida_enumerate_modules(target: str = "open-igi.exe", host: str = DEFAULT_HOST) -> str:
    """List all loaded PE/ELF/Mach-O modules, base addresses, and sizes for a running process.
    
    Args:
        target: Process name (e.g. 'open-igi.exe') or PID.
        host: Host:port of the Frida server (default: 127.0.0.1:27042).
    """
    try:
        device = _get_device(host)
        session = device.attach(int(target) if target.isdigit() else target)
        script = session.create_script("""
            var modules = Process.enumerateModules();
            send(modules.map(m => ({ name: m.name, base: m.base, size: m.size, path: m.path })));
        """)
        results = []
        event = asyncio.Event()

        def on_message(message, data):
            if message.get("type") == "send":
                results.extend(message.get("payload", []))
            event.set()

        script.on("message", on_message)
        script.load()
        await asyncio.wait_for(event.wait(), timeout=5.0)
        session.detach()

        lines = [f"{'BASE':<12} {'SIZE':<10} {'MODULE NAME'}"]
        lines.append("-" * 50)
        for m in sorted(results, key=lambda x: int(str(x['base']), 16)):
            lines.append(f"{str(m['base']):<12} {hex(m['size']):<10} {m['name']}")
        return "\n".join(lines)
    except Exception as e:
        return f"Error enumerating modules in {target}: {str(e)}"


@mcp.tool()
async def frida_read_memory(target: str = "open-igi.exe", address: str = "0x00400000", length: int = 64, host: str = DEFAULT_HOST) -> str:
    """Read bytes and return a formatted hexdump at a specified memory address in a process.
    
    Args:
        target: Process name (e.g. 'open-igi.exe') or PID.
        address: Hex address string (e.g. '0x00400000').
        length: Number of bytes to read (default: 64).
        host: Host:port of the Frida server (default: 127.0.0.1:27042).
    """
    try:
        device = _get_device(host)
        session = device.attach(int(target) if target.isdigit() else target)
        js_code = f"""
            try {{
                var p = ptr("{address}");
                var dump = hexdump(p, {{ length: {length}, header: true }});
                send({{ status: "ok", dump: dump }});
            }} catch(err) {{
                send({{ status: "error", message: err.toString() }});
            }}
        """
        script = session.create_script(js_code)
        output = {}
        event = asyncio.Event()

        def on_message(message, data):
            if message.get("type") == "send":
                output.update(message.get("payload", {}))
            event.set()

        script.on("message", on_message)
        script.load()
        await asyncio.wait_for(event.wait(), timeout=5.0)
        session.detach()

        if output.get("status") == "ok":
            return output.get("dump", "")
        return f"Error reading memory at {address}: {output.get('message', 'Unknown error')}"
    except Exception as e:
        return f"Error reading memory: {str(e)}"


@mcp.tool()
async def frida_execute_script(target: str = "open-igi.exe", script_code: str = "", timeout_seconds: float = 3.0, host: str = DEFAULT_HOST) -> str:
    """Execute a custom JavaScript Frida snippet against a target process and collect all console logs and send() messages.
    
    Args:
        target: Process name (e.g. 'open-igi.exe') or PID.
        script_code: JavaScript code to execute inside the process.
        timeout_seconds: Duration to run script and collect output before detaching (default: 3.0).
        host: Host:port of the Frida server (default: 127.0.0.1:27042).
    """
    try:
        device = _get_device(host)
        session = device.attach(int(target) if target.isdigit() else target)
        logs = []

        def on_message(message, data):
            if message.get("type") == "send":
                logs.append(f"[SEND] {json.dumps(message.get('payload'))}")
            elif message.get("type") == "error":
                logs.append(f"[ERROR] {message.get('description')}")
            else:
                logs.append(f"[MSG] {message}")

        script = session.create_script(script_code)
        script.on("message", on_message)
        script.load()

        await asyncio.sleep(timeout_seconds)
        try:
            session.detach()
        except Exception:
            pass

        return "\n".join(logs) if logs else "Script executed cleanly with no messages."
    except Exception as e:
        return f"Error executing Frida script: {str(e)}"


def main():
    asyncio.run(mcp.run_stdio_async())


if __name__ == "__main__":
    main()
