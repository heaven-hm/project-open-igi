#!/usr/bin/env python3
"""Capture only the game window, by PID/CGWindowID, after bringing it to the front.

Full-screen capture grabs whatever is on the desktop (an editor, a terminal) and is not
evidence of the game. This tool enumerates the windows of a target process, raises the
game window, captures it with `screencapture -l <window id>`, and reports the pid,
window id, bounds and captured image size so the image can be checked against the
window instead of the screen.

The target can be named by `--pid`, by `--process <exe name>` (matched against process
command lines, excluding shells and this tool), or by `--title` regex. With `--timeout`
the tool polls until the window appears, which is required for a candidate that opens
its window and then terminates.
"""
import argparse
import json
import re
import subprocess
import sys
import time
from pathlib import Path

import AppKit
import Quartz
from PIL import Image

SCREENCAPTURE = '/usr/sbin/screencapture'


def process_ids(exe_name):
    listing = subprocess.run(['ps', '-axo', 'pid=,command='], capture_output=True, text=True).stdout
    found = []
    for line in listing.splitlines():
        pid_text, _, command = line.strip().partition(' ')
        if exe_name.lower() not in command.lower():
            continue
        if any(shell in command for shell in ('bash -c', 'grep', 'capture_game_window', 'ps -axo')):
            continue
        found.append(int(pid_text))
    return found


def windows_for(pid=None, title_pattern=None, exclude_owner=None, owner_pattern=None):
    # All windows, not only on-screen ones: the game window is frequently behind other
    # applications and must still be raiseable and capturable by window id.
    windows = []
    for window in Quartz.CGWindowListCopyWindowInfo(Quartz.kCGWindowListOptionAll, Quartz.kCGNullWindowID):
        owner = window.get(Quartz.kCGWindowOwnerName, '') or ''
        title = window.get(Quartz.kCGWindowName, '') or ''
        owner_pid = int(window.get(Quartz.kCGWindowOwnerPID, 0))
        if exclude_owner and exclude_owner.lower() in owner.lower():
            continue
        if pid is not None and owner_pid != pid:
            continue
        if owner_pattern and not re.search(owner_pattern, owner, re.IGNORECASE):
            continue
        if title_pattern and not re.search(title_pattern, title or owner, re.IGNORECASE):
            continue
        bounds = window.get(Quartz.kCGWindowBounds, {})
        width, height = int(bounds.get('Width', 0)), int(bounds.get('Height', 0))
        if width < 64 or height < 64:
            continue
        windows.append({
            'window_id': int(window.get(Quartz.kCGWindowNumber, 0)),
            'pid': owner_pid,
            'owner': owner,
            'title': title,
            'bounds': {'x': int(bounds.get('X', 0)), 'y': int(bounds.get('Y', 0)), 'w': width, 'h': height},
            'layer': int(window.get(Quartz.kCGWindowLayer, 0)),
        })
    windows.sort(key=lambda item: (item['layer'] != 0, -item['bounds']['w'] * item['bounds']['h']))
    return windows


def discover(args):
    if args.pid is not None:
        return windows_for(args.pid, args.title, args.exclude_owner)
    if args.process is not None:
        for candidate_pid in process_ids(args.process):
            found = windows_for(candidate_pid, args.title, args.exclude_owner)
            if found:
                return found
        return windows_for(None, args.title, args.exclude_owner, re.escape(args.process))
    return windows_for(None, args.title, args.exclude_owner)


def raise_window(pid):
    application = AppKit.NSRunningApplication.runningApplicationWithProcessIdentifier_(pid)
    if application is not None:
        application.activateWithOptions_(AppKit.NSApplicationActivateIgnoringOtherApps)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--pid', type=int, help='window owner process id')
    parser.add_argument('--process', help='executable name to find the window owner by, for example igi.exe')
    parser.add_argument('--title', help='regex on the window title or owner name')
    parser.add_argument('--exclude-owner', default='DeepSeek Harness', help='owner name to ignore')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--settle', type=float, default=0.5, help='seconds between raising and capturing')
    parser.add_argument('--timeout', type=float, default=0.0, help='poll for the window until this many seconds pass')
    parser.add_argument('--poll', type=float, default=0.1)
    args = parser.parse_args()

    deadline = time.time() + max(0.0, args.timeout)
    candidates = []
    while not candidates:
        candidates = discover(args)
        if candidates or time.time() >= deadline:
            break
        time.sleep(args.poll)
    if not candidates:
        print(json.dumps({'result': 'no-window', 'pid': args.pid, 'process': args.process, 'title': args.title}))
        return 2

    chosen = candidates[0]
    raise_window(chosen['pid'])
    time.sleep(args.settle)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    capture = subprocess.run([SCREENCAPTURE, '-x', '-o', '-l', str(chosen['window_id']), str(args.out)],
                             capture_output=True, text=True)
    report = {'result': 'captured' if capture.returncode == 0 else 'capture-failed',
              'window': chosen, 'out': str(args.out), 'stderr': capture.stderr.strip()}
    if args.out.is_file():
        image = Image.open(args.out)
        expected = (chosen['bounds']['w'], chosen['bounds']['h'])
        scale = image.size[0] // expected[0] if expected[0] else 1
        report['image_size'] = list(image.size)
        report['scale'] = scale
        report['matches_window'] = (image.size[0] == expected[0] * scale and image.size[1] == expected[1] * scale)
    print(json.dumps(report))
    return 0 if report['result'] == 'captured' else 3


if __name__ == '__main__':
    sys.exit(main())
