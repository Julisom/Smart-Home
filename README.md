# Smart-Home Projektarbeit

**von Julius Sommermann und Noah Habermann**

## Projektbeschreibung
Ein Smart-Home-System das über Handgesten gesteuert wird. Sensoren und Aktoren werden über einen ESP32 in Wokwi simuliert, die Kommunikation läuft über MQTT. Ein Python-Dashboard visualisiert alle Zustände in Echtzeit.

---

## Systemarchitektur
```
MediaPipe (Noah)  →  MQTT-Broker  →  ESP32 in Wokwi (Julius)
                           ↕
                      Dashboard (Python)
```

---

## Komponenten

| Komponente | Technologie | Zuständig |
|---|---|---|
| Gestensteuerung | Python, MediaPipe | Noah |
| ESP32 Simulation | Wokwi, Arduino C++ | Julius |
| Dashboard | Python, Tkinter, paho-mqtt | Gemeinsam |

---

## Voraussetzungen
- Python 3.x
- Installierte Bibliotheken: `pip install -r requirements.txt`
- Wokwi-Projekt im Browser geöffnet
- Internetverbindung (für MQTT-Broker)

---
## Mediapipe

Eine von Google enteickelte KI zur Bilderkennung. Es benötingt zugriff auf eine Kamera und kann zwei Gesten erkennen.
| 👍 Daumen hoch | Licht AN |
| 👎 Daumen runter | Licht AUS |

     Vor ausführung des Programms folgende Befehle ins Terminal eingeben:

          1. `uv sync` – installiert alle Abhängigkeiten
          2. Modell herunterladen:
          `curl -o hand_landmarker.task https://storage.googleapis.com/mediapipe-models/hand_landmarker/hand_landmarker/float16/1/hand_landmarker.task`
          3. `uv run main.py` - startet das Programm

---

## MQTT Topics

| Topic | Beschreibung | Payload |
|---|---|---|
| `home223/livingroom/light` | Licht steuern | `ON` / `OFF` |
| `home223/livingroom/blinds` | Rollo steuern | `OPEN` / `CLOSE` |
| `home223/kitchen/temperature` | Temperatur (DHT22) | z.B. `22.5` |
| `home223/bedroom/pir` | Bewegungsmelder | `DETECTED` / `CLEAR` |
| `home223/button` | Taster am ESP32 | `PRESSED` |
| `home223/gesture` | Erkannte Geste | `thumbs_up` / `thumbs_down` |

**Broker:** `test.mosquitto.org` Port `1883`

---

## ESP32 Pinbelegung

| Pin | Bauteil |
|---|---|
| D2 | LED (Wohnzimmerlicht) |
| D4 | Taster |
| D15 | DHT22 (Temperatur/Feuchte) |
| D13 | PIR-Sensor (Bewegungsmelder) |
| D12 | Servo (Rollo) |

---

## Dashboard starten
```bash
cd Smart-Home
pip install -r requirements.txt
python main.py
```

---

## Gesten

| Geste | Aktion |
|---|---|
| 👍 Daumen hoch | Licht AN |
| 👎 Daumen runter | Licht AUS |

---

## Arbeitsteilung

| Aufgabe | Person |
|---|---|
| Gestensteuerung & MQTT-Publisher | Noah |
| ESP32 in Wokwi, Sensoren & Aktoren | Julius |
| Dashboard, Tests, Dokumentation | Gemeinsam |

---

## Abschlusspräsentation
01.07.2026
