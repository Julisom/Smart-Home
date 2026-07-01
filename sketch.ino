#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP32Servo.h>

// WiFi (Wokwi-intern)
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

// MQTT-Broker
const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;
const char* CLIENT_ID   = "esp32-julius-223";

// Pins
#define LED_PIN   2
#define BTN_PIN   4
#define DHT_PIN   15
#define PIR_PIN   13
#define SERVO_PIN 12

// Topics
#define TOPIC_LIGHT       "home223/livingroom/light"
#define TOPIC_BLINDS      "home223/livingroom/blinds"
#define TOPIC_TEMPERATURE "home223/kitchen/temperature"
#define TOPIC_PIR         "home223/bedroom/pir"
#define TOPIC_BUTTON      "home223/button"
#define TOPIC_GESTURE     "home223/gesture"

DHT dht(DHT_PIN, DHT22);
Servo blindsServo;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

unsigned long lastSensorRead = 0;
unsigned long lastReconnect  = 0;
bool lastBtnState = HIGH;
bool lastPirState = LOW;
bool lightOn = false;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  String t = String(topic);

  if (t == TOPIC_LIGHT || t == TOPIC_GESTURE) {
    if (msg == "ON" || msg == "licht_an" || msg == "thumbs_up") {
      lightOn = true;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("[LED] AN");
    } else if (msg == "OFF" || msg == "licht_aus" || msg == "thumbs_down") {
      lightOn = false;
      digitalWrite(LED_PIN, LOW);
      Serial.println("[LED] AUS");
    }
  }

  if (t == TOPIC_BLINDS) {
    if (msg == "OPEN") {
      blindsServo.write(90);
      Serial.println("[ROLLO] Offen");
    } else if (msg == "CLOSE") {
      blindsServo.write(0);
      Serial.println("[ROLLO] Geschlossen");
    }
  }
}

void connectWiFi() {
  Serial.print("Verbinde mit WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" verbunden!");
}

bool connectMQTT() {
  Serial.print("Verbinde mit MQTT...");
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  if (mqtt.connect(CLIENT_ID)) {
    Serial.println(" verbunden!");
    mqtt.subscribe(TOPIC_LIGHT);
    mqtt.subscribe(TOPIC_BLINDS);
    mqtt.subscribe(TOPIC_GESTURE);
    Serial.println("Topics abonniert.");
    return true;
  }
  Serial.print(" Fehler rc=");
  Serial.println(mqtt.state());
  return false;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);

  blindsServo.attach(SERVO_PIN);
  blindsServo.write(0);

  dht.begin();
  connectWiFi();
  connectMQTT();
}

void loop() {
  // MQTT Verbindung aufrechterhalten
  if (!mqtt.connected()) {
    unsigned long now = millis();
    if (now - lastReconnect >= 5000) {
      lastReconnect = now;
      connectMQTT();
    }
  }
  mqtt.loop();

  // Taster prüfen
  bool btnState = digitalRead(BTN_PIN);
  if (btnState == LOW && lastBtnState == HIGH) {
    delay(50);
    lightOn = !lightOn;
    digitalWrite(LED_PIN, lightOn ? HIGH : LOW);
    mqtt.publish(TOPIC_LIGHT, lightOn ? "ON" : "OFF");
    mqtt.publish(TOPIC_BUTTON, "PRESSED");
    Serial.printf("[BUTTON] Licht %s\n", lightOn ? "AN" : "AUS");
  }
  lastBtnState = btnState;

  // PIR prüfen
  bool pirState = digitalRead(PIR_PIN);
  if (pirState != lastPirState) {
    mqtt.publish(TOPIC_PIR, pirState ? "DETECTED" : "CLEAR");
    Serial.printf("[PIR] %s\n", pirState ? "Bewegung erkannt" : "Klar");
    lastPirState = pirState;
  }

  // DHT22 alle 5 Sekunden
  if (millis() - lastSensorRead >= 5000) {
    lastSensorRead = millis();
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();
    if (!isnan(temp) && !isnan(hum)) {
      char buf[32];
      snprintf(buf, sizeof(buf), "%.1f", temp);
      mqtt.publish(TOPIC_TEMPERATURE, buf);
      Serial.printf("[DHT22] Temp: %.1f C  Feuchte: %.1f%%\n", temp, hum);
    }
  }
}
