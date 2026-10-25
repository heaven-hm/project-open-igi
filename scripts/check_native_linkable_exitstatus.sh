#!/bin/sh
# Fail-closed linkability check: reports by EXIT STATUS, never by parsing text.
# Written after a commit added a non-linkable file to the build because the
# checker's NOT LINKABLE output was piped through sed and printed under a
# "link=" label, rendering a failure as a plausible success.
# Usage: check_native_linkable_exitstatus.sh <file.cpp> [file.cpp ...]
rc=0
for f in "$@"; do
  if python3 scripts/check_native_linkable.py "$f" >/dev/null 2>&1; then
    printf 'LINKABLE      %s\n' "$f"
  else
    printf 'NOT LINKABLE  %s\n' "$f"
    rc=1
  fi
done
exit $rc
