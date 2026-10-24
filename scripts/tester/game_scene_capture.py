#!/usr/bin/env python3
"""Launch a game level, wait for it to load, skip the intro with Escape, and capture only the game window.

Full-screen capture grabs whatever is on the desktop, which is not game evidence. This runs the whole
scene-check sequence the owner asked for:

  1. launch `<exe> window level<N>` from the game directory through the CrossOver wine launcher,
  2. poll for the game window (by process name) until it exists,
  3. wait the load time for the level,
  4. bring the window to the front and send Escape to skip the intro/cutscene,
  5. capture that window by CGWindowID with `screencapture -l`, with a region fallback if the window
     has no image backing,
  6. terminate the process this run started.

Writes a JSON report (pid, window id, bounds, image size, scale, capture mode) and the PNG.
"""
import argparse
import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path

import AppKit
import Quartz
from PIL import Image

WINE = '/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine'
GAME_DIR = Path('/Users/haseeb-mir/Downloads/IGI')
SCREENCAPTURE = '/usr/sbin/screencapture'
ESCAPE_KEYCODE = 53


def process_ids(exe_name):
    listing = subprocess.run(['ps', '-axo', 'pid=,command='], capture_output=True, text=True).stdout
    ids = []
    for line in listing.splitlines():
        pid_text, _, command = line.strip().partition(' ')
        if exe_name.lower() not in command.lower():
            continue
        if any(shell in command for shell in ('bash -c', 'grep', 'capture', 'ps -axo')):
            continue
        ids.append(int(pid_text))
    return ids


def windows_for(pid, title_pattern=None):
    found = []
    for window in Quartz.CGWindowListCopyWindowInfo(Quartz.kCGWindowListOptionAll, Quartz.kCGNullWindowID):
        if int(window.get(Quartz.kCGWindowOwnerPID, 0)) != pid:
            continue
        title = window.get(Quartz.kCGWindowName, '') or ''
        bounds = window.get(Quartz.kCGWindowBounds, {})
        width, height = int(bounds.get('Width', 0)), int(bounds.get('Height', 0))
        if width < 200 or height < 200:
            continue
        if title_pattern and not re.search(title_pattern, title, re.IGNORECASE):
            continue
        found.append({
            'window_id': int(window.get(Quartz.kCGWindowNumber, 0)),
            'pid': pid,
            'owner': window.get(Quartz.kCGWindowOwnerName, ''),
            'title': title,
            'bounds': {'x': int(bounds.get('X', 0)), 'y': int(bounds.get('Y', 0)), 'w': width, 'h': height},
            'layer': int(window.get(Quartz.kCGWindowLayer, 0)),
        })
    found.sort(key=lambda item: (item['layer'] != 0, -item['bounds']['w'] * item['bounds']['h']))
    return found


def raise_window(pid):
    application = AppKit.NSRunningApplication.runningApplicationWithProcessIdentifier_(pid)
    if application is not None:
        application.activateWithOptions_(AppKit.NSApplicationActivateIgnoringOtherApps)


def send_escape(pid):
    """Send Escape to the game process: a posted key event first, AppleScript fallback."""
    sent = []
    try:
        for key_down in (True, False):
            event = Quartz.CGEventCreateKeyboardEvent(None, ESCAPE_KEYCODE, key_down)
            Quartz.CGEventPostToPid(pid, event)
        sent.append('CGEventPostToPid')
    except Exception as error:  # pragma: no cover - depends on the host's accessibility grants
        sent.append(f'CGEventPostToPid failed: {error}')
    time.sleep(0.4)
    script = 'tell application "System Events" to key code 53'
    result = subprocess.run(['osascript', '-e', script], capture_output=True, text=True)
    sent.append('osascript' if result.returncode == 0 else f'osascript failed: {result.stderr.strip()}')
    return sent


def capture(window, out):
    out.parent.mkdir(parents=True, exist_ok=True)
    tries = [['-x', '-o', '-l', str(window['window_id']), str(out)]]
    bounds = window['bounds']
    tries.append(['-x', '-o', '-R', f"{bounds['x']},{bounds['y']},{bounds['w']},{bounds['h']}", str(out)])
    errors = []
    for index, arguments in enumerate(tries):
        result = subprocess.run([SCREENCAPTURE, *arguments], capture_output=True, text=True)
        if result.returncode == 0 and out.is_file() and out.stat().st_size > 0:
            image = Image.open(out)
            expected = (bounds['w'], bounds['h'])
            scale = image.size[0] // expected[0] if expected[0] else 1
            return {'capture_mode': 'window' if index == 0 else 'region',
                    'image_size': list(image.size), 'scale': scale,
                    'matches_window': image.size[0] == expected[0] * scale and image.size[1] == expected[1] * scale,
                    'errors': errors}
        errors.append(result.stderr.strip() or f'returncode {result.returncode}')
    return {'capture_mode': 'failed', 'errors': errors}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--level', required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--title', default='IGI')
    parser.add_argument('--load-seconds', type=float, default=22.0)
    parser.add_argument('--window-timeout', type=float, default=60.0)
    parser.add_argument('--settle', type=float, default=1.5)
    parser.add_argument('--skip-intro', action='store_true')
    parser.add_argument('--keep-running', action='store_true')
    args = parser.parse_args()

    report = {'exe': args.exe, 'level': args.level, 'out': str(args.out)}
    if process_ids(args.exe):
        report['result'] = 'already-running'
        print(json.dumps(report))
        return 2

    environment = dict(os.environ, CX_BOTTLE='open-igi')
    log = Path(f"/tmp/{Path(args.exe).stem}-scene.log")
    with log.open('wb') as log_file:
        process = subprocess.Popen([WINE, 'start', '/unix', f'./{args.exe}', 'window', args.level],
                                   cwd=str(GAME_DIR), env=environment, stdout=log_file, stderr=subprocess.STDOUT)
    report['launcher_pid'] = process.pid

    deadline = time.time() + args.window_timeout
    windows = []
    while time.time() < deadline:
        for pid in process_ids(args.exe):
            windows = windows_for(pid, args.title)
            if windows:
                break
        if windows:
            break
        time.sleep(0.2)
    if not windows:
        report['result'] = 'no-window'
        report['log'] = str(log)
        if not args.keep_running:
            subprocess.run(['pkill', '-f', args.exe])
        print(json.dumps(report))
        return 3

    window = windows[0]
    report['window'] = window
    raise_window(window['pid'])
    time.sleep(args.load_seconds)
    if args.skip_intro:
        report['escape'] = send_escape(window['pid'])
        time.sleep(args.settle)
        # Re-read the window: the title or size can change after the level loads.
        refreshed = windows_for(window['pid'], args.title) or windows_for(window['pid'])
        if refreshed:
            window = refreshed[0]
            report['window'] = window
        raise_window(window['pid'])
        time.sleep(0.5)

    report.update(capture(window, args.out))
    report['result'] = 'captured' if report.get('capture_mode') in ('window', 'region') else 'capture-failed'
    if not args.keep_running:
        subprocess.run(['pkill', '-f', args.exe])
        time.sleep(0.5)
    report['log'] = str(log)
    report['still_running'] = bool(process_ids(args.exe))
    print(json.dumps(report))
    return 0 if report['result'] == 'captured' else 4


if __name__ == '__main__':
    sys.exit(main())
