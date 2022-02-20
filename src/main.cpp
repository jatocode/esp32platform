#include <Arduino.h>

// Mina egna
#include "esp32platform_eeprom.h"
#include "esp32platform_ota.h"
#include "esp32platform_wifi.h"

// Starta en webserver
WiFiServer server(80);

// wifi connection status
bool connected = false;

// Lista av nätverk
String networks = "";

void setup() {
    Serial.begin(115200);
    Serial.println();

    startEeprom();
    startOTA();

    String savedSSID = SSIDFromEeprom();
    String savedPASS = PwdFromEeprom();

    connected = connectWifi(savedSSID, savedPASS);

    if (connected) {
        Serial.print("Connected to wifi: ");
        Serial.println(savedSSID);
        // setupMQTT();
    } else {
        // Start Access Point
        Serial.println("Setting up access point");

        WiFi.disconnect();
        networks = scanNetworks();
        setupAP();
    }

    server.begin();
    Serial.println("Server started");
}

void loop() {
    if (connected) {
        // if (!mqttClient.connected()) {
        //     Serial.println("Reconnecting mqttClient");
        //     reconnect();
        // }
        // mqttClient.loop();  // Loopar inte, ska bara köras i loopen
    } else {
        // Captive portal. Give our IP to everything
        dnsServer.processNextRequest();
    }

    ArduinoOTA.handle();

    WiFiClient client = server.available();
}