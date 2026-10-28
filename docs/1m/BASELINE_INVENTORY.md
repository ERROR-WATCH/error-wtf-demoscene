# BASELINE INVENTORY — ERROR.wtf // 1MiB Folgeauftrag
Stand: 2026-10-28 · Branch: `feature/errorwtf-1m` · Baseline-Commit: `fc0fe0f` (Gate 2, main)

## Realer Ausgangszustand (§1.2 Baseline-Gate)

**Befund: Der ursprüngliche Master-Auftrag (vier Produktionen) ist NICHT abgeschlossen.**
Die vier Werke existieren nicht; ein globaler „Best-of-Vergleich" ist daher ausdrücklich
nicht möglich (§1.2). Dokumentiert statt verschleiert:

| Produktion | Artefakt | Größe | Status | Beleg |
|---|---|---|---|---|
| SEED (4 KiB) | — | — | **MISSING** | src/4k/ leer, kein Build |
| FRACTURE (16 KiB) | — | — | **MISSING** | src/16k/ leer, kein Build |
| EMERGENCE (64 KiB) | — | — | **MISSING** | src/64k/ leer, kein Build |
| BEYOND THE ERROR (Ultimate) | — | — | **MISSING** | src/ultimate/ leer, kein Build |
| AV-Laufzeit-Grundlage (Gate 2) | `build/smoke_av` + 5 PPM-Frames + ALSA-Beleg | — | **VERIFIED** (Partiell: kein Kunstwerk, sondern bewiesene Render-/Audio-Pipeline) | Commit `fc0fe0f`, `captures/smoke_frame_*.ppm/png`, EVIDENCE-Log (RTX 4060 Ti, GL 3.3, hw:0,0, 96000/96000 Samples) |

## BASELINE_PARTIAL-Entscheidung (§1.2)

Einziges ausreichend funktionierendes Fundament = die **Gate-2-AV-Laufzeit**
(EGL/GLFW/OpenGL 3.3 + ALSA-Synthese, auf der realen GPU verifiziert). Die
1-MiB-Produktion beginnt daher als **Neukonzeption auf dieser bewiesenen
Architektur** — nicht als Destillation nicht existenter vier Werke.
`BEST_OF_THE_BEST.md` ist bis zum Existieren von Vergleichswerken
**BLOCKED** (keine erfundenen Vergleiche).

## Bestehende verifizierte Fundamente (usable)

| Fundament | Nachweis |
|---|---|
| EGL-headless + GL 3.3 core Render→PPM-Evidenz | Commit `fc0fe0f`, tools/smoke_av.c |
| ALSA prozedurale Synthese auf hw:0,0 (96000/96000 Samples, exit 0) | dito |
| Environment-Audit (1× RTX 4060 Ti 16 GB — NICHT 3×, LLM teilt VRAM) | reports/environment.json |
| Provider-Kostenprüfung 0 € | docs/PROVIDER_COST_AUDIT.md |
| MorphDemo-Referenzanalyse (Spec v1.1, MIT, Architektursignale) | docs/REFERENCE_TECHNICAL_ANALYSIS.md |

## Ausstehend (Hauptauftrag Phase 3–14, unverändert parallel)

Konzept-Triade, 64K/16K/4K-Werke, Ultimate, Release-Gates. Diese Blocks
keinen Teil des 1MiB-Folgeauftrags (§1.2 erlaubt BASELINE_PARTIAL-Start).
