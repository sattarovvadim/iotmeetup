#ifndef IOTSERVER_H
#define IOTSERVER_H
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <WiFiUdp.h>

class IoTServer {
private:
        ESP8266WebServer *server = NULL;
        //WiFiUDP UDP;
        String serial;
        unsigned int localPort;
    
        void startWebServer();
        void handleRoot();
        void handleLed1(int state);
        void handleGetState();
        void handleServo(int angle);
        
public:
        IoTServer();
        IoTServer(unsigned int port);
        ~IoTServer();
        void readArduinoState();
        void serverLoop();
};
 
#endif
