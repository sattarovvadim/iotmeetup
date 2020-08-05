#ifndef DEFINES_H
#define DEFINES_H

#include <ArduinoJson.h>

#define I2C_ARDUINO_ADDRESS  9    // адрес платы Arduino в шине I2C
#define I2C_LCD_ADDRESS      0x27 // адрес LCD-дисплея в шине I2C

/*
  На вход http-сервер на порт 80 необходимо подавать JSON-строку в формате:

  {
  "simple_led": 1
  }

  где ключ (ключи) это имена узлов в системе, а значения это состояние, в которое необходимо установить узел.
  Список узлов объявлен в nodes ниже.
*/


// Исполнительный узел или сенсор в системе
struct Node {
  char id;                // код для внутренней идентификации команды
  char key[16];           // ключ для идентификации
  int  state;             // текущее состояние (значение) узла
  bool is_command;        // является ли узел исполнительным (true) или сенсором (false)
  bool fire_http_request; // отправлять ли отдельный http запрос при изменении показаний этого сенсора
};

const int numNodes = 9;
const int jsonCapacity = numNodes * JSON_OBJECT_SIZE(5);

class NodesList {
  private:
    Node nodes[numNodes] = {
      {1,    "simple_led",  0, true,  false},
      {2,    "220v_sock",   0, true,  false},
      {3,    "servo",       0, true,  false},
      {1001, "is_light",    0, false, true},
      {1002, "potent",      0, false, false},
      {1003, "range",       0, false, false},
      {1004, "is_motion",   0, false, true},
      {1005, "is_barrier",  0, false, false},
      {1006, "temperature", 0, false, false}
    };

    char packNodeState(char command, int state);
    
  public:
    char set_value(const char *key, int value);    // в случае успешного выполнения возвращает id выполненной команды
    char set_value(char command, int value);
    void get_values(char *buffer);
    void get_json(char *buffer);
    bool parse_from_bin(char *buffer);
};

extern NodesList nodes_list;


#endif
