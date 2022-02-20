#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFi.h>

struct ssid_pass {
    String ssid;
    String pass;
};

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

void writeOkHeader(WiFiClient client);
void writeMainResponse(WiFiClient client);
void writeAPResponse(WiFiClient client, String networks);
int handleHttp(WiFiServer server, bool connected, String networks, struct ssid_pass *ssidpwd);