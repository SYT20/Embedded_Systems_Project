// put previous model as it is then change some parts //
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "Nodemcu16";  // Enter your SSID here
const char* password = "12345678";     // Enter your password here

/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

uint8_t IN_1 = 4;  // D6  L298N in1 motors Right //d2
uint8_t IN_2 = 0;  // D7  L298N in2 motors Right //d3
uint8_t IN_3 = 13;   // D3  L298N in3 motors Left //d7
uint8_t IN_4 = 15;   // D2  L298N in4 motors Left //d8
uint8_t ENA = 5;   // D5  Enable/speed motors Right //d1
uint8_t ENB = 12;   // D4  Enable/speed motors Left //d6

const int interruptPin = 2; // connect interrupt pin to d8 of nodemcu //d4      // give another pin of button to 3.3v //
volatile bool stopCar = false;

void ICACHE_RAM_ATTR stopCarInterrupt() {
  stopCar = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", handle_OnConnect);
  server.on("/forward", handle_forward);
  server.on("/backward", handle_backward);
  server.on("/left", handle_left);
  server.on("/right", handle_right);
  server.on("/stop", handle_stop);

  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), stopCarInterrupt, RISING);
}

void loop() {
  server.handleClient();

  if (stopCar) {
    stopCarFunction();
  }
}

void stopCarFunction() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, 0);
  Serial.println("Stopping due to interrupt..");
  stopCar = false;
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}

void carFront() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, 255);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, 255);
  Serial.println("Going Forwards..");
}

void carBack() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, 255);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, 255);
  Serial.println("Going Backwards..");
}

void carRight() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, 255);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, 255);
  Serial.println("Going Right..");
}

void carLeft() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, 255);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, 255);
  Serial.println("Going Left..");
}

void handle_forward() {
  carFront();
  server.send(200, "text/html", SendHTML());
}

void handle_backward() {
  carBack();
  server.send(200, "text/html", SendHTML());
}

void handle_left() {
  carLeft();
  server.send(200, "text/html", SendHTML());
}

void handle_right() {
  carRight();
  server.send(200, "text/html", SendHTML());
}

void handle_stop() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, 0);
  Serial.println("Stopping..");
  server.send(200, "text/html", SendHTML());
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Car Control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body { margin-top: 50px; }\n";
  ptr += "h1 { color: #444444; margin: 50px auto 30px; }\n";
  ptr += ".button { width: 100px; height: 60px; border-radius: 10px; font-size: 20px; margin: 10px; text-decoration: none; display: inline-block; text-align: center; padding-top: 20px; }\n";
  ptr += ".button-on { background-color: #1abc9c; color: white; }\n";
  ptr += ".button-off { background-color: #34495e; color: white; }\n";
  ptr += "p { font-size: 14px; color: #888; margin-bottom: 10px; }\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Car Control</h1>\n";
  ptr += "<a class=\"button button-on\" href=\"/forward\">Forward</a>\n";
  ptr += "<br><a class=\"button button-on\" href=\"/left\">Left</a>";
  ptr += "<a class=\"button button-off\" href=\"/stop\">Stop</a>";
  ptr += "<a class=\"button button-on\" href=\"/right\">Right</a><br>";
  ptr += "<a class=\"button button-on\" href=\"/backward\">Backward</a>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}