#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "defines.h"
#include "server.h"

#include <LiquidCrystal_I2C.h>

#define READ_ARDUINO_DELAY 500

const char* ssid     = "HatkaNet";   // имя wifi-точки доступа
const char* password = "BebKHvp5";   // пароль точки доступа

boolean connectWifi();
boolean wifiConnected = false;

unsigned long last_arduino_read = 0;

IoTServer *server;
LiquidCrystal_I2C lcd(I2C_LCD_ADDRESS, 16, 2);


void setup()
{
  Serial.begin(115200);

  wifiConnected = connectWifi();

  if (wifiConnected) {
    server = new IoTServer(80);    
  }
  Wire.begin(D1, D2); /* начать соединение по I2C шине с пинами D1 D2 */

  lcd.init();                      // Инициализация дисплея
  lcd.backlight();                 // Подключение подсветки
  lcd.setCursor(0, 0);             // Установка курсора в начало первой строки
  lcd.print("I'm got the power");  // Набор текста на первой строке
  lcd.setCursor(0, 1);             // Установка курсора в начало второй строки
  lcd.print("YeSSS!1one");
}

void loop()
{
  if (wifiConnected) {
    server->serverLoop();
  }

  if (millis() - last_arduino_read < READ_ARDUINO_DELAY) {
    return;
  }
  last_arduino_read = millis();
  server->readArduinoState();
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}
