#include <Arduino.h>

// Mina egna
#include "esp32platform_eeprom.h"
#include "esp32platform_http.h"
#include "esp32platform_mqtt.h"
#include "esp32platform_ota.h"
#include "esp32platform_wifi.h"

// Starta en webserver
WiFiServer server(80);
PubSubClient mqttClient;
DNSServer dnsServer;

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
        mqttClient = setupMQTT();
    } else {
        // Start Access Point
        Serial.println("Setting up access point");

        WiFi.disconnect();
        networks = findNetworksNearby();
        dnsServer = setupAP();
    }

    server.begin();
    Serial.println("Web server started");
}

void loop() {
    if (connected) {
        if (!mqttClient.connected()) {
            Serial.println("Reconnecting mqttClient");
            reconnectMQTT(mqttClient);
        }

        mqttClient.loop();  // Loopar inte, ska bara köras i loopen

    } else {
        // Captive portal. Give our IP to everything
        networks = findNetworksNearby();
        dnsServer.processNextRequest();
    }

    ArduinoOTA.handle();

    handleHttp(server, connected, networks);
}