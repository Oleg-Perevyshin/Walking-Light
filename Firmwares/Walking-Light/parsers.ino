
// ******************************************* //
//    ОБРАБОТЧИКИ ДАННЫХБ ПРИНЯТЫХ ПО UART     //
// ******************************************* //

// ################ //
// ## ПАРСЕР SYS ## //
// ################ //
void sysParser(const char *argument) {
  if (!strcmp(argument, "upd")) {
    // ␁SYS␟upd␟␄
    if (latVer > VERSION) {
      uartSendOAD("UpDate to Version: ", latVer);
      otaUpdate();
    } else {
      uartSendEA("No Updates");
    }
  } else if (!strcmp(argument, "upd-check")) {
    // ␁SYS␟upd-check␟␄
    if (latVer > VERSION) {
      uartSendOA(argument);
      Serial.print("Update Available | Latest FW: "); Serial.println(latVer);
    } else {
      uartSendEA(argument);
      Serial.print("No Updates | Current FW: "); Serial.println(VERSION);
    }
  } else if ("heap") {
    Serial.println(ESP.getFreeHeap());
  } else {
    uartSendEA("SYS Argument");
  }
}

// ################ //
// ## ПАРСЕР GET ## //
// ################ //
void getParser(const char *argument) {
  if (!strcmp(argument, "status"))
    switch (WiFi.status()) {
      case WL_IDLE_STATUS:
        uartSendOAD(argument, "Idle");
        break;
      case WL_SCAN_COMPLETED:
        uartSendOAD(argument, "Scan Completed");
        break;
      case WL_CONNECTED:
        uartSendOAD(argument, "Connected");
        break;
      case WL_NO_SSID_AVAIL:
      case WL_CONNECT_FAILED:
      case WL_CONNECTION_LOST:
      case WL_DISCONNECTED:
        uartSendOAD(argument, "Failed");
        break;
      default:
        uartSendOAD(argument, "Unknown Status");
    }
  else if (!strcmp(argument, "vcc")) {
    uartSendOAD(argument, getVCC());
  }
  else if (!strcmp(argument, "hn"))
    uartSendOAD(argument, WiFi.hostname().c_str());
  else if (!strcmp(argument, "ssid"))
    uartSendOAD(argument, WiFi.SSID().c_str());
  else if (!strcmp(argument, "psk"))
    uartSendOAD(argument, WiFi.psk().c_str());
  else if (!strcmp(argument, "ip"))
    uartSendOAD(argument, (WiFi.getMode() == WIFI_STA ? WiFi.localIP() : WiFi.softAPIP()).toString().c_str());
  else if (!strcmp(argument, "rssi"))
    uartSendOAD(argument, WiFi.RSSI());
  else
    uartSendEA("GET Argument");
}

// ################ //
// ## ПАРСЕР SET ## //
// ################ //
void setParser(const char * argument, const char * body) {
  if (!strcmp(argument, "da")) {
    strcpy(da, body);
    configSave();
    uartSendOA(argument);
  } else if (!strcmp(argument, "dl")) {
    strcpy(dl, body);
    configSave();
    uartSendOA(argument);
  } else if (!strcmp(argument, "staSSID")) {
    strcpy(staSSID, body);
    configSave();
    uartSendOA(argument);
  } else if (!strcmp(argument, "staPSK")) {
    strcpy(staPSK, body);
    configSave();
    uartSendOA(argument);
  } else {
    uartSendEA("SET Argument");
  }
}

// ################ //
// ## ПАРСЕР OK! ## //
// ################ //
void okParser(const char *argument, const char *body) {
  // Системные
  if (!strcmp(argument, "ready"))           // MCU готов к обмену
    void ();
  else if (!strcmp(argument, "status")) {
    //parsingStatusMCU();                   // Радио адрес, частотный канал, уровень приема, качество сигнала...
  }
  else if (!strcmp(argument, "rst"))        // MCU скоро инициирует перезагрузку
    void ();
  else if (!strcmp(argument, "def"))        // MCU загрузил параметры по умолчанию
    void ();
  else if (!strcmp(argument, "upd"))        // MCU готов к обновлению
    void ();
  else if (!strcmp(argument, "da"))         // Установлен новый радио адрес
    void ();
  else if (!strcmp(argument, "settings"))   // Парсим "Settings" и возвращаем в WEB (Пример: ␁OK!␟settings␟␂24;2;5;60;1;9;0;15;7;2;1;9;3;10;1;9;3;10;1;9;3;10;1;9;3;10;0;0;␃52␄)
    void ();
  //parsSettings(body);
  else if (!strcmp(argument, "api"))        // Возвращаем API в WEB (Пример: ␁OK!␟api␟␂[[10,10],[10,10]]␃67␄)
    ws.textAll(body);
  else if (!strcmp(argument, "sc"))         // Парсим "Сценарии" и возвращаем в WEB
    void ();
  //parsSCFromMCU(body);
  // Настройки
  else if (!strcmp(argument, "gac") || !strcmp(argument, "glt") || !strcmp(argument, "gcs") || !strcmp(argument, "gpi") || !strcmp(argument, "gmd") || !strcmp(argument, "gsp") || !strcmp(argument, "glb") || !strcmp(argument, "gub") || !strcmp(argument, "gsb") || !strcmp(argument, "gsh"))
    ws.textAll(body);
  // Тема
  else if (!strcmp(argument, "sch"))        // Произошло сохранение Визуальная темы
    ws.textAll(body);
  else if (!strcmp(argument, "stp") || !strcmp(argument, "itp") || !strcmp(argument, "ftp") || !strcmp(argument, "otp") || !strcmp(argument, "ssp") || !strcmp(argument, "isp") || !strcmp(argument, "fsp") || !strcmp(argument, "osp") || !strcmp(argument, "slb") || !strcmp(argument, "ilb") || !strcmp(argument, "flb") || !strcmp(argument, "olb") || !strcmp(argument, "sub") || !strcmp(argument, "iub") || !strcmp(argument, "fub") || !strcmp(argument, "oub"))
    ws.textAll(body);
  else if (!strcmp(argument, "prv"))        // Запущен режим предпросмотра визуальной темы
    ws.textAll(body);
  else
    uartSendEA("OK! Argument");
}

// ################ //
// ## ПАРСЕР ER! ## //
// ################ //
void erParser(const char *argument, const char *body) {
  void();
}




// ******************************************* //
//      ВНУТРЕННИЕ ФУНКЦИИ ОБРАБОТЧИКОВ        //
// ******************************************* //

// Парсим ответ SETTINGS и возвращаем в Web
void parsSettings(char *body) {
  int i = 0;
  char *arr_settings[27];
  char *chars_array = strtok(body, ";");
  arr_settings[i] = chars_array;
  i++;
  while (chars_array) {
    chars_array = strtok(NULL, ";");
    arr_settings[i] = chars_array;
    i++;
  }
  DynamicJsonDocument jsonBuffer(2048);
  JsonObject settings = jsonBuffer.to<JsonObject>();
  settings["gac"] = arr_settings[0];
  settings["glt"] = arr_settings[1];
  settings["gcs"] = arr_settings[2];
  settings["gpi"] = arr_settings[3];
  settings["gmd"] = arr_settings[4];
  settings["gsp"] = arr_settings[5];
  settings["glb"] = arr_settings[6];
  settings["gub"] = arr_settings[7];
  settings["gsb"] = arr_settings[8];
  settings["gsh"] = arr_settings[9];
  settings["stp"] = arr_settings[10];
  settings["ssp"] = arr_settings[11];
  settings["slb"] = arr_settings[12];
  settings["sub"] = arr_settings[13];
  settings["itp"] = arr_settings[14];
  settings["isp"] = arr_settings[15];
  settings["ilb"] = arr_settings[16];
  settings["iub"] = arr_settings[17];
  settings["ftp"] = arr_settings[18];
  settings["fsp"] = arr_settings[19];
  settings["flb"] = arr_settings[20];
  settings["fub"] = arr_settings[21];
  settings["otp"] = arr_settings[22];
  settings["osp"] = arr_settings[23];
  settings["olb"] = arr_settings[24];
  settings["oub"] = arr_settings[25];
  String response;
  serializeJson(settings, response);
  ws.textAll(response);
}

// Парсим ответ SC и возвращаем в Web (ответ от MCU пишем в файл scripts_mcu и затем парсим данные из файла)
void parsSCFromMCU(char *body) {
//  char *data = body;
//  File f = SPIFFS.open("/scripts_mcu", "w");
//  f.print(data);
//  f.flush();
//  f.close();
//
//  if (!SPIFFS.exists("/script_names")) {
//    File f = SPIFFS.open("/script_names", "w");
//    f.flush();
//    f.close();
//  }
//
//  File sf = SPIFFS.open("/scripts_mcu", "r");
//  File snf = SPIFFS.open("/script_names", "r");
//  DynamicJsonDocument jsonBuffer(2048);
//  JsonObject json = jsonBuffer.to<JsonObject>();
//  bool finished = false;
//  int pos = -1;
//  int id = 0; // script id
//  int elem = 0; // element (id, custom_param, events, actions)
//  int is_radio_addr = true;
//  bool create_ea = false;
//  bool create_param = false;
//  bool is_option = false;
//  char c, nc;
//  char script_name[17];
//  bool enabled = false;
//  char custom_param[9] = "";
//  char script_radio_addr[7] = "";
//  char param_key[4] = "";
//  char param_option[4] = "";
//  JsonVariant script, events, actions, ea, params, param;
//  while (sf.available() > 0 && !finished) {
//    c = sf.read();
//    pos++;
//    switch (c) {
//      case '\n':
//        if (elem != 3) {
//          finished = true;
//          continue;
//        }
//        if (strlen(param_option) > 0) {
//          param.as<JsonArray>().add(jsonBuffer.strdup(param_option));
//          param_option[0] = '\0';
//        }
//        id++;
//        enabled = false;
//        custom_param[0] = '\0';
//        script_radio_addr[0] = '\0';
//        elem = 0;
//        is_radio_addr = true;
//        create_ea = false;
//        create_param = false;
//        is_option = false;
//        continue;
//      case ';':
//        if (strlen(param_option) > 0) {
//          param.as<JsonArray>().add(jsonBuffer.strdup(param_option));
//          param_option[0] = '\0';
//        }
//        if (elem == 1)
//          script["custom_param"] = jsonBuffer.strdup(custom_param);
//        elem++;
//        is_radio_addr = true;
//        create_ea = false;
//        create_param = false;
//        is_option = false;
//        continue;
//      case ':':
//        if (strlen(param_option) > 0) {
//          param.as<JsonArray>().add(jsonBuffer.strdup(param_option));
//          param_option[0] = '\0';
//        }
//        is_radio_addr = !is_radio_addr;
//        create_ea = !is_radio_addr;
//        create_param = false;
//        is_option = false;
//        continue;
//      case ',':
//        if (strlen(param_option) > 0) {
//          param.as<JsonArray>().add(jsonBuffer.strdup(param_option));
//          param_option[0] = '\0';
//        }
//        create_param = true;
//        is_option = false;
//        continue;
//      case '.':
//        is_option = true;
//        continue;
//    }
//    switch (elem) {
//      case 0:
//        script = json.createNestedObject();
//        events = script.as<JsonObject>().createNestedArray("events");
//        actions = script.as<JsonObject>().createNestedArray("actions");
//        script["id"] = id;
//        script["enabled"] = c != '0';
//        script_name[0] = '\0';
//        while (snf.available()) {
//          nc = snf.read();
//          if (nc == '\n')
//            break;
//          snprintf(script_name, sizeof(script_name), "%s%c", script_name, nc);
//        }
//        script["name"] = jsonBuffer.strdup(script_name);
//        break;
//      case 1:
//        snprintf(custom_param, sizeof(custom_param), "%s%c", custom_param, c);
//        break;
//      case 2:
//      case 3:
//        if (is_radio_addr) {
//          snprintf(script_radio_addr, sizeof(script_radio_addr), "%s%c", script_radio_addr, c);
//        } else {
//          if (create_ea) {
//            if (elem == 2) {
//              ea = events.as<JsonArray>().createNestedObject();
//            } else if (elem == 3) {
//              ea = actions.as<JsonArray>().createNestedObject();
//            }
//            params = ea.as<JsonObject>().createNestedArray("params");
//            create_ea = false;
//            create_param = true;
//          } else if (create_param) {
//            param = params.as<JsonArray>().createNestedArray();
//            create_param = false;
//          }
//          if (strlen(script_radio_addr) > 0) {
//            ea["da"] = jsonBuffer.strdup(script_radio_addr);
//            script_radio_addr[0] = '\0';
//          }
//          if (!is_option) {
//            snprintf(param_key, sizeof(param_key), "%s%c", param_key, c);
//          } else {
//            if (strlen(param_key) > 0) {
//              param.as<JsonArray>().add(jsonBuffer.strdup(param_key));
//              param_key[0] = '\0';
//            }
//            snprintf(param_option, sizeof(param_option), "%s%c", param_option, c);
//          }
//        }
//        break;
//    }
//  }
//  sf.close();
//  snf.close();
//  String response = "[]";
//  serializeJson(json, response);
//  ws.textAll(response);
}
