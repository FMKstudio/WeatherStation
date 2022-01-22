conf_data_t loadConfig(const char *filename)
{
  conf_data_t _data;

  File file = LittleFS.open(filename, "r");

  if (!file)
  {
    DBG_OUT_PORT.print(F("Failed to open "));
    DBG_OUT_PORT.print(filename);
    DBG_OUT_PORT.println(F(", using default configuration"));
    _data = defaultConfig();
    saveConfig(conf_f, _data);
  }
  else
  {
    // Allocate the document on the stack.
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/assistant to compute the capacity.
    DynamicJsonDocument doc(3100);

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
      DBG_OUT_PORT.print(F("deserializeJson() for configFile failed: "));
      DBG_OUT_PORT.println(error.c_str());
      DBG_OUT_PORT.println(F("Using default configuration"));
      _data = defaultConfig();
      saveConfig(conf_f, _data);
      return _data;
    }

    file.close();

    if (!error)
    {
      DBG_OUT_PORT.println(F("Read configFile sucsses!!!"));

      // Get the root object in the document
      memset(_data.sta_ssid,   0, 17);
      memset(_data.sta_pass,   0, 17);
      memset(_data.ap_ssid,    0, 17);
      memset(_data.ap_pass ,   0, 17);

      //---Wifi.html----------------------------------------
      //---AP-----------------------------------------------
      strcpy(_data.ap_ssid,    doc["ap_ssid"]);
      strcpy(_data.ap_pass,    doc["ap_pass"]);

      //---STA----------------------------------------------
      strcpy(_data.sta_ssid,   doc["sta_ssid"]);
      strcpy(_data.sta_pass,   doc["sta_pass"]);

      _data.str1_on              = doc["on1_code"];
      _data.str1_off             = doc["off1_code"];
      _data.str2_on              = doc["on2_code"];
      _data.str2_off             = doc["off2_code"];
      _data.pin1                 = doc["pin1"];
      _data.pin2                 = doc["pin2"];
      _data.lim_l                = doc["lim_l"];
      _data.lim_h                = doc["lim_h"];
    }
    else
    {
      DBG_OUT_PORT.print(F("deserializeJson() failed: "));
      DBG_OUT_PORT.println(error.c_str());
      DBG_OUT_PORT.println(F("Failed to read configFile, using default configuration"));
      _data = defaultConfig();
      saveConfig(conf_f, _data);
    }
  }
  return _data;
}

void saveConfig(const char *filename, conf_data_t _data)
{
  if (debug_level == 3) DBG_OUT_PORT.println(F("Start saving conf_data to config.json"));

  if ( _data.ap_ssid[0] == ' ' || _data.ap_ssid[0] ==  0) strcpy( _data.ap_ssid, "Radio_Clock");

  DynamicJsonDocument doc(3000);
  JsonObject json = doc.to<JsonObject>();

  //---Wifi.html----------------------------------------
  //---AP-----------------------------------------------
  json["ap_ssid"]             = _data.ap_ssid;
  json["ap_pass"]             = _data.ap_pass;

  //---STA----------------------------------------------
  json["sta_ssid"]            = _data.sta_ssid;
  json["sta_pass"]            = _data.sta_pass;


  json["pin1"]                = _data.pin1;
  json["pin2"]                = _data.pin2;

  json["on1_code"]            = _data.str1_on;
  json["off1_code"]           = _data.str1_off;
  json["on2_code"]            = _data.str2_on;
  json["off2_code"]           = _data.str2_off;

  json["lim_l"]               = _data.lim_l;
  json["lim_h"]               = _data.lim_h;

  // Delete existing file, otherwise the configuration is appended to the file
  LittleFS.remove(filename);
  File configFile = LittleFS.open(filename, "w"); //Open config file for writing
  if (!configFile)
  {
    DBG_OUT_PORT.println(F("Failed to open config file for writing"));
    return;
  }
  if (serializeJson(doc, configFile) == 0) DBG_OUT_PORT.println(F("Failed write to file"));
  DBG_OUT_PORT.println(F("End write buffer to file"));
  configFile.close();
}

conf_data_t defaultConfig()
{
  conf_data_t _data;

  // ---------------------------------------------------- WiFi Default

  if (debug_level == 3) DBG_OUT_PORT.println(F("Start inital conf_data with config.json"));

  strcpy(_data.sta_ssid,  "MyWiFi");
  strcpy(_data.sta_pass,  "12345678");
  strcpy(_data.ap_ssid,   "Relay");
  strcpy(_data.ap_pass ,  "12345678");

  _data.str1_on = 10;
  _data.str1_off = 20;
  _data.str2_on = 30;
  _data.str2_off = 40;
  _data.pin1 = 13;
  _data.pin2 = 12;
  _data.lim_l = 128;
  _data.lim_h = 128;

  return _data;
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
      DBG_OUT_PORT.printf(" FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
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

String formatBytes(uint32_t bytes)
{
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024 / 1024) + "MB";
  } else {
    return String(bytes / 1024 / 1024 / 1024) + "GB";
  }
}
