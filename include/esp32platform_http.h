#include <WiFiClient.h>
#include <WiFiServer.h>

const String mainHtmlOutput =
    "<!DOCTYPE html><html><head>		<title>ESP32</title>	"
    "	<link rel=\"stylesheet\" "
    "href=\"https://cdnjs.cloudflare.com/ajax/libs/mini.css/3.0.1/"
    "mini-default.min.css\">		<style>.button.large "
    "{text-align:center;padding:2em ; margin: "
    "1em;font-size:2em;color:black}</style></head><body>		<h1 "
    "align=\"center\">ESP32 action</h1>		<br/><br/>		<div "
    "class=\"row cols-sm-10\">				<a class=\"button "
    "large\" onClick='run(\"A\")' href=\"#\">Blink LED</a>		"
    "		<a class=\"button large\" onClick='run(\"B\")' "
    "href=\"#\">V&aring;gform: <span id=\"wave\">%WAVE%</span></a>	"
    "	</div>		<div><small>Connected to WiFi: "
    "%WIFI%</small></div>		<script>			"
    "	async function run(param) {					"
    "	let result = await fetch('/' + param);	let r = await result.json(); "
    "document.getElementById('wave').innerHTML = r.wave; "
    "console.log(r);					/* Use result for "
    "something - or not */				}		"
    "</script>		</body></html>";
// access point, select wifi as one line
const String apHtmlOutput =
    "<!DOCTYPE html><html><head>	<title>ESP32 connect to "
    "Wifi</title>	<link rel=\"stylesheet\" "
    "href=\"https://cdnjs.cloudflare.com/ajax/libs/mini.css/3.0.1/"
    "mini-default.min.css\"></head><body>	<h1 align=\"center\">ESP32 "
    "connect to Wifi</h1>	<br /><br />	<form action=\"/C\" "
    "method=\"GET\">		<fieldset>			"
    "<legend>Connect to wifi</legend>			<div class=\"col-sm-12 "
    "col-md-6\">				<select name=\"ssid\">	"
    "				%SSIDLIST%				"
    "</select>			</div>			<div "
    "class=\"row\">				<div class=\"col-sm-8 "
    "col-md-8\">					<label "
    "for=\"password\">Password</label>					<input "
    "name=\"p\" type=\"password\" id=\"password\" placeholder=\"Password\" "
    "/>				</div>			</div>		"
    "	<button class=\"submit\">Connect</button>			"
    "</div>		</fieldset>	</form></body></html>";

void writeOkHeader(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
}

void writeMainResponse(WiFiClient client, String ipaddress) {
    writeOkHeader(client);
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

void handleHttp(WiFiServer server, bool connected, String ipaddress, String networks) {
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
                            writeMainResponse(client, ipaddress);
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