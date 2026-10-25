#!/usr/bin/env python3
"""Fail if a new native references a symbol the tree does not DEFINE.

This exists because an earlier check grepped engine/src for each undefined name and
accepted a match on a DECLARATION. Two natives passed that check, were integrated, and
then failed to link: a pure `extern` declaration satisfies a grep and satisfies nothing
the linker needs.

The question a native must answer before integration is not "is this name mentioned in
the tree" but "is there a definition the linker will bind". So this compares the object's
undefined set against the DEFINED symbols of the already-built object archive.
"""
import argparse, subprocess, sys, pathlib

def symbols(cmd):
    return {l.split()[-1].lstrip('_') for l in
            subprocess.run(cmd, capture_output=True, text=True).stdout.splitlines() if l.split()}

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('source')
    ap.add_argument('--archive', default='build-win32-no-verify-gate/CMakeFiles/igi.dir/objects.a')
    ap.add_argument('--object', default='/tmp/_probe.o')
    ap.add_argument('--allow-stale', action='store_true',
                    help='compare against a stale archive deliberately (the answer is about the archive)')
    a = ap.parse_args()
    if not pathlib.Path(a.archive).is_file():
        print(f'archive missing: {a.archive}', file=sys.stderr); return 2

    # A stale archive is worse than a missing one: it answers, and the answer looks like a
    # fact about the tree. handler-abi-worker hit exactly this - a default archive five days
    # old returned NOT LINKABLE for two natives that link cleanly, because the archive could
    # not contain symbols landed since it was built. Refuse rather than answer, and say the
    # age, unless --allow-stale is passed deliberately.
    import os, time
    age_days = (time.time() - os.path.getmtime(a.archive)) / 86400.0
    if age_days > 0.5 and not a.allow_stale:
        print(f'archive is {age_days:.1f} days old: {a.archive}\n'
              f'  a stale archive cannot contain recently landed symbols, so a NOT LINKABLE\n'
              f'  answer from it is about the archive and not about the tree.\n'
              f'  Rebuild first, or pass --allow-stale to compare against it deliberately.',
              file=sys.stderr)
        return 2
    r = subprocess.run(['i686-w64-mingw32-g++', '-std=c++20', '-O2', '-Wall', '-Wextra',
                        '-c', '-Iengine/include', a.source, '-o', a.object], capture_output=True, text=True)
    if r.returncode:
        print(r.stderr.strip(), file=sys.stderr); return 2
    need = symbols(['i686-w64-mingw32-nm', '-u', a.object])
    have = symbols(['nm', '--defined-only', a.archive])
    have = {s for s in have if s}

    # CRT/libmingwex helpers the linker resolves from the runtime rather than from the tree.
    # The `mingw_` forms matter: the archive spells them `___mingw_sprintf`, and `lstrip('_')`
    # in symbols() turns that into `mingw_sprintf`, which then escapes BOTH the `__` prefix
    # filter below and a bare `sprintf` entry. So normalise the prefix here rather than
    # assuming the strip left a name that matches the ignore set.
    # libmingwex/libm long-double and float math routines the compiler emits
    # itself for sincosl/sincosf and friends. They are runtime, not natives.
    MATH_HELPERS = {
        # libmingwex/libm routines GCC emits itself when it fuses or widens
        # floating-point work: sin/cos fusion and the long-double rounding family.
        # These are runtime, not natives. The list is a class, not a name: the
        # first version named only sincosl and two committed files were still
        # reported NOT LINKABLE for sincosf and llrintl.
        'sincos', 'sincosf', 'sincosl', '__sincos', '__sincosf', '__sincosl',
        'llrint', 'llrintf', 'llrintl', 'lrint', 'lrintf', 'lrintl',
        'llround', 'llroundf', 'llroundl', 'round', 'roundf', 'roundl',
        'sinl', 'cosl', 'tanl', 'sqrtl', 'powl', 'fmodl', 'atan2l',
        'fabsf', 'fabsl', 'floorl', 'ceill', 'truncl', 'nearbyintl',
        'rint', 'rintf', 'rintl', 'copysignl', 'scalbnl', 'ldexpl',
    }

    # Compiler-emitted stack probes. When a frame exceeds a page the compiler
    # calls a CRT helper that lowers esp itself instead of emitting `sub esp`.
    # Retail does this too: the MSVC 6 probe at 0x004A54A0 appears in
    # native_41aaf0.cpp (frame 0x2244) and is never hand-transcribed. GCC emits
    # ___chkstk_ms for the same frame, and `lstrip('_')` in symbols() reduces it
    # to `chkstk_ms`, which escapes the `__`/`___` prefix filter above. A stack
    # probe is a property of the frame size, not a native, so it belongs here.
    CRT_PROBES = {'chkstk', 'chkstk_ms', 'alloca', 'alloca_probe'}

    def runtime_helper(n):
        return n in {'memcpy', 'memset', 'memmove', 'memcmp', 'strlen', 'sprintf', 'printf',
                     'snprintf', 'vsnprintf', 'vprintf', 'fprintf', 'abort', 'exit'} \
            or n.startswith('mingw_') or n in MATH_HELPERS or n in CRT_PROBES

    missing = sorted(n for n in need - have
                     if not n.startswith(('__', '___')) and not runtime_helper(n))
    if missing:
        print(f'{a.source}: NOT LINKABLE — undefined in tree:', *missing, sep='\n  ')
        return 1
    print(f'{a.source}: linkable ({len(need)} undefined, all defined in tree)')
    return 0

if __name__ == '__main__':
    sys.exit(main())
