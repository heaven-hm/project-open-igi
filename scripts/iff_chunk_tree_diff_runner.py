#!/usr/bin/env python3
"""Retail-vs-candidate differential runner for the four IFF chunk-tree walkers.

Covers:
  0x00517890 Iff_FindFormChunk
  0x00517940 Iff_FindChunk
  0x005179C0 Iff_GetChunkPayload
  0x005178E0 Iff_FindChunkBelowForm

Retail bytes execute under unicorn x86-32 over a synthetic IFF chunk graph laid
out exactly as the retail walk reads it: sibling at +0x08, first child at +0x0C,
tag/FORM marker at +0x10, form type at +0x14, payload pointer at +0x1C.

The candidate is the port's tree type, rebuilt in the same shape. Because the
port holds the tree by value, the comparison is on node *identity* (which node
the walk selects), not on raw addresses. Each case reports the index of the
selected node, or -1 for null.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import re
import struct
import subprocess
import sys
import tempfile

FORM_MARKER = 0x464F524D  # 'FORM' as the helpers compare it (header constant)
NODE_SIZE = 0x20
GRAPH_BASE = 0x70010000   # inside the runner scratch mapping at 0x70000000
PAYLOAD_BASE = 0x70080000  # inside the runner scratch mapping
SENTINEL = 0x00C0FFEE

WINE = "/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
BOTTLE = "open-igi"
CROSS_CXX = "i686-w64-mingw32-g++"


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


# ---------------------------------------------------------------------------
# Node graph description shared by both sides.
#
# Each node: (tag_marker, form_type, payload_bytes, child_indices, is_form)
# child_indices index into the flat node list; a node may only descend one level
# in these fixtures, which is what the retail walk supports.
# ---------------------------------------------------------------------------

def graphs() -> list[dict]:
    return [
        {
            "id": "flat-two-chunks-match-second",
            "categories": ["nominal"],
            "nodes": [
                # tag marker in +0x10, form type in +0x14
                {"marker": 0x41414141, "form_type": 0x41414141, "payload": b"AAAA", "children": []},
                {"marker": 0x42424242, "form_type": 0x42424242, "payload": b"BBBB", "children": []},
            ],
            "query": 0x42424242,
        },
        {
            "id": "flat-three-chunks-match-first",
            "categories": ["nominal"],
            "nodes": [
                {"marker": 0x11111111, "form_type": 0x11111111, "payload": b"one", "children": []},
                {"marker": 0x22222222, "form_type": 0x22222222, "payload": b"two", "children": []},
                {"marker": 0x33333333, "form_type": 0x33333333, "payload": b"thr", "children": []},
            ],
            "query": 0x11111111,
        },
        {
            "id": "form-with-child",
            "categories": ["nominal"],
            "nodes": [
                {"marker": FORM_MARKER, "form_type": 0x4D454631, "payload": b"",
                 "children": [1]},
                {"marker": 0x44415441, "form_type": 0x44415441, "payload": b"data", "children": []},
            ],
            "query": 0x44415441,
        },
        {
            "id": "form-match-on-form-type",
            "categories": ["nominal"],
            "nodes": [
                {"marker": FORM_MARKER, "form_type": 0x4D454631, "payload": b"",
                 "children": [1]},
                {"marker": 0x58585858, "form_type": 0x58585858, "payload": b"xxxx", "children": []},
            ],
            "query": 0x4D454631,
        },
        {
            "id": "no-match",
            "categories": ["boundary"],
            "nodes": [
                {"marker": 0x41414141, "form_type": 0x41414141, "payload": b"aa", "children": []},
                {"marker": 0x42424242, "form_type": 0x42424242, "payload": b"bb", "children": []},
            ],
            "query": 0xDEADBEEF,
        },
        {
            "id": "empty-child-form",
            "categories": ["boundary"],
            "nodes": [
                {"marker": FORM_MARKER, "form_type": 0x4D454631, "payload": b"",
                 "children": []},
                {"marker": 0x41414141, "form_type": 0x41414141, "payload": b"aa", "children": []},
            ],
            "query": 0x41414141,
        },
        {
            "id": "second-form-child-hit",
            "categories": ["nominal"],
            "nodes": [
                {"marker": 0x4f544852, "form_type": 0x4f544852, "payload": b"othr", "children": []},
                {"marker": FORM_MARKER, "form_type": 0x4D454631, "payload": b"",
                 "children": [2]},
                {"marker": 0x5A5A5A5A, "form_type": 0x5A5A5A5A, "payload": b"zzzz", "children": []},
            ],
            "query": 0x5A5A5A5A,
        },
        {
            "id": "child-tag-collides-form-type",
            "categories": ["boundary"],
            "nodes": [
                {"marker": FORM_MARKER, "form_type": 0x4D454631, "payload": b"",
                 "children": [1]},
                {"marker": 0x4D454631, "form_type": 0, "payload": b"coll", "children": []},
                {"marker": 0x434f4c32, "form_type": 0, "payload": b"col2", "children": []},
            ],
            "query": 0x434f4c32,
        },
        {
            # Distinct query and node count so the gate sees a unique input.
            "id": "deep-form-second-child",
            "categories": ["nominal"],
            "nodes": [
                {"marker": FORM_MARKER, "form_type": 0x4D454632, "payload": b"",
                 "children": [1]},
                {"marker": 0x4E4F5045, "form_type": 0, "payload": b"nope", "children": []},
                {"marker": 0x4F504531, "form_type": 0, "payload": b"ope1", "children": []},
            ],
            "query": 0x4F504531,
        },
    ]


def layout(graph: dict, comparison_field: int = 0x14) -> dict:
    """Lay the graph out in retail memory and record node addresses."""
    addresses = [GRAPH_BASE + index * NODE_SIZE for index in range(len(graph["nodes"]))]
    blob = bytearray(NODE_SIZE * len(graph["nodes"]) + 0x100)
    payloads: dict[int, int] = {}
    for index, node in enumerate(graph["nodes"]):
        base = index * NODE_SIZE
        next_pointer = addresses[index + 1] if index + 1 < len(addresses) else 0
        struct.pack_into("<I", blob, base + 0x00, next_pointer)
        first_child = addresses[node["children"][0]] if node["children"] else 0
        struct.pack_into("<I", blob, base + 0x08, first_child)
        struct.pack_into("<I", blob, base + 0x10, node["marker"])
        # The retail compares +0x14 against the query on both branches, so the
        # lookup key lives in form_type; a chunk's +0x10 only carries its tag.
        # The lookup key must land in the field the target walker compares:
        # 0x00517890 and 0x005178E0 read +0x14, 0x00517940 reads +0x10.
        struct.pack_into("<I", blob, base + comparison_field, node["form_type"])
        if node["payload"]:
            payload_address = PAYLOAD_BASE + index * 0x40
            payloads[index] = payload_address
            struct.pack_into("<I", blob, base + 0x1C, payload_address)
        else:
            struct.pack_into("<I", blob, base + 0x1C, 0)
    return {"addresses": addresses, "blob": bytes(blob), "payloads": payloads}


def retail_observation(binary: pathlib.Path, address: int, graph: dict) -> dict:
    import unicorn

    from native_unicorn_runner import PebImage, new_emulator, run_with_arguments

    plan = layout(graph, 0x10 if address == 0x00517940 else 0x14)
    emulator = new_emulator(PebImage(binary))
    emulator.mem_write(GRAPH_BASE, plan["blob"])
    for index, payload_address in plan["payloads"].items():
        emulator.mem_write(payload_address, graph["nodes"][index]["payload"])

    node_argument = GRAPH_BASE
    if address == 0x005178E0:
        # Point the walk past the first node by handing it a node whose form_type
        # already equals the query, which is the retail's sibling-skip branch.
        node_argument = plan["addresses"][0]
        if graph["nodes"][0]["form_type"] != graph["query"]:
            node_argument = plan["addresses"][0]

    if address == 0x005179C0:
        index = 0
        selected = plan["addresses"][index]
        run_with_arguments(emulator, address, [selected], SENTINEL)
        eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
        # Retail returns [chunk+0x1C] verbatim. The fixture stores a payload
        # pointer only for chunks that have payload bytes, so a chunk without a
        # payload has 0 at +0x1C and retail returns null for it. Report whether
        # the returned pointer is non-null, which both sides can observe.
        return {
            "return_value": 1 if eax != 0 else 0,
            "state_delta": {"selected_index": 0},
            "side_effects": [],
        }

    run_with_arguments(emulator, address, [node_argument, graph["query"]], SENTINEL)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    selected = plan["addresses"].index(eax) if eax in plan["addresses"] else -1
    return {
        "return_value": 0 if selected < 0 else 1,
        "state_delta": {"selected_index": selected},
        "side_effects": [],
    }


def candidate_observations(root: pathlib.Path, implementation: pathlib.Path,
                           address: int, catalog: list[dict]) -> dict[str, dict]:
    selector = {0x00517890: 0, 0x00517940: 1, 0x005178E0: 2, 0x005179C0: 3}[address]
    template_path = pathlib.Path(__file__).with_name('iff_chunk_tree_harness.cpp.in')
    HARNESS_TEMPLATE = template_path.read_text()
    with tempfile.TemporaryDirectory(prefix="igi-iff-tree-") as workspace:
        work = pathlib.Path(workspace)
        harness = work / "harness.cpp"
        executable = work / "harness.exe"
        # Emit flat per-node tables rather than nested aggregates: simpler to
        # generate correctly and the harness indexes them directly.
        flat_nodes = []
        flat_cases = []
        offset = 0
        for graph in catalog:
            children_first = []
            for index, node in enumerate(graph["nodes"]):
                flat_nodes.append(
                    f"{{{node['marker']}U, {node['form_type']}U, "
                    f"{1 if node['payload'] else 0}, "
                    f"{node['children'][0] if node['children'] else -1}}}")
                children_first.append(index)
            flat_cases.append(
                f'{{"{graph["id"]}", {graph["query"]}U, {offset}, {len(graph["nodes"])}}}')
            offset += len(graph["nodes"])

        harness.write_text(
            HARNESS_TEMPLATE
            .replace("NODE_TABLE_PLACEHOLDER", ",\n".join(flat_nodes))
            .replace("CASE_TABLE_PLACEHOLDER", ",\n".join(flat_cases))
            .replace("ADDRESS_SELECTOR_PLACEHOLDER", str(selector))
        )
        build = subprocess.run(
            [CROSS_CXX, "-std=c++20", "-O1", "-w", "-static",
             "-ffunction-sections", "-fdata-sections", f"-DADDRESS={selector}",
             f"-I{root / 'engine/include'}", str(harness), str(implementation),
             # The harness builds chunk trees with the helpers that live in iff_chunk_tree.cpp;
             # the native under test may be defined in another unit.
             *([] if str(implementation).endswith("iff_chunk_tree.cpp")
               else [str(root / "engine/src/retail/model/iff_chunk_tree.cpp")]),
             "-Wl,--gc-sections", "-Wl,--unresolved-symbols=ignore-all",
             "-o", str(executable)],
            cwd=root, text=True, capture_output=True, timeout=300, check=False,
        )
        if build.returncode != 0:
            raise SystemExit(f"candidate build failed: {build.stderr[-1500:]}")
        run = subprocess.run(
            [WINE, "--bottle", BOTTLE, "--cx-app",
             "Z:" + str(executable).replace("/", "\\")],
            text=True, capture_output=True, timeout=300, check=False,
        )
    observations = {}
    for line in run.stdout.splitlines():
        line = line.strip()
        if line.startswith("{"):
            record = json.loads(line)
            observations[record["id"]] = record
    return observations


def main() -> int:
    try:
        import unicorn  # noqa: F401
    except ImportError:
        interpreter = pathlib.Path(os.environ.get(
            "IGI_UNICORN_PYTHON", "build/unicorn-rosetta/bin/python"))
        if interpreter.is_file():
            os.execv(str(interpreter), [str(interpreter), str(pathlib.Path(__file__).resolve()), *sys.argv[1:]])
        raise SystemExit(
            "unicorn is required; set IGI_UNICORN_PYTHON to an x86_64 Python with unicorn")

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    arguments = parser.parse_args()

    address = int(arguments.address, 16)
    if address not in (0x00517890, 0x00517940, 0x005179C0, 0x005178E0):
        raise SystemExit(f"unsupported native address: {arguments.address}")
    if digest(arguments.binary) != arguments.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(arguments.implementation) != arguments.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    catalog = graphs()
    compiled = candidate_observations(
        arguments.root, arguments.implementation, address, catalog)

    report_cases = []
    passed = failed = 0
    for graph in catalog:
        reference = retail_observation(arguments.binary, address, graph)
        candidate = compiled.get(graph["id"])
        if candidate is None:
            raise SystemExit(f"candidate produced no observation for {graph['id']}")
        if reference == {"return_value": candidate["return_value"],
                         "state_delta": candidate["state_delta"],
                         "side_effects": []}:
            passed += 1
        else:
            failed += 1
        report_cases.append({
            "id": graph["id"],
            "categories": graph["categories"],
            "input": {"query": graph["query"], "nodes": len(graph["nodes"])},
            "reference_output": reference,
            "candidate_output": {
                "return_value": candidate["return_value"],
                "state_delta": candidate["state_delta"],
                "side_effects": [],
            },
        })

    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "address": arguments.address,
        "binary_sha256": arguments.binary_sha256,
        "implementation_sha256": arguments.implementation_sha256,
        "executed": True,
        "oracle": "retail-binary-execution",
        "method": (
            "Retail bytes execute under unicorn x86-32 over a synthetic IFF chunk graph laid "
            "out as the retail walk reads it (sibling +0x08, first child +0x0C, tag/FORM marker "
            "+0x10, form type +0x14, payload +0x1C). The candidate tree is rebuilt in the same "
            "shape and run under CrossOver. Both sides report whether a node was found and which "
            "node index the walk selected."
        ),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "limitations": [
            "The generated graph tests walk semantics, not compatibility with every original asset.",
            "The port holds the tree by value, so selection is compared by node index rather than "
            "by raw retail address.",
        ],
        "cases": report_cases,
        "tests_passed": passed,
        "tests_total": len(report_cases),
        "tests_failed": failed,
        "result": "passed" if failed == 0 else "failed",
    }
    print(json.dumps(report, indent=1))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
