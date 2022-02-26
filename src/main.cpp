#include <Arduino.h>

// Mina egna
#include "esp32platform_eeprom.h"
#include "esp32platform_http.h"
#include "esp32platform_mqtt.h"
//#include "esp32platform_ota.h"
#include "esp32platform_wifi.h"

// Starta en webserver
WiFiServer server(80);
PubSubClient mqttClient;
DNSServer dnsServer;
void wifiAndHttp();
void wifiloop();

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
    // startOTA();

    //  SSIDPwdToEeprom("wind up bird", "murakam1");

    String savedSSID = SSIDFromEeprom();
    String savedPASS = PwdFromEeprom();

    connected = connectWifi(savedSSID, savedPASS);

    if (connected) {
        Serial.print("Connected to wifi: ");
        Serial.println(savedSSID);
        // mqttClient = setupMQTT();
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
    // wifiAndHttp();

    wifiloop();

    // Gör något

    // Visa att vi lever
    if (millis() - last_blink > 1000) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        last_blink = millis();
    }
}

String prepareMainPage() {
    String htmlPage;
    htmlPage.reserve(2048);  // prevent ram fragmentation
    htmlPage = F(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<!DOCTYPE html><html><head>		<title>ESP32</title>	"
        "	<link rel=\"stylesheet\" "
        "href=\"https://cdnjs.cloudflare.com/ajax/libs/mini.css/3.0.1/"
        "mini-default.min.css\">		<style>.button.large "
        "{text-align:center;padding:2em ; margin: "
        "1em;font-size:2em;color:black}</style></head><body>		<h1 "
        "align=\"center\">ESP32 action</h1>		<br/><br/>	"
        "	<div "
        "class=\"row cols-sm-10\">				<a "
        "class=\"button "
        "large\" onClick='run(\"A\")' href=\"#\">Blink LED</a>		"
        "		<a class=\"button large\" onClick='run(\"B\")' "
        "href=\"#\">V&aring;gform: <span id=\"wave\">%WAVE%</span></a>	"
        "	</div>		<div><small>Connected to WiFi: "
        "%WIFI%</small></div>		<script>			"
        "	async function run(param) {				"
        "	"
        "	let result = await fetch('/' + param);	let r = await "
        "result.json(); "
        "document.getElementById('wave').innerHTML = r.wave; "
        "console.log(r);					/* Use result "
        "for "
        "something - or not */				}		"
        "</script>		</body></html>");
    htmlPage += "\r\n";
    return htmlPage;
}

void wifiloop() {
    WiFiClient client = server.available();  // server.accept();
    // wait for a client (web browser) to connect
    if (client) {
        Serial.println("\n[Client connected]");
        while (client.connected()) {
            // read line by line what the client (web browser) is requesting
            if (client.available()) {
                String line = client.readStringUntil('\r');
                Serial.print(line);
                // wait for end of client's request, that is marked with an
                // empty line
                if (line.length() == 1 && line[0] == '\n') {
                    client.println(prepareMainPage());
                    break;
                }
            }
        }

        while (client.available()) {
            client.read();
        }

        // close the connection:
        client.stop();
        Serial.println("[Client disconnected]");
    }
}

int lastwifiloop = 0;
int lasthttploop = 0;

void wifiAndHttp() {
    if (connected) {
        //   if (!mqttClient.connected()) {
        //      Serial.println("Reconnecting mqttClient");
        //    reconnectMQTT(mqttClient);
        // }

        // Serial.println("looping mqtt");
        // mqttClient.loop();  // Loopar inte, ska bara köras i loopen

        if (millis() - lastwifiloop > 1000) {
            Serial.println("Still connected");
            lastwifiloop = millis();
        }
    } else {
        // Captive portal. Give our IP to everything
        networks = findNetworksNearby();

        if (millis() - lastwifiloop > 1000) {
            Serial.println("in ap mode");
            lastwifiloop = millis();
        }

        dnsServer.processNextRequest();
    }

    //    ArduinoOTA.handle();

    int status = handleHttp(server, connected, networks, &ssidpwd);
    if (millis() - lasthttploop > 1000) {
        Serial.print("http handle status: ");
        Serial.println(status);
        lasthttploop = millis();
    }
    if (status == REQUEST_RECONNECT) {
        Serial.println("REQUEST RECONNECT");
        connected = connectWifi(ssidpwd.ssid, ssidpwd.pass);

        if (connected) {
            SSIDPwdToEeprom(ssidpwd.ssid, ssidpwd.pass);
        }
    }
}