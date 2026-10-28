# ENVIRONMENT AUDIT — ERROR.wtf Demoscene
Stand: 2026-10-28 · Maschinenlesbar: `reports/environment.json`

## Kernbefunde (korrigieren das Auftragsbriefing)

| Annahme im Briefing | Realität (gemessen) |
|---|---|
| 3 × RTX 4060 Ti 16 GB | **Genau 1 × RTX 4060 Ti 16 GB** (Driver 595.91.07) |
| ~16 GB VRAM frei für Rendering | Nur ~3,5 GB frei zur Audit-Zeit — LLM-Infrastruktur (colibri ~13 GB) teilt die Karte. Demo-Rendering braucht <1 GB → kein Konflikt, aber Benchmarks nur ohne gleichzeitige LLM-Lasten messen. |
| /mnt/DATEN verfügbar | ✓ /dev/sdb1, 1,9 TB, 221 GB frei, schreibbar als error:error |
| gh CLI vorhanden | ✗ Nicht installiert → Repo-Anlage auf GitHub BLOCKED (siehe unten) |

## System
- Kali Linux, Kernel 7.1.5+kali-amd64, x86_64, 16 Kerne, 62 GB RAM (~35 GB verfügbar)
- Session: Wayland (mutter) + Xwayland, User `error` · Agent-Terminals laufen headless (leere DISPLAY-Variablen)
- Runtime-Teststrategie: Builds/Headless-Verifikation via EGL-offscreen als root; sichtbare Fenster-/Audio-Tests im error-Kontext (`sudo -u error` + Sitzungs-Env), weil Audio/GPU-Session dem User `error` gehört

## Grafik-Stack (verifiziert)
- libGL 1.2, EGL 1.5, X11 1.8.13, **GLFW 3.5.1** ✓, libXrandr ✓
- Vulkan-Tools vorhanden (vulkaninfo); SDL2 ✗, OpenAL ✗ (nicht nötig)
- Architektur-Entscheidung (ADR-Preview): 64K/16K/Ultimate = **GLFW + OpenGL 3.3 core**; 4K = eigenes Mini-ELF über **GLX/X11 + ALSA** (Referenztechnik-Analogie, eigenständig implementiert)

## Audio-Stack (verifiziert)
- ALSA 1.2.16 (libasound.so) ✓ · PulseAudio-Compat 17.0 ✓ · User-Server: PipeWire (error-Session)
- Synthese-Runtime: direkter ALSA-PCM-Ausgabe (keine Asset-/Sample-Dateien), samplebasierte Masterclock

## Toolchain (verifiziert)
gcc/g++ 16.2.0 · make 4.4.1 · NASM 3.01 · Python 3.14.8 · ffmpeg 9.0.2 (nur für Captures) · glxinfo + vulkaninfo vorhanden

## Verbote (aus dem Auftrag, bestätigt)
Keine Treiber-/Kernel-/GRUB-/SSH-/Firewall-Änderungen, keine globalen Bibliotheksersetzungen, keine Störung der laufenden LLM-Infrastruktur.
