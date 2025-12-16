# Wiring

## RC522 ↔ ESP8266 (SPI)

| RC522 Pin | ESP8266 Pin |
|----------|-------------|
| SDA / SS | D2 (GPIO4)  |
| RST      | D1 (GPIO5)  |
| SCK      | D5          |
| MOSI     | D7          |
| MISO     | D6          |
| 3.3V     | 3.3V        |
| GND      | GND         |

> Important: RC522 is **3.3V only**. Do not power it from 5V.

## Outputs
- Buzzer: D0 (GPIO16) (passive buzzer)
- Red LED: D3 (GPIO0)
- Green LED: D4 (GPIO2)
