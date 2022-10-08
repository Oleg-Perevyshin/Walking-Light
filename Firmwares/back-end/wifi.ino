
// СТАРТОВАЯ ПОДГОТОВКА WIFI
void wifiStart() {
  WiFi.disconnect(true);
  WiFi.softAPdisconnect();
  WiFi.persistent(false);
  WiFi.setPhyMode(WIFI_PHY_MODE_11N); // WIFI_PHY_MODE_11B = 1, WIFI_PHY_MODE_11G = 2, WIFI_PHY_MODE_11N = 3
  delay(100);
  digitalWrite(STATUS_LED_PIN, LOW);
}

// ИНИЦИАЛИЗАЦИЯ WIFI
void wifiInit() {
  wifiStart();
  WiFi.mode(WIFI_STA);
  if (staIPMode == IP_STATIC) {
    IPAddress address, netmask, gateway;
    address.fromString(staIP);
    netmask.fromString(staMS);
    gateway.fromString(staGW);
    WiFi.config(address, gateway, netmask);
  }
  Serial.printf("Connecting to SSID: %s, PSK: %s", staSSID, staPSK);
  Serial.println();
  WiFi.begin(staSSID, staPSK);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
    return wifiStartAPSTA();
  digitalWrite(STATUS_LED_PIN, HIGH);
  WiFi.hostname(apSSID);
  WiFi.setAutoReconnect(true);
  wifiPrintStatus();
  configTime(dz * 3600, 0, "pool.ntp.org", "time.nist.gov");
  WiFi.scanNetworks(true);
  otaAvailable();                                   // Проверяем наличие обновлений на сервере
  ledBlinkTicker.detach();                          // Отключаем моргание светодиодом - признак режима STA
//  time_t now = time(nullptr); Serial.println(ctime(&now));
  digitalWrite(STATUS_LED_PIN, HIGH);               // Гасим светодиод
}

// ЗАПУСК РЕЖИМА AP+STA
void wifiStartAPSTA() {
  wifiStart();
  WiFi.mode(WIFI_AP_STA);
  IPAddress address, netmask, gateway;
  address.fromString(apIP);
  netmask.fromString(apMS);
  gateway.fromString(apGW);
  WiFi.softAPConfig(address, gateway, netmask);
  WiFi.softAP(apSSID, apPSK);
  WiFi.hostname(apSSID);
  wifiPrintStatus();
  WiFi.scanNetworks(true);
  ledBlinkTicker.attach(1, blinkLed);             // Запускаем моргание светодиодом - признак режима AP+STA
}

// ВЫВОД СТАТУСА
void wifiPrintStatus() {
  char address[16], netmask[16], gateway[16];
  switch (WiFi.getMode()) {
    case WIFI_AP:
    case WIFI_AP_STA:
      strncpy(address, WiFi.softAPIP().toString().c_str(), 15);
      break;
    case WIFI_STA:
      strncpy(address, WiFi.localIP().toString().c_str(), 15);
      break;
  }
  address[strlen(address)] = '\0';
  uartSendHAD("SYS", "sdk", ESP.getSdkVersion());
  uartSendHAD("SYS", "ip", address);
  uartSendHAD("SYS", "hn", WiFi.hostname().c_str());
  Serial.println();
}

// УСТАНОВКА ФЛАГА ДЛЯ ПРОВЕРКИ ПОДКЛЮЧЕНИЯ К СЕТИ
void flagWiFi() {
  flagWConnect = true;
}

// СМЕНА СОСТОЯНИЯ СВЕТОДИОДА STATUS
void blinkLed() {
  digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
}

// ПРОВЕРКА ПОДКЛЮЧЕНИЯ К СЕТИ
void checkConnect() {
  if (flagWConnect) {
    if (WiFi.getMode() == WIFI_STA && WiFi.status() != WL_CONNECTED) {           // Запуск режима AP+STA при отсутствии соединения в режиме STA
      wifiStartAPSTA();
    }
    if (WiFi.getMode() == WIFI_AP_STA && !WiFi.softAPgetStationNum()) {          // Подключение к появившейся сети, если нет подключенных клиентов
      int n = WiFi.scanNetworks();
      bool found = false;
      for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == staSSID) {
          found = true;
        }
      }
      WiFi.scanDelete();
      if (found) {
        wifiInit();
      }
    }
    flagWConnect = false;
  }
}





// ПЕРЕДАЧА ДАННЫХ НА СЕРВЕР SOI-TECH
void backUpConfig() {
  String request = "http://updates.soi-tech.com/WalkingLight/backUp/?";
  request += WiFi.macAddress().c_str();
  String answer = "";
  Serial.println(request);
  HTTPClient http;
  http.begin(request);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    answer = http.getString();
  }
  http.end();
  Serial.println(answer);
}
