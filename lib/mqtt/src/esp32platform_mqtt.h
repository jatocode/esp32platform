#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void mqttCallback(char *topic, byte *message, unsigned int length);
void reconnectMQTT();
void pubmqttstatus(String key, String msg, String card);
void setupMQTT();