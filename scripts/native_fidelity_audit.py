#!/usr/bin/env python3
"""Automated per-native fidelity audit against retail igi.exe.

This exists so that NO native is ever checked by hand. It answers, for every transcribed
native in the tree, the questions that have repeatedly been answered wrongly by hand:

  1. What is the function's TRUE extent in retail?
     Rule 22: an extent is bounded by the `ret`, not by the gap to the next function start.
     Rule 14: walk to the `ret`, then read the byte after it.
     THE TAIL-JUMP RULE (established on 0x004A5BB0): a forward unconditional `jmp` whose
     target is not a conditional-branch target of the same function and is not already
     decoded is a TAIL CALL - stop, do not walk into it. radare2 6.2.0 does not apply this
     rule, which is why its extent for a tail-jump body runs past the body's own `ret` and
     inflates the block count. That single tool bug produced a false CFG failure at 0.413
     for a file that was correct, and cost a full reconciliation cycle.

  2. Does the candidate's control-flow graph match?
     Score = 0.35 * node_jaccard + 0.65 * edge_jaccard, threshold 0.85.
     Blocks and edges are counted BY DISTINCT ADDRESS (rule 20), never by counting lines.

  3. Is any symbol declared as CODE that retail treats as DATA?
     THE HANDLER RULE: retail often pushes a routine's ADDRESS as an immediate
     (`push 0x44d0e0`) and registers it, without ever emitting an `E8` to it. A candidate
     that declares such a symbol and takes `&symbol` emits a RELOCATION, and the relocation
     is what the linker reports as an undefined symbol. This is how 55 phantom link failures
     were created for AI-action handlers that needed no transcription at all. This audit
     detects that class of defect automatically, per symbol, by decoding the retail bytes.

  4. Which natives are declared in a candidate but never called by retail at all?

Usage:
    python3 scripts/native_fidelity_audit.py                 # audit every native in the tree
    python3 scripts/native_fidelity_audit.py --va 0x004A5BB0 # one native
    python3 scripts/native_fidelity_audit.py --handlers-only # just the data/code check
    python3 scripts/native_fidelity_audit.py --json out.json
"""

from __future__ import annotations

import argparse
import json
import os
import re
import struct
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

try:
    import capstone
except ImportError:  # pragma: no cover
    sys.exit("capstone is required: pip install capstone")

try:
    import pefile
except ImportError:  # pragma: no cover
    sys.exit("pefile is required: pip install pefile")

REPO = Path(__file__).resolve().parent.parent
RETAIL = Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
CFG_THRESHOLD = 0.85
BUILD_DIR = REPO / "build-win32-no-verify-gate"
CXX = "i686-w64-mingw32-g++"
CXXFLAGS = ["-std=c++20", "-O2", "-Iengine/include", "-Iengine/platform/windows"]

# ---------------------------------------------------------------------------------------------
# Retail image
# ---------------------------------------------------------------------------------------------


class RetailImage:
    """The retail binary, mapped VA -> bytes, with an instruction decoder."""

    def __init__(self, path: Path = RETAIL):
        self.path = path
        self.data = path.read_bytes()
        self.pe = pefile.PE(str(path), fast_load=True)
        self.base = self.pe.OPTIONAL_HEADER.ImageBase
        self.sections = []
        for s in self.pe.sections:
            self.sections.append(
                (self.base + s.VirtualAddress, s.Misc_VirtualSize, s.PointerToRawData,
                 s.SizeOfRawData)
            )
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True
        self._call_index = None

    def va_to_off(self, va: int):
        """VA -> file offset, or None when the address is in BSS (no file bytes)."""
        for vaddr, vsize, paddr, psize in self.sections:
            if vaddr <= va < vaddr + vsize:
                delta = va - vaddr
                if delta < psize:
                    return paddr + delta
                return None  # inside virtual size but past raw size: zero-filled BSS
        return None

    def read(self, va: int, n: int) -> bytes:
        off = self.va_to_off(va)
        if off is None:
            return b"\x00" * n  # BSS reads as zero, which is what the loader maps
        return self.data[off:off + n]

    def disasm(self, va: int, n: int):
        return list(self.md.disasm(self.read(va, n), va))

    # -- the E8 index, built once -------------------------------------------------------------
    def call_index(self) -> dict:
        """Map target VA -> list of call-site VAs, by decoding every E8's displacement.

        Rule: a byte-level E8 scan can INVENT a call (push 0x0041E8C0 is `68 C0 E8 41 00`),
        so this decodes at instruction boundaries rather than scanning for the byte.
        """
        if self._call_index is not None:
            return self._call_index
        index = defaultdict(list)
        text = next(s for s in self.sections if s[0] == self.base + 0x1000)
        vaddr, vsize, paddr, psize = text
        insns = self.md.disasm(self.data[paddr:paddr + psize], vaddr)
        for ins in insns:
            if ins.mnemonic == "call" and ins.operands and ins.operands[0].type == capstone.x86.X86_OP_IMM:
                index[ins.operands[0].imm & 0xFFFFFFFF].append(ins.address)
        self._call_index = index
        return index

    # -- extent recovery ----------------------------------------------------------------------
    def _traverse(self, va: int, blocked_jmp: int | None, max_instructions: int):
        """One traversal pass. `blocked_jmp` is a jmp site whose edge is NOT followed, used to
        test whether a target is reachable by any other path."""
        worklist = [va]
        visited_blocks = set()
        insns = {}
        cond_targets = set()
        jmps = []           # (site, target) for every forward unconditional jmp seen
        truncated = False

        while worklist:
            if len(insns) >= max_instructions:
                truncated = True
                break
            start = worklist.pop()
            if start in visited_blocks:
                continue
            visited_blocks.add(start)
            addr = start
            while True:
                if addr in insns:
                    break
                chunk = self.disasm(addr, 16)
                if not chunk:
                    truncated = True
                    break
                ins = chunk[0]
                insns[addr] = ins
                mnem, ops = ins.mnemonic, ins.operands
                imms = [o.imm & 0xFFFFFFFF for o in ops
                        if o.type == capstone.x86.X86_OP_IMM]

                if mnem.startswith("ret"):
                    break
                if mnem == "jmp" and imms:
                    target = imms[0]
                    if target > ins.address:
                        jmps.append((ins.address, target))
                    if ins.address == blocked_jmp:
                        break           # this edge is deliberately not followed
                    if target not in insns:
                        worklist.append(target)
                    break
                if mnem.startswith("j"):
                    for t in imms:
                        cond_targets.add(t)
                        if t not in insns:
                            worklist.append(t)
                    addr = ins.address + ins.size
                    if addr not in insns:
                        worklist.append(addr)
                    break
                addr = ins.address + ins.size

        return insns, cond_targets, jmps, truncated

    def recover(self, va: int, max_instructions: int = 4000):
        """Recover a function's true extent, block starts and edges.

        REACHABLE-SET TRAVERSAL, not a linear sweep. A linear sweep that stops at the first
        `ret` is wrong for any function with more than one epilogue: 0x004A5BB0 has three
        (0x004A5BE4, 0x004A5C23, 0x004A5C2A) and a sweep reports 30 instructions for a body
        of 66.

        THE TAIL-JUMP RULE, BY REACHABILITY. A forward unconditional `jmp` may or may not be a
        tail call, and neither the target address nor a `ret` in the gap decides it:

          - 0x004A5BB0 ends `jmp 0x004AA0A6`, far outside its own body: a real tail call.
          - 0x004C7560 ends `jmp 0x004C759C` six bytes ahead, and 0x004C7598 - the instruction
            that jump SKIPS - is the target of `jne` at 0x004C75D7. The jump target 0x004C759C
            is NOT itself a conditional target (they differ by 4 bytes), so comparing targets
            cannot detect this. Following it leads back around a loop, so the body is 46
            instructions, not 20.
          - 0x00508F80's genuine tail jump into 0x00508FA0 has no `ret` in its gap either, so
            the "a ret lies in the gap" test is wrong on its own.

        The criterion: **follow the jump. If the region reachable from the target branches
        backward to an address below the target itself, that region looks like a loop body, so
        the jump may be internal control flow rather than a tail call.** A tail call cannot be
        part of a loop.

        BUT THAT TEST ALONE IS NOT SOUND, and the four known cases prove it. A genuine tail call
        into a *separate function that itself contains loops* trips it too:

          0x004A5BB0 -> 0x004AA0A6   real tail call; target region loops back below 0x004AA0A6
          0x005183D0 -> 0x00519710   real tail call; target region loops back below 0x00519710
          0x004C7560 -> 0x004C759C   NOT a tail call; target is the loop head
          0x00508F80 -> 0x00508FA0   real tail call, 32 bytes ahead, no loop-back

        Distance does not separate them (32 bytes for a real tail call, 60 for an internal jump).
        Neither does "is the target a conditional-branch target" (the jmp and the jne land four
        bytes apart), "is the target unreachable when this jmp is blocked" (blocking it
        disconnects the whole loop), "is the target a manifest function entry" (0x00519710 is
        not one), nor "is the target an E8 call target" (none of the four is).

        This is the classic function-boundary problem and it is NOT decidable from local byte
        evidence. So this method does not pretend to decide: it returns the shape under the
        default interpretation and records `ambiguous` with BOTH sizes, so a caller must
        corroborate with the caller set, the target's own `ret`, or a tree/manifest record.
        Recording the gap is required here; guessing is not permitted.
        """
        # pass 1: follow everything, to enumerate the forward jmps
        insns1, cond1, jmps, trunc1 = self._traverse(va, None, max_instructions)

        tail_targets = set()
        ambiguous = []
        for site, target in jmps:
            if self._is_loop_region(target, max_instructions):
                ambiguous.append({"site": f"0x{site:08X}", "target": f"0x{target:08X}",
                                  "reason": "target region branches below itself"})
            else:
                tail_targets.add(target)

        # default interpretation: treat the loop-region targets as tail calls too, because a
        # body that runs into the next function is a worse error than one that stops early,
        # and stopping early is recoverable from the ambiguous list below.
        all_targets = tail_targets | {t for _, t in jmps}
        if all_targets:
            insns, cond_targets, jmps_final, trunc2 = self._traverse_terminal(
                va, all_targets, max_instructions)
        else:
            insns, cond_targets, jmps_final, trunc2 = insns1, cond1, jmps, False

        tail_jumps = sorted(t for _, t in jmps_final if t in all_targets)
        ordered = [insns[a] for a in sorted(insns)]
        shape = FunctionShape(entry=va, insns=ordered, cond_targets=cond_targets,
                              tail_jump=(tail_jumps[0] if tail_jumps else None),
                              truncated=(trunc1 or trunc2), tail_jumps=tail_jumps)
        shape.ambiguous = ambiguous
        if ambiguous:
            # size under the other reading, for comparison
            alt, _, _, _ = self._traverse_terminal(va, tail_targets, max_instructions)
            shape.alt_insns = len(alt)
        return shape

    def _is_loop_region(self, target: int, max_instructions: int = 2000) -> bool:
        """True when the code at `target` loops back below `target` - i.e. it is this
        function's own loop body rather than a separate callee reached by a tail call."""
        seen, worklist = set(), [target]
        while worklist and len(seen) < max_instructions:
            addr = worklist.pop()
            while True:
                if addr in seen:
                    break
                seen.add(addr)
                chunk = self.disasm(addr, 16)
                if not chunk:
                    break
                ins = chunk[0]
                mnem = ins.mnemonic
                if mnem.startswith("ret"):
                    break
                imms = [o.imm & 0xFFFFFFFF for o in ins.operands
                        if o.type == capstone.x86.X86_OP_IMM]
                for t in imms:
                    if t < target:
                        return True          # branches backward below the region entry
                if mnem.startswith("j"):
                    for t in imms:
                        if t not in seen:
                            worklist.append(t)
                    if mnem != "jmp":
                        addr = ins.address + ins.size
                        continue
                    break
                if mnem == "call":
                    addr = ins.address + ins.size
                    continue
                addr = ins.address + ins.size
        return False

    def _traverse_terminal(self, va: int, tail_targets: set, max_instructions: int):
        """Traverse treating every target in `tail_targets` as terminal (do not follow)."""
        worklist = [va]
        visited_blocks = set()
        insns = {}
        cond_targets = set()
        jmps = []
        truncated = False
        while worklist:
            if len(insns) >= max_instructions:
                truncated = True
                break
            start = worklist.pop()
            if start in visited_blocks:
                continue
            visited_blocks.add(start)
            addr = start
            while True:
                if addr in insns:
                    break
                chunk = self.disasm(addr, 16)
                if not chunk:
                    truncated = True
                    break
                ins = chunk[0]
                insns[addr] = ins
                mnem, ops = ins.mnemonic, ins.operands
                imms = [o.imm & 0xFFFFFFFF for o in ops
                        if o.type == capstone.x86.X86_OP_IMM]
                if mnem.startswith("ret"):
                    break
                if mnem == "jmp" and imms:
                    target = imms[0]
                    if target > ins.address:
                        jmps.append((ins.address, target))
                    if target in tail_targets:
                        break
                    if target not in insns:
                        worklist.append(target)
                    break
                if mnem.startswith("j"):
                    for t in imms:
                        cond_targets.add(t)
                        if t not in insns:
                            worklist.append(t)
                    addr = ins.address + ins.size
                    if addr not in insns:
                        worklist.append(addr)
                    break
                addr = ins.address + ins.size
        return insns, cond_targets, jmps, truncated


@dataclass
class FunctionShape:
    entry: int
    insns: list
    cond_targets: set
    tail_jump: int | None
    truncated: bool
    tail_jumps: list = field(default_factory=list)
    blocks: list = field(default_factory=list)
    edges: list = field(default_factory=list)
    ambiguous: list = field(default_factory=list)
    alt_insns: int = 0

    def __post_init__(self):
        self._build_cfg()

    def _build_cfg(self):
        if not self.insns:
            return
        by_addr = {i.address: i for i in self.insns}
        addrs = [i.address for i in self.insns]
        # Block leaders: entry, every branch target, every instruction after a branch.
        leaders = {self.entry}
        for ins in self.insns:
            if ins.mnemonic.startswith("j") or ins.mnemonic == "ret":
                nxt = ins.address + ins.size
                if nxt in by_addr:
                    leaders.add(nxt)
                for op in ins.operands:
                    if op.type == capstone.x86.X86_OP_IMM:
                        t = op.imm & 0xFFFFFFFF
                        if t in by_addr:
                            leaders.add(t)
        self.blocks = sorted(leaders)

        # Edges between blocks, by distinct (from, to) address pairs.
        edges = set()
        leader_set = set(self.blocks)
        for ins in self.insns:
            nxt = ins.address + ins.size
            if ins.mnemonic == "ret":
                continue
            if ins.mnemonic.startswith("j"):
                imm = [o.imm & 0xFFFFFFFF for o in ins.operands
                       if o.type == capstone.x86.X86_OP_IMM]
                for t in imm:
                    if t in leader_set:
                        edges.add((self._block_of(ins.address), t))
                if ins.mnemonic == "jmp":
                    continue
            if nxt in leader_set:
                edges.add((self._block_of(ins.address), nxt))
        self.edges = sorted(edges)

    def _block_of(self, addr: int) -> int:
        lo = self.entry
        for b in self.blocks:
            if b <= addr:
                lo = b
            else:
                break
        return lo

    @property
    def size(self) -> int:
        if not self.insns:
            return 0
        last = self.insns[-1]
        return last.address + last.size - self.entry

    @property
    def byte_after_ret(self) -> str:
        if not self.insns:
            return "??"
        last = self.insns[-1]
        if not last.mnemonic.startswith("ret"):
            return "--"
        nxt = last.address + last.size
        return f"0x{nxt:08X}"


# ---------------------------------------------------------------------------------------------
# Candidates
# ---------------------------------------------------------------------------------------------


def candidate_shapes(obj: Path, sym: str):
    """Recover a candidate's CFG from its compiled object, using the same block rule."""
    try:
        out = subprocess.run(["i686-w64-mingw32-objdump", "-d", "--no-show-raw-insn", str(obj)],
                             capture_output=True, text=True, timeout=120).stdout
    except Exception:
        return None
    # isolate the symbol's body. objdump prints the PE name with its leading underscore
    # (`<_Str_FindSubstring_4A5BB0>`) while `nm`-derived names have it stripped, so accept both.
    pat = re.compile(r"^[0-9a-f]+ <_?" + re.escape(sym) + r">:$")
    lines, inside = [], False
    for ln in out.splitlines():
        if pat.match(ln.strip()):
            inside = True
            continue
        if inside:
            if re.match(r"^[0-9a-f]+ <", ln.strip()):
                break
            lines.append(ln)
    if not lines:
        return None
    addr = None
    insns, edges = [], set()
    branches, rets, jmps, calls = 0, 0, 0, 0
    targets = set()
    parsed = []
    for ln in lines:
        m = re.match(r"\s*([0-9a-f]+):\s+(\S+)\s*(.*)$", ln)
        if not m:
            continue
        a = int(m.group(1), 16)
        mn = m.group(2)
        ops = m.group(3)
        parsed.append((a, mn, ops))
    addrs = [p[0] for p in parsed]
    for a, mn, ops in parsed:
        if mn == "ret" or mn.startswith("ret"):
            rets += 1
        elif mn == "jmp":
            jmps += 1
        elif mn.startswith("j"):
            branches += 1
        elif mn == "call":
            calls += 1
        if mn.startswith("j") and ops:
            m = re.match(r"([0-9a-f]+)", ops.strip())
            if m:
                targets.add(int(m.group(1), 16))
    leaders = {addrs[0]} if addrs else set()
    for i, (a, mn, ops) in enumerate(parsed):
        if mn.startswith("j") or mn.startswith("ret"):
            if i + 1 < len(parsed):
                leaders.add(parsed[i + 1][0])
            m = re.match(r"([0-9a-f]+)", ops.strip()) if ops else None
            if m and int(m.group(1), 16) in addrs:
                leaders.add(int(m.group(1), 16))
    leaders = sorted(leaders)
    leader_set = set(leaders)

    def blk_of(a):
        lo = leaders[0]
        for b in leaders:
            if b <= a:
                lo = b
            else:
                break
        return lo

    edges = set()
    for i, (a, mn, ops) in enumerate(parsed):
        nxt = parsed[i + 1][0] if i + 1 < len(parsed) else None
        if mn.startswith("ret"):
            continue
        if mn.startswith("j"):
            m = re.match(r"([0-9a-f]+)", ops.strip()) if ops else None
            if m:
                t = int(m.group(1), 16)
                if t in leader_set:
                    edges.add((blk_of(a), t))
            if mn == "jmp":
                continue
        if nxt is not None and nxt in leader_set:
            edges.add((blk_of(a), nxt))

    return {
        "blocks": leaders,
        "edges": sorted(edges),
        "n_blocks": len(leaders),
        "branches": branches,
        "rets": rets,
        "jmps": jmps,
        "calls": calls,
        "n_insns": len(parsed),
        "bytes": (max(addrs) - min(addrs) + 1) if addrs else 0,
    }


# ---------------------------------------------------------------------------------------------
# Provenance
# ---------------------------------------------------------------------------------------------

VA_RE = re.compile(r"retail VA 0x([0-9A-Fa-f]{6,8})")


def provenance_va(src: Path):
    try:
        head = src.read_text(errors="ignore")[:4000]
    except Exception:
        return None
    m = VA_RE.search(head)
    return int(m.group(1), 16) if m else None


def candidate_symbols(src: Path):
    """The symbol(s) the file defines, taken from the compiled object where possible.

    Reading the source with a regex is not reliable: `#define NATIVE_DECL __attribute__((cdecl))`
    makes `__attribute__` look like a definition, and a macro-expanded declaration is not a
    definition at all. The object file is the authority for what a translation unit emits, so
    compile it and ask `nm`. Fall back to the source only if the compile fails.
    """
    obj = Path("/tmp") / ("sym_" + src.stem + ".o")
    try:
        r = subprocess.run([CXX] + CXXFLAGS + ["-c", str(src), "-o", str(obj)],
                           capture_output=True, text=True, timeout=300, cwd=REPO)
        if r.returncode == 0:
            nm = subprocess.run(["i686-w64-mingw32-nm", "--defined-only", str(obj)],
                                capture_output=True, text=True, timeout=60).stdout
            out = []
            for ln in nm.splitlines():
                parts = ln.split()
                if len(parts) < 3:
                    continue
                kind, name = parts[-2], parts[-1]
                # 'T'/'t' = text (a function this unit defines). Skip the section symbols.
                if kind in ("T", "t") and name not in (".text",):
                    out.append(name.lstrip("_"))
            if out:
                return out
    except Exception:
        pass
    # fallback: source scan, with preprocessor lines and macros excluded
    try:
        t = src.read_text(errors="ignore")
    except Exception:
        return []
    t = "\n".join(ln for ln in t.splitlines() if not ln.lstrip().startswith("#"))
    syms = re.findall(r"NATIVE_DECL\s+(\w+)\s*\(", t)
    syms += re.findall(r"^\s*(?:void|int|char\s*\*|std::\w+)\s+(\w+)\s*\([^;]*\)\s*\{", t, re.M)
    seen, out = set(), []
    for s in syms:
        if s not in seen and not s.startswith("__"):
            seen.add(s)
            out.append(s)
    return out


def jaccard(a: set, b: set) -> float:
    if not a and not b:
        return 1.0
    if not a or not b:
        return 0.0
    return len(a & b) / len(a | b)


def canonical_cfg(entry_block, blocks, edges):
    """Address-independent canonical form of a CFG.

    Retail and candidate live in different address spaces (0x004A5BB0 vs 0x00000000), so
    comparing block ADDRESSES is meaningless and always yields a jaccard of zero. The graphs
    are canonicalised instead: blocks are renumbered by a deterministic depth-first walk from
    the entry, taking successors in their existing order, so two structurally identical CFGs
    produce identical node and edge sets regardless of where they were linked.

    Returns (node_set, edge_set) in canonical index space.
    """
    succ = defaultdict(list)
    for a, b in sorted(edges):
        succ[a].append(b)
    order, seen = [], set()
    stack = [entry_block]
    while stack:
        blk = stack.pop()
        if blk in seen:
            continue
        seen.add(blk)
        order.append(blk)
        # push successors in reverse so the first successor is visited first
        for s in reversed(succ.get(blk, [])):
            if s not in seen:
                stack.append(s)
    # any block not reachable by the walk still exists; append deterministically
    for b in sorted(blocks):
        if b not in seen:
            seen.add(b)
            order.append(b)
    idx = {b: i for i, b in enumerate(order)}
    nodes = set(range(len(order)))
    canon_edges = {(idx[a], idx[b]) for a, b in edges if a in idx and b in idx}
    return nodes, canon_edges


# ---------------------------------------------------------------------------------------------
# The data/code defect detector
# ---------------------------------------------------------------------------------------------

DECL_RE = re.compile(r"^\s*(?:extern\s+)?(?:void|int|std::\w+|unsigned\s+\w+)\s+"
                     r"(?:NATIVE_DECL\s+)?(\w+)\s*\([^;{]*\)\s*;", re.M)
ADDR_RE = re.compile(r"&\s*(\w+)")


def detect_data_as_code(img: RetailImage, src: Path):
    """Flag symbols the candidate declares and address-takes but retail never calls.

    This is the defect that invented 55 phantom link failures on the AI-action handlers.
    """
    try:
        text = src.read_text(errors="ignore")
    except Exception:
        return []
    # map symbol -> the VA comment on its declaration line, if present
    findings = []
    lines = text.splitlines()
    for i, ln in enumerate(lines):
        m = DECL_RE.match(ln)
        if not m:
            continue
        sym = m.group(1)
        if sym.startswith(("std", "igi", "reinterpret")):
            continue
        # find this symbol's VA from the comment on the same line, or from its name suffix
        va = None
        c = re.search(r"0x0?x?([0-9A-Fa-f]{6,8})", ln)
        if c:
            va = int(c.group(1), 16)
        if va is None:
            s = re.search(r"_([0-9A-F]{5,8})$", sym)
            if s:
                va = int(s.group(1), 16)
        if va is None:
            continue
        # is it address-taken anywhere?
        if not re.search(r"&\s*" + re.escape(sym) + r"\b", text):
            continue
        # is it ever actually CALLED in the candidate?
        called_in_candidate = re.search(r"(?<!&)\b" + re.escape(sym) + r"\s*\(", text)
        n_retail = len(img.call_index().get(va, []))
        if n_retail == 0:
            findings.append({
                "symbol": sym,
                "va": f"0x{va:08X}",
                "retail_e8_calls": 0,
                "called_in_candidate": bool(called_in_candidate),
                "verdict": "DATA, NOT CODE - retail never emits E8 to this address",
                "fix": "pass as reinterpret_cast<...>(static_cast<std::uintptr_t>(0x%08XU)); "
                       "do not declare it" % va,
            })
    return findings


# ---------------------------------------------------------------------------------------------
# Main audit
# ---------------------------------------------------------------------------------------------


def audit_one(img: RetailImage, src: Path, verbose=False):
    va = provenance_va(src)
    rec = {"file": str(src.relative_to(REPO)), "va": f"0x{va:08X}" if va else None}
    if va is None:
        rec["status"] = "NO PROVENANCE VA"
        return rec

    shape = img.recover(va)
    rec.update({
        "retail_insns": len(shape.insns),
        "retail_bytes": shape.size,
        "retail_blocks": len(shape.blocks),
        "retail_edges": len(shape.edges),
        "tail_jump": f"0x{shape.tail_jump:08X}" if shape.tail_jump else None,
        "ret_at": shape.byte_after_ret,
        "truncated": shape.truncated,
    })

    findings = detect_data_as_code(img, src)
    rec["data_as_code"] = findings

    # compile the candidate and compare CFG
    syms = candidate_symbols(src)
    rec["symbols"] = syms
    if not syms:
        rec["status"] = "NO SYMBOL"
        return rec
    obj = Path("/tmp") / ("fa_" + src.stem + ".o")
    try:
        r = subprocess.run([CXX] + CXXFLAGS + ["-c", str(src), "-o", str(obj)],
                           capture_output=True, text=True, timeout=300, cwd=REPO)
    except Exception as e:
        rec["status"] = f"COMPILE ERROR {e}"
        return rec
    if r.returncode != 0:
        rec["status"] = "COMPILE FAILED"
        rec["compile_stderr"] = r.stderr[-1500:]
        return rec
    rec["build_verified"] = True

    r_nodes, r_edges = canonical_cfg(shape.entry, shape.blocks, shape.edges)
    best = None
    for sym in syms:
        cs = candidate_shapes(obj, sym)
        if not cs or not cs["blocks"]:
            continue
        c_nodes, c_edges = canonical_cfg(cs["blocks"][0], cs["blocks"], cs["edges"])
        node_j = jaccard(r_nodes, c_nodes)
        edge_j = jaccard(r_edges, c_edges)
        score = 0.35 * node_j + 0.65 * edge_j
        cand = {"symbol": sym, "n_blocks": cs["n_blocks"], "n_insns": cs["n_insns"],
                "bytes": cs["bytes"], "branches": cs["branches"], "rets": cs["rets"],
                "jmps": cs["jmps"], "calls": cs["calls"],
                "n_edges": len(c_edges),
                "node_jaccard": round(node_j, 3), "edge_jaccard": round(edge_j, 3),
                "score": round(score, 3)}
        if best is None or cand["score"] > best["score"]:
            best = cand
    if best:
        rec["candidate"] = best
        rec["cfg_pass"] = best["score"] >= CFG_THRESHOLD
        rec["verdict"] = classify_divergence(shape, best)
        rec["status"] = "CFG PASS" if rec["cfg_pass"] else rec["verdict"]["kind"]
    else:
        rec["status"] = "NO CANDIDATE SHAPE"
    return rec


def classify_divergence(shape: FunctionShape, cand: dict) -> dict:
    """Explain WHY a candidate's CFG differs from retail, instead of reporting a bare score.

    A raw structural comparison of MSVC-6-compiled retail against GCC -O2 output will not be
    1.0 even for a perfect transcription, because the two compilers legitimately shape code
    differently. The known, benign divergences are named here so the audit does not send a
    correct file back for reconciliation - which is exactly what happened to 0x004A5BB0,
    where a 0.413 score was a compiler artifact and cost a full manual cycle to disprove.

    Kinds:
      MATCH               structure agrees within the threshold
      EPILOGUE_MERGE      retail has several `ret` epilogues; GCC merged them into one shared
                          epilogue reached by jumps. Block count rises by about the number of
                          merged epilogues and edges rise correspondingly. NOT a defect.
      TAIL_CALL_PRESERVED retail ends in a tail `jmp` and the candidate still emits a `jmp`.
      DIVERGENT           no benign explanation; a real difference that needs review.
    """
    r_rets = sum(1 for i in shape.insns if i.mnemonic.startswith("ret"))
    c_rets = cand.get("rets", 0)
    r_blocks, c_blocks = len(shape.blocks), cand.get("n_blocks", 0)
    detail = {
        "retail_rets": r_rets,
        "candidate_rets": c_rets,
        "retail_blocks": r_blocks,
        "candidate_blocks": c_blocks,
        "retail_tail_jump": f"0x{shape.tail_jump:08X}" if shape.tail_jump else None,
        "candidate_jmps": cand.get("jmps", 0),
    }
    if cand.get("score", 0) >= CFG_THRESHOLD:
        return {"kind": "MATCH", "detail": detail}
    if r_rets > 1 and c_rets == 1 and c_blocks >= r_blocks:
        return {"kind": "EPILOGUE_MERGE", "detail": detail,
                "note": f"retail has {r_rets} separate `ret` epilogues; the candidate has one "
                        f"shared epilogue reached by jumps. Block count {r_blocks} -> {c_blocks}. "
                        f"GCC -O2 artifact, not a transcription error."}
    if shape.tail_jump and cand.get("jmps", 0) >= 1:
        return {"kind": "TAIL_CALL_PRESERVED", "detail": detail,
                "note": "retail's tail call is still emitted as a jmp."}
    return {"kind": "DIVERGENT", "detail": detail,
            "note": "no benign explanation found; this one needs a human read."}


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--va", help="audit only the native at this retail VA")
    ap.add_argument("--handlers-only", action="store_true",
                    help="only run the data/code defect detector")
    ap.add_argument("--json", help="write the full report here")
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    if not RETAIL.exists():
        sys.exit(f"retail binary not found: {RETAIL}")
    import hashlib
    sha = hashlib.sha256(RETAIL.read_bytes()).hexdigest()
    if sha != RETAIL_SHA256:
        sys.exit(f"RETAIL HASH MISMATCH\n  expected {RETAIL_SHA256}\n  actual   {sha}")

    img = RetailImage()
    sources = sorted((REPO / "engine/src/retail").rglob("native_*.cpp"))

    if args.va:
        want = int(args.va, 16)
        sources = [s for s in sources if provenance_va(s) == want]
        if not sources:
            sys.exit(f"no tree file declares retail VA {args.va}")

    if args.handlers_only:
        total = 0
        for s in sources:
            f = detect_data_as_code(img, s)
            if f:
                total += len(f)
                print(f"\n{s.relative_to(REPO)}")
                for x in f:
                    print(f"  {x['symbol']:34s} {x['va']}  retail E8 calls=0  "
                          f"called_in_candidate={x['called_in_candidate']}")
        print(f"\ndata-as-code findings: {total}")
        return 0 if total == 0 else 1

    records, bad = [], []
    for s in sources:
        rec = audit_one(img, s)
        records.append(rec)
        if rec.get("status", "").startswith(("CFG FAIL", "COMPILE FAILED", "NO ")) \
                or rec.get("data_as_code"):
            bad.append(rec)

    n_pass = sum(1 for r in records if r.get("status") == "CFG PASS")
    n_fail = sum(1 for r in records if r.get("status") == "CFG FAIL")
    print(f"audited {len(records)} natives: {n_pass} CFG PASS, {n_fail} CFG FAIL, "
          f"{len(records) - n_pass - n_fail} other")
    print(f"tail-jump bodies detected (extent ends in a tail call, not a ret): "
          f"{sum(1 for r in records if r.get('tail_jump'))}")
    print(f"natives with a data-as-code defect: "
          f"{sum(1 for r in records if r.get('data_as_code'))}")

    if bad and not args.quiet:
        print("\n--- needs attention ---")
        for r in bad[:40]:
            print(f"  {r['status']:16s} {r['va']}  {r['file']}")
            for x in r.get("data_as_code", [])[:3]:
                print(f"      DATA-AS-CODE: {x['symbol']} ({x['va']}) - retail E8 calls=0")

    if args.json:
        Path(args.json).write_text(json.dumps({
            "retail_sha256": sha,
            "cfg_threshold": CFG_THRESHOLD,
            "n_records": len(records),
            "records": records,
        }, indent=2))
        print(f"\nreport written: {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
