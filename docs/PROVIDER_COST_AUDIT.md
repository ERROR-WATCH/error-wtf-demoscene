# PROVIDER COST AUDIT — ERROR.wtf Demoscene

Stand: 2026-10-28 · Geprüft gegen die AKTUELLE Hermes-Konfiguration (`/root/.hermes/config.yaml`) und Live-Status, nicht gegen alte Notizen.
Keine Secrets in dieser Datei.

## Zweck
Für alle KI-Nutzung im ERROR.wtf-Projekt gilt: **0,00 € zusätzliche Kosten**, verifiziert pro Transportweg.
Fail-closed: COST_UNVERIFIED → nicht verwenden. Nicht erreichbarer kostenneutraler Provider ≠ Erlaubnis für bezahlten Fallback.

## Inventar (Stand 28.10.2026)

| Provider | Modell | Transport | Klasse | Verifiziert | Verwendung im Projekt |
|---|---|---|---|---|---|
| `zai` | glm-5.3-flash | Remote-API, Abo-Quota (Coding-Plan) | SUBSCRIPTION_VERIFIED | ja — bestehendes Flat-Abo, nutzungsabhängig abgerechnet via Quota, kein PAYG | Hauptmodell für Design-/Review-Aufgaben, sparsam |
| `openai-codex` | gpt-6.1-sol | Remote, ChatGPT-Abo-Backend | SUBSCRIPTION_VERIFIED | ja — Abo-Backend (kein API-Key-PAYG-Weg) | Code-Review / zweite Meinung |
| `colibri-local` | qwen3.6-colibri | Lokal, Gateway :8102 (GPU) | LOCAL_VERIFIED | ja — 28.10. live getestet („bereit“) | Standard-Dev-Iterationen, Subagenten |
| `strata-local-qwen` | qwen3.8-flash-next-iq3_s | Lokal, Gateway :8112 (GPU) | LOCAL_VERIFIED | ja — 28.10. live getestet | Standard-Dev-Iterationen |
| `ollama-local` | gemma4:latest | Lokal, :11434 | LOCAL_VERIFIED | ja — 28.10. live getestet + 8195-Token-Kontexttest | leichte Aufgaben |
| `ollama-cloud` | deepseek-v4-pro:0813 | Remote free tier, ollama.com | FREE_TIER — **aktuell BROKEN** (Pro-Limit-429 bis Monatswechsel, 28.10. im Gateway-Log belegt) | nein (aktuell) | nicht verwenden bis Reset; kein Fallback-Zwang |
| `nous` | step-3.7-flash:free | Remote free tier, inference-api.nousresearch | FREE_TIER (Rate-Limits) | COST_UNVERIFIED bei Quotenänderungen | nur mit Vorsicht, nicht kritisch |
| `gemini` | gemini-3.8-flash (FB8) | Remote | **BROKEN** (limit:0 free, nur via Paid-Key) | BLOCKED | verboten ohne Lino-Freigabe |
| Claude/Vertex | — | — | VERBOTEN laut Auftrag | — | niemals |

## Fallback-Ketten-Audit (bestehende Hermes-Kette)
Aktive Kette: zai → openai-codex → ollama-cloud (BROKEN/429) → colibri-local → strata-local-qwen → nous → ollama-local → gemini(BROKEN).
**Befund:** Kein PAYG-/credit-basierter Provider in der Kette. Alle Mitglieder sind Abo-abgedeckt, lokal oder free-tier → ein automatischer Fallback löst **keine Zusatzkosten** aus. Grenzfälle dokumentiert: ollama-cloud/gemini sind BROKEN (nicht bezahlt) — sie scheitern, sie kosten nichts.

## Projekt-Lock (fail closed)
- Hermes hat keine per-Projekt-Provider-Isolation → **globale Konfiguration wurde NICHT verändert** (Auftrag §2.3).
- Umsetzung: Projekt-Regel — Dev-Iterationen bevorzugt über lokale Gateways (`colibri-local`, `strata-local-qwen`, `ollama-local`); Remote nur für abgegrenzte Design-/Review-Schritte mit `zai`/`codex`.
- Verifizierte Aufrufwege im Projekt: `hermes -z --provider <allowlist> -m <model>` (28.10. je Provider live getestet).
- „Keine Claude“-Regel: nicht konfiguriert ✓.
- Bekannte Grenze: Ein hartes „Blockiere jeden anderen Provider“-Enforcer existiert in Hermes nicht; dokumentiert als technische Grenze (Auftrag erlaubt Doku statt riskanter globaler Änderung).

## Bisherige Kosten des Projekts
- KI-Nutzung: 0,00 € zusätzlich (lokale Modelle + bestehende Abos).
- Cloud/GPU-Miete, Bild-/Audio-Dienste, CI-Runner: nicht verwendet.
- Status: VERIFIED_ZERO (Stand 28.10.2026).
