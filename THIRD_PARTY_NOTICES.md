# THIRD PARTY NOTICES

## MorphDemo benchmark (technical reference)
- Source: https://github.com/TheMorpheus407/morphdemo-benchmark
- License: MIT, Copyright (c) 2026 Cedric Mössner (The Morpheus)
- Usage: Engineering constraints (exact sizes 4096/16384/65536 bytes, native
  self-contained ELF, runtime/audio/graphics requirements) were studied as a
  reference. No upstream source code is included in this repository as of
  2026-10-28. If code is ever adopted, its license header and copyright will
  be reproduced here and in the affected files.

## Runtime libraries used by built demos (system libraries, not bundled)
- GLFW 3 (windowing) — zlib/libpng license
- OpenGL / EGL via NVIDIA driver + libglvnd — per vendor
- ALSA (libasound) — LGPL 2.1 (system library, used via dynamic linking)
- X11 via Xwayland — MIT-style (X.Org)

## Tools
- ffmpeg 9.0 (capture/transcode of our own renders only)
- gcc 16.2, GNU make, NASM 3.01, Python 3.14 (build/verification tooling)
