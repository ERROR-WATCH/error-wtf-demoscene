#!/bin/bash
# Offline 30fps render pass of errorwtf-1m (deterministic, same engine as live):
# frames piped raw into ffmpeg, muxed with the reference score WAV.
set -euo pipefail
cd "$(dirname "$0")/../.."
mkdir -p captures/1m
FPS="${1:-30}"
echo "[offline] rendering ${FPS}fps x 120s..."
ERRORWTF_HEADLESS=1 ERRORWTF_OFFLINE=$FPS ./build/1m/errorwtf_1m_prod 2>/tmp/offline_render.log \
  | ffmpeg -y -loglevel error -f rawvideo -pix_fmt rgb24 -s 1280x720 -r "$FPS" -i - \
           -i music/1m/errorwtf_1m_score_preview.wav \
           -c:v libx264 -preset medium -crf 20 -pix_fmt yuv420p \
           -c:a aac -b:a 192k -shortest captures/1m/errorwtf_1m_full_30fps.mp4
tail -1 /tmp/offline_render.log
ffprobe -v error -show_entries format=duration,size -of csv=p=0 captures/1m/errorwtf_1m_full_30fps.mp4
echo "[offline] DONE"
