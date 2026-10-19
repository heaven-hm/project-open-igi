#!/usr/bin/env python3
"""List retail native candidates referenced in production source.

This is a static reachability filter for development builds.  A native is
selected when its retail address or reconstructed symbol appears in a
compiled implementation, including calls from another function in the same
source file and the Windows integration that calls it.  The native's own provenance comment is deliberately ignored,
and an own-file function definition/declaration is excluded, so leaf bodies
are not selected merely because they are present
in the tree.  Runtime captures remain a separate required check: static text
reachability cannot prove a branch is executed in a particular level.
"""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys


ADDRESS_RE = re.compile(r"0x[0-9A-Fa-f]{6,8}")
# Remove comments and literals before searching for references. In particular,
# provenance comments must never make an otherwise unused native look active.
NON_CODE_RE = re.compile(
    r"//[^\n]*|/\*[\s\S]*?\*/|R\"([^()]*)\([\s\S]*?\)\1\"|"
    r"(?:u8|u|U|L)?\"(?:\\.|[^\"\\])*\"|"
    r"(?:u8|u|U|L)?'(?:\\.|[^'\\])*'",
)


def code_only(text: str) -> str:
    """Keep executable tokens while discarding comments and string/char data."""
    return NON_CODE_RE.sub(" ", text)



def has_same_file_reference(text: str, symbol: str) -> bool:
    """Find non-declaration uses after comments/literals have been removed.

    This lexical candidate filter covers ordinary free-function declarations;
    it is not a C++ call graph or proof that the referencing code executes.
    """
    for match in re.finditer(rf"\b{re.escape(symbol)}\b", text):
        tail = text[match.end():].lstrip()
        if not tail.startswith("("):
            return True  # e.g. a function-pointer registration
        depth = 0
        end = None
        for index, token in enumerate(tail):
            if token == "(":
                depth += 1
            elif token == ")":
                depth -= 1
                if depth == 0:
                    end = index + 1
                    break
        if end is None:
            return True  # unresolved syntax remains a candidate, never hidden
        suffix = re.sub(r"^\s*noexcept(?:\([^)]*\))?", "", tail[end:]).lstrip()
        if suffix.startswith("{"):
            continue
        boundary = max(text.rfind(c, 0, match.start()) for c in "\n;{}")
        prefix = text[boundary + 1:match.start()].strip()
        declaration_prefix = (prefix and not prefix.endswith("::")
                              and re.fullmatch(r"[\w:<>,*&\s]+", prefix)
                              and not re.search(r"\b(return|co_return|throw|else|do|if|for|while|switch|case|catch|co_await)\b", prefix))
        if suffix.startswith(";") and declaration_prefix:
            continue
        return True
    return False

def norm(value: str) -> str:
    return f"0x{int(value, 16):08X}"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    parser.add_argument("--manifest", type=pathlib.Path, default=pathlib.Path("docs/fidelity/native-verification.json"))
    parser.add_argument("--tree", type=pathlib.Path, default=pathlib.Path("engine/src/retail"),
                        help="Native implementation tree.")
    parser.add_argument("--reference-tree", action="append", type=pathlib.Path,
                        help="Compiled source root that may reference a native; repeatable. "
                             "Defaults to engine/src and engine/platform.")
    args = parser.parse_args()
    root = args.root.resolve()
    manifest_path = args.manifest if args.manifest.is_absolute() else root / args.manifest
    tree = args.tree if args.tree.is_absolute() else root / args.tree
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        print(f"discover_referenced_natives: cannot read manifest: {error}", file=sys.stderr)
        return 2

    records = manifest.get("natives")
    if not isinstance(records, list):
        print("discover_referenced_natives: manifest has no natives list", file=sys.stderr)
        return 2
    reference_roots = args.reference_tree or [pathlib.Path("engine/src"), pathlib.Path("engine/platform")]
    source_set: set[pathlib.Path] = set()
    for source_root_arg in reference_roots:
        source_root = source_root_arg if source_root_arg.is_absolute() else root / source_root_arg
        if source_root.is_dir():
            # Count references from implementation translation units. Header
            # declarations alone do not mean the executable links or calls a
            # native.
            for suffix in ("*.cpp", "*.cc", "*.c", "*.cxx"):
                source_set.update(source_root.rglob(suffix))
    sources = sorted(source_set)
    texts = {path: code_only(path.read_text(encoding="utf-8", errors="replace")) for path in sources}
    selected: set[str] = set()
    for record in records:
        if not isinstance(record, dict):
            continue
        try:
            address = norm(str(record["address"]))
        except (KeyError, TypeError, ValueError):
            continue
        implementation = record.get("implementation")
        own_path = implementation.get("path") if isinstance(implementation, dict) else None
        symbol = record.get("name") or record.get("cpp_name")
        symbol_re = re.compile(rf"\b{re.escape(symbol)}\b") if isinstance(symbol, str) and symbol else None
        for path, text in texts.items():
            if own_path and path.relative_to(root).as_posix() == own_path:
                if isinstance(symbol, str) and symbol and has_same_file_reference(text, symbol):
                    selected.add(address)
                    break
                continue
            if any(norm(match.group(0)) == address for match in ADDRESS_RE.finditer(text)):
                selected.add(address)
                break
            if symbol_re and symbol_re.search(text):
                selected.add(address)
                break
    for address in sorted(selected):
        print(address)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
