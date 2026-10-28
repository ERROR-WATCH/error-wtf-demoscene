# Master-Auftrag ERROR.wtf — Edition 3.0 (Kernregeln, getreue Kurzfassung)

> Die vollständige Edition 3.0 (Teil I–IV: Produktionsauftrag, Art-Direction-Bible,
> Musik-Bibel, Production-Engineering/Evidence-Vertrag) stammt wortgleich vom Künstler
> (Chat, 28.10.2026) und ist die verbindliche Grundlage. Diese Datei hält die
> verbindlichen Kernregeln maschinennah fest; bei Konflikt gilt das Volltext-Briefing.

## Prioritäten (bei Zielkonflikt)
1. 0,00 € Zusatzkosten + kein riskanter Systemeingriff
2. Tatsächlich funktionierende, unabhängig geprüfte Produktionen
3. ERROR.wtf-Identität, eigene Musik, künstlerische Qualität
4. AV-Synchronisation, Hochwertige Musik
5. Stabilität, Reproduzierbarkeit
6. Exakte Sizecoding-Größen (4096/16384/65536 Byte — exakt, kein Mogeln)
7. Zusatz-Features

## Cost-Firewall (fail closed)
- Nur verifizierte Zugänge: lokale Gateways (colibri/strata/ollama-local) + Abo (zai, openai-codex).
- COST_UNVERIFIED → nicht aufrufen. Keine PAYG-Ausweichwege, kein Claude/Vertex, kein Bezahl-CI.
- Bestehende Fallback-Kette geprüft: kein PAYG-Mitglied → Fallbacks kostenneutral. Details: docs/PROVIDER_COST_AUDIT.md.

## System-Schutz
Keine Treiber-/Kernel-/GRUB-/PAM-/SSH-/Firewall-/VPN-Änderungen; keine globalen Bibliotheksersetzungen;
keine Störung laufender LLM-/Forschungs-Projekte; fremde Binaries nur statisch/isoliert.

## GitHub
Repo: github.com/error-wtf/error-wtf-demoscene (PUBLIC), nur nach Identitäts-/Rechte-Prüfung.
Keine Secrets/Private-Daten/Force-Pushes. Releases nur nach bestandenen Gates; v1.0.0 erst bei allen
Pflicht-Artefakten. Vorabstände klar als alpha/preview labeln.

## Gates (je Sizecoding-Demo, Zustände nur PASS/FAIL/NOT_TESTED/BLOCKED mit Evidence)
G-SIZE exakte Bytes · G-ELF native x86_64 · G-RUN 50–70s + Exit 0 · G-AUDIO echte hörbare Synthese ·
G-VIDEO echte Animation · G-CUES ≥3 Phasen · G-SYNC AV gekoppelt (Event-IDs/Clock) ·
G-TEXT ERROR.wtf lesbar (1080p + 320px-Thumbnail-Test) · G-ASSETS self-contained · G-NET netzfrei ·
G-EXIT ESC sauber · G-REBUILD rebuild + SHA · G-LICENSE · G-SECRETS · G-ART (Self-Review mit Befunden).

## Ehrlichkeits-Regeln
- Ein compile-PASS ist kein Grafik-PASS; exit 0 ist kein Qualitäts-PASS; Größe ist nicht Vollständigkeit.
- Captures/Audio nur aus dem finalen Build-Hash (verification/<prod>/<sha256>/).
- Test-Harness mit Negativ-Tests gegenprüfen (Korruptes Artefakt muss FAIL erzeugen).
- Keine erfundenen FPS/LUFS/Scores; ESTIMATE labeln; NOT_TESTED ehrlich.
- Q-FUNCTION ≠ Q-ENGINEERING ≠ Q-ART; getrennt berichten.
- Reward-Hacking-Gegenmaßnahmen: Hash-Bindung Captures↔Binary, Blackbox-Läufe aus leerem cwd,
  adversariale Eingaben, getrennte Review-Rollen (als Selbstreview deklariert, keine Schein-Unabhängigkeit).

## Kreativ-Regeln (Kurzfassung)
- Marke exakt: ERROR.wtf · Motiv: BEYOND THE ERROR · FEHLER→STÖRUNG→MUSTER→STRUKTUR→BRUCH→NEUORDNUNG→ERROR.wtf
- Palette: Near Black #050508, Blue Black #0D101B, Signal Red #ED173B, Deep Crimson #650A25,
  Electric Cyan #28E4F0, Cold White #E8F4FF, Pale Amber #F7B851 (Gestaltungsziele, keine Konstanten)
- Wortmarke prozedural aus der Szene (kein Font-Asset, keine Titelkarte über fremder Animation)
- Musik: düsterer elektronischer Groove (Breakbeat/IDM/Industrial/Dark Synth/Dubstep-Anklänge),
  eigenständige Motive, prozedurale Synthese, keine fremden Songs/Samples/APIs
- Sample-basierte Masterclock; Event-IDs konsistent in Audio/Visuals/Test-Harness
- 2–4 Motive pro kleiner Demo konsequent; Quiet-Phase vor dem Bruch; Marke hält lesbar
- Glitch-Klassen: Fault / Propagation / Collapse / Resynthesis — dramaturgisch gesetzt, nicht Zufall
- Mathematik als Form, nicht als unbelegte Physik-Behauptung; SSZ-DNA nur als künstlerisches Motiv
- 3 kreative Richtungen (A Minimal / B Cathedral / C Recursive) als echte Mini-Prototypen testen
- Signature Moments ≥5 entwerfen, 2–3 prototypisieren; kein Moment nur auf Einzelframe
- Qualitätsplateau: 5 Revisionen ohne messbaren Gewinn → Konzept-/Prioritätsfrage statt Drehen
