#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <HTTPClient.h>
#include "DHT.h"

// -------------------- WiFi --------------------
#define WIFI_SSID "Afifa"
#define WIFI_PASSWORD "1234567@A"

// -------------------- Firebase --------------------
#define API_KEY "AIzaSyDGSxcGhjB7a7y0JZbC5kd9SK47vLC2jrw"
#define DATABASE_URL "https://forest-fire-05-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// -------------------- Telegram --------------------
String botToken = "8329668490:AAG0rpMMkfwyf0cn-mrtP6JyognXUw2_T0o";  // 🔹 Your bot token
String chatID   = "-1003113957027";  // 🔹 Your group chat ID (-100xxxxxxxxxx)

// -------------------- Sensors --------------------
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MQ2_PIN 34
#define FLAME_PIN 35

// -------------------- Actuators --------------------
#define LED_PIN 5
#define BUZZER_PIN 18

// -------------------- Location --------------------
String latitude = "13.0123919";
String longitude = "77.7042954";

// -------------------- Timers --------------------
unsigned long lastSend = 0;

// -------------------- URL Encoder --------------------
String urlEncode(String msg) {
  String encodedMsg = "";
  char c;
  for (int i = 0; i < msg.length(); i++) {
    c = msg.charAt(i);
    if (isalnum(c)) {
      encodedMsg += c;
    } else if (c == ' ') {
      encodedMsg += "%20";
    } else {
      encodedMsg += "%" + String(c, HEX);
    }
  }
  return encodedMsg;
}

// -------------------- Send Telegram Notification --------------------
void sendTelegramNotification(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    String locationLink = "https://www.google.com/maps?q=" + latitude + "," + longitude;
    String finalMessage = message + "\n📍 Location: " + locationLink;

    String encodedMessage = urlEncode(finalMessage);
    String url = "https://api.telegram.org/bot" + botToken +
                 "/sendMessage?chat_id=" + chatID +
                 "&text=" + encodedMessage;

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("📨 Telegram sent: " + message);
    } else {
      Serial.println("❌ Failed to send Telegram message");
    }
    http.end();
  }
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(115200);

  // WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected!");

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (!Firebase.signUp(&config, &auth, "", "")) {
    Serial.print("❌ Sign-up failed: ");
    Serial.println(config.signer.signupError.message.c_str());
  } else {
    Serial.println("✅ Signed in anonymously!");
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Sensor and actuator setup
  dht.begin();
  pinMode(MQ2_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println("✅ Setup complete!");
}

// -------------------- Main Loop --------------------
int lastAlertLevel = 0; // 0 = safe, 1 = warning, 2 = fire, 3 = critical
bool highTempLowHumAlertSent = false;

void loop() {
  if (!Firebase.ready()) return;

  if (millis() - lastSend > 5000) {
    lastSend = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int gasValue = analogRead(MQ2_PIN);
    int flameDetected = digitalRead(FLAME_PIN);

    if (isnan(temp) || isnan(hum)) {
      Serial.println("❌ Failed to read from DHT!");
      return;
    }

    String fireStatus = "✅ Environment Safe";
    int currentAlertLevel = 0;
    bool fireAlert = false;

    // -------------------- Condition 1: Gas Level (Low to High) --------------------
    if (gasValue > 800) {
      fireStatus = "🚨 CRITICAL FIRE! Gas=" + String(gasValue);
      currentAlertLevel = 3;
      fireAlert = true;
    } else if (gasValue > 600) {
      fireStatus = "🔥 Fire Detected! Gas=" + String(gasValue);
      currentAlertLevel = 2;
      fireAlert = true;
    } else if (gasValue > 300) {
      fireStatus = "⚠️ Warning: Possible fire. Gas=" + String(gasValue);
      currentAlertLevel = 1;
      fireAlert = true;
    }

    // -------------------- Condition 2: Flame Sensor (LOW = Fire) --------------------
    if (flameDetected==0) {
      fireStatus = "🔥 Flame Sensor Triggered! Fire Detected.";
      currentAlertLevel = max(currentAlertLevel, 3);
      fireAlert = true;
    }

    // -------------------- Condition 3: High Temp + Low Humidity --------------------
    if (temp > 50 && hum < 35) {
      fireStatus = "🌡 High Temp & Low Humidity! Possible Fire due to heat.";
      currentAlertLevel = max(currentAlertLevel, 2);
      fireAlert = true;
    }

    // -------------------- Actuators --------------------
    if (fireAlert) {
      digitalWrite(LED_PIN, HIGH);
      tone(BUZZER_PIN, 1000);
    } else {
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
    }

    // -------------------- Telegram Alerts --------------------
    // Send alert when alert level changes or new temp/hum danger occurs
    if (fireAlert && currentAlertLevel != lastAlertLevel) {
      sendTelegramNotification(
        fireStatus +
        "\n🌡 Temp: " + String(temp) + "°C" +
        "\n💧 Hum: " + String(hum) + "%" +
        "\n💨 Gas: " + String(gasValue)
      );
      lastAlertLevel = currentAlertLevel;
    }
    else if (!fireAlert && lastAlertLevel != 0) {
      sendTelegramNotification("✅ Environment Safe again! Situation under control.");
      lastAlertLevel = 0;
    }

    // -------------------- Firebase Update --------------------
    FirebaseJson json;
    json.set("temperature", temp);
    json.set("humidity", hum);
    json.set("gas", gasValue);
    json.set("flame", flameDetected);
    json.set("fireStatus", fireStatus);
    json.set("fireAlert", fireAlert);

    if (Firebase.RTDB.setJSON(&fbdo, "/sensors", &json)) {
      Serial.println("✅ Data sent to Firebase:");
      Serial.println(json.raw());
    } else {
      Serial.println("❌ Firebase push failed: " + fbdo.errorReason());
    }
  }
}
