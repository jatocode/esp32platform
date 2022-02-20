#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

bool connectWifi(String ssid, String pass);
DNSServer setupAP();
String findNetworksNearby();