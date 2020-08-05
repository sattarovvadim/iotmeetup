#include <Wire.h>
#include "server.h"
#include "defines.h"

//<<constructor>>
IoTServer::IoTServer(unsigned int port) {
  localPort = port;

  startWebServer();
}

//<<destructor>>
IoTServer::~IoTServer() {
}

void IoTServer::serverLoop() {
  if (server != NULL) {
    server->handleClient();
    delay(1);
  }
}

void IoTServer::startWebServer() {
  server = new ESP8266WebServer(localPort);

  server->on("/", [&]() {
    handleRoot();
  });

  server->on("/index.html", [&]() {
    String response = "<html><body><h1>It works!</h1></body></html>";
    server->send(200, "text/html", response.c_str());
  });
  server->on("/control/led1/on", [&]() {
    handleLed1(1);
  });
  server->on("/control/led1/off", [&]() {
    handleLed1(0);
  });
  server->on("/control/servo", [&]() {
    handleServo(server->arg("angle").toInt());
  });


  server->on("/get-state", [&]() {
    handleGetState();
  });


  //server->onNotFound(handleNotFound);
  server->begin();

  Serial.println("WebServer started on port: ");
  Serial.println(localPort);
}


void IoTServer::handleRoot() {
  server->send(200, "text/plain", "Hello World from ESP8622!");
}

void IoTServer::handleLed1(int state) {
  Wire.beginTransmission(I2C_ARDUINO_ADDRESS); /* begin with device address 8 */
  if (state) {
    Serial.println("Got Turn on LED request");
    Wire.write("{\"simple_led\":1}");
  } else {
    Serial.println("Got Turn off LED request");
    Wire.write("{\"simple_led\":0}");
  }
  Wire.endTransmission();

  server->send(200, "text/plain", "LED state is " + String(state ? "on" : "off"));
}



void IoTServer::handleGetState()
{
  readArduinoState();
  char buffer[jsonCapacity];
  nodes_list.get_json(buffer);

  Serial.println(buffer);
  server->send(200, "text/json", buffer);
}

void IoTServer::readArduinoState()
{
  Wire.requestFrom(I2C_ARDUINO_ADDRESS, numNodes);
  char data[numNodes];
  if (numNodes == Wire.available()) {
    for (char i = 0; i < numNodes; i++) {
      data[i] = Wire.read();
    }
    nodes_list.parse_from_bin(data);
  }
}


void IoTServer::handleServo(int angle)
{
  Wire.beginTransmission(I2C_ARDUINO_ADDRESS); /* begin with device address 8 */
  Wire.write(String("{\"servo\":" + String(angle) + "}").c_str());
  Wire.endTransmission();

  server->send(200, "text/plain", "Servo angle is " + String(angle));
}
