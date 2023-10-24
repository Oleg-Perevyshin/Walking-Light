
#include "lwip/inet.h"
#include "progMemory.h"


void onRequest(AsyncWebServerRequest *request) {
  if (request->method() == HTTP_OPTIONS) {
    request->send(200, "text/plain; charset=utf-8", "OK");
  } else {
    request->send(404, "text/plain; charset=utf-8", "Not Found");
  }
}

// Закачка файла прошивки MCU //
void mcuHexUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    Serial.printf("UploadStart: %s\n", filename.c_str());
    if (mcuHex)
      mcuHex.close();
    SPIFFS.remove(MCU_HEX_PATH);
    mcuHex = SPIFFS.open(MCU_HEX_PATH, "w");
  }
  Serial.printf("%s", (const char*)data);
  for (size_t i = 0; i < len; i++) {
    char(mcuHex.write(data[i]));
    //mcuHex.write(data[i]);
  }
  if (final) {
    mcuHex.flush();
    mcuHex.close();
    Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
  }
}

// ******************************************* //
//                 SERVER INIT                 //
// ******************************************* //
void serverInit() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);       // Асинхронный WebSocket
  server.addHandler(&events);

  // SSDP
  server.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest * request) {
    StreamString output;
    if (output.reserve(1024)) {
      if (!WiFi.getMode() == WIFI_STA)
        return;
      uint32_t ip = WiFi.localIP();
      uint32_t chipId = ESP.getChipId();
      output.printf(
        ssdpTemplate,
//        IP2STR(&ip),                        // URLBase            (IP адрес устройства)
        DEVICE_MODEL, dn,                   // friendlyName       (WalkingLight | Пользовательское имя)
        VENDOR,                             // manufacturer       (SOI Tech)
        VENDOR_URL,                         // manufacturerURL    (http://www.soi-tech.com/)
        DEVICE_MODEL,                       // modelName          (WalkingLight)
        DEVICE_ID,                          // modelNumber        (28)
        DEVICE_URL,                         // modelURL           (http://www.soi-tech.com/walkinglight/)
        da,                                 // serialNumber       (Адрес устройства в Радио сети)
        (uint8_t) ((chipId >> 16) & 0xff),  // UDN
        (uint8_t) ((chipId >>  8) & 0xff),  // UDN
        (uint8_t)   chipId        & 0xff    // UDN EDC3B25E-C2D0-411E-B04B-F9BD5E%02x%02x%02x
      );
      request->send(200, "text/xml; charset=utf-8", (String)output);
    } else
      request->send(500);
  });

  // INDEX.HTM
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(du, dp))
      return request->requestAuthentication();
    AsyncWebServerResponse *response;
    response = request->beginResponse_P(200, "text/html; charset=utf-8", index_htm);
    response->addHeader("Server", VENDOR);
    request->send(response);
  });

  // ИКОНКА УСТРОЙСТВА ДЛЯ МЕНЕДЖЕРА СЕТИ
  server.on("/icon", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/svg+xml; charset=utf-8", icon);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  // DEV.HTM
  server.on("/dev", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html; charset=utf-8", dev);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  // CONFIG.JSON FULL
  server.on("/config.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/config.json", "text/json; charset=utf-8");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  // ПОДГРУЗКА СООТВЕТСТВУЮЩЕГО ЯЗЫКОВОГО ПАКЕТА (dl/*.json)
  server.on("/lang", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response;
    if (request->arg("dl").equals("de.json"))
      response = request->beginResponse(SPIFFS, "/dl/de.json", "text/json; charset=utf-8");
    else if (request->arg("dl").equals("es.json"))
      response = request->beginResponse(SPIFFS, "/dl/es.json", "text/json; charset=utf-8");
    else if (request->arg("dl").equals("fr.json"))
      response = request->beginResponse(SPIFFS, "/dl/fr.json", "text/json; charset=utf-8");
    else if (request->arg("dl").equals("it.json"))
      response = request->beginResponse(SPIFFS, "/dl/it.json", "text/json; charset=utf-8");
    else if (request->arg("dl").equals("ru.json"))
      response = request->beginResponse(SPIFFS, "/dl/ru.json", "text/json; charset=utf-8");
    else
      response = request->beginResponse(SPIFFS, "/dl/en.json", "text/json; charset=utf-8");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  // УСТАНОВИТЬ ПАРАМЕТРЫ ИНТЕРФЕЙСА dl (Device Language), dv (Device Visual Design)
  server.on("/interface", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("dl")) {
      strncpy(dl, request->arg("dl").c_str(), sizeof(dl) - 1);
      uartSendHAD("SET", "dl", dl);
    } else if (request->hasArg("dv")) {
      strncpy(dv, request->arg("dv").c_str(), sizeof(dv) - 1);
      uartSendHAD("SET", "dv", dv);
    }
    configSave();
    request->send(200, "text/plain; charset=utf-8", "OK");
  });

  // ******************************************* //
  //          ФУНКЦИИ ДЛЯ РАБОТЫ С MCU           //
  // ******************************************* //
  // ПОЛУЧИТЬ API УСТРОЙСТВА
  server.on("/mcu/api", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/json; charset=utf-8", "[[3,2,5],[4,5]]");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    uartSendHAD("GET", "api", NULL);
  });

  // ПОЛУЧИТЬ СЦЕНАРИИ
  server.on("/mcu/scr", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("text/json; charset=utf-8");
    response->addHeader("Access-Control-Allow-Origin", "*");
//    File sf = SPIFFS.open("/scripts_mcu", "r");
//    File snf = SPIFFS.open("/script_names", "r");
//    DynamicJsonBuffer jb;
//    JsonArray& json = jb.createArray();
//    bool finished = false;
//    int pos = -1;
//    int id = 0; // script id
//    int elem = 0; // element (id, custom_param, events, actions)
//    int is_radio_addr = true;
//    bool create_ea = false;
//    bool create_param = false;
//    bool is_option = false;
//    char c, nc;
//    char script_name[64];
//    bool enabled = false;
//    char custom_param[9] = "";
//    char script_radio_addr[7] = "";
//    char param_key[4] = "";
//    char param_option[4] = "";
//    JsonVariant script, events, actions, ea, params, param;
//    while (sf.available() > 0 && !finished) {
//      c = sf.read();
//      pos++;
//      switch (c) {
//        case '\n':
//          if (elem != 3) {
//            finished = true;
//            continue;
//          }
//          if (strlen(param_option) > 0) {
//            param.as<JsonArray>().add(jb.strdup(param_option));
//            param_option[0] = '\0';
//          }
//          id++;
//          enabled = false;
//          custom_param[0] = '\0';
//          script_radio_addr[0] = '\0';
//          elem = 0;
//          is_radio_addr = true;
//          create_ea = false;
//          create_param = false;
//          is_option = false;
//          continue;
//        case ';':
//          if (strlen(param_option) > 0) {
//            param.as<JsonArray>().add(jb.strdup(param_option));
//            param_option[0] = '\0';
//          }
//          if (elem == 1)
//            script["custom_param"] = jb.strdup(custom_param);
//          elem++;
//          is_radio_addr = true;
//          create_ea = false;
//          create_param = false;
//          is_option = false;
//          continue;
//        case ':':
//          if (strlen(param_option) > 0) {
//            param.as<JsonArray>().add(jb.strdup(param_option));
//            param_option[0] = '\0';
//          }
//          is_radio_addr = !is_radio_addr;
//          create_ea = !is_radio_addr;
//          create_param = false;
//          is_option = false;
//          continue;
//        case ',':
//          if (strlen(param_option) > 0) {
//            param.as<JsonArray>().add(jb.strdup(param_option));
//            param_option[0] = '\0';
//          }
//          create_param = true;
//          is_option = false;
//          continue;
//        case '.':
//          is_option = true;
//          continue;
//      }
//      switch (elem) {
//        case 0:
//          script = json.createNestedObject();
//          events = script.as<JsonObject>().createNestedArray("events");
//          actions = script.as<JsonObject>().createNestedArray("actions");
//          script["id"] = id;
//          script["enabled"] = c != '0';
//          script_name[0] = '\0';
//          while (snf.available()) {
//            nc = snf.read();
//            if (nc == '\n')
//              break;
//            snprintf(script_name, sizeof(script_name), "%s%c", script_name, nc);
//          }
//          script["name"] = jb.strdup(script_name);
//          break;
//        case 1:
//          snprintf(custom_param, sizeof(custom_param), "%s%c", custom_param, c);
//          break;
//        case 2:
//        case 3:
//          if (is_radio_addr) {
//            snprintf(script_radio_addr, sizeof(script_radio_addr), "%s%c", script_radio_addr, c);
//          } else {
//            if (create_ea) {
//              if (elem == 2) {
//                ea = events.as<JsonArray>().createNestedObject();
//              } else if (elem == 3) {
//                ea = actions.as<JsonArray>().createNestedObject();
//              }
//              params = ea.as<JsonObject>().createNestedArray("params");
//              create_ea = false;
//              create_param = true;
//            } else if (create_param) {
//              param = params.as<JsonArray>().createNestedArray();
//              create_param = false;
//            }
//            if (strlen(script_radio_addr) > 0) {
//              ea["da"] = jb.strdup(script_radio_addr);
//              script_radio_addr[0] = '\0';
//            }
//            if (!is_option) {
//              snprintf(param_key, sizeof(param_key), "%s%c", param_key, c);
//            } else {
//              if (strlen(param_key) > 0) {
//                param.as<JsonArray>().add(jb.strdup(param_key));
//                param_key[0] = '\0';
//              }
//              snprintf(param_option, sizeof(param_option), "%s%c", param_option, c);
//            }
//          }
//          break;
//      }
//    }
//    sf.close();
//    snf.close();
//    json.printTo(*response);
    request->send(response);
    uartSendHAD("GET", "sc", NULL);
  });

  // СОХРАНИТЬ СЦЕНАРИИ
  server.on("/mcu/scw", HTTP_POST, [](AsyncWebServerRequest * request) {},
  [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {},
  [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
//    File sf = SPIFFS.open("/scripts_mcu", "w");
//    File snf = SPIFFS.open("/script_names", "w");
//    DynamicJsonBuffer jb;
//    JsonArray& json = jb.parseArray((char*)data);
//    int max_len = ((2 + 33 + 256 + 256 + 1) * json.size()) + 1;
//    char *sc = (char*)malloc(max_len);
//    if (sc == NULL) {
//      free(sc);
//      sf.close();
//      snf.close();
//      return request->send(500, "text/plain", "Low Memory");
//    }
//    sc[0] = '\0';
//    for (JsonArray::iterator it = json.begin(); it != json.end(); ++it) {
//      JsonObject& script = it->as<JsonObject>();
//      int enabled = script["enabled"].as<bool>() ? 1 : 0;
//      char custom_param[5];
//      const char *custom_param_in = script["custom_param"].as<const char*>();
//      int custom_param_pos = 0, custom_param_in_pos = 0;
//      while (custom_param_pos < 8) {
//        if ('0' <= custom_param_in[custom_param_in_pos] <= '9') {
//          custom_param[custom_param_pos] = custom_param_in[custom_param_in_pos];
//          custom_param_pos++;
//        }
//        custom_param_in_pos++;
//      }
//      custom_param[custom_param_pos] = '\0';
//      snf.printf("%s\n", script["name"].as<const char*>());
//      JsonArray& eventsArray = script["events"].as<JsonArray>();
//      char events[256] = "";
//      bool first_e = true;
//      for (JsonArray::iterator eit = eventsArray.begin(); eit != eventsArray.end(); ++eit) {
//        if (!first_e)
//          strncat(events, ":", sizeof(events) - 1 - strlen(events));
//        first_e = false;
//        JsonObject& eventsObject = eit->as<JsonObject>();
//        snprintf(events, sizeof(events), "%s%s:", events, eventsObject["da"].as<const char*>());
//        JsonArray& eventsParamArray = eventsObject["params"].as<JsonArray>();
//        bool first_ep = true;
//        for (JsonArray::iterator epit = eventsParamArray.begin(); epit != eventsParamArray.end(); ++epit) {
//          if (!first_ep)
//            strncat(events, ",", sizeof(events) - 1 - strlen(events));
//          first_ep = false;
//          JsonArray& event_params = epit->as<JsonArray>();
//          snprintf(
//            events, sizeof(events), "%s%d.%d",
//            events, event_params[0].as<int>(), event_params[1].as<int>()
//          );
//        }
//      }
//      JsonArray& actionsArray = script["actions"].as<JsonArray>();
//      char actions[256] = "";
//      bool first_a = true;
//      for (JsonArray::iterator ait = actionsArray.begin(); ait != actionsArray.end(); ++ait) {
//        if (!first_a)
//          strncat(actions, ":", sizeof(actions) - 1 - strlen(actions));
//        first_a = false;
//        JsonObject& actionsObject = ait->as<JsonObject>();
//        snprintf(actions, sizeof(actions), "%s%s:", actions, actionsObject["da"].as<const char*>());
//        JsonArray& actionsParamArray = actionsObject["params"].as<JsonArray>();
//        bool first_ap = true;
//        for (JsonArray::iterator apit = actionsParamArray.begin(); apit != actionsParamArray.end(); ++apit) {
//          if (!first_ap)
//            strncat(actions, ",", sizeof(actions) - 1 - strlen(actions));
//          first_ap = false;
//          JsonArray& action_params = apit->as<JsonArray>();
//          snprintf(
//            actions, sizeof(actions), "%s%s.%s",
//            actions, action_params[0].as<const char*>(), action_params[1].as<const char*>()
//          );
//        }
//      }
//      sf.printf("%d;%s;%s;%s\n", enabled, custom_param, events, actions);
//      snprintf(sc, max_len, "%s%d;%s;%s;%s\n", sc, enabled, custom_param, events, actions);
//    }
//    uartSendHAD("SET", "sc", sc);
//    free(sc);
//    sf.close();
//    snf.close();
    request->send(200, "text/plain; charset=utf-8", "OK");
  });

  // Получить настройки устройства из MCU
  server.on("/mcu/settings", HTTP_GET, [](AsyncWebServerRequest * request) {
    uartSendHAD("GET", "settings", NULL);
  });

  // Получить сервисные параметры из MCU
  server.on("/mcu/srv", HTTP_GET, [](AsyncWebServerRequest * request) {
    uartSendHAD("GET", "srv", NULL);
  });

  // Сохранить текущую визуальную схему
  server.on("/mcu/sch", HTTP_GET, [](AsyncWebServerRequest * request) {
    char gsh[3], stp[3], ssp[3], slb[3], sub[3], itp[3], isp[3], ilb[3], iub[3], ftp[3], fsp[3], flb[3], fub[3], otp[3], osp[3], olb[3], oub[3];
    strncpy(gsh, request->arg("gsh").c_str(), sizeof(gsh) - 1);
    strncpy(stp, request->arg("stp").c_str(), sizeof(stp) - 1);
    strncpy(ssp, request->arg("ssp").c_str(), sizeof(ssp) - 1);
    strncpy(slb, request->arg("slb").c_str(), sizeof(slb) - 1);
    strncpy(sub, request->arg("sub").c_str(), sizeof(sub) - 1);
    strncpy(itp, request->arg("itp").c_str(), sizeof(itp) - 1);
    strncpy(isp, request->arg("isp").c_str(), sizeof(isp) - 1);
    strncpy(ilb, request->arg("ilb").c_str(), sizeof(ilb) - 1);
    strncpy(iub, request->arg("iub").c_str(), sizeof(iub) - 1);
    strncpy(ftp, request->arg("ftp").c_str(), sizeof(ftp) - 1);
    strncpy(fsp, request->arg("fsp").c_str(), sizeof(fsp) - 1);
    strncpy(flb, request->arg("flb").c_str(), sizeof(flb) - 1);
    strncpy(fub, request->arg("fub").c_str(), sizeof(fub) - 1);
    strncpy(otp, request->arg("otp").c_str(), sizeof(otp) - 1);
    strncpy(osp, request->arg("osp").c_str(), sizeof(osp) - 1);
    strncpy(olb, request->arg("olb").c_str(), sizeof(olb) - 1);
    strncpy(oub, request->arg("oub").c_str(), sizeof(oub) - 1);
    char data[sizeof(gsh) + sizeof(stp) + sizeof(ssp) + sizeof(slb) + sizeof(sub) + sizeof(itp) + sizeof(isp) + sizeof(ilb) + sizeof(iub) + sizeof(ftp) + sizeof(fsp) + sizeof(flb) + sizeof(fub) + sizeof(otp) + sizeof(osp) + sizeof(olb) + sizeof(oub)];
    snprintf(data, sizeof(data),    "%s;"
             "%s;%s;%s;%s;"
             "%s;%s;%s;%s;"
             "%s;%s;%s;%s;"
             "%s;%s;%s;%s;",
             gsh,
             stp, ssp, slb, sub,
             itp, isp, ilb, iub,
             ftp, fsp, flb, fub,
             otp, osp, olb, oub
            );
    uartSendHAD("SET", "sch", data);    // [SOH]SET[US]sch[US][STX]gsh;stp;ssp;slb;sub;itp;isp;ilb;iub;ftp;fsp;flb;fub;otp;osp;olb;oub[ETX]CRC8[EOT]
    request->send(200, "text/plain; charset=utf-8", "OK");
  });

  // ******************************************* //
  //       ФУНКЦИИ ДЛЯ ТЕСТА ОБНОВЛЕНИЯ          //
  // ******************************************* //
  server.on("/ota/check", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (latVer > VERSION) {
      request->send(200, "text/plain; charset=utf-8", "true");
    } else {
      request->send(200, "text/plain; charset=utf-8", "false");
    }
  });

  // Обновление прошивок ESP
  server.on("/ota/upd", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (latVer > VERSION)
      otaUpdate();
    request->send(200, "text/plain; charset=utf-8", "OK");
  });

  // Загрузка файла прошивки MCU в SPIFFS
  server.on("/mcu/upload", HTTP_POST, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain; charset=utf-8", "OK");
  }, mcuHexUpload);

  // Выполнить обновление прошивок MCU
  server.on("/mcu/upd", HTTP_GET, [](AsyncWebServerRequest * request) {
    mcuUpdateInit();
    request->send(200, "text/plain; charset=utf-8", "OK");
  });

  // =========================== //
  // Simple Firmware Update Form //
  // =========================== //
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    flagReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", flagReboot ? "OK" : "FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      Serial.printf("Update Start: %s\n", filename.c_str());
      Update.runAsync(true);
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
        Update.printError(Serial);
      }
    }
    if (!Update.hasError()) {
      if (Update.write(data, len) != len) {
        Update.printError(Serial);
      }
    }
    if (final) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %uB\n", index + len);
      } else {
        Update.printError(Serial);
      }
    }
  });

  // ******************************************* //
  server.serveStatic("/", SPIFFS, "/");   // Доступ к файловой системе
  server.onNotFound(onRequest);
  server.onFileUpload(mcuHexUpload);
  server.begin();
}
