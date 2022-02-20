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
void wifiAndHttp();

// wifi connection status
bool connected = false;

// Lista av nätverk
String networks = "";

int last_blink = 0;
struct ssid_pass ssidpwd;

void setup() {
    Serial.begin(115200);
    Serial.println();

    pinMode(LED_BUILTIN, OUTPUT);

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
    // Hantera allt med nätet
    wifiAndHttp();

    // Gör något

    // Visa att vi lever
    if (millis() - last_blink > 1000) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        last_blink = millis();
    }
}

void wifiAndHttp() {
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

    int status = handleHttp(server, connected, networks, &ssidpwd);
    if(status == REQUEST_RECONNECT) {
        connected = connectWifi(ssidpwd.ssid, ssidpwd.pass);  

        if(connected) {
            SSIDPwdToEeprom(ssidpwd.ssid, ssidpwd.pass);
        }
    }
}