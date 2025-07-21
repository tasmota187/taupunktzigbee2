# 💧 Keller-Taupunktregelung mit ESP32-C6 + Zigbee

Dieses Projekt misst den Taupunkt in einem Keller mithilfe von **zwei BME280-Sensoren**, zeigt den Wert auf einem **OLED-Display** an und **schaltet eine Zigbee-Steckdose für den Lüfter**, wenn der Taupunkt zu hoch ist.

✅ Hardware:
- ESP32-C6-WROOM-1-N8 (RISC-V, 160 MHz, Zigbee, Matter, WiFi 6)
- 2× GY-BME280 (I²C, Adressen: `0x76` und `0x77`)
- 0.49" OLED-Display (SSD1315, 64×32, I²C, Adresse: `0x3C`)
- Zigbee-kompatible Steckdose (z. B. Tuya oder Lidl Silvercrest)
- Lüfter an Steckdose angeschlossen

---

## 🛠️ Installation

### 📦 Voraussetzungen

- **ESP-IDF v5.1 oder neuer** installiert  
  [ESP-IDF Setup Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/get-started/)

```bash
git clone https://github.com/dein-benutzername/keller-taupunktregelung.git
cd keller-taupunktregelung
idf.py set-target esp32c6
⚙️ Konfiguration
bash
Kopieren
Bearbeiten
idf.py menuconfig
Optional: Aktiviere Logging und Zigbee-Debug-Optionen unter
Component config > Zigbee > Enable debug mode
Setze I2C SDA GPIO = 10, SCL GPIO = 11

🔌 Verdrahtung
Bauteil	ESP32-C6 Pin
BME280 (1)	I2C SDA = GPIO10, SCL = GPIO11, Adresse = 0x76
BME280 (2)	I2C SDA = GPIO10, SCL = GPIO11, Adresse = 0x77
OLED Display	I2C SDA = GPIO10, SCL = GPIO11, Adresse = 0x3C
VCC/GND	3.3V / GND

🧪 Flashen und Monitor starten
bash
idf.py build flash monitor

🔄 Zigbee Pairing & Automatik
ESP32-C6 startet automatisch im Zigbee-Coordinator-Modus

Steckdose in den Pairing-Modus versetzen (z. B. 5 Sekunden Taste gedrückt halten)
In der Konsole erscheint:

Network steering finished
Zigbee device joined: [Adresse]
Die Adresse wird gespeichert – ab jetzt schaltet das System die Steckdose automatisch bei kritischem Taupunkt (z. B. > 12 °C)

📊 Display
Auf dem OLED wird der aktuelle Taupunkt angezeigt:
Tau: 12.4 C

⚙️ Funktionsweise
Alle 5 Sekunden werden beide Sensoren ausgelesen
Der niedrigere Taupunktwert wird gewählt
Wird der Schwellwert überschritten, wird per Zigbee ein ON-Befehl an die Steckdose gesendet
Wenn Taupunkt wieder sinkt → OFF

🧠 TODO (für Entwickler)
 BME280 I²C-Implementierung in sensors.c vervollständigen
OLED-Textanzeige (display_update()) anpassen
Speicherung der Zigbee-Geräteadresse in NVS
Hysterese für An/Aus
OTA-Update oder Webinterface für Konfiguration
