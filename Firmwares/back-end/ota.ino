
void flagUpdate() {
  flagCheckUpdate = true;
}


// ================================== //
// ПРОВЕРКА НАЛИЧИЯ ОБНОВЛЕНИЯ (HTTP) //
// ================================== //
void otaAvailable() {
  flagCheckUpdate = false;
  if (WiFi.status() == WL_CONNECTED) {
    if (aClient)
      return;
    else
      aClient = new AsyncClient();

    if (!aClient)
      return;

    aClient->onError([](void * arg, AsyncClient * client, int error) {
      aClient = NULL;
      delete client;
    }, NULL);

    aClient->onConnect([](void * arg, AsyncClient * client) {
      aClient->onError(NULL, NULL);
      client->onDisconnect([](void * arg, AsyncClient * c) {
        aClient = NULL;
        delete c;
      }, NULL);
      client->onData([](void * arg, AsyncClient * c, void * data, size_t len) {
        if (strstr((char*)data, "404 Not Found")) {
          uartSendEA("UpDate Files Not Found");
          return;
        }
        const char * response = strstr((char*)data, "\r\n\r\n") + 4;
        const char * _espMD5 = strchr(response, ' ') + 1;
        const char * _ffsMD5 = strchr(_espMD5, ' ') + 1;
        const char * _mcuMD5 = strchr(_ffsMD5, ' ') + 1;
        latVer = strtol(response, NULL, 10);
        strncpy(espMD5, _espMD5, 32);
        espMD5[32] = '\0';
        strncpy(ffsMD5, _ffsMD5, 32);
        ffsMD5[32] = '\0';
        strncpy(mcuMD5, _mcuMD5, 32);
        mcuMD5[32] = '\0';
        uartSendOAD("latVer: ", latVer);
        uartSendOAD("espMD5: ", espMD5);
        uartSendOAD("ffsMD5: ", ffsMD5);
        uartSendOAD("mcuMD5: ", mcuMD5);
      }, NULL);
      char request[128];
      sprintf(request,
              "GET %s HTTP/1.1\r\n"
              "Host: %s\r\n"
              "User-Agent: %s\r\n"
              "Connection: close\r\n\r\n",
              OTA_PATH, OTA_HOST, DEVICE_MODEL);
      client->write(request);
    }, NULL);

    if (!aClient->connect(OTA_HOST, 80)) {
      AsyncClient * client = aClient;
      aClient = NULL;
      delete client;
    }
  }
}

// ПЕРЕДАЧА ДАННЫХ НА СЕРВЕР SOI-TECH ДЛЯ BACKUP
void backUpConfig() {
  WiFiClient client;
  HTTPClient httpClient;
  httpClient.useHTTP10(true);
  httpClient.setTimeout(5000);
  char request[128];
  sprintf(request, "http://%s/%s/%s/backUp/?", OTA_HOST, DEVICE_MODEL, WiFi.macAddress().c_str());
  String answer = "";
  Serial.println(request);
  httpClient.begin(client, request);
  if (httpClient.GET() == HTTP_CODE_OK) {
    answer = httpClient.getString();
  }
  httpClient.end();
  Serial.println(answer);
}

// ОБНОВЛЕНИЕ FS И ПРОШИВКИ ESP
bool _otaUpdate(bool spiffs) {
  ledBlinkTicker.attach(0.1, blinkLed);
  Serial.println(spiffs ? "Updating SPIFFS" : "Updating FLASH");
  WiFiClient client;
  HTTPClient httpClient;
  httpClient.useHTTP10(true);
  httpClient.setTimeout(5000);
  char url[128];
  sprintf(url, "http://%s/%s-%d-%s.bin", OTA_HOST, DEVICE_MODEL, latVer, spiffs ? "fs" : "esp");
  httpClient.begin(client, url);
  if (httpClient.GET() != HTTP_CODE_OK) {
    Serial.printf("Unable to fetch update %s\r\n", url);
    httpClient.end();
    return false;
  }
  if (!Update.setMD5(spiffs ? ffsMD5 : espMD5)) {
    Serial.printf("Error: Update.setMD5(%s): ", spiffs ? ffsMD5 : espMD5);
    Update.printError(Serial);
    httpClient.end();
    return false;
  }
  if (!Update.begin(httpClient.getSize(), spiffs ? U_FS : U_FLASH)) {
    Serial.printf("Error: Update.begin(%d, U_FLASH): ", httpClient.getSize());
    Update.printError(Serial);
    httpClient.end();
    return false;
  }
  if (!Update.writeStream(*httpClient.getStreamPtr())) {
    Serial.print("Error: Update.writeStream(): ");
    Update.printError(Serial);
    httpClient.end();
    return false;
  }
  if (!Update.end()) {
    Serial.print("Error: Update.end(): ");
    Update.printError(Serial);
    httpClient.end();
    return false;
  }
  Serial.println(spiffs ? "SPIFFS update OK" : "FLASH update OK");
  httpClient.end();
  return true;
}

void otaUpdate() {
  _otaUpdate(true) && _otaUpdate(false);
  ledBlinkTicker.detach();
  uartSendHAD("SYS", "restart", NULL);
}

// ======================================== //
// ПРОВЕРКА НАЛИЧИЯ ОБНОВЛЕНИЯ (HTTPS + FP) //
// ======================================== //
//void otaAvailable() {
//  flagCheckUpdate = false;
//  WiFiClientSecure httpsClient;
//  if (!httpsClient.connect(OTA_HOST, 443)) {
//    uartSendEA("Connection OTA Failed");
//    httpsClient.stop();
//    return;
//  }
//  if (httpsClient.verify(OTA_FP, OTA_HOST)) {
//    httpsClient.print(String("GET ") + OTA_PATH + " HTTP/1.1\r\nHost: " + OTA_HOST + "\r\nConnection: close\r\n\r\n");
//    unsigned long timeout = millis();
//    while (httpsClient.available() == 0) {
//      if (millis() - timeout > 5000) {
//        uartSendEA("Client Timeout");
//        httpsClient.stop();
//        return;
//      }
//    }
//  } else {
//    uartSendEA("Certificate");
//    httpsClient.stop();
//    return;
//  }
//  String line = "";
//  while (httpsClient.available()) {
//    line = httpsClient.readStringUntil('\r');
//  }
//  const char * response = line.c_str();
//  const char * _espMD5 = strchr(response, ' ') + 1;
//  const char * _ffsMD5 = strchr(_espMD5, ' ') + 1;
//  const char * _mcuMD5 = strchr(_ffsMD5, ' ') + 1;
//  latVer = strtol(response, NULL, 10);
//  strncpy(espMD5, _espMD5, 32); espMD5[32] = '\0';
//  strncpy(ffsMD5, _ffsMD5, 32); ffsMD5[32] = '\0';
//  strncpy(mcuMD5, _mcuMD5, 32); mcuMD5[32] = '\0';
//}
