# RFID Access Control System (ESP8266 + RC522 + Google Sheets Dashboard)

IoT-based RFID access control system using an **ESP8266** + **RC522 RFID reader** with:
- Real-time cloud logging to **Google Sheets** (via **Google Apps Script**)
- A clean **web dashboard** (HTML/CSS/JS) for monitoring events and statistics

## Features
- RFID UID scan (RC522 over SPI)
- Local decision (GRANTED/DENIED) with LED + buzzer feedback
- Cloud event logging (`timestamp | uid | name | status | door_id`)
- Users database in Sheets (`uid | name | role | active`) with automatic name lookup
- Dashboard filters (status, door) + stats (total, granted, denied, unique users)

## Hardware
- ESP8266 (NodeMCU / Wemos D1 mini)
- RC522 RFID reader (SPI, **3.3V only**)
- Green LED (GRANTED), Red LED (DENIED)
- Passive buzzer

### Wiring (ESP8266 ↔ RC522)
| RC522 | ESP8266 |
|------|---------|
| SDA  | D2 (GPIO4) |
| RST  | D1 (GPIO5) |
| SCK  | D5 |
| MOSI | D7 |
| MISO | D6 |
| 3.3V | 3.3V |
| GND  | GND |

Extra outputs used by firmware:
- Buzzer: D0 (GPIO16)
- Red LED: D3 (GPIO0)
- Green LED: D4 (GPIO2)

See `docs/wiring.md`.

## Project Structure
- `src/` Arduino firmware (ESP8266)
- `apps-script/` Google Apps Script backend
- `dashboard/` standalone dashboard copy (optional)
- `docs/` wiring, architecture, API docs + screenshots

## Setup
### 1) Firmware (Arduino IDE)
1. Install ESP8266 board support + required libraries:
   - MFRC522
   - ESP8266WiFi
   - ESP8266HTTPClient
2. Create `src/secrets.h` from the template:
   - Copy `src/secrets.example.h` → `src/secrets.h`
   - Fill in WiFi + Apps Script URL
3. Upload `src/main.ino` to your ESP8266.

### 2) Google Sheets
Create a Google Spreadsheet with two sheets:

**Sheet name: `users`**
Columns:
- `uid | name | role | active`

**Sheet name: `logs`**
Columns:
- `timestamp | uid | name | status | door_id`

### 3) Apps Script Deployment
1. Open Google Apps Script linked to the spreadsheet.
2. Paste `apps-script/Code.gs` into your project.
3. Create an HTML file named **dashboard** and paste `apps-script/dashboard.html` inside it.
4. Deploy as Web App:
   - Execute as: **Me**
   - Who has access: **Anyone with the link**
5. Copy the `/exec` URL into `src/secrets.h`.

### 4) Dashboard
Open your deployed Apps Script URL (no parameters). It serves the dashboard by default.

## Screenshots
See `docs/screenshots/`.

## License
MIT (see `LICENSE`).
