/*
 * ============================================================
 *  Automated Coir Moisture Monitoring & Control System
 *  ESP32 Firmware — IoT Gateway to ThingSpeak
 *  IDE     : Arduino IDE
 *  Board   : ESP32 Dev Module
 * ============================================================
 *
 *  Connections:
 *  ------------
 *  ESP32 RX2 (GPIO16) ── PIC16F877A TX (RC6)
 *  ESP32 TX2 (GPIO17) ── PIC16F877A RX (RC7)
 *  Common GND
 */

#include <WiFi.h>
#include <HTTPClient.h>

// ---- Wi-Fi Credentials (update before flashing) ----
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ---- ThingSpeak Settings (update before flashing) ----
const char* thingspeak_api_key = "YOUR_THINGSPEAK_WRITE_API_KEY";
const char* thingspeak_url     = "http://api.thingspeak.com/update";

// ---- UART2 for PIC communication ----
#define PIC_SERIAL Serial2
#define BAUD_RATE  9600

// ---- Variables ----
String incoming_data = "";
int moisture_value   = 0;

// ============================================================
//  SETUP
// ============================================================
void setup() {
    Serial.begin(115200);
    PIC_SERIAL.begin(BAUD_RATE, SERIAL_8N1, 16, 17);  // RX=GPIO16, TX=GPIO17

    Serial.println("ESP32 Coir Monitor Starting...");

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

// ============================================================
//  LOOP
// ============================================================
void loop() {
    // Read data from PIC via UART
    while (PIC_SERIAL.available()) {
        char c = PIC_SERIAL.read();
        if (c == '\n') {
            process_data(incoming_data);
            incoming_data = "";
        } else {
            incoming_data += c;
        }
    }
    delay(100);
}

// ============================================================
//  Parse incoming UART string from PIC
//  Expected format: "M:<value>"
// ============================================================
void process_data(String data) {
    data.trim();
    if (data.startsWith("M:")) {
        moisture_value = data.substring(2).toInt();
        Serial.print("Moisture Received: ");
        Serial.print(moisture_value);
        Serial.println("%");
        send_to_thingspeak(moisture_value);
    }
}

// ============================================================
//  Upload moisture data to ThingSpeak (Field 1)
// ============================================================
void send_to_thingspeak(int moisture) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(thingspeak_url) +
                     "?api_key=" + thingspeak_api_key +
                     "&field1=" + String(moisture);

        http.begin(url);
        int http_code = http.GET();

        if (http_code > 0) {
            Serial.print("ThingSpeak Response: ");
            Serial.println(http_code);
        } else {
            Serial.println("ThingSpeak upload failed.");
        }
        http.end();

        delay(15000);  // ThingSpeak free tier requires >= 15s between updates
    } else {
        Serial.println("Wi-Fi disconnected. Skipping upload.");
    }
}
