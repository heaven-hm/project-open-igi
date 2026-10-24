#!/usr/bin/env python3
"""Compatibility entry point; implementation lives in native_unicorn_runner.py."""
from native_unicorn_runner import main

if __name__ == "__main__":
    raise SystemExit(main())
