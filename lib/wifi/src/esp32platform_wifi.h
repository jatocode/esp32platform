#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

// Används för AP
DNSServer dnsServer;
const char *ssid = "esp32platform";
const char *password = "supersecret";

// MDSN namnet på nätverket
const char *mdnsname = "esp32platform";

String ipaddress = "";

bool connectWifi(String ssid, String pass);
void setupAP();
String findNetworksNearby();