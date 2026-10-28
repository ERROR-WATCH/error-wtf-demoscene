# B-POLISH v3 — FRACTURED ORDER nach unabhängigem Review (Phase-3-Nacharbeit)
Stand: 2026-10-28 · Branch `feature/errorwtf-1m`

## Ausgangslage
Das Vision-Review der Concept Triad bestätigte B als Richtung, forderte aber vier
Verbesserungen. Umsetzung in `src/1m/concept_triad.c` (FS_B v2→v3) mit
Pixel-Statistik-Gegenprobe je Schritt (v1-Referenz archiviert: captures/1m/v1/).

## Review-Punkte → Messergebnis (Luma/Std/Rot%, t=9 bzw. Ordnungs-Frames)

| Review-Forderung | Umsetzung | Beleg v1 → v3 |
|---|---|---|
| (1) Hauptstruktur am Bruch beteiligen | Warp auf allen Ebenen (spine/bays/grid, gestaffelte Stärke) | Std @t=9: 30,8 → **42,6** (+38 %: rot strukturiert statt flach) |
| (2) Hierarchie im Ruhestand | Spines + zwei Bay-Größen + Minor-Grid | Std Ordnung: 31–33 → **36–37**; schwarz 74 → 57 % (mehr sichtbare Struktur) |
| (3) Puls in Ordnungs-Frames | Expanding-Ring + Beat-Flash | Luma Ordnung: 20–23 → **26–28** mit Frame-Varianz |
| (4) Langsamere rote Decay | Fracture-Fenster 8–12,5 s → 7,5–14 s | t=12: v1 27,8/0 % → v3 **69,9/79,5 %** (klar fractured) |

## Regression und Korrektur (ehrlich)
Zwischenversion v2 strukturierte den Bruch so stark, dass die Signatur-Energie
einbrach (t=9: 86,4 % → 6,6 % rot). Korrigiert in v3: Fracture-Energie auf
v1-Niveau zurück (**79,8 % rot @ t=9**), aber strukturgetragen — kein flaches
Panel (Std +38 % gegenüber v1). Beide Ziele gleichzeitig messbar erfüllt.

## Endstand v3 (Pixel-Statistik)
```
t=  0.5  luma 26.4  std 37.2  rot  0.0   (Ordnung)
t=  3.0  luma 25.7  std 36.2  rot  0.0
t=  6.0  luma 28.0  std 37.3  rot  0.0
t=  9.0  luma 69.5  std 42.6  rot 79.8   (FRACTURE-Signatur)
t= 12.0  luma 69.9  std 43.4  rot 79.5   (langsame Decay)
t= 14.5  luma 26.4  std 40.4  rot  0.0   (Rückkehr)
```

## Evidenz
- Kontaktband v3: `captures/1m/triad_B_v3_band.png` (SHA256-Präfix 9a95462b950ea91a, 598816 B)
- Archiv: `captures/1m/v1/` (vorher), `captures/1m/v2/` (Zwischenstand)
- Quelle: `src/1m/concept_triad.c`

## Kennzeichnung
Ästhetische Gesamtwirkung ist **SELF_REVIEW** (zai-Vision-Guthaben aufgebraucht,
429; Aufladen verboten). Statistische Aussagen sind CPU-geprüft. Menschlicher
Blick gewünscht: Band ansehen und Rückmeldung — bevor der 120-s-Bogen poliert wird.
