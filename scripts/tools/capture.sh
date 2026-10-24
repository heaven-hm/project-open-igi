#!/usr/bin/env bash
# Screen capture helpers for IGI visual evidence.
# Screenshots: PNG of the whole screen (or a rectangle with -R x,y,w,h).
# Video: QuickTime/avfoundation screen device recording, plus frame extraction
# from an existing recording so screenshots can be taken from video.
#
# Usage:
#   scripts/tools/capture.sh shot   <out.png> [x,y,w,h]
#   scripts/tools/capture.sh video  <out.mp4> <seconds> [fps]
#   scripts/tools/capture.sh frames <in.mp4> <outdir> [fps]
#   scripts/tools/capture.sh devices
#
# Screen device index is printed by `devices`; on this machine capture screen is
# index 3 (see TOOLS.md). Everything is bounded: no daemon, no background jobs.
set -euo pipefail
cmd="${1:-}"; shift || true
case "$cmd" in
  devices)
    ffmpeg -hide_banner -f avfoundation -list_devices true -i "" 2>&1 | sed -n '/AVFoundation video devices/,/AVFoundation audio devices/p'
    ;;
  shot)
    out="${1:?usage: shot <out.png> [x,y,w,h]}"; rect="${2:-}"
    mkdir -p "$(dirname "$out")"
    if [ -n "$rect" ]; then screencapture -x -R "$rect" "$out"; else screencapture -x "$out"; fi
    printf 'screenshot: %s (%s bytes)\n' "$out" "$(wc -c <"$out" | tr -d ' ')"
    ;;
  video)
    out="${1:?usage: video <out.mp4> <seconds> [fps]}"; secs="${2:?seconds}"; fps="${3:-15}"
    dev="${CAPTURE_SCREEN_DEVICE:-3}"
    mkdir -p "$(dirname "$out")"
    ffmpeg -y -hide_banner -loglevel error -f avfoundation -capture_cursor 0 -framerate "$fps" \
      -i "${dev}:none" -t "$secs" -c:v libx264 -pix_fmt yuv420p "$out"
    printf 'video: %s (%s bytes, %ss @ %sfps, device %s)\n' "$out" "$(wc -c <"$out" | tr -d ' ')" "$secs" "$fps" "$dev"
    ;;
  frames)
    in="${1:?usage: frames <in.mp4> <outdir> [fps]}"; outdir="${2:?outdir}"; fps="${3:-1}"
    mkdir -p "$outdir"
    ffmpeg -y -hide_banner -loglevel error -i "$in" -vf "fps=$fps" "$outdir/frame_%04d.png"
    printf 'frames: %s -> %s (%s files)\n' "$in" "$outdir" "$(find "$outdir" -name 'frame_*.png' | wc -l | tr -d ' ')"
    ;;
  *)
    sed -n '2,16p' "$0"; exit 2 ;;
esac
