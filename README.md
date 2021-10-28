# WeatherStations

Проект метеостанции на ESP8266/32 (среда - Arduino IDE 1.8.5)

Поддерживаются:

Дисплеи на 7ми и 14ти сегментных индикаторах с драйверами на TM1637, HT16K33, MAX7219 (4/8 Знакомест).

LCD16х2 дисплей со встроеным знакогенератором кириллицы, интерфейс  I2C.

Матричные дисплеи на:
        
        MAX7219 8х32 и 16х32 пикселей.
        отдельном контроллере MEGA2560, 16х32 пикселей, полный цвет, интерфейс HUB75
        в версии ЕСП32, 64х64 пикселей, полный цвет, интерфейс HUB75 
        Holtek HT1632, 16х32 пикселей, 3 цвета (R, G, Orange) от Sure Electronics

Модули RTC на DS1307, DS1302, DS3231.

Управление яркостью дисплеев посредством датчика освещенности ВН1750 или фоторезистора.

Погодные датчики DHT22, Si7021, BME(/P)085/180/280, AM2320, вcтроеный в DS3231 датчик температуры.

Обмен данными между 2мя и более устройствами внутри сети посредством JSON запросов и через облачный сервис ThingSpeak.

Клиент/серверная архитектура.

Возможность работы в режиме "WiFi датчик".

Прогноз погоды с GisMeteo и OpenWeatherMap.

Синхронизация времени с NTP.

6 многофункциональных будильников с 15ю мелодиями.

Настойка через веб интерфейс.

Обновление по OTA, mdns.

Новости с newsapi.

Конфигурирование GPIO.

Дауншифтовая версия на MEGA2560 без возможности перенастройки, без прогноза, без новостей.

Немного мультиков
https://yadi.sk/i/UXBMaSDD3HfCZC

https://yadi.sk/i/Ew0egvWq3Hg48u

https://yadi.sk/i/os1wRLuz3Hg493

https://yadi.sk/i/jCs5TbVB3aA5pw теперь только один мк, безо всяких мег

https://youtu.be/F3vVPQLzyJk версия с новостями, там же есть ссылки на фото платы

Cхемы в папочке extras, там же и инфа для изготовления печатных плат.

Файлы из папки Data необходимо залить на LittleFS плагином (лежат в  папке dop).

По умолчанию первый раз запускается точка доступа с именем  Web_Clock и паролем 12345678 (задается в файле config.json из папки data перед загрузкой в LittleFS строкой  "ap_ssid":"Web_Clock","ap_pass":"12345678").Веб интерфейс будет доступен по адресу 192.168.4.1.
