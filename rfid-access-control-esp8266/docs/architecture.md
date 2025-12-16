# Architecture

## System flow
1. User taps RFID card on RC522
2. ESP8266 reads card UID
3. ESP8266 decides GRANTED / DENIED locally (demo allow-list)
4. ESP8266 sends a HTTPS GET request to Apps Script (`action=log`)
5. Apps Script writes a new row into `logs` sheet and resolves `name` from `users`
6. Dashboard requests `get_logs` and `get_users` and renders stats + table

## Components
- **Embedded**: ESP8266 + RC522 + LED + buzzer
- **Backend**: Google Apps Script (API + dashboard hosting)
- **Database**: Google Sheets (users + logs)
- **Frontend**: HTML/CSS/JS dashboard
