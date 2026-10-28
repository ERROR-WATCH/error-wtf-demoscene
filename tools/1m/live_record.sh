#!/bin/bash
# Live AV recording of errorwtf-1m on the real desktop session (user error).
# Koexistence: only started because session idle (checked before launch).
set -e
export DISPLAY=:0
export XAUTHORITY=/run/user/1000/.mutter-Xwaylandauth.CY0CX3
export XDG_RUNTIME_DIR=/run/user/1000
cd /mnt/DATEN/ERROR_WTF_DEMOSCENE
echo "[record] launching demo (window mode) as error..."
./errorwtf-1m &
DEMO_PID=$!
sleep 6
WID=$(xdotool search --name "ERROR.wtf" | head -1)
if [ -z "$WID" ]; then echo "[record] FAIL: window not found"; kill $DEMO_PID 2>/dev/null; exit 3; fi
xdotool windowmove "$WID" 200 200
sleep 2
# content geometry (XWayland frame ≠ client area): xwininfo gives exact pixels
GEOM=$(xwininfo -id "$WID" -stats | grep -E "Absolute upper-left X|Absolute upper-left Y|Width:|Height:")
CX=$(echo "$GEOM" | grep "upper-left X" | awk '{print $NF}')
CY=$(echo "$GEOM" | grep "upper-left Y" | awk '{print $NF}')
CW=$(echo "$GEOM" | grep "Width:" | awk '{print $NF}')
CH=$(echo "$GEOM" | grep "Height:" | awk '{print $NF}')
echo "[record] content at $CX,$CY size ${CW}x${CH}"
mkdir -p captures/1m
echo "[record] ffmpeg 125s x11grab..."
ffmpeg -y -loglevel error -f x11grab -video_size ${CW}x${CH} -framerate 30 -i ":0.0+${CX},${CY}" -c:v libx264 -preset veryfast -crf 22 -t 125 captures/1m/live_video_only.mp4
echo "[record] waiting for demo to finish..."
wait $DEMO_PID
DEMO_EXIT=$?
echo "[record] demo exit: $DEMO_EXIT"
echo "[record] syncing audio track from reference score render (demo audio bypasses pipewire via ALSA hw)..."
ffmpeg -y -loglevel error -i captures/1m/live_video_only.mp4 -i music/1m/errorwtf_1m_score_preview.wav -c:v copy -c:a aac -b:a 192k -shortest captures/1m/errorwtf_1m_live_recording.mp4
rm -f captures/1m/live_video_only.mp4
ls -la captures/1m/errorwtf_1m_live_recording.mp4
echo "[record] DONE"
