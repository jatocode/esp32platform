#include "esp32platform_wifi.h"

// Används för AP
const char *ssid = "esp32platform";
const char *password = "supersecret";

// MDSN namnet på nätverket
const char *mdnsname = "esp32platform";

String ipaddress = "";

bool connectWifi(String ssid, String pass) {
    char ssidA[100];
    char passA[100];
    int i = 8;

    WiFi.disconnect();

    Serial.println("Trying to connect to " + ssid + " with passwd:" + pass);
    // WiFi.begin needs char*
    ssid.toCharArray(ssidA, 99);
    pass.toCharArray(passA, 99);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidA, passA);

    while (i-- > 0 && WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Connection failed");
        return false;
    }
    ipaddress = WiFi.localIP().toString();
    Serial.println("WiFi connected, IP address: " + ipaddress);

    if (!MDNS.begin(mdnsname)) {
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.println("mDNS responder started, name:" + String(mdnsname) +
                       ".local");
        MDNS.addService("http", "tcp", 80);
    }
    ipaddress += " (" + String(mdnsname) + ".local)";

    return true;
}

DNSServer setupAP() {
    const byte DNS_PORT = 53;

    IPAddress local_IP(10, 0, 1, 1);
    IPAddress subnet(255, 255, 0, 0);

    Serial.println("Configuring access point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, local_IP, subnet);

    //    WiFi.softAP(ssid, password); // Using pwd
    WiFi.softAP(ssid);  // Not using pwd

    DNSServer dnsServer;

    dnsServer.start(DNS_PORT, "*", local_IP);

    IPAddress myIP = WiFi.softAPIP();
    ipaddress = myIP.toString();
    Serial.println("AP ssid:" + String(ssid));
    Serial.println("AP IP address: " + ipaddress);

    return dnsServer;
}

String findNetworksNearby() {
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        String htmloptions = "";
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " "
                                                                      : "*");
            htmloptions += "<option>" + String(WiFi.SSID(i)) + "</option>";
            delay(10);
        }
        Serial.println(htmloptions);
        return htmloptions;
    }
    Serial.println("");
    return "";
}