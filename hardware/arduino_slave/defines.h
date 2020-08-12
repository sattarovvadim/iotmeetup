#ifndef DEFINES_H
#define DEFINES_H

#include <ArduinoJson.h>

#define I2C_ARDUINO_ADDRESS 9 // адрес платы Arduino в шине I2C
#define I2C_LCD_ADDRESS 0x27  // адрес LCD-дисплея в шине I2C

// Исполнительный узел или сенсор в системе
struct Node
{
  char id;           // код для внутренней идентификации команды
  char key[16];      // ключ для идентификации
  int state;         // текущее состояние (значение) узла
  bool is_command;   // является ли узел исполнительным (true) или сенсором (false)
  bool fire_publish; // публиковать ли состояние сенсора при изменении показаний этого сенсора
};

const int numNodes = 9;
const int jsonCapacity = numNodes * JSON_OBJECT_SIZE(5);

// Список подключенных узлов к системе
class NodesList
{
private:
  Node nodes[numNodes] = {
      {1, "simple_led", 0, true, false},
      {2, "sock_220v", 0, true, false},
      {3, "servo", 0, true, false},
      {101, "is_light", 0, false, true},
      {102, "potent", 0, false, true},
      {103, "range", 0, false, false},
      {104, "is_motion", 0, false, true},
      {105, "is_barrier", 0, false, true},
      {106, "temperature", 0, false, false}};

  char packNodeState(char command, int state);

public:
  // в случае успешного выполнения возвращает id выполненной команды
  char set_value(const char *key, int value);
  char set_value(char command, int value);

  // получить данные узлов в двоичном виде в buffer
  void get_values(char *buffer);

  // получить узел по индексу
  Node get_node_by_ind(char i) { return nodes[i]; }

  // получить данные в виде json
  void get_json(char *buffer);

  // распарсить двоичный массив из buffer, присвоить значения
  bool parse_from_bin(char *buffer);
};

extern NodesList nodes_list;

#endif