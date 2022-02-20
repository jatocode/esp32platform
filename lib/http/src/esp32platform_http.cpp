#include "esp32platform_http.h"

void writeOkHeader(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
}

void writeMainResponse(WiFiClient client) {
    writeOkHeader(client);
    String ipaddress = WiFi.localIP().toString();
    String htmlParsed = mainHtmlOutput;
    htmlParsed.replace("%WIFI%", ipaddress);
    htmlParsed.replace("%WAVE%", "not in use");
    client.println(htmlParsed);
    client.println();
    Serial.println("Wrote main response to client");
}

void writeAPResponse(WiFiClient client, String networks) {
    writeOkHeader(client);
    String htmlParsed = apHtmlOutput;
    htmlParsed.replace("%SSIDLIST%", networks);
    client.println(htmlParsed);
    client.println();
    Serial.println("Wrote AP response to client");
}

void handleHttp(WiFiServer server, bool connected, String networks) {
    WiFiClient client = server.available();

    if (client) {
        String currentLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    // if the current line is blank, you got two newline
                    // characters in a row. that's the end of the client HTTP
                    // request, so send a response:
                    if (currentLine.length() == 0) {
                        if (connected) {
                            // Connected to WiFi. Respond with main page
                            writeMainResponse(client);
                        } else {
                            // Access point. Respond with wifi-select page
                            writeAPResponse(client, networks);
                        }
                        break;
                    } else {
                        if (currentLine.startsWith("GET")) {
                            // Trim to make cleaner switch
                            // GET /xxxx HTTP/1.1
                            String path = currentLine.substring(5, currentLine.length() - 9);
                            switch (path[0]) {
                                case 'A':
                                    Serial.println("I'm doing a thing");
                                    client.println("HTTP/1.1 200 OK");
                                    client.println(
                                        "Content-type:application/json");
                                    client.println();
                                    client.println();
                                    client.stop();
                                    break;
                                default:
                                    break;
                            }
                        }
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        client.stop();
    }
}