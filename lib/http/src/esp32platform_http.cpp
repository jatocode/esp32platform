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

void writeRedirectHeader(WiFiClient client, String redirectUrl) {
    Serial.println("Redirecting to " + redirectUrl);
    client.println("HTTP/1.1 301 Moved Permanently");
    client.println("Content-type:text/html");
    client.print("Location: ");
    client.println(redirectUrl);
    client.println();
    client.println();
}

struct ssid_pass connectionRequest(String params) {
    struct ssid_pass ssidpwd;
    Serial.println(params);
    // Parse parameters by finding =
    // Format is: C?ssid=SSID&p=PASSWORD
    int i = params.indexOf('=', 0);
    int i2 = params.indexOf('&', 0);
    String ssid = params.substring(i + 1, i2);
    ssid.replace('+', ' ');  // remove html encoding

    i = params.indexOf('=', i2);
    String pass = params.substring(i + 1);

    Serial.println(ssid);
    Serial.println(pass);

    ssidpwd.ssid = ssid;
    ssidpwd.pass = pass;

    return ssidpwd;
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

void wifiloop(WiFiServer *server) {
    WiFiClient client = server->available();  // server.accept();
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

int handleHttp(WiFiServer *server, bool connected, String networks, struct ssid_pass *ssidpwd) {
    WiFiClient client = server->available();

    int status = REQUEST_OK;

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
                            String path = currentLine.substring(
                                5, currentLine.length() - 9);
                            switch (path[0]) {
                                case 'A':
                                    Serial.println("GET A");
                                    client.println("HTTP/1.1 200 OK");
                                    client.println(
                                        "Content-type:application/json");
                                    client.println("{\"A\":\"OK\"}");
                                    client.println();
                                    client.println();
                                    break;
                                case 'C':
                                    *ssidpwd = connectionRequest(path);
                                    status = REQUEST_RECONNECT;
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
    
    return status;
}