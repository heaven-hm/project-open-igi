"""PE reader over the retail executable using pefile.

Provides section metadata, VA<->RVA<->file-offset mapping, the .text byte window,
and the export directory. All addresses are retail virtual addresses (VA).
If pefile is unavailable, the module raises PEUnavailable so callers can degrade
gracefully and record the tool gap honestly.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

try:
    import pefile  # type: ignore[import-not-found]
except Exception:  # pragma: no cover - optional dependency
    pefile = None  # type: ignore[assignment]


class PEUnavailable(Exception):
    """pefile is required for offline PE access but could not be imported."""


@dataclass
class Section:
    name: str
    va: int          # virtual address (image_base + virtual_start)
    rva: int
    vsize: int
    raw_ptr: int     # file offset of raw data
    raw_size: int

    @property
    def end_va(self) -> int:
        return self.va + max(0, self.vsize)


@dataclass
class PEImage:
    path: Path
    image_base: int
    entry_point_va: int
    sections: list[Section] = field(default_factory=list)
    exports: list[dict[str, Any]] = field(default_factory=list)
    _pe: Any = None

    def section_by_name(self, name: str) -> Section | None:
        for section in self.sections:
            if section.name == name:
                return section
        return None

    def section_containing_va(self, va: int) -> Section | None:
        for section in self.sections:
            if section.va <= va < section.end_va:
                return section
        return None

    def va_to_rva(self, va: int) -> int:
        return va - self.image_base

    def va_to_offset(self, va: int) -> int | None:
        section = self.section_containing_va(va)
        if section is None:
            return None
        return section.raw_ptr + (va - section.va)

    def offset_to_va(self, offset: int) -> int | None:
        for section in self.sections:
            if section.raw_ptr <= offset < section.raw_ptr + section.raw_size:
                return section.va + (offset - section.raw_ptr)
        return None

    def read_bytes(self, va: int, length: int) -> bytes | None:
        section = self.section_containing_va(va)
        if section is None:
            return None
        start = section.raw_ptr + (va - section.va)
        end = start + length
        if end > section.raw_ptr + section.raw_size:
            return None
        self._pe.stream_copy  # noqa: B018 - keep a reference so the PE stays mapped
        with open(self.path, "rb") as handle:
            handle.seek(start)
            return handle.read(length)

    def read_dword(self, va: int) -> int | None:
        raw = self.read_bytes(va, 4)
        if raw is None or len(raw) < 4:
            return None
        return int.from_bytes(raw, "little")

    def read_cstring(self, va: int, max_len: int = 256) -> str | None:
        section = self.section_containing_va(va)
        if section is None:
            return None
        start = section.raw_ptr + (va - section.va)
        end_limit = section.raw_ptr + section.raw_size
        with open(self.path, "rb") as handle:
            handle.seek(start)
            buf = bytearray()
            while len(buf) < max_len:
                pos = handle.tell()
                if pos >= end_limit:
                    break
                chunk = handle.read(end_limit - pos)
                if not chunk:
                    break
                nul = chunk.find(b"\x00")
                if nul != -1:
                    buf.extend(chunk[:nul])
                    break
                buf.extend(chunk)
        try:
            return buf.decode("ascii", errors="replace")
        except Exception:
            return None

    def disassembly_target_bytes(self, va: int, length: int) -> bytes | None:
        """Return raw bytes for a VA window, spanning sections if necessary."""
        section = self.section_containing_va(va)
        if section is None:
            return None
        out = bytearray()
        cursor_va = va
        while len(out) < length:
            sec = self.section_containing_va(cursor_va)
            if sec is None:
                break
            rel = cursor_va - sec.va
            avail = sec.vsize - rel
            if avail <= 0:
                break
            want = min(length - len(out), avail)
            raw = self.read_bytes(cursor_va, want)
            if raw is None:
                break
            out.extend(raw)
            cursor_va = sec.va + sec.vsize  # advance to next section boundary
        return bytes(out) if out else None


def load_image(path: Path, image_base: int | None = None) -> PEImage:
    if pefile is None:
        raise PEUnavailable("pefile is not installed in this interpreter")
    pe = pefile.PE(str(path), fast_load=True)
    try:
        pe.parse_data_directories(directories=[
            pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]])
    except Exception:
        pass  # export parsing is best effort
    base = image_base if image_base is not None else pe.OPTIONAL_HEADER.ImageBase
    sections: list[Section] = []
    for sec in pe.sections:
        name = sec.Name.rstrip(b"\x00").decode("ascii", errors="replace")
        sections.append(Section(
            name=name,
            va=base + sec.VirtualAddress,
            rva=sec.VirtualAddress,
            vsize=sec.Misc_VirtualSize,
            raw_ptr=sec.PointerToRawData,
            raw_size=sec.SizeOfRawData,
        ))
    exports: list[dict[str, Any]] = []
    if hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
            exports.append({
                "name": exp.name.decode("ascii", errors="replace") if exp.name else None,
                "address": base + exp.address,
                "ordinal": exp.ordinal,
            })
    return PEImage(
        path=Path(path),
        image_base=int(base),
        entry_point_va=int(base + pe.OPTIONAL_HEADER.AddressOfEntryPoint),
        sections=sections,
        exports=exports,
        _pe=pe,
    )
