# ERROR.wtf — DEMO SPECIFICATION v0.1 (WIP)

> ERROR.wtf-Projektspezifikation. Diese Datei ersetzt NICHT die MorphDemo-SPEC.md
> (read-only Referenz, commitfixiert analysiert in docs/REFERENCE_TECHNICAL_ANALYSIS.md).

## Produktionen

| Target | Bytes exakt | Titel | Kernidee |
|---|---|---|---|
| errorwtf-4k | 4096 | SEED | Ein Fehler lernt Rhythmus |
| errorwtf-16k | 16384 | FRACTURE | Die Ordnung bricht ihr eigenes Gesetz |
| errorwtf-64k | 65536 | EMERGENCE | Vom Rauschen zur Struktur zum Finale |
| errorwtf-ultimate | offen | BEYOND THE ERROR | Flaggschiff, 2:30–3:30, eigene Dramaturgie |

## Harte Constraints je Sizecoding-Demo (aus dem Auftrag, verbindlich)

1. Native ELF, Linux x86-64. Keine Shell-/Interpreter-Wrapper, keine Launcher.
2. Exakte Bytezahl auf der Platte (Padding erlaubt, Binary bleibt gültig und vollständig funktional).
3. Native Linux-x86-64-Produktion: kontinuierliche Echtzeit-Grafik (GPU), prozedural synthetisiertes Audio,
   gemeinsame samplebasierte Zeitbasis, bewusste AV-Synchronisation.
4. ≥3 klar unterscheidbare Phasen mit echten Übergängen.
5. ERROR.wtf-Wortmarke prozedural erzeugt (keine Font-Datei, kein Font-Rendering als Production-Content)
   und mindestens einmal klar lesbar sichtbar.
6. Laufzeit 50–70 s (Ziel ~60–64 s), danach sauberes automatisches Ende; ESC = sauberer sofortiger Abbruch.
7. Vollständig self-contained: keine externen Assets (Texturen, Meshes, Fonts, Samples, Musik, Video,
   Shader-Dateien, Timeline-Dateien, Netzwerk). Erlaubt: kompakte eingebettete Daten, OS-/GPU-/Audio-APIs.
8. Self-Decompression nur im Binary selbst (falls genutzt); kein externer Entpacker.
9. Jede der drei Demos läuft unabhängig von den anderen; geteilter Quell-Unterbau in src/common erlaubt.
10. Wortmarke exakt: `ERROR.wtf` (E-R-R-O-R groß, Punkt, wtf klein).

## Zielumgebung (gemessen 28.10.2026)

Kali, 1× RTX 4060 Ti 16 GB (geteilt mit lokaler LLM-Infrastruktur), GLFW 3.5.1 / OpenGL 3.3 / EGL 1.5,
ALSA 1.2.16, gcc 16.2, NASM 3.01, ffmpeg 9.0 (nur Captures). Details: docs/ENVIRONMENT_AUDIT.md.

## Qualitäts-Gates

Siehe docs/AUFTRAG_KERNREGELN.md §Gates und den Master-Auftrag (Edition 3.0) von Lino.
Zustände ausschließlich: PASS / FAIL / NOT_TESTED / BLOCKED mit Evidence-Pfad.
