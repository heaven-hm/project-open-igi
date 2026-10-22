#!/usr/bin/env python3
"""Retail-vs-candidate differential runner for IGI float/x87 natives.

The retail oracle executes the true retail function bytes under the unicorn
emulator (x86-32) loaded from the selected PE image; the candidate is compiled
from the current C++ implementation and executed natively; both sides receive
the same synthetic state and their observables are compared case by case.

This runner exists because the touchstone natives for the first-person view
build (0x004659E0, 0x00491D40, 0x00497D70) use x87 floating point (fptan,
fld/fmul/fdiv/fstp), which Radare2 ESIL and angr's VEX executor do not
implement. Unicorn 2.1.4 executes those instructions faithfully with the real
PE image, so the reference output still comes from retail-binary-execution.

Observables are emitted as JSON numbers/strings so the gate can re-compare
reference and candidate within the declared float tolerance.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import os
import pathlib
import struct
import subprocess
import sys
import tempfile

CXX = os.environ.get("CXX", "c++")

IMAGE_BASE_FALLBACK = 0x400000
STACK_TOP = 0x70008000
SCRATCH_BASE = 0x70010000
CONTEXT_ADDR = SCRATCH_BASE + 0x200
VIEW_SOURCE_ADDR = SCRATCH_BASE
CAMERA_SOURCE_ADDR = SCRATCH_BASE + 0x100
STUB_ADDR = SCRATCH_BASE + 0x9000
STUB_RESULT_ADDR = SCRATCH_BASE + 0x9100
SCRATCH_SIZE = 0x200000
FILL_PATTERN = 0xA5

FLOAT_TOLERANCE = {"absolute": 1e-6, "relative": 1e-6}


def sha256(path: pathlib.Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1 << 20), b""):
            hasher.update(block)
    return hasher.hexdigest()


class PebImage:
    """Minimal PE32 loader: sections written at image_base + virtual_address."""

    def __init__(self, binary: pathlib.Path) -> None:
        raw = binary.read_bytes()
        e_lfanew = struct.unpack_from("<I", raw, 0x3C)[0]
        if raw[e_lfanew:e_lfanew + 4] != b"PE\0\0":
            raise SystemExit("retail binary is not a PE image")
        section_count = struct.unpack_from("<H", raw, e_lfanew + 6)[0]
        optional_size = struct.unpack_from("<H", raw, e_lfanew + 20)[0]
        self.image_base = struct.unpack_from("<I", raw, e_lfanew + 24 + 28)[0]
        self.sections = []
        offset = e_lfanew + 24 + optional_size
        for _ in range(section_count):
            virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from(
                "<IIII", raw, offset + 8)
            self.sections.append(
                (self.image_base + virtual_address, raw[raw_pointer:raw_pointer + raw_size]))
            offset += 40

    def map_into(self, emulator) -> None:  # noqa: ANN001 - unicorn.Uc
        size = 0x1000000
        emulator.mem_map(self.image_base, size)
        for address, data in self.sections:
            if data:
                emulator.mem_write(address, data)


def new_emulator(image: PebImage):
    import unicorn

    emulator = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
    image.map_into(emulator)
    emulator.mem_map(0x70000000, SCRATCH_SIZE)
    return emulator


def read_u32(emulator, address: int) -> int:
    return struct.unpack("<I", emulator.mem_read(address, 4))[0]


def read_f32(emulator, address: int) -> float:
    return struct.unpack("<f", emulator.mem_read(address, 4))[0]


def run_with_arguments(emulator, entry: int, arguments: list[int], sentinel: int) -> None:
    """cdecl stack call: [esp] = sentinel return address, then the arguments."""
    emulator.mem_write(STACK_TOP, struct.pack("<I", sentinel))
    for index, value in enumerate(arguments):
        emulator.mem_write(STACK_TOP + 4 + index * 4, struct.pack("<I", value & 0xFFFFFFFF))
    import unicorn

    emulator.reg_write(unicorn.x86_const.UC_X86_REG_ESP, STACK_TOP)
    emulator.emu_start(entry, sentinel)


# ---------------------------------------------------------------------------
# 0x004659E0 Human_TaskViewReset
# ---------------------------------------------------------------------------

VIEW_RESET_ADDRESS = 0x004659E0
VIEW_RESET_BUFFER = 0x70011000
VIEW_RESET_TANGENT_X = 0x1E4
VIEW_RESET_TANGENT_VM = 0x1E8
VIEW_RESET_AUX = 0x1EC


def view_reset_cases() -> list[dict]:
    cases = []
    for index in range(6):
        cases.append({
            "id": f"human-view-reset-{index}",
            "categories": ["nominal"],
            "pointer_offset": index * 0x400,
            "seed": index,
        })
    cases.append({
        "id": "human-view-reset-unaligned",
        "categories": ["boundary"],
        "pointer_offset": 1,
        "seed": 6,
    })
    cases.append({
        "id": "human-view-reset-tail",
        "categories": ["boundary"],
        "pointer_offset": 0x4000 - 0x300,
        "seed": 7,
    })
    return cases


def view_reset_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    pointer = VIEW_RESET_BUFFER + case["pointer_offset"]
    emulator.mem_write(pointer, bytes([FILL_PATTERN]) * 0x400)
    sentinel = 0x00C0FFEE
    run_with_arguments(emulator, VIEW_RESET_ADDRESS, [pointer], sentinel)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    buffer = emulator.mem_read(pointer, 0x400)
    return {
        "return_value": eax - pointer,
        "state_delta": {
            "tangent_x": struct.unpack("<f", buffer[VIEW_RESET_TANGENT_X:VIEW_RESET_TANGENT_X + 4])[0],
            "tangent_viewmodel": struct.unpack("<f", buffer[VIEW_RESET_TANGENT_VM:VIEW_RESET_TANGENT_VM + 4])[0],
            "aux": struct.unpack("<i", buffer[VIEW_RESET_AUX:VIEW_RESET_AUX + 4])[0],
            "changed_before": sum(1 for value in buffer[:VIEW_RESET_TANGENT_X] if value != FILL_PATTERN),
            "changed_after": sum(1 for value in buffer[0x1F0:] if value != FILL_PATTERN),
        },
        "side_effects": [],
    }


def view_reset_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/human/human_view.hpp"

int main()
{
    struct Case { const char* id; std::size_t pointer_offset; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char human[0x8000];
        std::memset(human, 0xA5, sizeof(human));
        unsigned char* pointer = human + item.pointer_offset;
        void* raw = igi::retail::human::Human_TaskViewReset(pointer);
        float tangent_x = 0.0F;
        float tangent_viewmodel = 0.0F;
        std::int32_t aux = 0;
        std::memcpy(&tangent_x, pointer + 0x1E4, 4);
        std::memcpy(&tangent_viewmodel, pointer + 0x1E8, 4);
        std::memcpy(&aux, pointer + 0x1EC, 4);
        std::size_t changed_before = 0;
        std::size_t changed_after = 0;
        for (std::size_t index = 0; index < 0x1E4; ++index)
        {
            if (pointer[index] != 0xA5) { ++changed_before; }
        }
        for (std::size_t index = 0x1F0; index < 0x400; ++index)
        {
            if (pointer[index] != 0xA5) { ++changed_after; }
        }
        const auto return_offset =
            static_cast<long long>(reinterpret_cast<std::uintptr_t>(raw) -
                                   reinterpret_cast<std::uintptr_t>(pointer));
        std::printf(
            "{\"id\":\"%s\",\"return_value\":%lld,\"state_delta\":{\"tangent_x\":%.17g,\"tangent_viewmodel\":%.17g,"
            "\"aux\":%d,\"changed_before\":%zu,\"changed_after\":%zu},\"side_effects\":[]}\n",
            item.id, return_offset, static_cast<double>(tangent_x), static_cast<double>(tangent_viewmodel),
            static_cast<int>(aux), changed_before, changed_after);
    }
    return 0;
}
"""


# ---------------------------------------------------------------------------
# 0x00491D40 screen aspect factor
# ---------------------------------------------------------------------------

ASPECT_ADDRESS = 0x00491D40
DISPLAY_SINGLETON = 0x00C28B40


def aspect_cases() -> list[dict]:
    tables = [
        (1024, 640, "nominal"),   # the shipped config.qvm resolution
        (640, 480, "nominal"),
        (800, 600, "nominal"),
        (1280, 1024, "nominal"),
        (320, 200, "nominal"),
        (1920, 1080, "nominal"),
        (1279, 1023, "boundary"),
        (4096, 2161, "boundary"),
    ]
    return [
        {"id": f"screen-aspect-{width}x{height}", "categories": [category],
         "display_width": width, "display_height": height, "seed": index}
        for index, (width, height, category) in enumerate(tables)
    ]


def aspect_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    emulator.mem_write(DISPLAY_SINGLETON + 4, struct.pack("<i", case["display_width"]))
    emulator.mem_write(DISPLAY_SINGLETON + 8, struct.pack("<i", case["display_height"]))
    # Stub at STUB_ADDR: `fstp qword [STUB_RESULT_ADDR]` then `ret` to the second sentinel,
    # because the retail value is returned in st(0) and unicorn does not expose x87
    # registers through reg_read on this build.
    stub = b"\xDD\x1D" + struct.pack("<I", STUB_RESULT_ADDR) + b"\xC3"
    emulator.mem_write(STUB_ADDR, stub)
    second_sentinel = 0x00D0FFEE
    emulator.mem_write(STACK_TOP, struct.pack("<I", STUB_ADDR))
    emulator.mem_write(STACK_TOP + 4, struct.pack("<I", second_sentinel))
    emulator.reg_write(unicorn.x86_const.UC_X86_REG_ESP, STACK_TOP)
    emulator.emu_start(ASPECT_ADDRESS, second_sentinel)
    value = struct.unpack("<d", emulator.mem_read(STUB_RESULT_ADDR, 8))[0]
    return {
        "return_value": value,
        "state_delta": {"display_width": case["display_width"], "display_height": case["display_height"]},
        "side_effects": [],
    }


def aspect_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstdint>
#include "igi/retail/render/view_block.hpp"

int main()
{
    struct Case { const char* id; int width; int height; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        igi::retail::render::Display_HostSetDimensions(item.width, item.height);
        const double value = igi::retail::render::Display_ComputeScreenAspect();
        std::printf(
            "{\"id\":\"%s\",\"return_value\":%.17g,\"state_delta\":{\"display_width\":%d,\"display_height\":%d},"
            "\"side_effects\":[]}\n",
            item.id, value, item.width, item.height);
    }
    return 0;
}
"""


# ---------------------------------------------------------------------------
# 0x00497D70 transform-context view block builder
# ---------------------------------------------------------------------------

BUILDER_ADDRESS = 0x00497D70
SCREEN_BLOCK_BASE = 0x00BCABB8


def builder_cases() -> list[dict]:
    specifications = [
        # (id, category, display, rect, fov_x, camera, view pattern revision)
        ("build-view-block-fullscreen-1024x640", "nominal", (1024, 640), (0.0, 0.0, 1024.0, 640.0),
         1.0, (12.5, -64.25, 40.0), 1),
        ("build-view-block-fullscreen-640x480", "nominal", (640, 480), (0.0, 0.0, 640.0, 480.0),
         1.0, (0.0, 0.0, 0.0), 2),
        ("build-view-block-viewmodel-tangent", "nominal", (1024, 640), (0.0, 0.0, 1024.0, 640.0),
         0.57735026, (-8.0, 3.5, 12.0), 3),
        ("build-view-block-subrect", "nominal", (1280, 1024), (64.0, 32.0, 1152.0, 960.0),
         1.0, (100.0, 200.0, 300.0), 4),
        ("build-view-block-half-height-rect", "nominal", (800, 600), (0.0, 0.0, 800.0, 300.0),
         1.25, (-1.0, -2.0, -3.0), 5),
        ("build-view-block-zoom-in", "boundary", (1024, 640), (0.0, 0.0, 1024.0, 640.0),
         0.1, (5.0, 5.0, 5.0), 6),
        ("build-view-block-zoom-out", "boundary", (640, 480), (0.0, 0.0, 640.0, 480.0),
         5.0, (-100.0, 250.0, 0.5), 7),
        ("build-view-block-odd-screen", "boundary", (1279, 1023), (17.0, 11.0, 1024.0, 640.5),
         0.9, (0.125, -0.5, 4096.0), 8),
    ]
    cases = []
    for index, (case_id, category, display, rect, fov_x, camera, revision) in enumerate(specifications):
        cases.append({
            "id": case_id,
            "categories": [category],
            "display": list(display),
            "rect": list(rect),
            "fov_x": fov_x,
            "camera": list(camera),
            "view_pattern_revision": revision,
            "seed": index,
        })
    return cases


def pattern_bytes(revision: int, size: int) -> bytes:
    return bytes(((revision * 31 + index * 7) & 0xFF) for index in range(size))


def builder_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    width, height = case["display"]
    emulator.mem_write(DISPLAY_SINGLETON + 4, struct.pack("<i", width))
    emulator.mem_write(DISPLAY_SINGLETON + 8, struct.pack("<i", height))
    x, y, rect_width, rect_height = case["rect"]
    block = [x, y, rect_width, rect_height, rect_width * 0.5, rect_height * 0.5]
    for index, value in enumerate(block):
        emulator.mem_write(SCREEN_BLOCK_BASE + index * 4, struct.pack("<f", value))
    view_source = pattern_bytes(case["view_pattern_revision"], 0x28)
    camera_source = struct.pack("<3d", *case["camera"])
    emulator.mem_write(VIEW_SOURCE_ADDR, view_source)
    emulator.mem_write(CAMERA_SOURCE_ADDR, camera_source)
    emulator.mem_write(CONTEXT_ADDR, b"\x00" * 0x68)
    fov_bits = struct.unpack("<I", struct.pack("<f", case["fov_x"]))[0]
    sentinel = 0x00C0FFEE
    run_with_arguments(emulator, BUILDER_ADDRESS,
                       [CONTEXT_ADDR, VIEW_SOURCE_ADDR, CAMERA_SOURCE_ADDR, fov_bits], sentinel)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    context = emulator.mem_read(CONTEXT_ADDR, 0x68)
    return {
        "return_value": eax,
        "state_delta": {
            "vfovx": struct.unpack("<f", context[0x40:0x44])[0],
            "vfovy": struct.unpack("<f", context[0x44:0x48])[0],
            "screen_fovx": struct.unpack("<f", context[0x48:0x4C])[0],
            "screen_fovy": struct.unpack("<f", context[0x4C:0x50])[0],
            "origin_x": struct.unpack("<f", context[0x50:0x54])[0],
            "origin_y": struct.unpack("<f", context[0x54:0x58])[0],
            "is_draw": struct.unpack("<i", context[0x58:0x5C])[0],
            "layer": struct.unpack("<i", context[0x60:0x64])[0],
            "view_copy_hex": context[0x00:0x28].hex(),
            "camera_copy_hex": context[0x28:0x40].hex(),
        },
        "side_effects": [],
    }


def builder_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstdint>
#include <cstring>
#include "igi/retail/render/view_block.hpp"

int main()
{
    struct Case { const char* id; int width; int height; float x; float y; float rect_w; float rect_h;
                  float fov_x; double cx; double cy; double cz; int revision; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        igi::retail::render::Display_HostSetDimensions(item.width, item.height);
        igi::retail::render::ScreenBlock_HostInstallRect(item.x, item.y, item.rect_w, item.rect_h);
        unsigned char view_source[0x28];
        for (std::size_t index = 0; index < sizeof(view_source); ++index)
        {
            view_source[index] = static_cast<unsigned char>((item.revision * 31 + index * 7) & 0xFF);
        }
        double camera_source[3] = {item.cx, item.cy, item.cz};
        unsigned char context[0x68];
        std::memset(context, 0, sizeof(context));
        const std::int32_t result = igi::retail::render::Transcontext_BuildViewBlock(
            context, view_source, camera_source, item.fov_x);
        float vfovx = 0.0F, vfovy = 0.0F, screen_fovx = 0.0F, screen_fovy = 0.0F;
        float origin_x = 0.0F, origin_y = 0.0F;
        std::int32_t is_draw = 0, layer = 0;
        std::memcpy(&vfovx, context + 0x40, 4);
        std::memcpy(&vfovy, context + 0x44, 4);
        std::memcpy(&screen_fovx, context + 0x48, 4);
        std::memcpy(&screen_fovy, context + 0x4C, 4);
        std::memcpy(&origin_x, context + 0x50, 4);
        std::memcpy(&origin_y, context + 0x54, 4);
        std::memcpy(&is_draw, context + 0x58, 4);
        std::memcpy(&layer, context + 0x60, 4);
        char view_hex[0x51];
        char camera_hex[0x31];
        for (std::size_t index = 0; index < 0x28; ++index)
        {
            std::snprintf(view_hex + index * 2, 3, "%02x", context[index]);
        }
        for (std::size_t index = 0; index < 0x18; ++index)
        {
            std::snprintf(camera_hex + index * 2, 3, "%02x", context[0x28 + index]);
        }
        std::printf(
            "{\"id\":\"%s\",\"return_value\":%d,\"state_delta\":{\"vfovx\":%.17g,\"vfovy\":%.17g,"
            "\"screen_fovx\":%.17g,\"screen_fovy\":%.17g,\"origin_x\":%.17g,\"origin_y\":%.17g,"
            "\"is_draw\":%d,\"layer\":%d,\"view_copy_hex\":\"%s\",\"camera_copy_hex\":\"%s\"},\"side_effects\":[]}\n",
            item.id, static_cast<int>(result), static_cast<double>(vfovx), static_cast<double>(vfovy),
            static_cast<double>(screen_fovx), static_cast<double>(screen_fovy),
            static_cast<double>(origin_x), static_cast<double>(origin_y),
            static_cast<int>(is_draw), static_cast<int>(layer), view_hex, camera_hex);
    }
    return 0;
}
"""


# ---------------------------------------------------------------------------
# 0x00465820 Human_CameraEyePosition and 0x00463180 Human_IsCrouched
# ---------------------------------------------------------------------------

EYE_ADDRESS = 0x00465820
CROUCH_ADDRESS = 0x00463180
HUMAN_ADDR = SCRATCH_BASE + 0x4000
EYE_OUT_ADDR = SCRATCH_BASE + 0x8000


def eye_cases() -> list[dict]:
    return [
        {"id": "eye-standing-nominal", "categories": ["nominal"], "seed": 0, "human_offset": 0,
         "feet_x": 1234.5, "feet_y": -2345.25, "feet_z": 100.0, "crouch": 0},
        {"id": "eye-crouched-nominal", "categories": ["nominal"], "seed": 1, "human_offset": 0,
         "feet_x": 1234.5, "feet_y": -2345.25, "feet_z": 100.0, "crouch": 1},
        {"id": "eye-origin", "categories": ["nominal"], "seed": 2, "human_offset": 0x100,
         "feet_x": 0.0, "feet_y": 0.0, "feet_z": 0.0, "crouch": 0},
        {"id": "eye-negative-feet", "categories": ["boundary"], "seed": 3, "human_offset": 0x180,
         "feet_x": -8192.0, "feet_y": 4096.0, "feet_z": -4096.5, "crouch": 1},
        {"id": "eye-fractional", "categories": ["boundary"], "seed": 4, "human_offset": 0x200,
         "feet_x": 0.5, "feet_y": 0.25, "feet_z": 0.125, "crouch": 0},
        {"id": "eye-large", "categories": ["boundary"], "seed": 5, "human_offset": 0x280,
         "feet_x": 1048576.0, "feet_y": -1048576.0, "feet_z": 2048.0, "crouch": 1},
        {"id": "eye-unaligned", "categories": ["boundary"], "seed": 6, "human_offset": 1,
         "feet_x": 7.0, "feet_y": 8.0, "feet_z": 9.0, "crouch": 0},
        {"id": "eye-unaligned-crouched", "categories": ["boundary"], "seed": 7, "human_offset": 3,
         "feet_x": 10.0, "feet_y": 11.0, "feet_z": 12.0, "crouch": 1},
    ]


def eye_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    emulator.mem_write(HUMAN_ADDR, bytes([FILL_PATTERN]) * 0x400)
    human = HUMAN_ADDR + case["human_offset"]
    emulator.mem_write(human + 0x20, struct.pack("<d", case["feet_x"]))
    emulator.mem_write(human + 0x28, struct.pack("<d", case["feet_y"]))
    emulator.mem_write(human + 0x30, struct.pack("<d", case["feet_z"]))
    emulator.mem_write(human + 0x33D, struct.pack("<B", case["crouch"]))
    emulator.mem_write(EYE_OUT_ADDR, bytes([FILL_PATTERN]) * 0x40)
    run_with_arguments(emulator, EYE_ADDRESS, [EYE_OUT_ADDR, human], 0x00C0FFEE)
    out = emulator.mem_read(EYE_OUT_ADDR, 0x40)
    return {
        "return_value": 0,
        "state_delta": {
            "eye_x": struct.unpack("<d", out[0:8])[0],
            "eye_y": struct.unpack("<d", out[8:16])[0],
            "eye_z": struct.unpack("<d", out[16:24])[0],
            "changed_after": sum(1 for value in out[0x18:] if value != FILL_PATTERN),
        },
        "side_effects": [],
    }


def eye_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/human/human_camera_eye.hpp"

int main()
{
    struct Case { const char* id; std::size_t human_offset; double feet_x; double feet_y; double feet_z; unsigned crouch; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char human[0x800];
        std::memset(human, 0xA5, sizeof(human));
        unsigned char* pointer = human + item.human_offset;
        std::memcpy(pointer + 0x20, &item.feet_x, 8);
        std::memcpy(pointer + 0x28, &item.feet_y, 8);
        std::memcpy(pointer + 0x30, &item.feet_z, 8);
        pointer[0x33D] = static_cast<unsigned char>(item.crouch);
        static unsigned char out[0x40];
        std::memset(out, 0xA5, sizeof(out));
        igi::retail::human::Human_CameraEyePosition(out, pointer);
        double eye_x = 0.0;
        double eye_y = 0.0;
        double eye_z = 0.0;
        std::memcpy(&eye_x, out + 0, 8);
        std::memcpy(&eye_y, out + 8, 8);
        std::memcpy(&eye_z, out + 16, 8);
        std::size_t changed_after = 0;
        for (std::size_t index = 0x18; index < 0x40; ++index)
        {
            if (out[index] != 0xA5) { ++changed_after; }
        }
        std::printf(
            "{\"id\":\"%s\",\"return_value\":0,\"state_delta\":{\"eye_x\":%.17g,\"eye_y\":%.17g,"
            "\"eye_z\":%.17g,\"changed_after\":%zu},\"side_effects\":[]}\n",
            item.id, eye_x, eye_y, eye_z, changed_after);
    }
    return 0;
}
"""


def crouch_cases() -> list[dict]:
    return [
        {"id": "crouch-clear-nominal", "categories": ["nominal"], "seed": 0, "human_offset": 0, "flag": 0},
        {"id": "crouch-set-nominal", "categories": ["nominal"], "seed": 1, "human_offset": 0x40, "flag": 1},
        {"id": "crouch-max", "categories": ["boundary"], "seed": 2, "human_offset": 0x80, "flag": 0xFF},
        {"id": "crouch-high-bit", "categories": ["boundary"], "seed": 3, "human_offset": 0xC0, "flag": 0x80},
        {"id": "crouch-odd", "categories": ["boundary"], "seed": 4, "human_offset": 0x100, "flag": 0x7F},
        {"id": "crouch-fill-pattern", "categories": ["boundary"], "seed": 5, "human_offset": 0x140, "flag": FILL_PATTERN},
        {"id": "crouch-unaligned-1", "categories": ["boundary"], "seed": 6, "human_offset": 1, "flag": 0x33},
        {"id": "crouch-unaligned-3", "categories": ["boundary"], "seed": 7, "human_offset": 3, "flag": 0x0F},
    ]


def crouch_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    emulator.mem_write(HUMAN_ADDR, bytes([FILL_PATTERN]) * 0x400)
    human = HUMAN_ADDR + case["human_offset"]
    emulator.mem_write(human + 0x33D, struct.pack("<B", case["flag"]))
    run_with_arguments(emulator, CROUCH_ADDRESS, [human], 0x00C0FFEE)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    buffer = emulator.mem_read(HUMAN_ADDR, 0x400)
    return {
        "return_value": eax & 0xFF,
        "state_delta": {
            "human_changed": sum(1 for value in buffer if value != FILL_PATTERN),
        },
        "side_effects": [],
    }


def crouch_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/human/human_camera_eye.hpp"

int main()
{
    struct Case { const char* id; std::size_t human_offset; unsigned flag; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char human[0x400];
        std::memset(human, 0xA5, sizeof(human));
        unsigned char* pointer = human + item.human_offset;
        pointer[0x33D] = static_cast<unsigned char>(item.flag);
        const std::uint8_t result = igi::retail::human::Human_IsCrouched(pointer);
        std::size_t changed = 0;
        for (std::size_t index = 0; index < sizeof(human); ++index)
        {
            if (human[index] != 0xA5) { ++changed; }
        }
        std::printf("{\"id\":\"%s\",\"return_value\":%u,\"state_delta\":{\"human_changed\":%zu},\"side_effects\":[]}\n",
                    item.id, static_cast<unsigned>(result), changed);
    }
    return 0;
}
"""


# ---------------------------------------------------------------------------
# 0x004EDB60 LODSettings_LoadParameters
# ---------------------------------------------------------------------------

LOD_ADDRESS = 0x004EDB60
LOD_CONTEXT_ADDR = SCRATCH_BASE + 0xC000
LOD_RECORD_ADDR = SCRATCH_BASE + 0xE000
LOD_REGISTRY_LOOKUP_ADDRESS = 0x004CEC10


def lod_cases() -> list[dict]:
    return [
        {"id": "lod-increasing-record", "categories": ["nominal"], "seed": 0, "name": "LOD_1",
         "distances": (7.0, 17.0, 30.0, 35.0, 50.0), "record_present": 1},
        {"id": "lod-decreasing-record", "categories": ["boundary"], "seed": 1, "name": "LOD_2",
         "distances": (50.0, 40.0, 30.0, 20.0, 10.0), "record_present": 1},
        {"id": "lod-mixed-record", "categories": ["nominal"], "seed": 2, "name": "LOD_3",
         "distances": (10.0, 5.0, 30.0, 25.0, 60.0), "record_present": 1},
        {"id": "lod-zero-record", "categories": ["boundary"], "seed": 3, "name": "LOD_4",
         "distances": (0.0, 0.0, 0.0, 0.0, 0.0), "record_present": 1},
        {"id": "lod-negative-missing", "categories": ["boundary"], "seed": 4, "name": "LOD_5",
         "distances": (-4.0, -8.0, 2.0, 1.0, 4.0), "record_present": 0},
        {"id": "lod-equal-missing", "categories": ["nominal"], "seed": 5, "name": "LOD_6",
         "distances": (2.5, 2.5, 2.5, 2.5, 2.5), "record_present": 0},
        {"id": "lod-large-record", "categories": ["boundary"], "seed": 6, "name": "LOD_7",
         "distances": (1000000.0, 999999.5, 12345.25, 65536.0, 4096.0), "record_present": 1},
        {"id": "lod-empty-name-missing", "categories": ["boundary"], "seed": 7, "name": "",
         "distances": (1.0, 2.0, 1.0, 4.0, 3.0), "record_present": 0},
    ]


def read_lod_record(record: bytes) -> dict:
    state = {
        "record_applied": struct.unpack("<i", record[0x24:0x28])[0],
        "record_changed_total": sum(1 for value in record if value != FILL_PATTERN),
        "record_changed_outside": sum(
            1 for index, value in enumerate(record)
            if value != FILL_PATTERN and not 0x24 <= index < 0x3C),
    }
    for index in range(5):
        state[f"record_distance_{index}"] = struct.unpack(
            "<f", record[0x28 + index * 4:0x2C + index * 4])[0]
    return state


def lod_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    # The retail call at 0x004EDB8F enters DynObj_Missing (0x004CEC10), whose registry walk needs
    # game-startup state (the list head at 0x00A4EC60 is zero in a cold image). The fixture replaces
    # that entry with the documented registry boundary: return the scratch record, or zero.
    if case["record_present"]:
        emulator.mem_write(LOD_REGISTRY_LOOKUP_ADDRESS,
                           b"\xb8" + struct.pack("<I", LOD_RECORD_ADDR) + b"\xc3")
    else:
        emulator.mem_write(LOD_REGISTRY_LOOKUP_ADDRESS, b"\x31\xc0\xc3")
    emulator.mem_write(LOD_RECORD_ADDR, bytes([FILL_PATTERN]) * 0x40)
    emulator.mem_write(LOD_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x100)
    emulator.mem_write(LOD_CONTEXT_ADDR + 0x20, case["name"].encode("ascii") + b"\0")
    for index, value in enumerate(case["distances"]):
        emulator.mem_write(LOD_CONTEXT_ADDR + 0x30 + index * 4, struct.pack("<f", value))
    run_with_arguments(emulator, LOD_ADDRESS, [LOD_CONTEXT_ADDR], 0x00C0FFEE)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    context = emulator.mem_read(LOD_CONTEXT_ADDR, 0x100)
    record = emulator.mem_read(LOD_RECORD_ADDR, 0x40)
    state: dict = {}
    for index in range(5):
        state[f"distance_{index}"] = struct.unpack("<f", context[0x30 + index * 4:0x34 + index * 4])[0]
    state["changed_after"] = sum(1 for value in context[0x44:] if value != FILL_PATTERN)
    state.update(read_lod_record(record))
    return {
        # The retail return register carries the dyn-object record pointer; the fixture compares the
        # resolved/not-resolved outcome because a raw host pointer is not comparable.
        "return_value": 1 if eax != 0 else 0,
        "state_delta": state,
        "side_effects": [],
    }


def lod_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/render/lod_settings.hpp"

static igi::retail::render::LODSettingsRecord* resolve_target = nullptr;

int main()
{
    struct Case { const char* id; const char* name; float d0; float d1; float d2; float d3; float d4; int record_present; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char context[0x100];
        std::memset(context, 0xA5, sizeof(context));
        std::snprintf(reinterpret_cast<char*>(context + 0x20), 0x10, "%s", item.name);
        const float distances[5] = {item.d0, item.d1, item.d2, item.d3, item.d4};
        for (std::size_t index = 0; index < 5; ++index)
        {
            std::memcpy(context + 0x30 + index * 4, &distances[index], 4);
        }
        static igi::retail::render::LODSettingsRecord record;
        std::memset(record.bytes.data(), 0xA5, record.bytes.size());
        resolve_target = item.record_present != 0 ? &record : nullptr;
        igi::retail::render::LODSettingsRecord* resolved =
            igi::retail::render::LODSettings_LoadParameters(
                context, [](const char*) noexcept -> igi::retail::render::LODSettingsRecord* {
                    return resolve_target;
                });
        float out[5] = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
        for (std::size_t index = 0; index < 5; ++index)
        {
            std::memcpy(&out[index], context + 0x30 + index * 4, 4);
        }
        std::size_t changed_after = 0;
        for (std::size_t index = 0x44; index < sizeof(context); ++index)
        {
            if (context[index] != 0xA5) { ++changed_after; }
        }
        std::int32_t record_applied = 0;
        std::memcpy(&record_applied, record.bytes.data() + 0x24, 4);
        std::size_t record_changed_total = 0;
        std::size_t record_changed_outside = 0;
        for (std::size_t index = 0; index < record.bytes.size(); ++index)
        {
            if (record.bytes[index] != 0xA5)
            {
                ++record_changed_total;
                if (index < 0x24 || index >= 0x3C) { ++record_changed_outside; }
            }
        }
        const float* record_distances = record.distances();
        std::printf(
            "{\"id\":\"%s\",\"return_value\":%d,\"state_delta\":{\"distance_0\":%.17g,\"distance_1\":%.17g,"
            "\"distance_2\":%.17g,\"distance_3\":%.17g,\"distance_4\":%.17g,\"changed_after\":%zu,"
            "\"record_applied\":%d,\"record_changed_total\":%zu,\"record_changed_outside\":%zu,"
            "\"record_distance_0\":%.17g,\"record_distance_1\":%.17g,\"record_distance_2\":%.17g,"
            "\"record_distance_3\":%.17g,\"record_distance_4\":%.17g},\"side_effects\":[]}\n",
            item.id, resolved != nullptr ? 1 : 0, static_cast<double>(out[0]), static_cast<double>(out[1]),
            static_cast<double>(out[2]), static_cast<double>(out[3]), static_cast<double>(out[4]),
            changed_after, static_cast<int>(record_applied), record_changed_total, record_changed_outside,
            static_cast<double>(record_distances[0]), static_cast<double>(record_distances[1]),
            static_cast<double>(record_distances[2]), static_cast<double>(record_distances[3]),
            static_cast<double>(record_distances[4]));
    }
    return 0;
}
"""


# ---------------------------------------------------------------------------
# Config graphic options natives (0x00404590, 0x00404F00, 0x00404F60,
# 0x00405620, 0x004055C0, 0x00405530)
# ---------------------------------------------------------------------------

CONFIG_ACTIVE_ADDRESS = 0x00404590
CONFIG_SET_ADDRESS = 0x00404F00
CONFIG_GET_ADDRESS = 0x00404F60
CONFIG_NUM_ADDRESS = 0x00405620
CONFIG_FLAGS_FROM_LEVEL_ADDRESS = 0x004055C0
CONFIG_LEVEL_FROM_FLAGS_ADDRESS = 0x00405530
SCRIPT_GETARG_ADDRESS = 0x004B8A20
CONFIG_ACTIVE_INDEX_ADDR = 0x00BC2384
CONFIG_RECORD_BASE_ADDR = 0x00BC2394
CONFIG_RECORD_FLAGS_OFFSET = 0x0C
CONFIG_CONTEXT_ADDR = SCRATCH_BASE + 0x30000
CONFIG_SCRIPT_VALUE_ADDR = SCRATCH_BASE + 0x32000

CONFIG_BLOCK_HELPER_NOTE = "read_block helper is defined below as config_read_block"


def config_read_block(buffer: bytes) -> dict:
    return {
        "block_integer": struct.unpack("<i", buffer[0:4])[0],
        "block_undefined": struct.unpack("<I", buffer[4:8])[0],
        "block_real": struct.unpack("<d", buffer[8:16])[0],
        "block_type": struct.unpack("<I", buffer[16:20])[0],
        "changed_after": sum(1 for value in buffer[0x18:] if value != FILL_PATTERN),
    }


def config_active_cases() -> list[dict]:
    return [
        {"id": f"config-active-index-{index}",
         "categories": ["nominal" if index < 2 else "boundary"],
         "seed": index, "index": index}
        for index in range(8)
    ]


def config_active_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    emulator.mem_write(CONFIG_ACTIVE_INDEX_ADDR, struct.pack("<I", case["index"]))
    emulator.mem_write(CONFIG_RECORD_BASE_ADDR + case["index"] * 0x564 + CONFIG_RECORD_FLAGS_OFFSET,
                       struct.pack("<I", 0))
    run_with_arguments(emulator, CONFIG_ACTIVE_ADDRESS, [], 0x00C0FFEE)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    return {
        "return_value": eax - CONFIG_RECORD_BASE_ADDR,
        "state_delta": {},
        "side_effects": [],
    }


def config_active_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/config/config_graphic_options.hpp"

int main()
{
    struct Case { const char* id; unsigned index; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        igi::retail::config::GraphicOptionsStore& store =
            igi::retail::config::GraphicOptionsStore::active();
        store.set_active_index(item.index);
        igi::retail::config::GraphicOptionsRecord* record =
            igi::retail::config::Config_GetActiveGraphicOptions();
        const long long offset = static_cast<long long>(
            reinterpret_cast<const unsigned char*>(record) -
            reinterpret_cast<const unsigned char*>(&store.record(0)));
        std::printf("{\"id\":\"%s\",\"return_value\":%lld,\"state_delta\":{},\"side_effects\":[]}\n",
                    item.id, offset);
    }
    return 0;
}
"""


def config_set_cases() -> list[dict]:
    values = [0x19E, 0x1AE, 0x0D, 0x60, 0x4D, 0x41, 0x00, 0xFFFFFFFF]
    return [
        {"id": f"config-set-flags-{index}",
         "categories": ["nominal" if index < 2 else "boundary"],
         "seed": index, "flags": value}
        for index, value in enumerate(values)
    ]


def config_set_retail(image: PebImage, case: dict) -> dict:
    emulator = new_emulator(image)
    emulator.mem_write(SCRIPT_GETARG_ADDRESS,
                       b"\xb8" + struct.pack("<I", case["flags"] & 0xFFFFFFFF) + b"\xc3")
    emulator.mem_write(CONFIG_ACTIVE_INDEX_ADDR, struct.pack("<I", 0))
    emulator.mem_write(CONFIG_RECORD_BASE_ADDR + CONFIG_RECORD_FLAGS_OFFSET, struct.pack("<I", 0))
    emulator.mem_write(CONFIG_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x100)
    run_with_arguments(emulator, CONFIG_SET_ADDRESS,
                       [CONFIG_CONTEXT_ADDR, 0, CONFIG_SCRIPT_VALUE_ADDR], 0x00C0FFEE)
    context = emulator.mem_read(CONFIG_CONTEXT_ADDR, 0x100)
    state = config_read_block(context)
    state["record_flags"] = struct.unpack(
        "<i", emulator.mem_read(CONFIG_RECORD_BASE_ADDR + CONFIG_RECORD_FLAGS_OFFSET, 4))[0]
    return {"return_value": 0, "state_delta": state, "side_effects": []}


def config_set_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/config/config_graphic_options.hpp"

int main()
{
    struct Case { const char* id; std::int32_t flags; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char context[0x100];
        std::memset(context, 0xA5, sizeof(context));
        igi::retail::config::Config_GraphicOptionsSetPerformanceFlags(context, item.flags);
        std::int32_t integer = 0;
        std::uint32_t undefined_slot = 0;
        double real = 0.0;
        std::uint32_t type = 0;
        std::memcpy(&integer, context + 0, 4);
        std::memcpy(&undefined_slot, context + 4, 4);
        std::memcpy(&real, context + 8, 8);
        std::memcpy(&type, context + 16, 4);
        std::size_t changed_after = 0;
        for (std::size_t index = 0x18; index < sizeof(context); ++index)
        {
            if (context[index] != 0xA5) { ++changed_after; }
        }
        const std::int32_t record_flags =
            igi::retail::config::Config_GetActiveGraphicOptions()->performance_flags();
        std::printf(
            "{\"id\":\"%s\",\"return_value\":0,\"state_delta\":{\"block_integer\":%d,"
            "\"block_undefined\":%u,\"block_real\":%.17g,\"block_type\":%u,\"changed_after\":%zu,"
            "\"record_flags\":%d},\"side_effects\":[]}\n",
            item.id, static_cast<int>(integer), static_cast<unsigned>(undefined_slot), real,
            static_cast<unsigned>(type), changed_after, static_cast<int>(record_flags));
    }
    return 0;
}
"""


def config_get_cases() -> list[dict]:
    values = [0x19E, 0x1AE, 0x0D, 0x60, 0x4D, 0x41, 0x00, 0x12345678]
    return [
        {"id": f"config-get-flags-{index}",
         "categories": ["nominal" if index < 2 else "boundary"],
         "seed": index, "stored": value}
        for index, value in enumerate(values)
    ]


def config_get_retail(image: PebImage, case: dict) -> dict:
    emulator = new_emulator(image)
    emulator.mem_write(SCRIPT_GETARG_ADDRESS,
                       b"\xb8" + struct.pack("<I", 0) + b"\xc3")
    emulator.mem_write(CONFIG_ACTIVE_INDEX_ADDR, struct.pack("<I", 0))
    emulator.mem_write(CONFIG_RECORD_BASE_ADDR + CONFIG_RECORD_FLAGS_OFFSET,
                       struct.pack("<I", case["stored"] & 0xFFFFFFFF))
    emulator.mem_write(CONFIG_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x100)
    run_with_arguments(emulator, CONFIG_GET_ADDRESS, [CONFIG_CONTEXT_ADDR], 0x00C0FFEE)
    context = emulator.mem_read(CONFIG_CONTEXT_ADDR, 0x100)
    state = config_read_block(context)
    state["record_flags"] = struct.unpack("<i", struct.pack("<I", case["stored"]))[0]
    return {"return_value": 0, "state_delta": state, "side_effects": []}


def config_get_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/config/config_graphic_options.hpp"

int main()
{
    struct Case { const char* id; std::int32_t stored; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char context[0x100];
        std::memset(context, 0xA5, sizeof(context));
        igi::retail::config::Config_GetActiveGraphicOptions()->set_performance_flags(item.stored);
        igi::retail::config::Config_GraphicOptionsGetPerformanceFlags(context);
        std::int32_t integer = 0;
        std::uint32_t undefined_slot = 0;
        double real = 0.0;
        std::uint32_t type = 0;
        std::memcpy(&integer, context + 0, 4);
        std::memcpy(&undefined_slot, context + 4, 4);
        std::memcpy(&real, context + 8, 8);
        std::memcpy(&type, context + 16, 4);
        std::size_t changed_after = 0;
        for (std::size_t index = 0x18; index < sizeof(context); ++index)
        {
            if (context[index] != 0xA5) { ++changed_after; }
        }
        const std::int32_t record_flags =
            igi::retail::config::Config_GetActiveGraphicOptions()->performance_flags();
        std::printf(
            "{\"id\":\"%s\",\"return_value\":0,\"state_delta\":{\"block_integer\":%d,"
            "\"block_undefined\":%u,\"block_real\":%.17g,\"block_type\":%u,\"changed_after\":%zu,"
            "\"record_flags\":%d},\"side_effects\":[]}\n",
            item.id, static_cast<int>(integer), static_cast<unsigned>(undefined_slot), real,
            static_cast<unsigned>(type), changed_after, static_cast<int>(record_flags));
    }
    return 0;
}
"""


def config_num_cases() -> list[dict]:
    return [
        {"id": f"config-num-levels-{index}",
         "categories": ["nominal" if index < 2 else "boundary"],
         "seed": index}
        for index in range(8)
    ]


def config_num_retail(image: PebImage, case: dict) -> dict:
    emulator = new_emulator(image)
    emulator.mem_write(CONFIG_ACTIVE_INDEX_ADDR, struct.pack("<I", 0))
    emulator.mem_write(CONFIG_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x100)
    run_with_arguments(emulator, CONFIG_NUM_ADDRESS, [CONFIG_CONTEXT_ADDR], 0x00C0FFEE)
    context = emulator.mem_read(CONFIG_CONTEXT_ADDR, 0x100)
    return {"return_value": 0, "state_delta": config_read_block(context), "side_effects": []}


def config_num_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/config/config_graphic_options.hpp"

int main()
{
    struct Case { const char* id; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char context[0x100];
        std::memset(context, 0xA5, sizeof(context));
        igi::retail::config::Config_GraphicOptionsNumPerfLevels(context);
        std::int32_t integer = 0;
        std::uint32_t undefined_slot = 0;
        double real = 0.0;
        std::uint32_t type = 0;
        std::memcpy(&integer, context + 0, 4);
        std::memcpy(&undefined_slot, context + 4, 4);
        std::memcpy(&real, context + 8, 8);
        std::memcpy(&type, context + 16, 4);
        std::size_t changed_after = 0;
        for (std::size_t index = 0x18; index < sizeof(context); ++index)
        {
            if (context[index] != 0xA5) { ++changed_after; }
        }
        std::printf(
            "{\"id\":\"%s\",\"return_value\":0,\"state_delta\":{\"block_integer\":%d,"
            "\"block_undefined\":%u,\"block_real\":%.17g,\"block_type\":%u,\"changed_after\":%zu},"
            "\"side_effects\":[]}\n",
            item.id, static_cast<int>(integer), static_cast<unsigned>(undefined_slot), real,
            static_cast<unsigned>(type), changed_after);
    }
    return 0;
}
"""


def config_flags_from_level_cases() -> list[dict]:
    levels = [0, 1, 2, 3, 4, 5, 6, 0]
    return [
        {"id": f"config-flags-level-{index}",
         "categories": ["nominal" if index < 2 else "boundary"],
         "seed": index, "level": level}
        for index, level in enumerate(levels)
    ]


def config_flags_from_level_retail(image: PebImage, case: dict) -> dict:
    emulator = new_emulator(image)
    emulator.mem_write(SCRIPT_GETARG_ADDRESS,
                       b"\xb8" + struct.pack("<I", case["level"]) + b"\xc3")
    emulator.mem_write(CONFIG_ACTIVE_INDEX_ADDR, struct.pack("<I", 0))
    emulator.mem_write(CONFIG_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x100)
    run_with_arguments(emulator, CONFIG_FLAGS_FROM_LEVEL_ADDRESS,
                       [CONFIG_CONTEXT_ADDR, 0, CONFIG_SCRIPT_VALUE_ADDR], 0x00C0FFEE)
    context = emulator.mem_read(CONFIG_CONTEXT_ADDR, 0x100)
    return {"return_value": 0, "state_delta": config_read_block(context), "side_effects": []}


def config_flags_from_level_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/config/config_graphic_options.hpp"

int main()
{
    struct Case { const char* id; std::int32_t level; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char context[0x100];
        std::memset(context, 0xA5, sizeof(context));
        igi::retail::config::Config_GraphicOptionsGetPerfFlagsFromLevel(context, item.level);
        std::int32_t integer = 0;
        std::uint32_t undefined_slot = 0;
        double real = 0.0;
        std::uint32_t type = 0;
        std::memcpy(&integer, context + 0, 4);
        std::memcpy(&undefined_slot, context + 4, 4);
        std::memcpy(&real, context + 8, 8);
        std::memcpy(&type, context + 16, 4);
        std::size_t changed_after = 0;
        for (std::size_t index = 0x18; index < sizeof(context); ++index)
        {
            if (context[index] != 0xA5) { ++changed_after; }
        }
        std::printf(
            "{\"id\":\"%s\",\"return_value\":0,\"state_delta\":{\"block_integer\":%d,"
            "\"block_undefined\":%u,\"block_real\":%.17g,\"block_type\":%u,\"changed_after\":%zu},"
            "\"side_effects\":[]}\n",
            item.id, static_cast<int>(integer), static_cast<unsigned>(undefined_slot), real,
            static_cast<unsigned>(type), changed_after);
    }
    return 0;
}
"""


def config_level_from_flags_cases() -> list[dict]:
    values = [0x19E, 0x1AE, 0x0D, 0x60, 0x4D, 0x41, 0x00, 0xFFFFFFFF]
    return [
        {"id": f"config-level-flags-{index}",
         "categories": ["nominal" if index < 2 else "boundary"],
         "seed": index, "flags": value}
        for index, value in enumerate(values)
    ]


def config_level_from_flags_retail(image: PebImage, case: dict) -> dict:
    emulator = new_emulator(image)
    emulator.mem_write(SCRIPT_GETARG_ADDRESS,
                       b"\xb8" + struct.pack("<I", case["flags"] & 0xFFFFFFFF) + b"\xc3")
    emulator.mem_write(CONFIG_ACTIVE_INDEX_ADDR, struct.pack("<I", 0))
    emulator.mem_write(CONFIG_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x100)
    run_with_arguments(emulator, CONFIG_LEVEL_FROM_FLAGS_ADDRESS,
                       [CONFIG_CONTEXT_ADDR, 0, CONFIG_SCRIPT_VALUE_ADDR], 0x00C0FFEE)
    context = emulator.mem_read(CONFIG_CONTEXT_ADDR, 0x100)
    return {"return_value": 0, "state_delta": config_read_block(context), "side_effects": []}


def config_level_from_flags_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "igi/retail/config/config_graphic_options.hpp"

int main()
{
    struct Case { const char* id; std::int32_t flags; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        static unsigned char context[0x100];
        std::memset(context, 0xA5, sizeof(context));
        igi::retail::config::Config_GraphicOptionsGetPerfLevelFromFlags(context, item.flags);
        std::int32_t integer = 0;
        std::uint32_t undefined_slot = 0;
        double real = 0.0;
        std::uint32_t type = 0;
        std::memcpy(&integer, context + 0, 4);
        std::memcpy(&undefined_slot, context + 4, 4);
        std::memcpy(&real, context + 8, 8);
        std::memcpy(&type, context + 16, 4);
        std::size_t changed_after = 0;
        for (std::size_t index = 0x18; index < sizeof(context); ++index)
        {
            if (context[index] != 0xA5) { ++changed_after; }
        }
        std::printf(
            "{\"id\":\"%s\",\"return_value\":0,\"state_delta\":{\"block_integer\":%d,"
            "\"block_undefined\":%u,\"block_real\":%.17g,\"block_type\":%u,\"changed_after\":%zu},"
            "\"side_effects\":[]}\n",
            item.id, static_cast<int>(integer), static_cast<unsigned>(undefined_slot), real,
            static_cast<unsigned>(type), changed_after);
    }
    return 0;
}
"""


def signed32(value: int) -> int:
    value &= 0xFFFFFFFF
    return value - (1 << 32) if value >= (1 << 31) else value


# ---------------------------------------------------------------------------
# Framework
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# 0x004C6670 Terrain_CreateMaterial / 0x004C6760 Terrain_CreateTileMap
# ---------------------------------------------------------------------------
#
# Both retail bodies build their inputs through the script-VM accessors
# Script_GetArg (0x004B8A20) and Script_GetArgFloat (0x004B8A50) and then write a
# fixed-layout global table (0x00C28FE0 + group*900, and 0x00C30080 + group*64).
# The two accessors are replaced with deterministic procedures that read the
# argument index off the synthetic cdecl stack, so the observed table bytes are
# a property of the reconstructed body and not of the script VM. The candidate
# receives an equivalent argument array because the port models the same
# boundary as args[index] with Script_GetArg = (int)args[index].

MATERIAL_ADDRESS = 0x004C6670
TILEMAP_ADDRESS = 0x004C6760
SCRIPT_GET_ARG_ADDRESS = 0x004B8A20
SCRIPT_GET_ARG_FLOAT_ADDRESS = 0x004B8A50
MATERIAL_TABLE_BASE = 0x00C28FE0
MATERIAL_GROUP_SIZE = 900
TILEMAP_TABLE_BASE = 0x00C30080
TILEMAP_TABLE_SIZE = 64
TERRAIN_CONTEXT_ADDR = SCRATCH_BASE + 0x300
TERRAIN_ARGS_ADDR = SCRATCH_BASE + 0x400
TERRAIN_RET_STUB = SCRATCH_BASE + 0x9800
TERRAIN_FLD_STUB = SCRATCH_BASE + 0x9810
TERRAIN_FLD_VALUE = SCRATCH_BASE + 0x9900
TERRAIN_SENTINEL = 0x00C0FFEE
ARG_MODULUS = 2000
ARG_OFFSET = 500


def terrain_arg_int(seed: int, index: int) -> int:
    return ((index * 131 + seed * 17) % ARG_MODULUS) - ARG_OFFSET


def terrain_arg_float(seed: int, index: int) -> float:
    return terrain_arg_int(seed, index) + 0.25


def terrain_seed_for_group(group_index: int) -> int:
    """Seed whose Script_GetArg(args, 0) is exactly `group_index`.

    17*353 == 1 (mod 2000), so the index-0 term collapses to group_index + 500.
    """
    return (353 * (ARG_OFFSET + group_index)) % ARG_MODULUS


def terrain_install_accessors(emulator, seed: int) -> None:
    import unicorn

    # `ret`, entered by rewriting EIP from the hook so the hook returns through the
    # same stack slot the retail `call` pushed.
    emulator.mem_write(TERRAIN_RET_STUB, b"\xC3")

    # Unicorn's x87 host backends do not implement FSTP/FMUL stores-to-memory
    # (observed: FLD loads ST0 correctly; FSTP [m32] stores zero). The retail
    # float-store sites are therefore emulated in Python: pending values come from
    # the scripted Script_GetArgFloat accessor hook, FMUL sites fold the 0.5
    # constant, and FSTP sites write the 32-bit value at the operand computed from
    # EDI before skipping the (broken) FPU instruction.
    pending_float: list[float] = []
    fstp_sites = {0x004C66BE: 2, 0x004C66CA: 3, 0x004C66E2: 3, 0x004C6702: 3}
    fmul_sites = {0x004C66D7: 6, 0x004C66F7: 6}
    fstp_offsets = {0x004C66BE: 0, 0x004C66CA: 4, 0x004C66E2: 8, 0x004C6702: 16}

    def scalar_hook(uc, address, size, user_data):
        esp = uc.reg_read(unicorn.x86_const.UC_X86_REG_ESP)
        index = struct.unpack("<i", uc.mem_read(esp + 8, 4))[0]
        # The port models Script_GetArg as (int)args[index] on the shared float
        # argument array (terrain_loaders.cpp Script_GetArg), so the scripted int
        # accessor must produce the same truncation toward zero of
        # terrain_arg_int + 0.25 that the candidate observes.
        value = terrain_arg_float(seed, index)
        uc.reg_write(unicorn.x86_const.UC_X86_REG_EAX,
                     math.trunc(value) & 0xFFFFFFFF)
        uc.reg_write(unicorn.x86_const.UC_X86_REG_EIP, TERRAIN_RET_STUB)

    def float_hook(uc, address, size, user_data):
        esp = uc.reg_read(unicorn.x86_const.UC_X86_REG_ESP)
        index = struct.unpack("<i", uc.mem_read(esp + 8, 4))[0]
        pending_float.append(terrain_arg_float(seed, index))
        uc.reg_write(unicorn.x86_const.UC_X86_REG_EIP, TERRAIN_RET_STUB)

    def fmul_hook(uc, address, size, user_data):
        if not pending_float:
            raise RuntimeError("retail FMUL hit with no pending float accessor value")
        pending_float[-1] = struct.unpack("<f", struct.pack("<f", pending_float[-1] * 0.5))[0]
        uc.reg_write(unicorn.x86_const.UC_X86_REG_EIP, address + fmul_sites[address])

    def fstp_hook(uc, address, size, user_data):
        if not pending_float:
            raise RuntimeError("retail FSTP hit with no pending float accessor value")
        value = pending_float.pop(0)
        edi = uc.reg_read(unicorn.x86_const.UC_X86_REG_EDI)
        uc.mem_write(edi + fstp_offsets[address], struct.pack("<f", struct.unpack("<f",
                                                                                   struct.pack("<f", value))[0]))
        uc.reg_write(unicorn.x86_const.UC_X86_REG_EIP, address + fstp_sites[address])

    emulator.hook_add(unicorn.UC_HOOK_CODE, scalar_hook,
                      begin=SCRIPT_GET_ARG_ADDRESS, end=SCRIPT_GET_ARG_ADDRESS + 1)
    emulator.hook_add(unicorn.UC_HOOK_CODE, float_hook,
                      begin=SCRIPT_GET_ARG_FLOAT_ADDRESS, end=SCRIPT_GET_ARG_FLOAT_ADDRESS + 1)
    for site in fmul_sites:
        emulator.hook_add(unicorn.UC_HOOK_CODE, fmul_hook, begin=site, end=site)
    for site in fstp_sites:
        emulator.hook_add(unicorn.UC_HOOK_CODE, fstp_hook, begin=site, end=site)


def material_cases() -> list[dict]:
    groups = [(0, "nominal"), (1, "nominal"), (6, "nominal"), (16, "nominal"),
              (30, "boundary"), (31, "boundary"), (2, "boundary"), (17, "boundary")]
    return [
        {"id": f"material-group-{group}", "categories": [category],
         "group_index": group, "seed": terrain_seed_for_group(group)}
        for group, category in groups
    ]


def tilemap_cases() -> list[dict]:
    groups = [(0, "nominal"), (1, "nominal"), (31, "nominal"), (32, "nominal"),
              (62, "boundary"), (63, "boundary"), (16, "boundary"), (47, "boundary")]
    return [
        {"id": f"tilemap-group-{group}", "categories": [category],
         "group_index": group, "seed": terrain_seed_for_group(group)}
        for group, category in groups
    ]


def material_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    terrain_install_accessors(emulator, case["seed"])
    emulator.mem_write(TERRAIN_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x18)
    run_with_arguments(emulator, MATERIAL_ADDRESS,
                       [TERRAIN_CONTEXT_ADDR, TERRAIN_ARGS_ADDR], TERRAIN_SENTINEL)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    group = case["group_index"]
    return {
        "return_value": eax,
        "state_delta": {
            "group_index": group,
            "group_hex": emulator.mem_read(
                MATERIAL_TABLE_BASE + group * MATERIAL_GROUP_SIZE, MATERIAL_GROUP_SIZE).hex(),
            "context_hex": emulator.mem_read(TERRAIN_CONTEXT_ADDR, 0x18).hex(),
        },
        "side_effects": [],
    }


def tilemap_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    terrain_install_accessors(emulator, case["seed"])
    emulator.mem_write(TERRAIN_CONTEXT_ADDR, bytes([FILL_PATTERN]) * 0x18)
    run_with_arguments(emulator, TILEMAP_ADDRESS,
                       [TERRAIN_CONTEXT_ADDR, TERRAIN_ARGS_ADDR], TERRAIN_SENTINEL)
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    group = case["group_index"]
    return {
        "return_value": eax,
        "state_delta": {
            "group_index": group,
            "table_hex": emulator.mem_read(
                TILEMAP_TABLE_BASE + group * TILEMAP_TABLE_SIZE, TILEMAP_TABLE_SIZE).hex(),
            "context_hex": emulator.mem_read(TERRAIN_CONTEXT_ADDR, 0x18).hex(),
        },
        "side_effects": [],
    }


def material_candidate_source() -> str:
    return r"""
#include <array>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include "igi/retail/terrain/terrain_loaders.hpp"

namespace
{
int arg_int(int seed, int index)
{
    return ((index * 131 + seed * 17) % 2000) - 500;
}
} // namespace

int main()
{
    struct Case { const char* id; int seed; unsigned group; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        std::vector<float> args(227, 0.0F);
        for (std::size_t index = 0; index < args.size(); ++index)
        {
            args[index] = static_cast<float>(arg_int(item.seed, static_cast<int>(index))) + 0.25F;
        }
        std::array<std::uint8_t, 0x18> context{};
        context.fill(0xA5);
        igi::retail::terrain::Host_ResetTerrainGlobals();
        void* result = igi::retail::terrain::Terrain_CreateMaterial(context.data(), args.data());
        std::array<std::uint8_t, 900> group{};
        igi::retail::terrain::Host_GetMaterialGroupBytes(item.group, group.data());
        char group_hex[901 * 2];
        char context_hex[0x18 * 2 + 1];
        for (std::size_t index = 0; index < group.size(); ++index)
        {
            std::snprintf(group_hex + index * 2, 3, "%02x", group[index]);
        }
        for (std::size_t index = 0; index < 0x18; ++index)
        {
            std::snprintf(context_hex + index * 2, 3, "%02x", context[index]);
        }
        const unsigned long long result_value =
            (result == static_cast<void*>(context.data())) ? @CONTEXT_ADDR@ : 0ULL;
        std::printf(
            "{\"id\":\"%s\",\"return_value\":%llu,\"state_delta\":{\"group_index\":%u,"
            "\"group_hex\":\"%s\",\"context_hex\":\"%s\"},\"side_effects\":[]}\n",
            item.id, result_value, item.group, group_hex, context_hex);
    }
    return 0;
}
"""


def tilemap_candidate_source() -> str:
    return r"""
#include <array>
#include <cstdio>
#include <cstdint>
#include <vector>
#include "igi/retail/terrain/terrain_loaders.hpp"

namespace
{
int arg_int(int seed, int index)
{
    return ((index * 131 + seed * 17) % 2000) - 500;
}
} // namespace

int main()
{
    struct Case { const char* id; int seed; unsigned group; };
    static const Case cases[] = {
@CASES@
    };
    for (const Case& item : cases)
    {
        std::vector<float> args(65, 0.0F);
        for (std::size_t index = 0; index < args.size(); ++index)
        {
            args[index] = static_cast<float>(arg_int(item.seed, static_cast<int>(index))) + 0.25F;
        }
        std::array<std::uint8_t, 0x18> context{};
        context.fill(0xA5);
        igi::retail::terrain::Host_ResetTerrainGlobals();
        void* result = igi::retail::terrain::Terrain_CreateTileMap(context.data(), args.data());
        std::array<std::uint8_t, 64> table{};
        igi::retail::terrain::Host_GetTileMapBytes(item.group, table.data());
        char table_hex[65 * 2];
        char context_hex[0x18 * 2 + 1];
        for (std::size_t index = 0; index < table.size(); ++index)
        {
            std::snprintf(table_hex + index * 2, 3, "%02x", table[index]);
        }
        for (std::size_t index = 0; index < 0x18; ++index)
        {
            std::snprintf(context_hex + index * 2, 3, "%02x", context[index]);
        }
        const unsigned long long result_value =
            (result == static_cast<void*>(context.data())) ? @CONTEXT_ADDR@ : 0ULL;
        std::printf(
            "{\"id\":\"%s\",\"return_value\":%llu,\"state_delta\":{\"group_index\":%u,"
            "\"table_hex\":\"%s\",\"context_hex\":\"%s\"},\"side_effects\":[]}\n",
            item.id, result_value, item.group, table_hex, context_hex);
    }
    return 0;
}
"""


# The terrain implementation unit reaches the script VM and the level loader, so the harness
# is compiled with per-function sections and the unused references are left unresolved.
TERRAIN_LINK_FLAGS = ("-Wl,-dead_strip", "-Wl,-undefined,dynamic_lookup")

TERRAIN_LIMITS = (
    "The retail context block is produced by `rep movsd` from a stack temporary whose dwords 1 "
    "and 5 are never initialized (disassembly 0x004C671E-0x004C6755 and 0x004C6798-0x004C67D0); "
    "the emulator's freshly mapped stack is zero, so those two dwords do not distinguish a "
    "reconstruction that writes zero (the documented port behaviour) from one that copies the "
    "same residue. The retail group index is not bounds checked; every case here uses an index "
    "inside the ported table so the port's documented clamping is not exercised."
)


# ---------------------------------------------------------------------------
# 0x0048F970 WindowProcedure
# ---------------------------------------------------------------------------

WNDPROC_ADDRESS = 0x0048F970
WNDPROC_LOOP_STATUS = 0x005C8DE8
WNDPROC_CHAIN_HEAD = 0x005C8DF0
WNDPROC_DEBUG_KEY_FLAG = 0x005C8E04
WNDPROC_WINDOW = 0x70012000
WNDPROC_CHAIN = 0x70012100
WNDPROC_ENTRY = 0x70012200
WNDPROC_HANDLER = 0x70012400
WNDPROC_HANDLER_CODE = 0x70012900
WNDPROC_CALLBACK_LOG = 0x70012400
WNDPROC_STUB_SHOWCURSOR = 0x70012500
WNDPROC_STUB_POSTQUIT = 0x70012600
WNDPROC_STUB_DEFWNDPROC = 0x70012700

# Import thunks used by the procedure; each is replaced with a local stub.
WNDPROC_IAT_SHOWCURSOR = 0x005331B8
WNDPROC_IAT_POSTQUIT = 0x005331A0
WNDPROC_IAT_DEFWNDPROC = 0x00533190
WNDPROC_IAT_POSTMESSAGE = 0x00533194
# The WM_DESTROY path calls the shutdown native 0x004010B0, which runs a deep
# process-teardown chain unrelated to message dispatch. Its entry is patched to a
# bare ret so the observable WndProc state transition is still retail's own code.
WNDPROC_CLOSE_NATIVE = 0x004010B0
WNDPROC_SENTINEL = 0x00C0FFEE
# The debug-key handler 0x0048F0A0 is not transcribed in this branch, so its
# entry is patched to a plain stdcall return; the WndProc's own decision to call
# it is still exercised by the retail code.
WNDPROC_DEBUG_KEY_HANDLER = 0x0048F0A0  # cdecl: caller cleans the 3 arguments
WNDPROC_STUB_POSTMESSAGE = 0x70012800
WNDPROC_STOP = 0x00C0FFFF

WM_ACTIVATE_RETAIL = 0x001C
WM_CREATE_RETAIL = 0x0001
WM_DESTROY_RETAIL = 0x0002
WM_KEYDOWN_RETAIL = 0x0104
WM_KEYUP_RETAIL = 0x0105
WM_CLOSE_RETAIL = 0x0010
WM_PAINT_RETAIL = 0x000F
VK_S = 0x73
VK_F12 = 0x12


def wndproc_cases() -> list[dict]:
    cases = [
        {"id": "activate-nominal", "categories": ["nominal"], "message": WM_ACTIVATE_RETAIL,
         "wparam": 1, "lparam": 0, "loop_status": 0, "debug_flag": 0, "chain": False},
        {"id": "create-nominal", "categories": ["nominal"], "message": WM_CREATE_RETAIL,
         "wparam": 0, "lparam": 0, "loop_status": 0, "debug_flag": 0, "chain": False},
        {"id": "destroy-nominal", "categories": ["nominal"], "message": WM_DESTROY_RETAIL,
         "wparam": 0, "lparam": 0, "loop_status": 1, "debug_flag": 0, "chain": False},
        {"id": "keyup-f12-nominal", "categories": ["nominal"], "message": WM_KEYUP_RETAIL,
         "wparam": VK_F12, "lparam": 0, "loop_status": 0, "debug_flag": 1, "chain": False},
        {"id": "keyup-other-nominal", "categories": ["nominal"], "message": WM_KEYUP_RETAIL,
         "wparam": VK_S, "lparam": 0, "loop_status": 0, "debug_flag": 1, "chain": False},
        {"id": "keydown-s-nominal", "categories": ["nominal"], "message": WM_KEYDOWN_RETAIL,
         "wparam": VK_S, "lparam": 0, "loop_status": 0, "debug_flag": 1, "chain": False},
        {"id": "keydown-s-no-flag", "categories": ["boundary"], "message": WM_KEYDOWN_RETAIL,
         "wparam": VK_S, "lparam": 0, "loop_status": 0, "debug_flag": 0, "chain": False},
        {"id": "chain-consumes", "categories": ["nominal"], "message": WM_KEYDOWN_RETAIL,
         "wparam": 0x41, "lparam": 0, "loop_status": 1, "debug_flag": 0, "chain": True,
         "chain_result": 0},
        {"id": "chain-passes-through", "categories": ["boundary"], "message": WM_PAINT_RETAIL,
         "wparam": 0, "lparam": 0, "loop_status": 1, "debug_flag": 0, "chain": True,
         "chain_result": 7},
        {"id": "default-passes", "categories": ["boundary"], "message": WM_CLOSE_RETAIL,
         "wparam": 0, "lparam": 0, "loop_status": 0, "debug_flag": 0, "chain": False},
    ]
    for index, case in enumerate(cases):
        case["seed"] = index
    return cases


def _wndproc_emulator(image: PebImage, case: dict):
    import unicorn

    emulator = new_emulator(image)
    emulator.mem_write(WNDPROC_LOOP_STATUS, struct.pack("<B", case["loop_status"]))
    emulator.mem_write(WNDPROC_DEBUG_KEY_FLAG, struct.pack("<I", case["debug_flag"]))
    emulator.mem_write(WNDPROC_WINDOW, b"\x00" * 0x100)
    # Stubs log which imported API was reached, then return without touching state.
    log = 0
    emulator.mem_write(WNDPROC_CALLBACK_LOG, struct.pack("<I", log))
    # USER32 exports are stdcall, so each stub must pop its own arguments:
    # ShowCursor(1 arg) and PostQuitMessage(1 arg) need `ret 4`, and
    # DefWindowProcA takes four arguments, needing `ret 0x10`.
    emulator.mem_write(WNDPROC_STUB_SHOWCURSOR, b"\x31\xC0\xC2\x04\x00")
    emulator.mem_write(WNDPROC_STUB_POSTQUIT, b"\xC2\x04\x00")
    emulator.mem_write(WNDPROC_STUB_DEFWNDPROC, b"\x31\xC0\xC2\x10\x00")
    emulator.mem_write(WNDPROC_IAT_SHOWCURSOR, struct.pack("<I", WNDPROC_STUB_SHOWCURSOR))
    emulator.mem_write(WNDPROC_IAT_POSTQUIT, struct.pack("<I", WNDPROC_STUB_POSTQUIT))
    emulator.mem_write(WNDPROC_IAT_DEFWNDPROC, struct.pack("<I", WNDPROC_STUB_DEFWNDPROC))
    emulator.mem_write(WNDPROC_CLOSE_NATIVE, b"\x31\xC0\xC3")
    emulator.mem_write(WNDPROC_DEBUG_KEY_HANDLER, b"\x31\xC0\xC3")
    emulator.mem_write(WNDPROC_STUB_POSTMESSAGE, b"\x31\xC0\xC2\x10\x00")
    emulator.mem_write(WNDPROC_IAT_POSTMESSAGE, struct.pack("<I", WNDPROC_STUB_POSTMESSAGE))
    # Retail loads the node pointer from 0x005C8DF0 and calls that node's own
    # handler at +0x08, so the global points straight at the first entry; the
    # node's +0x00 field is the next node. A set loop-status means at least one
    # entry must exist, otherwise retail would call through a null handler.
    # The chain handler is cdecl: the caller does `add esp, 0x10` after the
    # indirect call, so the stub returns the result and pops nothing.
    stub_ret = b"\xB8" + struct.pack("<I", case.get("chain_result", 0)) + b"\xC3"
    emulator.mem_write(WNDPROC_HANDLER_CODE, stub_ret)
    # Retail: eax = [0x5C8DF0]; esi = [eax]; if esi == 0 the whole chain is
    # skipped. Otherwise it calls [eax+8] and, when that returns non-zero,
    # advances eax = esi and repeats while the next pointer is non-null. So the
    # global points straight at the first node and only nodes that have a
    # successor are ever called.
    if case["loop_status"] or case.get("chain"):
        emulator.mem_write(WNDPROC_CHAIN_HEAD, struct.pack("<I", WNDPROC_CHAIN))
        emulator.mem_write(WNDPROC_CHAIN, struct.pack("<II", 0, WNDPROC_HANDLER_CODE))
    else:
        emulator.mem_write(WNDPROC_CHAIN_HEAD, struct.pack("<I", 0))
    return emulator


def wndproc_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = _wndproc_emulator(image, case)
    # stdcall: ret 0x10 pops the four arguments, so the sentinel follows them.
    # The sentinel is the return address; emu_start must NOT use it as the end
    # address or it halts before the stdcall cleanup completes.
    emulator.mem_write(STACK_TOP, struct.pack("<I", WNDPROC_SENTINEL))
    for index, value in enumerate((WNDPROC_WINDOW, case["message"], case["wparam"], case["lparam"])):
        emulator.mem_write(STACK_TOP + 4 + index * 4, struct.pack("<I", value & 0xFFFFFFFF))
    emulator.reg_write(unicorn.x86_const.UC_X86_REG_ESP, STACK_TOP)
    seen = []
    def on_code(uc, address, size, user):
        if len(seen) < 4000:
            seen.append(address)
    emulator.hook_add(unicorn.UC_HOOK_CODE, on_code)
    try:
        emulator.emu_start(WNDPROC_ADDRESS, WNDPROC_STOP)
    except unicorn.UcError as error:
        eip = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EIP)
        if eip == 0:
            eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
            raise SystemExit(f"fault eax=0x{eax:08X} headcell={emulator.mem_read(WNDPROC_CHAIN,4).hex()} "
                             f"node={emulator.mem_read(WNDPROC_ENTRY,8).hex()} "
                             f"handlercode={emulator.mem_read(WNDPROC_HANDLER_CODE,6).hex()} "
                             f"case_chain={case.get('chain')}")
        if eip == WNDPROC_SENTINEL:
            # Reaching the sentinel is the normal return, not a fault.
            eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
            return {
                "return_value": eax & 0xFFFFFFFF,
                "state_delta": {
                    "loop_status": emulator.mem_read(WNDPROC_LOOP_STATUS, 1)[0],
                    "debug_key_flag": read_u32(emulator, WNDPROC_DEBUG_KEY_FLAG),
                },
                "side_effects": [],
            }
        esp = emulator.reg_read(unicorn.x86_const.UC_X86_REG_ESP)
        stack = struct.unpack("<8I", emulator.mem_read(esp, 32))
        raise SystemExit(f"fault eip=0x{eip:08X} esp=0x{esp:08X} case={case['id']} "
                         f"last={[hex(v) for v in seen[-6:]]}")
    eax = emulator.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    return {
        "return_value": eax & 0xFFFFFFFF,
        "state_delta": {
            "loop_status": emulator.mem_read(WNDPROC_LOOP_STATUS, 1)[0],
            "debug_key_flag": read_u32(emulator, WNDPROC_DEBUG_KEY_FLAG),
        },
        "side_effects": [],
    }


def wndproc_case_literal(case: dict) -> str:
    return (f'        {{"{case["id"]}", {case["message"]}U, {case["wparam"]}U, '
            f'{case["lparam"]}, {case["loop_status"]}U, {case["debug_flag"]}U, '
            f'{1 if case.get("chain") else 0}, {case.get("chain_result", 0)}}},')


def wndproc_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include "igi/retail/core/native_0048f970.hpp"

namespace {

struct Case {
    const char* id;
    unsigned message;
    unsigned wparam;
    long lparam;
    unsigned loop_status;
    unsigned debug_flag;
    int chain;
    long chain_result;
};

Case cases[] = {
@CASES@
};

long g_chain_result = 0;

std::intptr_t chain_handler(void*, std::uint32_t, std::uintptr_t, std::intptr_t) {
    return static_cast<std::intptr_t>(g_chain_result);
}

}

int main()
{
    for (const Case& item : cases) {
        igi::retail::core::WindowHandlerEntry entry{};
        entry.next = nullptr;
        entry.handler = nullptr;
        WindowHandlerChain = nullptr;
        if (item.chain) {
            entry.next = nullptr;
            entry.handler = chain_handler;
            WindowHandlerChain = &entry;
        }
        g_chain_result = item.chain_result;
        LoopStatusByte = static_cast<std::uint8_t>(item.loop_status);
        DebugKeyFlag = static_cast<std::uint32_t>(item.debug_flag);
        const std::intptr_t result = WindowProcedure(nullptr, item.message,
            static_cast<std::uintptr_t>(item.wparam), static_cast<std::intptr_t>(item.lparam));
        std::printf("{\"id\":\"%s\",\"return_value\":%ld,\"state_delta\":{\"loop_status\":%u,"
                    "\"debug_key_flag\":%u},\"side_effects\":[]}\n",
                    item.id, static_cast<long>(result),
                    static_cast<unsigned>(LoopStatusByte), DebugKeyFlag);
    }
    return 0;
}
"""



# ---------------------------------------------------------------------------
# 0x004F1160 Script_GetParseMode (catalog label Script_ParseEnumReal32_5)
# ---------------------------------------------------------------------------

PARSE_MODE_ADDRESS = 0x004F1160
PARSE_MODE_BE = 0x00A758BE
PARSE_MODE_BD = 0x00A758BD


def parse_mode_cases() -> list[dict]:
    cases = [
        {"id": "both-clear", "categories": ["nominal"], "be": 0, "bd": 0},
        {"id": "be-set-nominal", "categories": ["nominal"], "be": 1, "bd": 0},
        {"id": "bd-set-nominal", "categories": ["nominal"], "be": 0, "bd": 1},
        {"id": "both-set", "categories": ["nominal"], "be": 1, "bd": 1},
        {"id": "be-high-bit", "categories": ["boundary"], "be": 0x80, "bd": 0},
        {"id": "bd-high-bit", "categories": ["boundary"], "be": 0, "bd": 0x80},
        {"id": "be-max", "categories": ["boundary"], "be": 0xFF, "bd": 0xFF},
        {"id": "bd-max-be-clear", "categories": ["boundary"], "be": 0, "bd": 0xFF},
    ]
    for index, case in enumerate(cases):
        case["seed"] = index
    return cases


def parse_mode_retail(image: PebImage, case: dict) -> dict:
    import unicorn

    emulator = new_emulator(image)
    emulator.mem_write(PARSE_MODE_BE, struct.pack("<B", case["be"]))
    emulator.mem_write(PARSE_MODE_BD, struct.pack("<B", case["bd"]))
    run_with_arguments(emulator, PARSE_MODE_ADDRESS, [], 0x00C0FFEE)
    eax = emulator.reg_read(unicorn.x86_const.X86_CONST_EAX) if False else emulator.reg_read(
        unicorn.x86_const.UC_X86_REG_EAX)
    return {
        "return_value": eax & 0xFFFFFFFF,
        "state_delta": {
            "be": emulator.mem_read(PARSE_MODE_BE, 1)[0],
            "bd": emulator.mem_read(PARSE_MODE_BD, 1)[0],
        },
        "side_effects": [],
    }


def parse_mode_case_literal(case: dict) -> str:
    return f'        {{"{case["id"]}", {case["be"]}U, {case["bd"]}U}},'


def parse_mode_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstdint>
#include "igi/retail/script/script_mode.hpp"

namespace {

struct Case { const char* id; unsigned be; unsigned bd; };

Case cases[] = {
@CASES@
};

}

int main()
{
    for (const Case& item : cases) {
        igi::retail::script::g_a758be = static_cast<std::uint8_t>(item.be);
        igi::retail::script::g_a758bd = static_cast<std::uint8_t>(item.bd);
        const std::uint32_t result = igi::retail::script::Script_GetParseMode();
        std::printf("{\"id\":\"%s\",\"return_value\":%u,\"state_delta\":{\"be\":%u,"
                    "\"bd\":%u},\"side_effects\":[]}\n",
                    item.id, result,
                    static_cast<unsigned>(igi::retail::script::g_a758be),
                    static_cast<unsigned>(igi::retail::script::g_a758bd));
    }
    return 0;
}
"""


# ---------------------------------------------------------------------------
# 0x004B7540 Ilff_ReadContainerHeader, 0x004B7570 Ilff_GetFormType,
# 0x004B7580 Ilff_GetChunkTag
# ---------------------------------------------------------------------------

ILFF_HDR_ADDRESS = 0x004B7540
ILFF_FORM_ADDRESS = 0x004B7570
ILFF_TAG_ADDRESS = 0x004B7580
ILFF_CONTEXT = 0x70013000
ILFF_OUT_SIZE = 0x70013100
ILFF_OUT_ALIGN = 0x70013200


def ilff_header_cases() -> list[dict]:
    cases = [
        {"id": "header-nominal", "categories": ["nominal"], "context": ILFF_CONTEXT,
         "size": 0x1234, "align": 0x10, "out_size": ILFF_OUT_SIZE, "out_align": ILFF_OUT_ALIGN},
        {"id": "header-zero-fields", "categories": ["nominal"], "context": ILFF_CONTEXT,
         "size": 0, "align": 0, "out_size": ILFF_OUT_SIZE, "out_align": ILFF_OUT_ALIGN},
        {"id": "header-null-context", "categories": ["boundary"], "context": 0,
         "size": 0, "align": 0, "out_size": ILFF_OUT_SIZE, "out_align": ILFF_OUT_ALIGN},
        {"id": "header-null-size-out", "categories": ["boundary"], "context": ILFF_CONTEXT,
         "size": 0x55, "align": 0x66, "out_size": 0, "out_align": ILFF_OUT_ALIGN},
        {"id": "header-null-align-out", "categories": ["boundary"], "context": ILFF_CONTEXT,
         "size": 0x77, "align": 0x88, "out_size": ILFF_OUT_SIZE, "out_align": 0},
        {"id": "header-both-null-out", "categories": ["boundary"], "context": ILFF_CONTEXT,
         "size": 1, "align": 2, "out_size": 0, "out_align": 0},
        {"id": "header-max-negative", "categories": ["boundary"], "context": ILFF_CONTEXT,
         "size": -1, "align": -2, "out_size": ILFF_OUT_SIZE, "out_align": ILFF_OUT_ALIGN},
        {"id": "header-high-bit", "categories": ["boundary"], "context": ILFF_CONTEXT,
         "size": 0x7FFFFFFF, "align": -2147483648, "out_size": ILFF_OUT_SIZE, "out_align": ILFF_OUT_ALIGN},
    ]
    for i, c in enumerate(cases):
        c["seed"] = i
    return cases


def _ilff_prepare(image: PebImage, case: dict):
    import unicorn
    emulator = new_emulator(image)
    emulator.mem_write(ILFF_CONTEXT, b"\x00" * 0x100)
    if case.get("size") is not None:
        emulator.mem_write(ILFF_CONTEXT + 4, struct.pack("<i", case["size"]))
    if case.get("align") is not None:
        emulator.mem_write(ILFF_CONTEXT + 8, struct.pack("<i", case["align"]))
    emulator.mem_write(ILFF_OUT_SIZE, b"\x00" * 4)
    emulator.mem_write(ILFF_OUT_ALIGN, b"\x00" * 4)
    return emulator


def ilff_header_retail(image: PebImage, case: dict) -> dict:
    import unicorn
    emulator = _ilff_prepare(image, case)
    run_with_arguments(emulator, ILFF_HDR_ADDRESS,
                       [case["out_size"], case["out_align"], case["context"]], 0x00C0FFEE)
    return {
        "return_value": 0,
        "state_delta": {
            "out_size": struct.unpack("<i", emulator.mem_read(ILFF_OUT_SIZE, 4))[0],
            "out_align": struct.unpack("<i", emulator.mem_read(ILFF_OUT_ALIGN, 4))[0],
        },
        "side_effects": [],
    }


def ilff_header_case_literal(case: dict) -> str:
    return (f'        {{"{case["id"]}", {case["out_size"]}U, {case["out_align"]}U, '
            f'{case["context"]}U, {case["size"]}, {case["align"]}}},')


def ilff_header_candidate_source() -> str:
    return r"""
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "igi/retail/sys/ilff_navigation.hpp"

namespace {
struct Case { const char* id; unsigned out_size; unsigned out_align; unsigned context; long size; long align; };
Case cases[] = {
@CASES@
};
}

int main()
{
    for (const Case& item : cases) {
        static std::uint8_t buffer[0x100];
        std::memset(buffer, 0, sizeof(buffer));
        *reinterpret_cast<std::int32_t*>(buffer + 4) = static_cast<std::int32_t>(item.size);
        *reinterpret_cast<std::int32_t*>(buffer + 8) = static_cast<std::int32_t>(item.align);
        std::int32_t out_size = 0, out_align = 0;
        igi::retail::sys::Ilff_ReadContainerHeader(
            item.out_size ? &out_size : nullptr,
            item.out_align ? &out_align : nullptr,
            item.context ? buffer : nullptr);
        std::printf("{\"id\":\"%s\",\"return_value\":0,\"state_delta\":{\"out_size\":%d,"
                    "\"out_align\":%d},\"side_effects\":[]}\n",
                    item.id, out_size, out_align);
    }
    return 0;
}
"""

NATIVES = {
    "0x00404590": {
        "cases": config_active_cases,
        "retail": config_active_retail,
        "candidate_source": config_active_candidate_source,
        "include": ("igi/retail/config/config_graphic_options.hpp",),
    },
    "0x00404F00": {
        "cases": config_set_cases,
        "retail": config_set_retail,
        "candidate_source": config_set_candidate_source,
        "include": ("igi/retail/config/config_graphic_options.hpp",),
    },
    "0x00404F60": {
        "cases": config_get_cases,
        "retail": config_get_retail,
        "candidate_source": config_get_candidate_source,
        "include": ("igi/retail/config/config_graphic_options.hpp",),
    },
    "0x00405530": {
        "cases": config_level_from_flags_cases,
        "retail": config_level_from_flags_retail,
        "candidate_source": config_level_from_flags_candidate_source,
        "include": ("igi/retail/config/config_graphic_options.hpp",),
    },
    "0x004055C0": {
        "cases": config_flags_from_level_cases,
        "retail": config_flags_from_level_retail,
        "candidate_source": config_flags_from_level_candidate_source,
        "include": ("igi/retail/config/config_graphic_options.hpp",),
    },
    "0x00405620": {
        "cases": config_num_cases,
        "retail": config_num_retail,
        "candidate_source": config_num_candidate_source,
        "include": ("igi/retail/config/config_graphic_options.hpp",),
    },
    "0x004B7540": {
        "cases": ilff_header_cases,
        "retail": ilff_header_retail,
        "candidate_source": ilff_header_candidate_source,
        "include": ("igi/retail/sys/ilff_navigation.hpp",),
    },
    "0x004F1160": {
        "cases": parse_mode_cases,
        "retail": parse_mode_retail,
        "candidate_source": parse_mode_candidate_source,
        "include": ("igi/retail/script/script_mode.hpp",),
    },
    "0x0048F970": {
        "cases": wndproc_cases,
        "retail": wndproc_retail,
        "candidate_source": wndproc_candidate_source,
        "include": ("igi/retail/core/native_0048f970.hpp",),
    },
    "0x00463180": {
        "cases": crouch_cases,
        "retail": crouch_retail,
        "candidate_source": crouch_candidate_source,
        "include": ("igi/retail/human/human_camera_eye.hpp",),
    },
    "0x00465820": {
        "cases": eye_cases,
        "retail": eye_retail,
        "candidate_source": eye_candidate_source,
        "include": ("igi/retail/human/human_camera_eye.hpp",),
    },
    "0x004EDB60": {
        "cases": lod_cases,
        "retail": lod_retail,
        "candidate_source": lod_candidate_source,
        "include": ("igi/retail/render/lod_settings.hpp",),
    },
    "0x004659E0": {
        "cases": view_reset_cases,
        "retail": view_reset_retail,
        "candidate_source": view_reset_candidate_source,
        "include": ("igi/retail/human/human_view.hpp",),
    },
    "0x00491D40": {
        "cases": aspect_cases,
        "retail": aspect_retail,
        "candidate_source": aspect_candidate_source,
        "include": ("igi/retail/render/view_block.hpp",),
    },
    "0x00497D70": {
        "cases": builder_cases,
        "retail": builder_retail,
        "candidate_source": builder_candidate_source,
        "include": ("igi/retail/render/view_block.hpp",),
    },
    "0x004C6670": {
        "cases": material_cases,
        "retail": material_retail,
        "candidate_source": material_candidate_source,
        "include": ("igi/retail/terrain/terrain_loaders.hpp",),
        "substitutions": {"@CONTEXT_ADDR@": f"0x{TERRAIN_CONTEXT_ADDR:08X}ULL"},
        "extra_cxx_flags": ("-ffunction-sections", "-fdata-sections"),
        "extra_link_flags": TERRAIN_LINK_FLAGS,
        "limits": TERRAIN_LIMITS,
    },
    "0x004C6760": {
        "cases": tilemap_cases,
        "retail": tilemap_retail,
        "candidate_source": tilemap_candidate_source,
        "include": ("igi/retail/terrain/terrain_loaders.hpp",),
        "substitutions": {"@CONTEXT_ADDR@": f"0x{TERRAIN_CONTEXT_ADDR:08X}ULL"},
        "extra_cxx_flags": ("-ffunction-sections", "-fdata-sections"),
        "extra_link_flags": TERRAIN_LINK_FLAGS,
        "limits": TERRAIN_LIMITS,
    },
}


def format_case_literal(address: str, case: dict) -> str:
    if address == "0x0048F970":
        return wndproc_case_literal(case)
    if address == "0x004F1160":
        return parse_mode_case_literal(case)
    if address == "0x004B7540":
        return ilff_header_case_literal(case)
    if address == "0x00465820":
        return (f'        {{"{case["id"]}", static_cast<std::size_t>({case["human_offset"]}), '
                f'{float(case["feet_x"])}, {float(case["feet_y"])}, {float(case["feet_z"])}, '
                f'{case["crouch"]}U}},')
    if address == "0x00463180":
        return (f'        {{"{case["id"]}", static_cast<std::size_t>({case["human_offset"]}), '
                f'{case["flag"]}U}},')
    if address == "0x004EDB60":
        values = ", ".join(repr(float(value)) for value in case["distances"])
        return (f'        {{"{case["id"]}", "{case["name"]}", {values}, {case["record_present"]}}},')
    if address == "0x00404590":
        return f'        {{"{case["id"]}", {case["index"]}U}},'
    if address in ("0x00404F00", "0x00405530"):
        return f'        {{"{case["id"]}", {signed32(case["flags"])}}},'
    if address == "0x00404F60":
        return f'        {{"{case["id"]}", {signed32(case["stored"])}}},'
    if address == "0x00405620":
        return f'        {{"{case["id"]}"}},'
    if address == "0x004055C0":
        return f'        {{"{case["id"]}", {case["level"]}}},'
    if address == "0x004659E0":
        return f'        {{"{case["id"]}", static_cast<std::size_t>({case["pointer_offset"]})}},'
    if address == "0x00491D40":
        return (f'        {{"{case["id"]}", {case["display_width"]}, {case["display_height"]}}},')
    if address in ("0x004C6670", "0x004C6760"):
        return (f'        {{"{case["id"]}", {case["seed"]}, '
                f'static_cast<unsigned>({case["group_index"]})}},')
    return (
        f'        {{"{case["id"]}", {case["display"][0]}, {case["display"][1]}, '
        f'{float(case["rect"][0])}, {float(case["rect"][1])}, {float(case["rect"][2])}, '
        f'{float(case["rect"][3])}, {float(case["fov_x"])}, '
        f'{case["camera"][0]}, {case["camera"][1]}, {case["camera"][2]}, {case["view_pattern_revision"]}}},'
    )


def numeric_close(reference, candidate, absolute: float, relative: float) -> bool:
    if isinstance(reference, bool) or isinstance(candidate, bool):
        return type(reference) is type(candidate) and reference == candidate
    if isinstance(reference, int) and isinstance(candidate, int):
        return reference == candidate
    if isinstance(reference, (int, float)) and isinstance(candidate, (int, float)):
        difference = abs(float(reference) - float(candidate))
        scale = max(abs(float(reference)), abs(float(candidate)), 1.0)
        return difference <= absolute or difference <= relative * scale
    return reference == candidate


def compare_outputs(reference: dict, candidate: dict) -> list[str]:
    problems = []
    for field in ("return_value", "state_delta", "side_effects"):
        reference_value = reference.get(field)
        candidate_value = candidate.get(field)
        if isinstance(reference_value, dict) and isinstance(candidate_value, dict):
            for key in sorted(set(reference_value) | set(candidate_value)):
                if key not in reference_value:
                    problems.append(f"{field}.{key} missing from reference")
                    continue
                if key not in candidate_value:
                    problems.append(f"{field}.{key} missing from candidate")
                    continue
                if not numeric_close(reference_value[key], candidate_value[key],
                                     FLOAT_TOLERANCE["absolute"], FLOAT_TOLERANCE["relative"]):
                    problems.append(
                        f"{field}.{key}: reference={reference_value[key]!r} candidate={candidate_value[key]!r}")
        elif not numeric_close(reference_value, candidate_value,
                               FLOAT_TOLERANCE["absolute"], FLOAT_TOLERANCE["relative"]):
            problems.append(f"{field}: reference={reference_value!r} candidate={candidate_value!r}")
    return problems


def ensure_emulator_runtime() -> None:
    """Select an interpreter whose Unicorn can actually map x86 guest memory.

    On macOS ARM, importing the arm64 Unicorn wheel can succeed while the first
    ``uc_mem_map`` raises SIGILL. Probe the operation used by this runner in a
    child process so that failure is reported and a configured Rosetta runtime
    can be selected without killing the native-gate process.
    """
    probe_code = (
        "import unicorn; "
        "u=unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32); "
        "u.mem_map(0x400000, 0x1000)"
    )

    def probe(interpreter: str) -> subprocess.CompletedProcess:
        return subprocess.run(
            [interpreter, "-c", probe_code], capture_output=True, text=True, check=False,
        )

    current = probe(sys.executable)
    if current.returncode == 0:
        return
    configured = os.environ.get("IGI_UNICORN_PYTHON", "").strip()
    candidates = [configured] if configured else []
    # The Rosetta venv lives beside the repository, not inside every worktree,
    # so resolve it from the shared checkout as well.
    root_dir = pathlib.Path(__file__).resolve().parents[1]
    candidates.append(str(root_dir / "build/unicorn-rosetta/python"))
    candidates.append(str(root_dir / "build/unicorn-rosetta/bin/python"))
    candidates.append(str(root_dir.parent / "build/unicorn-rosetta/bin/python"))
    candidates.append(str(pathlib.Path(__file__).resolve().parents[1] / ".venv-angr/bin/python"))
    candidates.append("/Users/haseeb-mir/Documents/Code/re-agent/.venv/bin/python")
    failures = [f"{sys.executable}: exit {current.returncode}"]
    for candidate in candidates:
        path = pathlib.Path(candidate).expanduser()
        if not path.is_file() or str(path) == sys.executable:
            continue
        result = probe(str(path))
        if result.returncode == 0:
            os.execv(str(path), [str(path), *sys.argv])
        failures.append(f"{path}: exit {result.returncode}")
    raise SystemExit(
        "Unicorn x86 memory-map preflight failed (" + "; ".join(failures) + "). "
        "On Apple Silicon, configure IGI_UNICORN_PYTHON to an x86_64 Python runtime "
        "with the matching x86_64 Unicorn wheel."
    )


def main() -> int:
    ensure_emulator_runtime()
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument("--cases", type=int, default=8)
    parser.add_argument("--report", type=pathlib.Path)
    arguments = parser.parse_args()

    address = "0x" + arguments.address[-8:].upper()
    if address not in NATIVES:
        raise SystemExit(f"no differential adapter registered for {address}")
    adapter = NATIVES[address]
    binary = arguments.binary.resolve()
    implementation = arguments.implementation.resolve()
    root = arguments.root.resolve()
    if sha256(binary) != arguments.binary_sha256:
        raise SystemExit("retail binary SHA-256 does not match the supplied digest")
    if sha256(implementation) != arguments.implementation_sha256:
        raise SystemExit("implementation SHA-256 does not match the supplied digest")

    try:
        import unicorn  # noqa: F401 - the retail oracle requires the emulator
    except ImportError as error:
        raise SystemExit(f"unicorn is required for the retail oracle: {error}") from error

    cases = adapter["cases"]()
    image = PebImage(binary)

    harness_cases = "\n".join(format_case_literal(address, case) for case in cases)
    harness_source = adapter["candidate_source"]().replace("@CASES@", harness_cases)
    for marker, replacement in adapter.get("substitutions", {}).items():
        harness_source = harness_source.replace(marker, replacement)

    with tempfile.TemporaryDirectory(prefix="igi-native-unicorn-") as workspace:
        workspace_path = pathlib.Path(workspace)
        harness_path = workspace_path / "candidate_harness.cpp"
        harness_path.write_text(harness_source, encoding="utf-8")
        candidate_path = workspace_path / "candidate_harness"
        build = subprocess.run(
            [CXX, "-std=c++20", "-O1", *adapter.get("extra_cxx_flags", []),
             f"-I{root / 'engine/include'}", str(implementation),
             str(harness_path), *adapter.get("extra_link_flags", []), "-o", str(candidate_path)],
            cwd=root, text=True, capture_output=True, check=False, timeout=120,
        )
        if build.returncode != 0:
            raise SystemExit(f"candidate harness failed to build: {build.stderr[-2000:]}")
        executed = subprocess.run([str(candidate_path)], text=True, capture_output=True,
                                  check=False, timeout=120)
        if executed.returncode != 0:
            raise SystemExit(f"candidate harness failed: {executed.stderr[-2000:]}")
    candidate_outputs = {}
    for line in executed.stdout.splitlines():
        line = line.strip()
        if line.startswith("{"):
            record = json.loads(line)
            candidate_outputs[record["id"]] = record

    report_cases = []
    passed = 0
    failed = 0
    for case in cases:
        reference = adapter["retail"](image, case)
        candidate = candidate_outputs.get(case["id"])
        if candidate is None:
            raise SystemExit(f"candidate output missing for case {case['id']}")
        problems = compare_outputs(reference, candidate)
        if problems:
            failed += 1
            reference["comparison_problems"] = problems
        else:
            passed += 1
        report_cases.append({
            "id": case["id"],
            "categories": case["categories"],
            "input": {
                "args": [case[key] for key in sorted(case) if key not in ("id", "categories")],
                "initial_state": {"seed": case["seed"]},
            },
            "reference_output": reference,
            "candidate_output": {
                "return_value": candidate["return_value"],
                "state_delta": candidate["state_delta"],
                "side_effects": candidate["side_effects"],
            },
        })

    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "address": address,
        "binary_sha256": arguments.binary_sha256,
        "implementation_sha256": arguments.implementation_sha256,
        "executed": True,
        "oracle": "retail-binary-execution",
        "result": "passed" if failed == 0 else "failed",
        "method": (
            "Retail instruction bytes for the selected function were executed under unicorn 2.1.4 "
            "(x86-32) with the true PE image mapped at 0x00400000 and a synthetic cdecl stack; the "
            "candidate C++ implementation was compiled and executed natively with the same synthetic "
            "state; return values, context fields, and copied bytes were compared. Unicorn is used "
            "for isolated retail-byte execution, rather than a live Windows game process, and because "
            "x87 (fptan/fld/fmul/fdiv/fstp) is not implemented by Radare2 ESIL or angr VEX."
        ),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": FLOAT_TOLERANCE,
        "limits": adapter.get("limits", ""),
        "tests_total": len(report_cases),
        "tests_passed": passed,
        "tests_failed": failed,
        "cases": report_cases,
    }
    if arguments.report is not None:
        arguments.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=1))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())

