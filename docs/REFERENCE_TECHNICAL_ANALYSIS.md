# REFERENCE TECHNICAL ANALYSIS — MorphDemo Benchmark
Stand: 2026-10-28 · Referenz-Commit-Quelle: lokaler Klon des Repos (Stand 2026-10-27, `main`)
Original: https://github.com/TheMorpheus407/morphdemo-benchmark · License: MIT © 2026 Cedric Mössner (The Morpheus)
Status: v1 (Phase 1 laufend — vertieft sich mit jedem untersuchten Build)

## 1. Original-Spezifikation (faktenbasiert)
- Drei native Linux-x86-64-ELF-Dateien: exakt 65.536 / 16.384 / 4.096 Byte auf der Platte (keine Obergrenzen; Padding erlaubt, Executable muss gültig bleiben).
- Verboten: Shell-Skripte, Interpreter-/Bytecode-Wrapper, Launcher zu einem anderen Programm, Archive als Binary.
- Erlaubt: Self-Decompression **im** Binary (Loader-Code selbst enthalten, auch memfd/in-memory exec).
- Systembibliotheken des Zielsystems erlaubt (NixOS + RTX 5090 dort; **unsere Zielumgebung ist Kali + RTX 4060 Ti** — siehe ENVIRONMENT_AUDIT).
- Pflichten je Binary: Real-Time-Rendering, kontinuierliche Animation, generiertes Audio, absichtsvolle AV-Synchronisation, ≥3 unterscheidbare Phasen mit Übergängen, Wort **MORPH** → bei uns **ERROR.wtf**, ~60 s (50–70 s), automatisches Ende, Escape.
- Self-contained: keine externen Produktionsassets (Texturen, Meshes, Fonts als Inhalt, Samples, Shader-Dateien, Timeline-Dateien, Netz).
- Erwartete Progression: Emergence → Transformation → Resolution.

## 2. Untersuchte Referenzimplementierungen (Überblick)
Vorhandene Modell-Läufe: Claude-Sonnet-5.5 (alle drei Größen + dev/previews + verification), GPT-6.1-Sol (alle drei + verification-Logs), GPT-6-Astra, Kimi-K3.1-Pro, Mistral-Large-4, Claude-Opus-5.5 (nur Quellcode, KEINE Binaries), Gemini-4-Argon (nur Quellcode).

### 2.1 Claude-Sonnet-5.5 (vollständiger Nachweis-Träger)
Struktur: `src/common`, `src/4k`, `src/16k`, `src/64k`, `tools/` (pack.py, glslmin.py, nasm.sh, payload.ld, genword.py, verify.py, dev/*), `dev/verification.txt`, `dev/previews/*.png`, Binaries exakt 4096/16384/65536.
Technische Architektur-Signale (bis v1): GLSL-basiert, Shader-Minifier (glslmin.py), NASM-Anteile (4k), Linker-Skript (payload.ld), Pack-Tool (pack.py → Self-Compression), Python-Verifier (verify.py), „genword.py“ (Wortmarken-Generator — Konzept analog zu unserem Branding-Ansatz).

### 2.2 GPT-6.1-Sol
Binaries exakt (3 Größen) + `verification/morphdemo-*/`-Logs — aber **mehrere Log-Dateien sind 0 Byte** (audio-capture.log, escape.log, runtime.log im 4k/16k): die dokumentierte „Verifikation“ ist unvollständig beweiskräftig. Unterricht: Evidence-Dateien müssen geprüft werden, nicht nur gezählt.

## 3. Bewertungs-Matrix (Eignung für ERROR.wtf auf 1× RTX 4060 Ti / Kali)

| Verfahren | Referenz-Evidenz | Eignung | Entscheidung (v1) |
|---|---|---|---|
| GLFW/OpenGL 3.3 + ALSA Runtime | Referenz nutzt NixOS-Stack; wir haben GLFW 3.5.1 + ALSA lokal | hoch | 64K/16K/Ultimate-Basis |
| Eigenes Mini-ELF (GLX + ALSA, Linker-Skript) | Sonnet-4k-Pfad (payload.ld, NASM) | hoch für 4K | eigenständig implementieren, keine Code-Übernahme ohne MIT-Notiz |
| Shader-Minifier (glslmin-artig) | vorhanden | hoch | eigenes Minifier-Skript schreiben |
| Self-Compression (packer + in-binary stub) | pack.py + stub (NASM) | mittel-hoch (komplex, Fehlerquelle) | erst native Größe optimieren; Packer nur wenn nötig |
| Sample-basierte Audio-Clock | SPEC verlangt implizit (drift-frei) | pflicht | ALSA Playback-Position/Buffer-Buchhaltung |
| Prozedurale Wortmarke (genword-artig) | vorhanden (Konzept) | pflicht (unsere: ERROR.wtf, 9 Zeichen + Punkt) | eigener Segment-/SDF-Lettering-Ansatz |
| Python-Verify-Harness | verify.py vorhanden | hoch | eigenes Harness + adversariale Negativ-Tests (Auftrag §24) |

## 4. Sicherheits-/Lizenzbefunde
- Lizenz Upstream: MIT © 2026 Cedric Mössner (The Morpheus). Übernähme wir Code, blieben Copyright + MIT-Text in THIRD_PARTY_NOTICES.md.
- Strategie: **eigenständige Implementierung**, Konzept-Lernen erlaubt; keine fremden Binaries ungeprüft ausführen (Auftrag §4.3) — Analyse statisch.
- Unvollständige Verifikationslogs bei Sol: wir übernehmen deren Messungen NICHT als eigene Belege (auch nicht als Referenz-FPS-Basis).

## 5. Konsequenzen für unsere Architektur (ADR-v1)
1. Runtime 64K/16K/Ultimate: C + GLFW + OpenGL 3.3 core + ALSA-PCM, samplebasierte Clock, alle Shader zur Buildzeit embedded.
2. 4K: eigenes ELF, direktes GLX + ALSA, NASM nur wo gewonnen; Ziel-First-Build nativ unter 4 KiB **ohne** Packer, Padding auf exakt 4096.
3. Build: Makefile mit exakt-größenprüfenden Targets + statischen Checks + Runtime-Harness (Headless-EGL-Render für Verifikation, Live-Test im error-Session-Kontext).
4. Wortmarke: prozedurales Segment-Lettering „ERROR.wtf“ (kein Font-Asset — SPEC verbietet Fonts als Production-Content).
