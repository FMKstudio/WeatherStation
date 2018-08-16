/////////////////////////////////////////////////////////////////////////////////////////
#include "conf.h"

void setup()
{
  //------------------------------------------------------  Определяем консоль
#ifdef DEBUG_UDP
  DBG_OUT_PORT.begin(4023, IPAddress(192, 168, 111, 132));

#else

  if (ram_data.type_disp == 0) DBG_OUT_PORT.begin(9600);
  else
  {
    DBG_OUT_PORT.begin(115200);
#if defined(ESP8266) || defined(ESP32)
    DBG_OUT_PORT.setDebugOutput(true);
#endif
  }
#endif


#if defined(ESP8266)
  //------------------------------------------------------  Get system information
  hwi.info();

  //------------------------------------------------------  Инициализируем встроенную файловую систему SPIFFS
#endif

# if defined(ESP8266) || defined(ESP32)
  fs_setup();
#endif
  DBG_OUT_PORT.println("file system started");

  //------------------------------------------------------  Читаем установки из EEPROM

  //conf_data = defaultConfig();
  conf_data = loadConfig();

  DBG_OUT_PORT.println("config loaded");

  //------------------------------------------------------  Запускаем I2C и проверяем наличие клиентов

# if defined(ESP8266) || defined(ESP32)
  Wire.begin(SDA_PIN, SCL_PIN);
# endif

# if defined(BOARD_RTL8710) || defined(BOARD_RTL8195A)  || defined(BOARD_RTL8711AM)
  Wire.begin();
# endif

  ram_data = fsys.i2c_scan(conf_data);
  //------------------------------------------------------  Инициализируем датчики

  sens.dht_preset(DHT_PIN, 22); //Тут устанавливается GPIO для DHT и его тип (11, 21, 22)

  ram_data_t sens_data = ram_data;

  sens_data = sens.init(ram_data);

  ram_data = sens_data;

  //------------------------------------------------------  Инициализируем GPIO
  pinMode(setting_pin, INPUT_PULLUP);
# if defined(ESP8266) || defined(ESP32)
  pinMode(A0,          INPUT);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, HIGH);
# endif

# if defined(ESP8266)
  pinMode(BUZ_PIN,    OUTPUT);
# endif
# if defined(ESP32)
  ledcAttachPin(BUZ2_PIN, BUZ_PIN);
  ledcSetup(BUZ2_PIN, 2000, 8); // 2 kHz PWM, 8-bit resolution
# endif

  //------------------------------------------------------  Инициализируем RTC
  if (ram_data.type_rtc > 0) rtc_init();
  DBG_OUT_PORT.println("rtc inital");

  //------------------------------------------------------  Инициализируем выбранный дисплей
  switch (ram_data.type_disp)
  {
    case 0:
      pinMode(uart_pin, INPUT_PULLUP);
      break;
    case 1:
      lcd_init();
      break;
    case 2:
      sevenseg_init();
      break;
    case 3:
      sevenseg_init();
      break;
    case 4:
      m7219_init();
      break;
    case 5:
      sevenseg_init();
      break;
    case 6:
      sevenseg_init();
      break;
    case 7:
      sevenseg_init();
      break;
    case 8:
      m1632_init();
      break;
    case 9:
#if defined(ESP32)
      m3264_init();
#endif
      break;
    case 10:
#if defined(ESP32)
      ili_init();
#endif
      break;
  }
  DBG_OUT_PORT.print("display selected...");
  DBG_OUT_PORT.println(ram_data.type_disp);

  //-------------------------------------------------------- Запускаем сетевые сервисы
# if defined(ESP8266) || defined(ESP32)
  //  WiFi.disconnect();
  //  WiFi.mode( WIFI_OFF );
  start_wifi();
# endif
  DBG_OUT_PORT.println("wifi started");

  if (web_cli || web_ap)
  {
# if defined(ESP8266) || defined(ESP32)
    digitalWrite(LED_BUILTIN, LOW);
# endif

    //------------------------------------------------------ Синхронизируем время с нтп если нету RTC
    if (ram_data.type_rtc == 0 && web_cli) GetNtp();

    //------------------------------------------------------ Получаем прогноз погоды от GisMeteo
    if (conf_data.use_pp == 1 && web_cli)
    {
      wf_data = e_srv.get_gm(gs_rcv(conf_data.pp_city_id));
    }


    //------------------------------------------------------ Получаем прогноз погоды от OpenWeatherMap
    if (conf_data.use_pp == 2 && web_cli)
    {
      wf_data = getOWM_forecast(conf_data.pp_city_id, conf_data.owm_key);
    }

    //------------------------------------------------------ Подключаем OTA, SSDP и MDNS
    nsys.OTA_init(conf_data.ap_ssid, conf_data.ap_pass);

# if defined(ESP8266) || defined(ESP32)
    MDNS.begin(conf_data.ap_ssid);
    nsys.ssdp_init();
    DBG_OUT_PORT.printf("Open http://%s", conf_data.ap_ssid);
    DBG_OUT_PORT.print(".local/edit to see the file browser\n");
# endif

    //------------------------------------------------------ Запускаем сервер
    web_setup();
    start_serv();
  }

  //-------------------------------------------------------  Опрашиваем датчики

  GetSnr();
  DBG_OUT_PORT.println("Snr data received");

  //-------------------------------------------------------- Гасим светодиод
#   if defined(ESP8266) || defined(ESP32)
  if (conf_data.use_es)  digitalWrite(LED_BUILTIN, HIGH);
#   endif

  //-------------------------------------------------------- Устанавливаем будильники
  set_alarm();
  DBG_OUT_PORT.println("alarm set");

  //-------------------------------------------------------- Регулируем яркость дисплея
  if (conf_data.auto_br)
  {
    snr_data.ft = sens.ft_read(ram_data.bh1750_present);
    cur_br = auto_br(snr_data.ft, conf_data.br_level);
  }
  else
  {
    cur_br = conf_data.man_br;  // Man brigthness
    snr_data.ft = cur_br;
  }
  DBG_OUT_PORT.println("brightness set");

  //------------------------------------------------------ Отправляем данные через UART
  if (ram_data.type_disp == 0) send_uart();


  //------------------------------------------------------ Радостно пищим по окончаниии подготовки к запуску
  Buzz.beep(BUZ_PIN);
  Buzz.play(songs[15], BUZ_PIN, true);   //inital sound card
  if (ram_data.type_disp == 9) m3264_upd();
  DBG_OUT_PORT.println("End of setup");
}

void loop()
{
  //------------------------------------------------------ Распределяем системные ресурсы
  irq_set();

  // ----------------------------------------------------- Обрабатываем клавиатуру
  keyb_read();

  // ----------------------------------------------------- Доп для будильника
  Buzz.play(songs[rtc_data.a_muz], BUZ_PIN, false);

  //------------------------------------------------------ Отправляем данные через UART
  if (ram_data.type_disp == 0 && !digitalRead(uart_pin)) send_uart();
}
