//------------------------------------------------------  Получаем данные с датчиков
snr_data_t GetSnr(ram_data_t rd, conf_data_t cf)
{
  snr_data_t td;
  snr_data_t ed1;
  snr_data_t ed2;
  snr_data_t sd;

  sd.t1 = 99;
  sd.t2 = 99;
  sd.t3 = 99;
  sd.h1 = 0;
  sd.h2 = 0;
  sd.h3 = 0;
  sd.p = 700;

  rd.temp_rtc = 99;

  if ((rd.type_snr1 == 5 || rd.type_snr2 == 5 || rd.type_snr3 == 5) && rd.type_rtc == 1)
  {
    rd.temp_rtc = myrtc.get_temperature();
  }

# if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
  if (wifi_data_cur.cli)
  {
    if (rd.type_snr1 == 1 || rd.type_snr2 == 1 || rd.type_snr3 == 1 || rd.type_snrp == 1)
    {
      dmsg.callback(cf.type_disp, 2, 0, cf.rus_lng); // сообщение на индикатор о начале обмена с TS
      String ts_str = ts_rcv(cf.ts_ch_id, cf.AKey_r);  // Получаем строчку данных от TS
      td = e_srv.get_ts(ts_str); // Парсим строчку от TS
      dmsg.callback(cf.type_disp, 2, 1, cf.rus_lng); // сообщение на индикатор о результатах обмена с TS
    }
    if (rd.type_snr1 == 2 || rd.type_snr2 == 2 || rd.type_snr3 == 2 || rd.type_snrp == 2) ed1 = e_srv.get_es(es_rcv(cf.esrv1_addr)); // Получаем данные от внешнего сервера1
    if (rd.type_snr1 == 3 || rd.type_snr2 == 3 || rd.type_snr3 == 3 || rd.type_snrp == 3) ed2 = e_srv.get_es(es_rcv(cf.esrv2_addr)); // Получаем данные от внешнего сервера2

    if (cf.use_pp == 2) {
      wf_data_cur = getOWM_current(cf.pp_city_id, cf.owm_key);// Получаем данные от OWM
    }
  }
#endif
  if ((rd.type_snr1 > 0 && rd.type_snr1 < 14) || (rd.type_snr2 > 0 && rd.type_snr2 < 14) || (rd.type_snr3 > 0 && rd.type_snr3 < 14) || (rd.type_snrp > 0 && rd.type_snrp < 14))
  {
    sd = sens.read_snr(rd.type_snr1, rd.type_snr2, rd.type_snr3, rd.type_snrp, rd.temp_rtc, td, ed1, ed2, wf_data_cur); // Заполняем матрицу данных с датчиков
  }

# if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
  if (wifi_data_cur.cli)
  {
    if (cf.use_ts > 0)
    {
      dmsg.callback(cf.type_disp, 1, 0, cf.rus_lng); // сообщение на индикатор о начале обмена с TS
      ts_snd(e_srv.put_ts(cf.AKey_w, cf.use_ts, sd)); // Отправляем инфу на TS
      dmsg.callback(cf.type_disp, 1, 1, cf.rus_lng); // сообщение на индикатор о результатах обмена с TS
    }

    if (cf.use_es > 0)put_to_es(cf.esrv1_addr, cf.use_es, sd); //отправляем показания датчиков на внешний сервер 1
  }
#endif
  return sd;
}

#if defined(__AVR_ATmega2560__)
void(* resetFunc) (void) = 0; //Programm reset
#endif

#if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
//------------------------------------------------------  Делаем запрос данных с Gismeteo
String gs_rcv(unsigned long city_id)
{
  if (debug_level == 10) DBG_OUT_PORT.println(F("True get data from GisMeteo"));
  String out = "No connect to network";
  if (wifi_data_cur.cli)
  {
    String addr = "http://informer.gismeteo.ru/xml/";
    addr += String(city_id);
    addr += ".xml";
    out = nsys.http_client(addr);
  }
  if (debug_level == 10) DBG_OUT_PORT.println(out);
  return out;
}

/*
  =======================================================================
  Берем текущую погоду с сайта openweathermap.org
  =======================================================================
*/
const char* owmHost = "api.openweathermap.org";


wf_data_t getOWM_current(unsigned long cityID, char weatherKey[32])
{
  wf_data_t prog;

  String out = "No connect to network";
  DBG_OUT_PORT.print(F("\n Current weather from ")); DBG_OUT_PORT.println(owmHost);
  String addr = "http://";
  addr += owmHost;
  addr += "/data/2.5/forecast?id=";
  addr += cityID;
  addr += "&units=metric&appid=";
  addr += weatherKey;
  addr += "&lang=ru&cnt=1";
  DBG_OUT_PORT.println(addr);

  out = nsys.http_client(addr);
  //DBG_OUT_PORT.println(F(out);

  String line = remove_sb(out);

  DBG_OUT_PORT.println("\n Now " + line);

  DynamicJsonDocument jsonBuf(2048);
  DeserializationError error = deserializeJson(jsonBuf, line);

  if (error)
  {
    DBG_OUT_PORT.print(F("deserializeJson() failed: "));
    DBG_OUT_PORT.println(error.c_str());
    return prog;
  }
  JsonObject root = jsonBuf.as<JsonObject>();

  prog.descript = root["list"]["weather"]["description"].as<String>();
  prog.descript.toLowerCase();
  prog.temp_min = root["list"]["main"]["temp"];
  prog.hum_min = root["list"]["main"]["humidity"];
  prog.press_min = root["list"]["main"]["grnd_level"]; //давление "на поверхности земли в текущей местности (с учетом высоты)"
  prog.press_max = root["list"]["main"]["sea_level"];  //давление "на уровне моря"

  prog.press_max -= prog.press_min; // разница давления "на уровне моря" и "у земли"
  prog.press_min /= 1.3332239; // перевод в мм.рт.ст

  prog.wind_min = root["list"]["wind"]["speed"];
  uint16_t dir = root["list"]["wind"]["deg"];
  prog.cloud = root["list"]["clouds"]["all"];
  prog.wind_dir = rumb_conv(dir);

  DBG_OUT_PORT.print(F("descript.."));
  DBG_OUT_PORT.println(prog.descript);
  DBG_OUT_PORT.print(F("temp min.."));
  DBG_OUT_PORT.println(prog.temp_min);
  DBG_OUT_PORT.print(F("humidity min.."));
  DBG_OUT_PORT.println(prog.hum_min);
  DBG_OUT_PORT.print(F("pressure min.."));
  DBG_OUT_PORT.println(prog.press_min);
  DBG_OUT_PORT.print(F("pressure max.."));
  DBG_OUT_PORT.println(prog.press_max);
  DBG_OUT_PORT.print(F("wind min.."));
  DBG_OUT_PORT.println(prog.wind_min);
  DBG_OUT_PORT.print(F("cloud.."));
  DBG_OUT_PORT.println(prog.cloud);
  DBG_OUT_PORT.print(F("win dir.."));
  DBG_OUT_PORT.println(prog.wind_dir);

  return prog;
}

/*
  =======================================================================
  Берем ПРОГНОЗ!!! погоды с сайта openweathermap.org
  =======================================================================
*/
wf_data_t getOWM_forecast(unsigned long cityID, char weatherKey[32])
{
  wf_data_t prog;

  String out = "No connect to network";
  DBG_OUT_PORT.print(F("\n Weather forecast for tomorrow from ")); DBG_OUT_PORT.println(owmHost);
  String addr = "http://";
  addr += owmHost;
  addr += "/data/2.5/forecast/daily?id=";
  addr += cityID;
  addr += "&units=metric&appid=";
  addr += weatherKey;
  addr += "&lang=ru&cnt=2";
  //if (debug_level == 10)
  DBG_OUT_PORT.println(addr);

  out = nsys.http_client(addr);
  if (debug_level == 10) DBG_OUT_PORT.println(out);

  String tempz = tvoday(out);
  DBG_OUT_PORT.println("\n" + tempz);

  DynamicJsonDocument jsonBuf(2048);
  DeserializationError error = deserializeJson(jsonBuf, tempz);
  if (error)
  {
    DBG_OUT_PORT.print(F("deserializeJson() failed: "));
    DBG_OUT_PORT.println(error.c_str());
    return prog;
  }
  JsonObject root = jsonBuf.as<JsonObject>();

  prog.cloud = root["clouds"];
  prog.wind_min = root["speed"];
  uint16_t dir = root["deg"];
  prog.temp_min = root["temp"]["min"];
  prog.temp_max = root["temp"]["max"];
  prog.descript = root["weather"]["description"].as<String>();
  prog.descript.toLowerCase();
  prog.hum_min = root["humidity"];
  prog.press_max = root["pressure"];

  prog.press_min = (prog.press_max - 43) / 1.3332239; // перевод в мм.рт.ст

  uint32_t pdt = root["dt"];
  RtcDateTime dt = RtcDateTime(pdt);

  prog.day = dt.Day();
  prog.month = dt.Month();

  prog.wind_dir = rumb_conv(dir);

  DBG_OUT_PORT.print(F("cloud.."));
  DBG_OUT_PORT.println(prog.cloud);
  DBG_OUT_PORT.print(F("wind min.."));
  DBG_OUT_PORT.println(prog.wind_min);
  DBG_OUT_PORT.print(F("win dir.."));
  DBG_OUT_PORT.println(prog.wind_dir);
  DBG_OUT_PORT.print(F("temp min.."));
  DBG_OUT_PORT.println(prog.temp_min);
  DBG_OUT_PORT.print(F("temp max.."));
  DBG_OUT_PORT.println(prog.temp_max);
  DBG_OUT_PORT.print(F("descript.."));
  DBG_OUT_PORT.println(prog.descript);
  DBG_OUT_PORT.print(F("humidity min.."));
  DBG_OUT_PORT.println(prog.hum_min);
  DBG_OUT_PORT.print(F("pressure max.."));
  DBG_OUT_PORT.println(prog.press_max);
  DBG_OUT_PORT.print(F("pressure min.."));
  DBG_OUT_PORT.println(prog.press_min);
  DBG_OUT_PORT.print(F("day.."));
  DBG_OUT_PORT.println(prog.day);
  DBG_OUT_PORT.print(F("month.."));
  DBG_OUT_PORT.println(prog.month);

  return prog;
}

// ------------------------------------------------------Удаляем квадратные скобки
String remove_sb(String in_str) {
  String out_str = String();
  uint16_t i = 0;
  while (i < in_str.length())
  {
    char c = in_str[i];
    if (c == '[' || c == ']') c = ' ';
    out_str += c;
    i++;
  }
  return out_str;
}

// ------------------------------------------------------Конвертиуем градусы в румбы (стороны света)
inline uint8_t rumb_conv(uint16_t dir)
{
  uint8_t w_dir = 0;
  if (dir > 23 && dir < 338) w_dir = ((dir - 23) / 45) + 1;
  return w_dir;
}

// ------------------------------------------------------Вырезаем данные по прогнозу на один день (из 2х)
String tvoday(String line) {
  String s = String();
  int start_sym = line.indexOf(']'); // позиция первого искомого символа ('{' после '[')

  start_sym = line.indexOf('{', start_sym + 1);

  int stop_sym = line.lastIndexOf(']');
  s = line.substring(start_sym, stop_sym);

  line = String();
  line = remove_sb(s);

  return line;
}

//------------------------------------------------------  Делаем запрос показаний датчиков с внешнего сервера
String es_rcv(char es_addr[17])
{
  if (debug_level == 10) DBG_OUT_PORT.println(F("True get data from ext server"));
  String out = "No ext srv";
  if (wifi_data_cur.cli)
  {
    String addr = "http://";
    addr += String(es_addr);
    addr += "/jsnr";
    if (debug_level == 10) DBG_OUT_PORT.println(addr);
    out = nsys.http_client(addr);
  }
  if (debug_level == 10) DBG_OUT_PORT.println(out);
  return out;
}

//------------------------------------------------------  Отправляем показания датчиков на внешний сервер
void put_to_es(char es_addr[17], uint8_t use_es, snr_data_t sd)
{
  // uint8_t dl = debug_level;
  // debug_level = 10;
  DBG_OUT_PORT.print(F("\nTrue put data to ext server -> "));

  if (wifi_data_cur.cli)
  {
    String postStr = "http://";
    postStr += String(es_addr);
    postStr += "/rcv_snr?";
    if ((use_es & 0b00000001) && sd.t1 < 99)
    {
      postStr += "&est1=";
      postStr += String(sd.t1);
    }

    if ((use_es & 0b00000010) && sd.t2 < 99)
    {
      postStr += "&est2=";
      postStr += String(sd.t2);
    }

    if ((use_es & 0b00000100) && sd.t3 < 99)
    {
      postStr += "&est3=";
      postStr += String(sd.t3);
    }

    if ((use_es & 0b00001000) && sd.h1 > 0 && sd.h1 < 100)
    {

      postStr += "&esh1=";
      postStr += String(sd.h1);
    }

    if ((use_es & 0b00010000) && sd.h2 > 0 && sd.h2 < 100)
    {
      postStr += "&esh2=";
      postStr += String(sd.h2);
    }

    if ((use_es & 0b00100000) && sd.h3 > 0 && sd.h3 < 100)
    {
      postStr += "&esh3=";
      postStr += String(sd.h3);
    }

    if ((use_es & 0b00100000) && sd.p > 700 && sd.p < 800)
    {
      postStr += "&esp=";
      postStr += String(sd.p);
    }
    //postStr += "\r\n";

    DBG_OUT_PORT.println(postStr);

    HTTPClient http;
    bool beg;
#if defined(ESP8266)
    WiFiClient client;
    beg = http.begin(client, postStr); //HTTP
#else
    beg = http.begin(postStr); //HTTP
#endif
    if (beg)
    {
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        postStr = http.getString();
        DBG_OUT_PORT.println(postStr);
      }
#if defined(ESP8266)
      delay(3000);
#endif
      http.end();
    }
  }
}

//------------------------------------------------------  Делаем запрос данных с ThingSpeak
String ts_rcv(unsigned long id, char api[17])
{
  if (debug_level == 10) DBG_OUT_PORT.println(F("True get data from TS"));
  String out = "No connect to network";
  if (wifi_data_cur.cli)
  {
    String addr = "http://api.thingspeak.com/channels/";
    addr += String(id);
    addr += "/feed/last?key=";
    addr += String(api);
    out = nsys.http_client(addr);
  }
  //if (debug_level == 10)
  DBG_OUT_PORT.print(F("TS<-R response from ts "));
  DBG_OUT_PORT.println(out);
  return out;
}

//------------------------------------------------------  Отправляем данные на ThingSpeak
String ts_snd(String inStr)
{
  if (debug_level == 10) DBG_OUT_PORT.println(F("True put data to TS"));
  String out = "No connect to network";
  if (wifi_data_cur.cli)
  {
    WiFiClient client;

    const int httpsPort = 80;
    const char* host = "api.thingspeak.com"; // Your domain

    if (!client.connect(host, httpsPort))
    {
      client.stop();
      out = "connection failed";
    }
    else
    {
      client.print(String("GET ") + inStr + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");

      out = client.readStringUntil('\r');
      client.stop();
    }
  }
  //if (debug_level == 10)
  DBG_OUT_PORT.print(F("TS->W response from ts "));
  DBG_OUT_PORT.println(out);
  return out;
}

//------------------------------------------------------  Управляем радиоприемничком по телнету
String radio_snd(String cmd)
{
  if (debug_level == 10) DBG_OUT_PORT.println(F("True put data to Radio"));
  String out = "No connect with Radio";
  if (wifi_data_cur.cli)
  {
    WiFiClient client;
    const int port = 23;

    if (!client.connect(conf_data.radio_addr, port))
    {
      client.stop();
      out = "connection failed";
    }
    else
    {
      client.print(cmd + "\r\n");
      out = client.readStringUntil('\r');
      client.stop();
    }
  }
  //if (debug_level == 10)
  DBG_OUT_PORT.print(F("Response from Radio: "));
  DBG_OUT_PORT.println(out);
  return out;
}
#endif

//------------------------------------------------------  Обрабатываем клавиатуру
void keyb_read()
{
  btn_released = btn_state_flag & digitalRead(conf_data.gpio_btn);
  if (btn_state_flag & !digitalRead(conf_data.gpio_btn)) tmr_started = true;
  if (btn_released) tmr_started = false;
  btn_state_flag = false;

  if (btn_released & (millis() - setting_ms > 150) & (millis() - setting_ms < 2000)) // держим от 0,15 до 2 сек
  {
    disp_mode++; // меняем содержимое экрана на 7ми сегментных индикаторах
    if (disp_mode > 12) disp_mode = 0;
    max_st = 7;
    runing_string_start(); //Запуск бегущей строки;
  }

#if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
  bool serv_act = (wifi_data_cur.cli || wifi_data_cur.ap);
  if (btn_released & (millis() - setting_ms > 2000 && millis() - setting_ms < 9000)) // держим от 2 до 9 сек
  {
    if (!serv_act)
    {
      serv_ms = millis();
      start_serv();  //Запускаем web морду
      if ((conf_data.type_thermo == 0) & (ram_data.type_vdrv != 5)) digitalWrite(conf_data.gpio_led, conf_data.led_pola ? LOW : HIGH); // Включаем светодиод
    }
    else
    {
      serv_ms = millis() + 60000L;
      stop_serv();  //Останавливаем web морду
      if ((conf_data.type_thermo == 0) & (ram_data.type_vdrv != 5)) digitalWrite(conf_data.gpio_led, conf_data.led_pola ? HIGH : LOW); // Выключаем светодиод
    }
  }

#endif
  if ((millis() - setting_ms > 9000) & (conf_data.type_thermo == 0) & (ram_data.type_vdrv != 5)) digitalWrite(conf_data.gpio_led, blinkColon); // Мигаем светодиодом

  if (btn_released & (millis() - setting_ms > 9000) & (millis() - setting_ms < 15000)) //держим от 9 до 15 сек
  {
#if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
    stop_serv();
    DBG_OUT_PORT.println(F("Reboot ESPp..."));
    ESP.restart();                                                                      //Перезагружаемся
#elif defined(__AVR__)
    DBG_OUT_PORT.println(F("Reboot Mega..."));
    resetFunc();
#endif
  }

  if (btn_released & (millis() - setting_ms > 15000))                  //держим больше 15 сек
  {
    if (debug_level == 10) DBG_OUT_PORT.println(F("Set default value and reboot...")); //Cбрасываем усе на дефолт и перезагружаемся
    conf_data = defaultConfig();
    conf_f = "/config.json";
    saveConfig(conf_f, conf_data);

#if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
    stop_serv();
    DBG_OUT_PORT.println(F("Reboot ESP..."));
    delay(200);
    ESP.restart();
#elif defined(__AVR_ATmega2560__)
    DBG_OUT_PORT.println(F("Reboot Mega..."));
    resetFunc();
#endif
  }
  if (millis() - setting_ms > 20000) tmr_started = false;
  if (!tmr_started) setting_ms = millis();
  /*  if (tmr_started)
    {
      DBG_OUT_PORT.print(F("Timer ..."));
      DBG_OUT_PORT.println(millis() - setting_ms);
    }
  */
}

#if defined(ESP8266)
void IRAM_ATTR isr1() //Отпускаем кнопку
{
  btn_state_flag = true;
}

#elif defined(__AVR__)
void ISR_ATTR isr1() //Отпускаем кнопку
{
  btn_state_flag = true;
}

#elif CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
void ARDUINO_ISR_ATTR isr1() //Отпускаем кнопку
{
  btn_state_flag = true;
}
#endif

//------------------------------------------------------  Отправляем данные по USART
String uart_st(snr_data_t sn, wf_data_t wf, conf_data_t cf, rtc_time_data_t rt, rtc_alm_data_t rta, uint8_t c_br)
{
  DynamicJsonDocument jsonBuffer(500);
  JsonObject json = jsonBuffer.to<JsonObject>();

  json["T"] = rt.ct;
  json["U"] = c_br;
  json["V"] = sn.t1;
  json["W"] = sn.t2;
  json["X"] = sn.h1;
  json["Y"] = sn.h2;
  json["Z"] = sn.p;
  json["M"] = rta.hour;
  json["N"] = rta.min;

  if (cf.use_pp > 0)
  {
    json["A"] = wf.tod;
    json["B"] = wf.cloud;
    json["C"] = wf.prec;
    json["D"] = wf.rpower;
    json["E"] = wf.spower;
    json["F"] = wf.press_min;
    json["G"] = wf.temp_max;
    json["H"] = wf.temp_min;
    json["I"] = wf.wind_max;
    json["J"] = wf.wind_min;
    json["K"] = wf.wind_dir;
    json["L"] = (int)(wf.hum_max + wf.hum_min) / 2;
    json["O"] = wf.day;
    json["P"] = wf.month;
  }

  json["Q"] = "End";

  String st = String();
  if (serializeJson(jsonBuffer, st) == 0) DBG_OUT_PORT.println(F("Failed write json to string"));
  return st;
}

void send_uart()
{
  DBG_OUT_PORT.println(uart_st(snr_data, wf_data, conf_data, rtc_time, rtc_alm, cur_br));
}

//------------------------------------------------------  Термостат
void Thermo(snr_data_t sn, conf_data_t cf)
{
  bool act = 0;
  if (cf.type_thermo > 0)
  {
    if (cf.src_thermo == 0)
    {
      act = (sn.t1 > cf.lb_thermo) && (sn.t1 < cf.hb_thermo);
    }
    else
    {
      act = (sn.t2 > cf.lb_thermo) && (sn.t2 < cf.hb_thermo);
    }
    if (act)
    {
      if (cf.type_thermo == 1)
      {
        //DBG_OUT_PORT.println(F("Thermostate OUT IS ON!!!");
        digitalWrite(cf.gpio_trm, HIGH);
      }
      else
      {
        //DBG_OUT_PORT.println(F("Thermostate OUT IS OFF!!!");
        digitalWrite(cf.gpio_trm, LOW);
      }
    }
    else
    {
      if (cf.type_thermo == 1)
      {
        //DBG_OUT_PORT.println(F("Thermostate OUT IS OFF!!!");
        digitalWrite(cf.gpio_trm, LOW);
      }
      else
      {
        //DBG_OUT_PORT.println(F("Thermostate OUT IS ON!!!");
        digitalWrite(cf.gpio_trm, HIGH);
      }
    }
  }
}


void alarm1_action()
{
  //  dmsg.alarm_msg(rtc_cfg.n_cur_alm, rtc_cfg.type_disp, rtc_cfg.rus_lng);  // Сообщение на индикатор

  switch (rtc_cfg.alarms[rtc_alm.num][4])     // Выполняем экшн
  {
    case 0:
      play_snd = true;
      break;

    case 1:
      rtc_time.nm_is_on = true;                       // Включаем ночной режим
      break;
    case 2:
      rtc_time.nm_is_on = false;                      // Выключаем ночной режим
      break;
    case 3:
      disp_on = true;
      if (ram_data.type_vdrv == 12)
      {
        lcd->backlight();
        lcd->display();
      }
      if (ram_data.type_vdrv == 2)
      {
        m7219->shutdown(false);
        m7219->write();
      }
      break;
    case 4:
      disp_on = false;
      cur_br = 0;
      snr_data.f = 0;
      f_dsp.CLS(screen, sizeof screen);
      switch (ram_data.type_vdrv)
      {
        case 1:
          tm1637->set_br(0);
          tm1637->clear();
          break;
        case 2:
          m7219->shutdown(true);
          m7219->write();
          break;
        case 3:
          if (conf_data.type_disp == 23 || conf_data.type_disp == 24 || conf_data.type_disp == 25)
          {
#if defined(__AVR_ATmega2560__) || CONFIG_IDF_TARGET_ESP32
            m3216_ramFormer(screen, cur_br, text_size);
#endif
          }
          break;
        case 12:
          lcd->noBacklight();
          lcd->noDisplay();
          break;
        case 11:
          ht1633->clear();
          ht1633->setBrightness(0);
          ht1633->write();
          break;
      }
      break;
    case 5:
#if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
      radio_snd("cli.start");
#endif
      break;
    case 6:
#if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3
      radio_snd("cli.stop");
#endif
      break;
  }
}

//------------------------------------------------------  Внутренняя флэшка LittleFS
#if defined(__xtensa__) || CONFIG_IDF_TARGET_ESP32C3

void printFile(const char* filename) {
  // Open file for reading
  File file = LittleFS.open(filename, "r");

  if (!file)
  {
    DBG_OUT_PORT.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) DBG_OUT_PORT.print((char)file.read());
  DBG_OUT_PORT.println();

  // Close the file
  file.close();
}

void fs_setup()
{
#if defined(ESP8266)
  if (!LittleFS.begin())
  {
    DBG_OUT_PORT.print("\n Failed to mount file system, try format it!\n");
    LittleFS.format();
  }
  else
  {
    Dir dir = LittleFS.openDir("/");
    while (dir.next())
    {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUT_PORT.printf(" FS File: %s, size: %s\n", fileName.c_str(), fsys.formatBytes(fileSize).c_str());
    }
  }
#elif CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
  if (!LittleFS.begin())
  {
    DBG_OUT_PORT.print("\n Failed to mount file system, try format it!\n");
    LittleFS.format();
  }
  else
  {
    File root = LittleFS.open("/");

    String output = "[";
    if (root.isDirectory()) {
      File file = root.openNextFile();
      while (file) {
        if (output != "[") output += ',';
        output += "{\"type\":\"";
        output += (file.isDirectory()) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += String(file.name());
        output += "\"}";
        file = root.openNextFile();
      }
      DBG_OUT_PORT.println(output);
    }
  }
# endif
}
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
void hard_restart() {
  esp_task_wdt_init(1, true);
  esp_task_wdt_add(NULL);
  while (true);
}
#endif
#endif
