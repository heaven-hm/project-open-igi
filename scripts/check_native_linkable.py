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
    a = ap.parse_args()
    if not pathlib.Path(a.archive).is_file():
        print(f'archive missing: {a.archive}', file=sys.stderr); return 2
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
    def runtime_helper(n):
        return n in {'memcpy', 'memset', 'memmove', 'memcmp', 'strlen', 'sprintf', 'printf',
                     'snprintf', 'vsnprintf', 'vprintf', 'fprintf', 'abort', 'exit'} \
            or n.startswith('mingw_')

    missing = sorted(n for n in need - have
                     if not n.startswith(('__', '___')) and not runtime_helper(n))
    if missing:
        print(f'{a.source}: NOT LINKABLE — undefined in tree:', *missing, sep='\n  ')
        return 1
    print(f'{a.source}: linkable ({len(need)} undefined, all defined in tree)')
    return 0

if __name__ == '__main__':
    sys.exit(main())
