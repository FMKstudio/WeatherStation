# ESP8266WeatherStations

Проект метеостанции на ESP8266 

Поддерживаются:

Семисегментные дисплеи на TM1637, HT16K33, MAX7219 (4/8 Знакомест).

Матричный дисплей на MAX7219 8х32 пикселей.

LCD I2C 16х2 дисплеи со встроеным знакогенератором кириллицы.

Матричный цветной дисплей 16х32 пикселей с интерфейсом HUB75 на отдельном контроллере MEGA2560. 

Добавлена поддержка 3х цветной матрицы 16х32 от Sure Electronics на контроллере Holtek HT1632 (подключается напрямую к ЕСП).

Модули RTC на DS1307, DS1302, DS3231.

Датчик освещенности ВН1750, фоторезистор.

Погодные датчики DHT22, Si7021, BME(/P)085/180/280, AM2320, вcтроеный в DS3231 датчик температуры.

Обмен данными между 2мя и более устройствами внутри сети посредством JSON запросов и через облачный сервис ThingSpeak.

Клиент/серверная архитектура.

Прогноз погоды с GisMeteo и OpenWeatherMap.

Синхронизация времени с NTP.

6 многофункциональных будильников с 15ю мелодиями.

Настойка через веб интерфейс.

Обновление по OTA, mdns.

Потихоньку переползаем на ЕSP32


Немного мультиков
https://yadi.sk/i/UXBMaSDD3HfCZC
https://yadi.sk/i/Ew0egvWq3Hg48u
https://yadi.sk/i/os1wRLuz3Hg493


Куда чего подключать смотрим в Conf.h
