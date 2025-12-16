#include <SPI.h>
#include <MFRC522.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include "config.h"
#include "secrets.h"

// ================== RFID & IO PINS ===================
// RC522 wiring
#define SS_PIN   4   // GPIO4  = D2 -> SDA/SS
#define RST_PIN  5   // GPIO5  = D1 -> RST

// Outputs
#define BUZZ_PIN 16  // GPIO16 = D0 (passive buzzer)
#define LED_R    0   // GPIO0  = D3 (red)
#define LED_G    2   // GPIO2  = D4 (green)

MFRC522 mfrc522(SS_PIN, RST_PIN);

// ================== LED HELPERS ==================
void ledsOff() { digitalWrite(LED_R, LOW); digitalWrite(LED_G, LOW); }
void greenOn() { digitalWrite(LED_R, LOW); digitalWrite(LED_G, HIGH); }
void redOn()   { digitalWrite(LED_R, HIGH); digitalWrite(LED_G, LOW); }

// ================== BUZZER HELPERS ==================
void beepTone(int freq, int ms) {
  tone(BUZZ_PIN, freq);
  delay(ms);
  noTone(BUZZ_PIN);
}
void beep_ok() {                 // 2 short beeps (higher pitch)
  beepTone(3500, 180);
  delay(80);
  beepTone(3500, 180);
}
void beep_bad() {                // 1 long beep (lower pitch)
  beepTone(1200, 500);
}

// ================== UID HELPERS ==================
// Convert current card UID into string "10 C2 69 59"
String uidToString() {
  String s = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (i > 0) s += " ";
    if (mfrc522.uid.uidByte[i] < 0x10) s += "0";
    s += String(mfrc522.uid.uidByte[i], HEX);
  }
  s.toUpperCase();
  return s;
}

// ================== SIMPLE URL ENCODER ==================
// Minimal encoding (spaces only). Good enough for simple names.
String urlencode(String str) {
  str.replace(" ", "%20");
  return str;
}

// ================== GOOGLE SHEET LOGGING ==================
void sendLogToGoogle(const String& uid,
                     const String& status,
                     const String& name,
                     const String& door_id = DEFAULT_DOOR_ID) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping log.");
    return;
  }

  // HTTPS client (school/demo project): ignore cert validation.
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  HTTPClient https;

  String url = String(SCRIPT_URL) +
               "?action=log" +
               "&uid="    + urlencode(uid) +
               "&name="   + urlencode(name) +
               "&status=" + urlencode(status) +
               "&door_id="+ urlencode(door_id);

  Serial.print("Sending log to: ");
  Serial.println(url);

  if (!https.begin(*client, url)) {
    Serial.println("https.begin() failed");
    return;
  }

  int httpCode = https.GET();
  Serial.print("HTTP code: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String payload = https.getString();
    Serial.print("Server reply: ");
    Serial.println(payload);
  } else {
    Serial.println("Request failed.");
  }

  https.end();
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(800);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  ledsOff();

  SPI.begin();                   // ESP8266 SPI: SCK=D5, MISO=D6, MOSI=D7
  mfrc522.PCD_Init();
  Serial.println("\nReady. Tap a card...");

  // ---- WiFi connection ----
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < WIFI_CONNECT_RETRIES) {
    delay(WIFI_CONNECT_RETRY_DELAY_MS);
    Serial.print(".");
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection FAILED. Logs will not be sent until WiFi is OK.");
  }
}

// ================== LOOP ==================
void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uidStr = uidToString();

  Serial.print("UID: ");
  Serial.println(uidStr);

  // In this version, the backend decides the user name based on UID.
  // The ESP only sends uid + status (+ optional door_id).
  // Local "authorization" decision can be done either:
  //  1) locally (hardcoded list), OR
  //  2) by reading "active" from backend (future improvement).
  //
  // Current behavior: local check using HARD_CODED_UID for demo.
  bool auth = (uidStr == String(HARD_CODED_UID));

  if (auth) {
    Serial.println("ACCESS GRANTED");
    greenOn();
    beep_ok();
    sendLogToGoogle(uidStr, "GRANTED", "");
  } else {
    Serial.println("ACCESS DENIED");
    redOn();
    beep_bad();
    sendLogToGoogle(uidStr, "DENIED", "");
  }

  delay(ACTION_HOLD_MS);
  ledsOff();
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(POST_READ_DELAY_MS);
}
