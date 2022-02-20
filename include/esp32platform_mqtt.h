#include <WiFiClient.h>
#include <PubSubClient.h>

const char *CLIENTID = "ESP32Platform-";
const char *TOPIC = "esp32platform/info";

const char *mqtt_server = "192.168.68.108";
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void mqttCallback(char *topic, byte *message, unsigned int length) {
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();
    if (String(topic) == TOPIC) {
        Serial.println(messageTemp);
    }
}

void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = CLIENTID;
        clientId += String(random(0xffff), HEX);
        if (mqttClient.connect(clientId.c_str())) {
            Serial.println("mqtt connected");
            mqttClient.publish(TOPIC, "{\"message\":\"I'm alive\"}");
            mqttClient.subscribe("esp32platform/#");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setupMQTT() {
    Serial.println("Configuring MQTT server");
    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(mqttCallback);
}
