#!/bin/bash
# ERROR.wtf // 1MiB — automated release gate. Self-measuring: every claim in
# this gate comes from a live measurement, never from a stored claim.
# Exit 0 only if ALL checks pass. Writes reports/1m/release_gate.json.
set -u
cd "$(dirname "$0")/../.."
ART=errorwtf-1m
OUT=reports/1m/release_gate.json
PASS=0; FAIL=0
declare -a RESULTS=()
check() { # name expected actual
    local name="$1" expected="$2" actual="$3"
    if [ "$expected" == "$actual" ]; then PASS=$((PASS+1)); RESULTS+=("\"$name\": \"PASS ($actual)\"");
    else FAIL=$((FAIL+1)); RESULTS+=("\"$name\": \"FAIL (expected $expected, got $actual)\""); fi
}
check_ge() { # name min actual
    local name="$1" minimum="$2" actual="$3"
    if awk -v a="$actual" -v m="$minimum" 'BEGIN{exit !(a>=m)}'; then PASS=$((PASS+1)); RESULTS+=("\"$name\": \"PASS ($actual >= $minimum)\"");
    else FAIL=$((FAIL+1)); RESULTS+=("\"$name\": \"FAIL ($actual < $minimum)\""); fi
}

echo "=== ERROR.wtf // 1MiB RELEASE GATE ==="

# 1 size exactly 1 MiB
SZ=$(stat -c %s "$ART" 2>/dev/null || echo 0)
check "size_exact_1048576" 1048576 "$SZ"

# 2 valid ELF x86-64
if file "$ART" | grep -q "ELF 64-bit LSB.*x86-64"; then check "elf_x86_64" PASS PASS; else check "elf_x86_64" PASS FAIL; fi

# 3 sha256 recorded
SHA=$(sha256sum "$ART" | cut -c1-16)
RESULTS+=("\"artifact_sha256_prefix\": \"$SHA\"")

# 4 full 120s headless run + captures (frames land in captures/1m/cap_*.ppm)
ERRORWTF_HEADLESS=1 ERRORWTF_CAPTURE_ALL=1 timeout 150 stdbuf -oL "./$ART" > /tmp/gate_run.log 2>&1
RUN_EXIT=$?
check "run_exit_clean" 0 "$RUN_EXIT"
AUDIO=$(grep -oP "audio complete \(\K[0-9]+" /tmp/gate_run.log | head -1)
check "audio_samples_5760000" 5760000 "${AUDIO:-0}"
SCENES=$(grep -c "EVIDENCE: scene=" /tmp/gate_run.log)
check_ge "scenes_hit" 8 "$SCENES"

# 5 pixel thresholds on fresh captures
stat_frame() { # ppm -> "luma std red white red_dark"
    /usr/bin/python3 - "$1" <<'EOF'
import sys
with open(sys.argv[1],'rb') as f:
    f.readline(); wh=f.readline().split(); w,h=int(wh[0]),int(wh[1]); f.readline(); d=f.read()
n=w*h; rs=d[0::3]; gs=d[1::3]; bs=d[2::3]
l=[0.299*r+0.587*g+0.114*b for r,g,b in zip(rs,gs,bs)]
m=sum(l)/n; s=(sum((x-m)**2 for x in l)/n)**0.5
r=sum(1 for a,b in zip(rs,gs) if a>90 and b<70)/n*100
w_=sum(1 for a,b,c in zip(rs,gs,bs) if a>200 and b>200 and c>200)/n*100
rd=sum(1 for a,b in zip(rs,gs) if a>45 and b<a*0.55)/n*100
print(f"{m:.1f} {s:.1f} {r:.1f} {w_:.2f} {rd:.1f}")
EOF
}
F70=$(stat_frame captures/1m/cap_070.ppm)
F107=$(stat_frame captures/1m/cap_107.ppm)
F116=$(stat_frame captures/1m/cap_116.ppm)
ROT70=$(echo "$F70" | awk '{print $3}')
RED70DARK=$(echo "$F70" | awk '{print $5}')
# fracture signature = bright red OR darkened crimson field (v6 polish darkens the
# warp deliberately; metric counts both bright red pixels and darkened crimson).
FRAC70=$(awk -v b="$ROT70" -v d="$RED70DARK" 'BEGIN{print b+d}')
check_ge "fracture_red_field_pct_at_t70" 50 "$FRAC70"
WEIS107=$(echo "$F107" | awk '{print $4}')
check_ge "wordmark_white_pct_at_t107" 0.70 "$WEIS107"
LUMA116=$(echo "$F116" | awk '{print $1}')
# (fade check: luma116 must be BELOW 15 -> invert compare)
if awk -v a="$LUMA116" 'BEGIN{exit !(a<15)}'; then PASS=$((PASS+1)); RESULTS+=("\"afterimage_fade_luma<15\": \"PASS ($LUMA116)\"");
else FAIL=$((FAIL+1)); RESULTS+=("\"afterimage_fade_luma<15\": \"FAIL ($LUMA116 not < 15)\""); fi

# 6 score preview intact
DUR=$(ffprobe -v error -show_entries format=duration -of csv=p=0 music/1m/errorwtf_1m_score_preview.wav 2>/dev/null || echo 0)
if awk -v a="$DUR" 'BEGIN{exit !(a>119.5 && a<120.5)}'; then check "score_preview_120s" PASS PASS; else check "score_preview_120s" PASS "FAIL($DUR)"; fi

# write report
{ echo "{"
  echo "  \"schema\": \"errorwtf.1m.release-gate.v1\","
  echo "  \"date\": \"$(date -Is)\","
  echo "  \"artifact_sha256_prefix\": \"$SHA\","
  echo "  \"passed\": $PASS, \"failed\": $FAIL, \"gate\": \"$([ $FAIL -eq 0 ] && echo PASS || echo FAIL)\","
  for i in "${!RESULTS[@]}"; do
      LAST=","; [ $((i+1)) -eq ${#RESULTS[@]} ] && LAST=""
      echo "  ${RESULTS[$i]}$LAST"
  done
  echo "}"; } > "$OUT"
echo "=== GATE: $([ $FAIL -eq 0 ] && echo PASS || echo FAIL) (passed=$PASS failed=$FAIL) ==="
cat "$OUT"
[ $FAIL -eq 0 ]
