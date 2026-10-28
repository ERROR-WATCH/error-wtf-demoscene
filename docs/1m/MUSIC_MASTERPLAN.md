# MUSIC MASTERPLAN — ERROR.wtf // 1MiB (Phase 4)
Stand: 2026-10-28 · Branch `feature/errorwtf-1m` · Engine: `src/1m/music_engine.c`

## Leitmotiv — „The Error Resolves"

Vier Noten mit Programm: **E4 → Bb4 → G4 → B4** (♪♪ ♩ ♩, 2,0 s)
- E4 (329,63 Hz) → **Bb4 (466,16 Hz): der Tritonus, diabolus in musica = „der Fehler"**
- G4 → B4: die Auflösung (große Terz) — der Fehler findet sein Ende

Das Motiv durchläuft die Dramaturgie in drei Zuständen:
| Zustand | Transformation | Szenen |
|---|---|---|
| pure | rein, präzise | FIRST ORDER, SIGNAL ARCHITECTURE, FALSE PERFECTION |
| fracture | auf 55 % Tempo gedehnt, Halbton-Gespenst-Overlay, Pitch-Drift | COLD BOOT (Boot-Fragment), SYSTEM FAULT |
| resolution | G4/B4-Terz-Paar + B5-Schimmer | REWRITE REALITY, THE SIGNATURE, AFTERIMAGE |

## Groove-Zustände (passend zur FRACTURED-ORDER-Regie)

1. **ORDER** (12–64 s): präziser 4/4-Kick (130→45-Hz-Sweep), Off-Beat-Hats,
   E-Moll-Pentatonik-Bass (E2/G2/A2/B2), ruhiges Pad. Verschiedene
   Energie-Stufen je Szene.
2. **FRACTURE** (64–78 s): gleiches Material, aber Timing-Jitter (±20 ms,
   seeded rnd), Pitch-Drift am Bass (±4 %), dissonanter Nachbarton (F2),
   Kick-Amplitude randomisiert. Der Groove „bricht" hörbar, ohne zu stoppen.
3. **RESOLUTION** (78–112 s): Groove kehrt in B-Region (98 Hz) zurück,
   Motiv in Auflösungs-Färbung, B5-Schimmer.

## Szenen-Mapping (120 s, Regieskizze §5.2)

| t | Szene | Musik |
|---|---|---|
| 0–12 | COLD BOOT | Drone E2+B2, fraktes Motiv-Fragment (leise) |
| 12–28 | FIRST ORDER | Groove ORDER etabliert, Motiv pure @15 s |
| 28–45 | SIGNAL ARCHITECTURE | Groove voll, Bass-Führung, Motiv @29,5/38,5 s |
| 45–64 | FALSE PERFECTION | reduziert (Kick 40 %), langsames Pad, Motiv @47 s |
| 64–78 | SYSTEM FAULT | **Groove FRACTURE**, Motiv fracture @65/71 s |
| 78–100 | REWRITE REALITY | Groove kehrt zurück, Motiv resolution @80/88 s |
| 100–112 | THE SIGNATURE | Motiv dreifach @101/104,5/108 s, E-Dur-Stack |
| 112–120 | AFTERIMAGE | Motiv-Fragment + Drone, 8-s-Fade |

## Hörbare Evidenz ( WAV, pcm_s16le 48 kHz stereo, ffprobe-geprüft)

| Datei | Ausschnitt | Beweist |
|---|---|---|
| music/1m/samples/01_motif_boot.wav | 0–8 s | Motiv fracture (Boot) + Drone |
| music/1m/samples/02_groove_order.wav | 28–36 s | Groove ORDER + Motiv pure |
| music/1m/samples/03_groove_fracture.wav | 64–72 s | Groove FRACTURE (Jitter/Drift hörbar) |
| music/1m/samples/04_break_return.wav | 76–88 s | Fault-Ausklang → Rückkehr → Resolution-Motiv |

## Architektur-Entscheidung

`score_sample(t)` ist die **einzige Quelle** der Partitur: Der Offline-Renderer
(diese WAVs) und der spätere Live-ALSA-Thread der Demo nutzen dieselbe Funktion —
was du hier hörst, ist exakt das, was die fertige Demo spielt (bis auf Live-Normalisierung).

## Status

- Kompositions-Struktur: VERIFIED (4 WAVs, ffprobe-Pass, Peak-Normalisierung ≤ 0,92)
- Künstlerische Feinabstimmung (Sound-Design, Mixing-Tiefe): SELF_REVIEW — Hörtest durch Lino erbeten
- Voll-Preview (120 s) renderbar: `./build/1m/music_engine full`
