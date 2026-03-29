#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "esp_sleep.h"

#define DHTPIN 4
#define DHTTYPE DHT22
#define BUZZER_PIN 25

const char* WIFI_SSID = "Hello";
const char* WIFI_PASSWORD = "random12";

const char* API_URL = "https://esp32-ml-api-1.onrender.com/predict";

float latitude  = 28.6139;
float longitude = 77.2090;

// ✅ 1 minute sleep
#define SLEEP_TIME_US (60ULL * 100000ULL)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n==== FIRE PREDICTION SYSTEM ====");

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  dht.begin();

  connectWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    bool success = sendData();

    if (!success) {
      Serial.println("[RETRY] Retrying in 3 seconds...");
      delay(3000);
      sendData();
    }
  } else {
    Serial.println("[ERROR] WiFi not connected, skipping API call");
  }

  Serial.println("[POWER] Going to sleep...");

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  esp_sleep_enable_timer_wakeup(SLEEP_TIME_US);
  esp_deep_sleep_start();
}

void loop() {}

void connectWiFi() {
  Serial.print("[WIFI] Connecting");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 30) {
    Serial.print(".");
    delay(500);
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WIFI] Connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[ERROR] WiFi Failed");
  }
}

bool sendData() {
  float t = dht.readTemperature()/10;
  float h = dht.readHumidity()/100;

  // ✅ Fallback values if sensor fails
  if (isnan(t)) {
    Serial.println("[WARNING] Temperature read failed, using default 30");
    t = 30.0;
  }

  if (isnan(h)) {
    Serial.println("[WARNING] Humidity read failed, using default 30");
    h = 30.0;
  }

  Serial.println("[SENSOR]");
  Serial.println("Temp: " + String(t));
  Serial.println("Humidity: " + String(h));

  WiFiClientSecure client;
  client.setInsecure();  // ⚠️ Not secure, but OK for testing

  HTTPClient https;
  https.setTimeout(10000);

  if (!https.begin(client, API_URL)) {
    Serial.println("[ERROR] HTTPS begin failed");
    return false;
  }

  https.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;
  doc["lat"] = latitude;
  doc["lon"] = longitude;

  String body;
  serializeJson(doc, body);

  Serial.println("[POST] " + body);

  int code = https.POST(body);
  Serial.println("[HTTP CODE] " + String(code));

  if (code <= 0) {
    Serial.println("[ERROR] POST failed");
    https.end();
    return false;
  }

  String res = https.getString();
  Serial.println("[RESPONSE] " + res);

  StaticJsonDocument<200> r;
  if (deserializeJson(r, res)) {
    Serial.println("[ERROR] JSON parse failed");
    https.end();
    return false;
  }

  if (!r.containsKey("prediction")) {
    Serial.println("[ERROR] Invalid response (no prediction)");
    https.end();
    return false;
  }

  int prediction = r["prediction"];

  if (prediction == 1) {
    Serial.println("[ALERT] FIRE DETECTED!");

    // 🔊 Buzzer alert
    for (int i = 0; i < 5; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }
  } else {
    Serial.println("[SAFE] No fire risk");
  }

  https.end();
  return true;
}
