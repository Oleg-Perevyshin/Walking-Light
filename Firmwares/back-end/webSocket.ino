
// ОБРАБОТЧИК СОБЫТИЙ WEBSOCKET
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    // WEBSOCKET ПОДКЛЮЧЕН
    Serial.printf("ws[%u] | Connect\n", client->id());
    client->ping();                                                 // Поддержание соединения (если закомм. предыдущий сокет отключается)
  } else if (type == WS_EVT_DISCONNECT) {                           // WEBSOCKET ОТКЛЮЧЕН
    Serial.printf("ws[%u] | Disconnect\n", client->id());
  } else if (type == WS_EVT_PONG) {
    Serial.printf("ws[%u] | Pong[%u]: %s\n", client->id(), len, (len) ? (char*)data : "");
  } else if (type == WS_EVT_ERROR) {                                // ОБРАБОТКА ОШИБОК
    Serial.printf("ws[%u] | Error(%u): %s\n", client->id(), *((uint16_t*)arg), (char*)data);
  } else if (type == WS_EVT_DATA) {                                 // ОБРАБОТКА ВХОДЯЩИХ ПАКЕТОВ WEBSOCKET
    // AsyncWebSocketClient * client = client->id();
    DynamicJsonDocument jb(4096);
    DeserializationError error = deserializeJson(jb, (char*)data);
    if (error) {
      uartSendHAD("ER!", "WS-Data", "Parser");
      return;
    }
    JsonObject root = jb.as<JsonObject>();

    if (root["actualInfo"]) {                                       // АКТУАЛЬНЫЕ ПАРАМЕТРЫ УСТРОЙСТВА
      DynamicJsonDocument jb(4096);
      JsonObject json = jb.to<JsonObject>();
      // Общие параметры
      json["actDM"] = DEVICE_MODEL; json["actDN"] = dn;
      json["actID"] = DEVICE_ID;
      json["actV"] = VERSION; json["actLV"] = latVer;
      json["actDU"] = du;
      json["actDP"] = dp;
      // Параметры "STA"
      json["actVCC"] = getVCC();
      json["actWCH"] = WiFi.channel();
      json["actWRSSI"] = WiFi.RSSI();
      json["actStaSSID"] = WiFi.SSID();
      json["actStaPSK"] = WiFi.psk();
      json["actStaIP"] = WiFi.localIP().toString();
      json["actStaMS"] = WiFi.subnetMask().toString();
      json["actStaGW"] = WiFi.gatewayIP().toString();
      json["actStaMAC"] = WiFi.macAddress();
      // Параметры "AP"
      json["actApSSID"] = apSSID;
      json["actApPSK"] = apPSK;
      json["actApIP"] = apIP;
      json["actApMS"] = apMS;
      json["actApGW"] = apGW;
      json["actApMAC"] = WiFi.softAPmacAddress();
      // Параметры Radio
      json["actDA"] = da;
      json["actRCH"] = rch;
      String actualInfo;
      serializeJson(json, actualInfo);
      sendDataWs(NULL, "actualInfo", actualInfo);
    } else if (root.containsKey("wifiScan")) {                                  // СПИСОК ДОСТУПНЫХ ТОЧЕК ДОСТУПА
      int networksFound = WiFi.scanNetworks();
      DynamicJsonDocument doc(2048);
      JsonArray apList = doc.to<JsonArray>();
      for (int i = 0; i < networksFound; ++i) {
        JsonObject net = apList.createNestedObject();
        net["ssid"] = WiFi.SSID(i);
        net["rssi"] = WiFi.RSSI(i);
        net["ch"] = WiFi.channel(i);
      }
      String netList;
      serializeJson(apList, netList);
      uartSendHAD("OK!", "netList", netList.c_str());
      sendDataWs(NULL, "NetList", netList);
    } else if (root["devsScan"]) {                                  // ЗАПУСК СКАНИРОВАНИЯ СЕТИ
      findDevices();                                                // список найденных устройств возвращается из функции udpScanStop()
    } else if (root["setID"]) {                                     // УСТАНОВИТЬ ПАРАМЕТРЫ ИДЕНТИФИКАЦИИ , dc, dn, du, dp
      strncpy(dc, root["setID"][0], sizeof(dc) - 1);
      strncpy(dn, root["setID"][1], sizeof(dn) - 1);
      strncpy(du, root["setID"][2], sizeof(du) - 1);
      strncpy(dp, root["setID"][3], sizeof(dp) - 1);
      configSave();
    } else if (root["setSTA"]) {                                    // УСТАНОВИТЬ ПАРАМЕТРЫ ПОДКЛЮЧЕНИЯ К СЕТИ staSSID, staPSK, staIPMode, staIP, staMS, staGW
      strncpy(staSSID, root["setSTA"][0], sizeof(staSSID) - 1);
      strncpy(staPSK, root["setSTA"][1], sizeof(staPSK) - 1);
      staIPMode = root["setSTA"][2].as<int>();
      strncpy(staIP, root["setSTA"][3], sizeof(staIP) - 1);
      strncpy(staMS, root["setSTA"][4], sizeof(staMS) - 1);
      strncpy(staGW, root["setSTA"][5], sizeof(staGW) - 1);
      configSave();
    } else if (root["setAP"]) {                                     // УСТАНОВИТЬ ПАРАМЕТРЫ УСТРОЙСТВА В РЕЖИМЕ ТОЧКИ ДОСТУПА apSSID, apPSK, apIP, apMS, apGW
      strncpy(apSSID, root["setAP"][0], sizeof(apSSID) - 1);
      strncpy(apPSK, root["setAP"][1], sizeof(apPSK) - 1);
      strncpy(apIP, root["setAP"][2], sizeof(apIP) - 1);
      strncpy(apMS, root["setAP"][3], sizeof(apMS) - 1);
      strncpy(apGW, root["setAP"][4], sizeof(apGW) - 1);
      configSave();
    } else if (root["setRF"]) {                                     // УСТАНОВИТЬ ПАРАМЕТРЫ РАДИО
      strncpy(da, root["setRF"][0], sizeof(da) - 1);
      strncpy(rch, root["setRF"][1], sizeof(rch) - 1);
      char rfData[sizeof(da) + sizeof(rch) + 1];
      snprintf(rfData, sizeof(rfData), "%s;%s", da, rch);
      uartSendHAD("SET", "rf", rfData);
      configSave();
    } else if (root["synch"]) {                                     // ПЕРЕДАЧА ПАРАМЕТРОВ АВТОРИЗАЦИИ (staSSID, staPSK - для рассылки по радио сети)
      char synchData[sizeof(staSSID) + sizeof(staPSK) + 1];
      snprintf(synchData, sizeof(synchData), "%s;%s", staSSID, staPSK);
      uartSendHAD("SET", "synch", synchData);
    } else if (root["dl"]) {                                        // УСТАНОВКА ЯЗЫКА ИНТЕРФЕЙСА (не работает в Web)
      strncpy(dl, root["dl"][0], sizeof(dl) - 1);
      uartSendHAD("SET", "dl", dl);
      configSave();
    } else if (root["dv"]) {                                        // УСТАНОВКА ВИЗУАЛЬНОЙ ТЕМЫ (не работает в Web)
      strncpy(dv, root["dv"][0], sizeof(dv) - 1);
      uartSendHAD("SET", "dv", dv);
      configSave();
    } else if (root["mcu"]) {                                       // ПЕРЕДАЧА КЛЮЧА И ЗНАЧЕНИЯ В MCU {"mcu":[key, val]} => ␁SET␟key␟␂val␃CRC8␄
      uartSendHAD("SET", root["mcu"][0], root["mcu"][1]);
    } else if (root["rst"]) {                                       // ПЕРЕЗАГРУЗКА УСТРОЙСТВА
      uartSendHAD("SYS", "restart", NULL);
    } else if (root["def"]) {                                       // СБРОС НАСТРОЕК ПО УМОЛЧАНИЮ
      configLoadDefaults();
      uartSendHAD("SYS", "def", NULL);
    } else                                                          // НЕВЕРНЫЕ ДАННЫЕ (НЕТ ЗАГОЛОВКА ПАКЕТА)
      uartSendEA("WS Unknown Data");
  }
  Serial.println(ESP.getFreeHeap());
}

// ********************************************************************** //
// ПЕРЕДАЧА ДАННЫХ В WEBSOCKET
// Передаем объект: { argument: data }
void sendDataWs(AsyncWebSocketClient *client, const char *argument, String value) {
  DynamicJsonDocument doc(2048);
  JsonObject root = doc.to<JsonObject>();
  root[argument] = value;
  size_t len = measureJson(doc);
  AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len);
  if (buffer) {
    serializeJson(root, (char*)buffer->get(), len + 1);
    ws.textAll(buffer);
  }
}
