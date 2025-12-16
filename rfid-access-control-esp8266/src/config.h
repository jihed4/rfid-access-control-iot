#pragma once

// Default door identifier (useful if you add multiple doors later)
static const char* DEFAULT_DOOR_ID = "door1";

// Demo/local allow-list (hardcoded UID string format: "10 C2 69 59")
// Backend still logs everything + resolves name via users sheet.
static const char* HARD_CODED_UID = "10 C2 69 59";

// WiFi connection behavior
static const int WIFI_CONNECT_RETRIES = 40;         // 40 * 500ms = ~20 seconds
static const int WIFI_CONNECT_RETRY_DELAY_MS = 500;

// Behavior timing
static const int ACTION_HOLD_MS = 1500;
static const int POST_READ_DELAY_MS = 250;
