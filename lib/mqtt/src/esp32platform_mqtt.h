#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

void mqttCallback(char *topic, byte *message, unsigned int length);
void reconnectMQTT(PubSubClient client);
void pubmqttstatus(PubSubClient client, String key, String msg, String card);
PubSubClient setupMQTT();