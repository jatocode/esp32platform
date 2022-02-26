#include "esp32platform_mqtt.h"

const char *CLIENTID = "ESP32Platform-";
const char *TOPIC = "esp32platform/status";
const char *TOPIC_IN = "esp32platform";

const char *mqtt_server = "192.168.68.108";

void mqttCallback(char *topic, byte *message, unsigned int length) {
    String messageTemp = "";
    for (unsigned int i = 0; i < length; i++) {
        messageTemp += (char)message[i];
    }
    Serial.println();
    if (String(topic) == TOPIC_IN) {
        Serial.print("Message on topic: ");
        Serial.print(TOPIC_IN);
        Serial.print(" : ");
        Serial.println(messageTemp);
    }
}

void reconnectMQTT(PubSubClient *mqttClient) {
    while (!mqttClient->connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = CLIENTID;
        clientId += String(random(0xffff), HEX);
        if (mqttClient->connect(clientId.c_str())) {
            Serial.println("mqtt connected");
            mqttClient->subscribe(TOPIC_IN);

            String ipaddress = WiFi.localIP().toString();
            String status =
                "{\"message\":\"I'm alive\", \"ip\":" + ipaddress + "}";
            mqttClient->publish(TOPIC, status.c_str());
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient->state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void pubmqttstatus(PubSubClient *mqttClient, String key, String msg,
                   String card) {
    String status =
        "{\"" + key + "\":\"" + msg + "\",\"card\":\"" + card + "\"}";
    char a[100];
    status.toCharArray(a, status.length() + 1);

    mqttClient->publish(TOPIC, a, false);
}

void setupMQTT(WiFiServer *server, PubSubClient *mqttClient) {
    Serial.println("Configuring MQTT server");
    Serial.println(mqtt_server);
    mqttClient->setServer(mqtt_server, 1883);
    mqttClient->setCallback(mqttCallback);

    String clientId = CLIENTID;
    clientId += String(random(0xffff), HEX);
    if (mqttClient->connect(clientId.c_str())) {
        Serial.println("mqtt connected");
        String ipaddress = WiFi.localIP().toString();
        String status = "{\"message\":\"I'm alive\", \"ip\":" + ipaddress + "}";
        mqttClient->publish(TOPIC, status.c_str());
        Serial.printf("Published %s to %s\n", status.c_str(), TOPIC);
        mqttClient->subscribe(TOPIC_IN);
        Serial.printf("Subscribing %s\n", TOPIC_IN);
    }
}
