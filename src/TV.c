#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

const char* ssid = ""; // nome rete
const char* password = ""; // password
MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head><title>TV</title></head>" \
                "<body>" \
                  "<h1>Canali tv" \
                  "<p><a href=\"ir?code=551489775\">ON/OFF</a></p>" \
                  "<p><a href=\"ir?code=551512725\">Send Netflix</a></p>" \
                  "<p><a href=\"ir?code=551502015\">volume +</a></p>" \
                  "<p><a href=\"ir?code=551534655\">volume -</a></p>" \
                  "<p><a href=\"ir?code=551485695\">canale_successivo</a></p>" \
                  "<p><a href=\"ir?code=551518335\">canale_precedente</a></p>" \
                  "<p><a href=\"ir?code=551520375\">1</a></p>" \
                  "<p><a href=\"ir?code=551504055\">2</a></p>" \
                  "<p><a href=\"ir?code=551536695\">3</a></p>" \
                  "<p><a href=\"ir?code=551495895\">4</a></p>" \
                  "<p><a href=\"ir?code=551528535\">5</a></p>" \
                  "<p><a href=\"ir?code=551512215\">6</a></p>" \
                  "<p><a href=\"ir?code=551544855\">7</a></p>" \
                  "<p><a href=\"ir?code=551491815\">8</a></p>" \
                  "<p><a href=\"ir?code=551524455\">9</a></p>" \
                  "<p><a href=\"ir?code=551541285\">exit</a></p>" \
                "</body>" \
              "</html>");
}

void handleIr() {
  for (uint16_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint64_t code = strtoul(server.arg(i).c_str(), NULL, 10);
      irsend.sendVestelAc(code,56,0);
      //irsend.sendSAMSUNG(code,32,0);
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.hostname("TV");
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
