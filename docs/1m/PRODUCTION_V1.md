# PRODUCTION v1 — Verifikationslauf (Phase 5)
Stand: 2026-10-28 · Branch `feature/errorwtf-1m` · Artefakt: `errorwtf-1m`

## Artefakt
- **Exakt 1.048.576 Bytes** (Payload 31.752 + Padding 1.021.856, getrennt dokumentiert,
  append-only, Budget-Guard aktiv)
- SHA-256-Präfix: `a00d08efae79eec0`
- Quelle: `src/1m/production.c` (eine TU: Score + Szenen-Shader + Wortmarken-Reveal)

## Verifikationslauf (headless, 120 s, exit=clean)
| Kriterium | Ergebnis |
|---|---|
| Laufzeit | 120 s vollständig, exit 0 |
| Audio | 5.760.000 / 5.760.000 Samples (score_sample, hw:0,0) |
| Szenen | alle 8 zu den Regie-Cues (12/28/45/64/78/100/112) |
| Evidenz-Frames | 8 (t=6/20/36/55/70/85/107/116) |
| Größe nach Lauf | unverändert 1.048.576 |

## Pixel-Statistik (Dramaturgie nachweisbar)
| t | Szene | Luma/Std/Rot%/Weiß% | Beleg |
|---|---|---|---|
| 6 | COLD BOOT | 32,6 / 28,8 / 9,1 | roter Anstieg |
| 20 | FIRST ORDER | 18,5 / 26,8 / 0 | Aufbau |
| 36+55 | ARCHITECTURE / FALSE PERFECTION | 25-27 / 37 | Hierarchie; Weiß 0,8-0,9 % (Spines) |
| 70 | SYSTEM FAULT | **71,2 / 43,8 / 78,9** | Fracture-Signatur |
| 85 | REWRITE REALITY | 29,0 / 43,8 / 0 | Rückkehr, hierarchisch |
| 107 | THE SIGNATURE | 28,1 / 40,9 / **Weiß 1,41 %** | **Wortmarke ERROR.WTF sichtbar** (Baseline 0,8 %) |
| 116 | AFTERIMAGE | 6,1 / 9,2 / 0 | Fade |

## Wortmarken-Reveal (neuer Höhepunkt)
„ERROR.WTF" — 9 prozedurale 5×7-Glyphen (kein Font-File), Reveal über 100–103 s:
Aufbau von links mit Fracture-Glitch (der erste Versuch zerbricht), ab 103,5 s sauber
stehend; statistisch belegt durch Weißpixel-Anstieg 0,79→1,41 % bei t=107.

## Performance (nachgetragen 28.10., Abend)

| Messung | Ergebnis | Modus |
|---|---|---|
| Live-Desktop-FPS | **56,7 fps** (6800 frames / 120 s, vsync-gebunden) | GLFW-Fenster auf der echten GNOME/Wayland-Session (error-User), exit=clean |
| Headless-Durchsatz | **170.898 fps** (21,5 Mio Frames / 126 s wall) | ERRORWTF_BENCH=1, kein VSync, GPU-Kapazität |

Performance ist damit keine Grenze: Der Offline-30fps-Pass nutzt <0,1 % der
messbaren Kapazität; live läuft die Demo vsync-stabil ruckelfrei.

## Known-Limits (ehrlich)
- Ästhetische Gesamtwirkung: SELF_REVIEW (zai-Vision-Guthaben leer, 429).
  Menschlicher Blick: `captures/1m/production_v1_band.png`.
- Headless-Captures = eine Evidenz-Ebene; Live-Fenstermodus unverändert verfügbar
  (GLFW-Pfad, ESC beendet).
- Live-Audio läuft über dieselbe score_sample()-Funktion wie die freigegebene WAV-Preview.
