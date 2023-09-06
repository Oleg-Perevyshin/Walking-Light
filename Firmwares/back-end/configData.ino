
// Загрузка параметров из файла config.json
void configLoad() {
  SPIFFS.begin();
  if (!SPIFFS.exists(CONFIG_PATH))
    return configLoadDefaults();
  File f = SPIFFS.open(CONFIG_PATH, "r");
  DynamicJsonDocument configBuffer(2048);
  DeserializationError error = deserializeJson(configBuffer, f);
  if (error) {
    return;
  }
  JsonObject root = configBuffer.as<JsonObject>();
  strncpy(da, root["da"].as<const char*>(), sizeof(da) - 1);
  strncpy(rch, root["rch"].as<const char*>(), sizeof(rch) - 1);
  strncpy(dn, root["dn"].as<const char*>(), sizeof(dn) - 1);
  strncpy(dc, root["dc"].as<const char*>(), sizeof(dc) - 1);
  strncpy(dl, root["dl"].as<const char*>(), sizeof(dl) - 1);
  strncpy(dls, root["dls"].as<const char*>(), sizeof(dls) - 1);
  dz = root["dz"].as<int>();
  strncpy(dv, root["dv"].as<const char*>(), sizeof(dv) - 1);
  strncpy(du, root["du"].as<const char*>(), sizeof(du) - 1);
  strncpy(dp, root["dp"].as<const char*>(), sizeof(dp) - 1);
  strncpy(staSSID, root["staSSID"].as<const char*>(), sizeof(staSSID) - 1);
  strncpy(staPSK, root["staPSK"].as<const char*>(), sizeof(staPSK) - 1);
  staIPMode = root["staIPMode"].as<int>();
  strncpy(staIP, root["staIP"].as<const char*>(), sizeof(staIP) - 1);
  strncpy(staMS, root["staMS"].as<const char*>(), sizeof(staMS) - 1);
  strncpy(staGW, root["staGW"].as<const char*>(), sizeof(staGW) - 1);
  strncpy(apSSID, root["apSSID"].as<const char*>(), sizeof(apSSID) - 1);
  strncpy(apPSK, root["apPSK"].as<const char*>(), sizeof(apPSK) - 1);
  strncpy(apIP, root["apIP"].as<const char*>(), sizeof(apIP) - 1);
  strncpy(apMS, root["apMS"].as<const char*>(), sizeof(apMS) - 1);
  strncpy(apGW, root["apGW"].as<const char*>(), sizeof(apGW) - 1);
  f.close();
  configBuffer.clear();
}

// Загрузка параметров по умолчанию
void configLoadDefaults() {
  snprintf(da, sizeof(da), "%02X", ESP.getChipId());
  strncpy(rch, "10", sizeof(rch) - 1);
  strncpy(dn, da, sizeof(dn) - 1);
  strncpy(dc, "999", sizeof(dc) - 1);
  strncpy(dl, "ru", sizeof(dl) - 1);
  langList();
  dz = 0;
  strncpy(dv, "0", sizeof(dv) - 1);
  strncpy(du, "", sizeof(du) - 1);
  strncpy(dp, "", sizeof(dp) - 1);
  strncpy(staSSID, "TAP-309", sizeof(staSSID) - 1);
  strncpy(staPSK, "MCMega2005!", sizeof(staPSK) - 1);
  staIPMode = IP_DHCP;
  strncpy(staIP, "", sizeof(staIP) - 1);
  strncpy(staMS, "", sizeof(staMS) - 1);
  strncpy(staGW, "", sizeof(staGW) - 1);
  snprintf(apSSID, sizeof(apSSID), "SOI-%s", da);
  strncpy(apPSK, "", sizeof(apPSK) - 1);
  strncpy(apIP, "192.168.108.1", sizeof(apIP) - 1);
  strncpy(apMS, "255.255.255.0", sizeof(apMS) - 1);
  strncpy(apGW, "192.168.108.1", sizeof(apGW) - 1);
  configSave();
}

// Сохранение параметров в файл config.json
void configSave() {
  DynamicJsonDocument configBuffer(2048);
  JsonObject root = configBuffer.to<JsonObject>();
  root["di"] = DEVICE_ID;
  root["dt"] = DEVICE_TYPE;
  root["dm"] = DEVICE_MODEL;
  root["da"] = da;
  root["rch"] = rch;
  root["dn"] = dn;
  root["dc"] = dc;
  root["dl"] = dl;
  root["dls"] = dls;
  root["dz"] = dz;
  root["dv"] = dv;
  root["du"] = du;
  root["dp"] = dp;
  root["staSSID"] = staSSID;
  root["staPSK"] = staPSK;
  root["staIPMode"] = staIPMode;
  root["staIP"] = staIP;
  root["staMS"] = staMS;
  root["staGW"] = staGW;
  root["apSSID"] = apSSID;
  root["apPSK"] = apPSK;
  root["apIP"] = apIP;
  root["apMS"] = apMS;
  root["apGW"] = apGW;
  File f = SPIFFS.open(CONFIG_PATH, "w");

  serializeJson(root, f);
  f.flush();
  f.close();
  configBuffer.clear();
}

void langList() {
  DynamicJsonDocument doc(512);
  JsonArray langList = doc.to<JsonArray>();
  Dir dir = SPIFFS.openDir("/dl");
  while (dir.next()) {
    File entry = dir.openFile("r");
    const char *path = entry.name();
    char langName[3] = "";
    int pos = 0, slash_cnt = 0;
    for (int in_pos = 0; in_pos < strlen(path); in_pos++) {
      if (path[in_pos] == '/')
        slash_cnt++;
      else if (slash_cnt == 2) {
        langName[pos] = path[in_pos];
        pos++;
      }
      if (pos > 1)
        break;
    }
    langName[pos] = '\0';
    langList.add(langName);
    entry.close();
  }
  serializeJson(langList, dls);
}
