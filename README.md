# iotmeetup
Код для митапа по теме "Elixir для интернета вещей"

Запуск:

1. Загрузить код в ESP8266 и Arduino. Необходимо проверить, что код соответствует вашей схемотехнике.

2. Зайти в iotmeetup/server/ и запустить
`mix phx.server`

3. Запустить MQTT-брокер такой командой: `docker run -d --name emqx -p 1883:1883 -p 8083:8083 -p 8883:8883 -p 8084:8084 -p 18083:18083 emqx/emqx`

4. Зайти в iotmeetup/server/frontend/ и запустить
`npm install && npm start`
