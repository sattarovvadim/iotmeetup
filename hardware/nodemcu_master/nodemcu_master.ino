#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>

#include "defines.h"

#define READ_ARDUINO_DELAY 700

const char *ssid = "HatkaNet";            // имя wifi-точки доступа
const char *password = "BebKHvp5";        // пароль точки доступа
const int mqtt_port = 1883;               // порт mqtt-брокера
const char *mqtt_server = "192.168.1.88"; // и его адрес

boolean connectWifi();
void readArduinoState(bool fire_publish);
void on_mqtt_receive(char *topic, byte *payload, unsigned int length);

boolean wifiConnected = false;
unsigned long last_arduino_read = 0;

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(I2C_LCD_ADDRESS, 16, 2);

void setup()
{
  Serial.begin(115200);

  wifiConnected = connectWifi();

  if (wifiConnected)
  {
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(on_mqtt_receive);
  }
  Wire.begin(D1, D2); /* начать соединение по I2C шине с пинами D1 D2 */

  lcd.init();      // Инициализация дисплея
  lcd.backlight(); // Подключение подсветки
  //  lcd.setCursor(0, 0);             // Установка курсора в начало первой строки
  //  lcd.print("Hello from the");     // Набор текста на первой строке
  //  lcd.setCursor(0, 1);             // Установка курсора в начало второй строки
  //  lcd.print("ESP8266 board!");
}

void loop()
{
  if (wifiConnected && !client.connected())
  {
    reconnect();
  }
  client.loop();

  if (millis() - last_arduino_read < READ_ARDUINO_DELAY)
    return;
  readArduinoState(true);
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (i > 10)
    {
      state = false;
      break;
    }
    i++;
  }

  if (state)
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

void reconnect()
{
  if (!wifiConnected)
  {
    return;
  }
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP-Client";
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // При присоединении послать текущее состояние в канал
      readArduinoState(false);
      char buffer[jsonCapacity];
      nodes_list.get_json(buffer);

      client.publish("iotmeetup/espdevice/handshake", buffer);
      // и подписаться на необходимые топики
      client.subscribe("iotmeetup/espdevice/command/+");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void readArduinoState(bool fire_publish)
{
  last_arduino_read = millis();
  char old_data[numNodes];
  char data[numNodes];

  nodes_list.get_values(old_data);
  Serial.print(old_data);

  Wire.requestFrom(I2C_ARDUINO_ADDRESS, numNodes);
  if (numNodes != Wire.available())
  {
    return;
  }

  for (char i = 0; i < numNodes; i++)
  {
    data[i] = Wire.read();
  }
  nodes_list.parse_from_bin(data);

  if (!fire_publish) {
    return;
  }

  // теперь проверим, что изменилось, и отправим для изменившихся значений
  // пуш в mqtt-топик
  for (char i = 0; i < numNodes; i++)
  {
    if (old_data[i] != data[i])
    {
      Node node = nodes_list.get_node_by_ind(i);
      if (!node.fire_publish) {
        continue;
      }
      char topic[32];
      snprintf(topic, 32, "iotmeetup/espdevice/%s", node.key);
      const uint8_t payload[1] = {data[i]};
      client.publish(topic, payload, 1);

      Serial.print("Send to topic ");
      Serial.print(topic);
      Serial.print(" value ");
      Serial.print(String(data[i], DEC));
      Serial.println();
    }
  }
}

void on_mqtt_receive(char *topic, byte *payload, unsigned int length)
{
  Serial.print("MQTT Message arrived [");
  Serial.print(topic);
  Serial.print("] - ");
  for (int i = 0; i < length; i++)
  {
    Serial.print(String(payload[i], DEC));
  }
  Serial.println();

  char value = payload[0];

  // арифметика указателей, чтобы получить кусок строки, содержащий имя узла/команды
  char *cmd = topic + strlen("iotmeetup/espdevice/command/");
  Serial.print("command_key - ");
  Serial.println(cmd);

  char command = nodes_list.set_value(cmd, value);

  handle_command(command, value);
}

void handle_command(char command, char value)
{
  Serial.print("command_id - ");
  Serial.println(String(command, DEC));
  switch (command)
  {
    case 1:
      write_to_i2c("{\"simple_led\":" + String(value, DEC) + "}");
      break;
    case 2:
      write_to_i2c("{\"220v_sock\":" + String(value, DEC) + "}");
      break;
    case 3:
      write_to_i2c("{\"servo\":" + String(value, DEC) + "}");
      break;

    case 127:
      //  вернуть текущее состояние в mqtt
      readArduinoState(false);
      char buffer[jsonCapacity];
      nodes_list.get_json(buffer);
      client.publish("iotmeetup/espdevice/data", buffer);
      break;

    default:
      Serial.println("Command from MQTT server is not recognized");
      break;
  }
}

void write_to_i2c(String command)
{
  Wire.beginTransmission(I2C_ARDUINO_ADDRESS); /* передать в шину I2C для ардуино */
  Wire.write(command.c_str());
  Wire.endTransmission();
}
