#include "esp32platform_mqtt.h"

const char *CLIENTID = "ESP32Platform-";
const char *TOPIC = "esp32platform/status";
const char *TOPIC_IN = "esp32platform/#";

const char *mqtt_server = "192.168.68.108";

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
            String ipaddress = WiFi.localIP().toString();
            String status = "{\"message\":\"I'm alive\", \"ip\":" + ipaddress + "}";
            mqttClient.publish(TOPIC, status.c_str());
            mqttClient.subscribe(TOPIC_IN);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void pubmqttstatus(String key, String msg, String card) {
    String status =
        "{\"" + key + "\":\"" + msg + "\",\"card\":\"" + card + "\"}";
    char a[100];
    status.toCharArray(a, status.length() + 1);
    
    mqttClient.publish(TOPIC, a, false);
}

void setupMQTT() {
    Serial.println("Configuring MQTT server");
    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(mqttCallback);
}
