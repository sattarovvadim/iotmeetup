#include <Wire.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include "defines.h"

// Номера пинов исполнительных узлов
#define PIN_IN_PIR_DETECT 2
#define PIN_OUT_SIMPLE_LED 4
#define PIN_OUT_220V_SOCK 3
#define PIN_IN_IS_LIGHT 5
#define PIN_OUT_SONIC_TRIG 8 // Trig дальномера
#define PIN_IN_SONIC_ECHO 9  // Echo дальномера
#define PIN_IN_BARRIER 6
#define PIN_OUT_SERVO 7

#define PIN_IN_POTENT 3 // аналоговый вход A3
#define PIN_IN_TEMPER 1 // аналоговый вход A2

Servo servo;
int potent = 0;

void setup()
{
  Wire.begin(I2C_ARDUINO_ADDRESS); /* присоединиться к шине i2c с указанным адресом */
  Wire.onReceive(receiveEvent);    /* зарегистрировать колбек при получении события из шины */
  Wire.onRequest(requestEvent);    /* зарегистрировать колбек при получении запроса из шины */
  Serial.begin(115200);            /* открыть порт для отладки */

  pinMode(PIN_OUT_SIMPLE_LED, OUTPUT);
  pinMode(PIN_OUT_220V_SOCK, OUTPUT);
  digitalWrite(PIN_OUT_220V_SOCK, HIGH);
  pinMode(PIN_IN_IS_LIGHT, INPUT);
  pinMode(PIN_OUT_SONIC_TRIG, OUTPUT);
  pinMode(PIN_IN_SONIC_ECHO, INPUT);
  pinMode(PIN_IN_PIR_DETECT, INPUT);
  pinMode(PIN_IN_BARRIER, INPUT);

  servo.attach(PIN_OUT_SERVO);
  servo.write(0);
}

void loop()
{
  potent = map(analogRead(PIN_IN_POTENT), 0, 1023, 0, 100);
}

// колбек-обработчик при получении данных из шины I2C
void receiveEvent(int howMany)
{
  String data = "";
  while (0 < Wire.available())
  {
    char c = Wire.read();
    data += c;
  }
  Serial.println(data);
  parseCommand(data);
}

void parseCommand(String &input)
{
  StaticJsonDocument<jsonCapacity> doc;

  DeserializationError err = deserializeJson(doc, input);

  if (err)
  {
    Serial.print("deserializeJson() failed with code ");
    Serial.println(err.c_str());
    return;
  }

  JsonObject root = doc.as<JsonObject>();
  char command = 0;
  int value = 0;

  for (JsonPair kv : root)
  {
    value = kv.value().as<int>();

    Serial.println(kv.key().c_str());
    Serial.println(value);

    if (command = nodes_list.set_value(kv.key().c_str(), value))
    {
      processCommand(command, value);
    }
  }
}

void processCommand(char command, int value)
{
  Serial.println("command");
  Serial.println(command);
  switch (command)
  {
    case 1:
      digitalWrite(PIN_OUT_SIMPLE_LED, value ? HIGH : LOW);
      break;
    case 2:
      digitalWrite(PIN_OUT_220V_SOCK, value ? LOW : HIGH);
      break;
    case 3:
      servo.write(value);
  }
}

void requestEvent()
{
  readInputs();
  char buffer[numNodes];
  nodes_list.get_values(buffer);
  Wire.write(buffer, numNodes);
}

void readInputs()
{
  nodes_list.set_value(101, !digitalRead(PIN_IN_IS_LIGHT));
  nodes_list.set_value(102, potent);
  nodes_list.set_value(103, read_distance_cm());
  nodes_list.set_value(104, digitalRead(PIN_IN_PIR_DETECT));
  nodes_list.set_value(105, !digitalRead(PIN_IN_BARRIER));
  nodes_list.set_value(106, read_temper());
}

int read_distance_cm()
{
  int impulseTime = 0;

  digitalWrite(PIN_OUT_SONIC_TRIG, LOW);          // Убираем импульс
  delayMicroseconds(2);                           // для лучшего измерения
  digitalWrite(PIN_OUT_SONIC_TRIG, HIGH);         // Подаем импульс на вход trig дальномера
  delayMicroseconds(10);                          // Импульс длится 10 микросекунд
  digitalWrite(PIN_OUT_SONIC_TRIG, LOW);          // Отключаем подачу импульса
  impulseTime = pulseIn(PIN_IN_SONIC_ECHO, HIGH); // Принимаем импульс и подсчитываем его длину
  return impulseTime / 58;                        //   58 это преобразование из длины импульса микросекундах в сантиметры
}

int read_temper()
{
  int val = analogRead(PIN_IN_TEMPER);

  // Данные, полученные с установленного термистора
  // 590 - 0
  // 470 - 23   - эксперимент
  // 400 - 36.6 - эксперимент
  // 100 - 95   - эксперимент
  // 75  - 100
  // 5,2 единицы АЦП - на один градус Цельсия

  Serial.println(val);
  //  return map(val, 75, 590, 100, 0);
  return map(val, 100, 400, 95, 37);
}
