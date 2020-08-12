#include "defines.h"

#include <ArduinoJson.h>

NodesList nodes_list;

char NodesList::set_value(const char *key, int value)
{

  if (!strcmp(key, "request"))
  {
    return 127;
  }

  char res = 0;
  for (char i = 0; i < numNodes; i++)
  {
    if (strcmp(this->nodes[i].key, key))
    {
      continue;
    }

    this->nodes[i].state = value;
    res = nodes[i].id;
  }
  return res;
}

char NodesList::set_value(char command, int value)
{
  char res = 0;
  for (char i = 0; i < numNodes; i++)
  {
    if (this->nodes[i].id != command)
    {
      continue;
    }

    this->nodes[i].state = value;
    res = nodes[i].id;
  }
  return res;
}

bool NodesList::parse_from_bin(char *buffer)
{
  for (char i = 0; i < numNodes; i++)
  {
    this->nodes[i].state = buffer[i];
  }
  return true;
}

void NodesList::get_values(char *buffer)
{

  for (char i = 0; i < numNodes; i++)
  {
    buffer[i] = packNodeState(this->nodes[i].id, this->nodes[i].state);
  }
}

char NodesList::packNodeState(char command, int state)
{
  return state & 0xff;
}

void NodesList::get_json(char *buffer)
{
  StaticJsonDocument<jsonCapacity> doc;

  for (char i = 0; i < numNodes; i++)
  {
    doc[nodes[i].key] = nodes[i].state;
  }
  serializeJson(doc, buffer, jsonCapacity);
}
