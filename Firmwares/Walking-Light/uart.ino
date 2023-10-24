
// Инициализация интерфейса UART
void uartInit(int speed) {
  Serial.begin(speed, SERIAL_8N2);
  Serial.println();
  uartSendHAD("SYS", "status", NULL);
}

// РАБОТАЕМ С UART
void uartRead() {
  if (uartMode == UART_NORMAL) {
    uartWork();
  } else if (uartMode == UART_PROG) {
    mcuProg();
  } else {
    uartMode = UART_NORMAL;
  }
}

// ОБРАБОТКА ПРИНЯТОГО ПАКЕТА ПО UART
void uartWork() {
  if (Serial.available()) {
    inChar = (char)Serial.read();
    int uartData_len = strlen(uartData);
    int uartData_max = sizeof(uartData) - 1;
    if (uartData_len < uartData_max) {
      uartData[uartData_len] = inChar;
      uartData[uartData_len + 1] = '\0';
    } else {
      uartSendEA("Cleaning Buffer");
      uartData[0] = '\0';
    }
    if (inChar == EOT) {
      if (uartData[0] != SOH) {
        uartData[0] = '\0';
        return;                                                                                       // Игнорируем неверные пакеты
      }
      bool has_body = strchr(uartData, STX) != NULL;                                                  // Проверяем наличие данных в пакете

      char header[strchr(uartData + 1, US) - uartData];                                               // Читаем Заголовок (header)
      strncpy(header, uartData + 1, sizeof(header) - 1);
      header[sizeof(header) - 1] = '\0';

      char argument[strchr(uartData + 1 + sizeof(header), US) - uartData - sizeof(header)];           // Читаем аргумент заголовка (argument)
      strncpy(argument, uartData + sizeof(header) + 1, sizeof(argument) - 1);
      argument[sizeof(argument) - 1] = '\0';

      int body_len = has_body ? (strchr(uartData, ETX) - strchr(uartData, STX) - 1) : 0;              // Читаем данные (body)
      char *body = (char*)malloc(body_len + 1);
      if (body_len) {
        strncpy(body, strchr(uartData, STX) + 1, body_len);
      }
      body[body_len] = '\0';

      char crc = 0x00;                                                                                // Задаем CRC8 по умолчанию
      if (has_body) {                                                                                 // Вычисляем CRC8 данных (Data), если они есть
        char hex[3];
        strncpy(hex, strchr(uartData, ETX) + 1, 2);
        hex[2] = '\0';
        crc = (char)strtol(hex, NULL, 16);
      }

      if (has_body && crc8(body, body_len) != crc) {                                                  // Сравниваем расчетную и принятую контрольные суммы
        uartSendEA("CRC");
        free(body);
        uartData[0] = '\0';
        return;
      }

      // Обрабатываем зоголовки
      if (!strcmp(header, "SYS")) {
        sysParser(argument);
      }
      else if (!strcmp(header, "GET")) {
        getParser(argument);
      }
      else if (!strcmp(header, "SET")) {
        setParser(argument, body);
      }
      else if (!strcmp(header, "OK!")) {
        okParser(argument, body);
      }
      else if (!strcmp(header, "ER!")) {
        erParser(argument, body);
      }
      else {
        uartSendEA("Header");
      }

      free(body);
      uartData[0] = '\0';
    }
  }
}

// Расчет контрольной суммы CRC8
byte crc8(const char *data, size_t len) {
  byte crc = 0x00;
  while (len--) {
    byte extract = *data++;
    for (byte i = 8; i; i--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum)
        crc ^= 0x8C;
      extract >>= 1;
    }
  }
  return crc;
}

// Передача пакета в UART (с данными и без)
void uartSendHAD(const char *header, const char *argument, const char *data) {
  if (data)
    Serial.printf(
      "%c%s%c%s%c%c%s%c%02X%c",
      SOH, header, US, argument, US, STX, data, ETX, crc8(data, strlen(data)), EOT
    );
  else
    Serial.printf(
      "%c%s%c%s%c%c",
      SOH, header, US, argument, US, EOT
    );
}

void uartSendEA(const char *argument) {
  Serial.printf(
    "%cER!%c%s%c%c",
    SOH, US, argument, US, EOT
  );
}

void uartSendOA(const char *argument) {
  Serial.printf(
    "%cOK!%c%s%c%c",
    SOH, US, argument, US, EOT
  );
}

void uartSendOAD(const char *argument, const char *data) {
  Serial.printf(
    "%cOK!%c%s%c%c%s%c%02X%c",
    SOH, US, argument, US, STX, data, ETX, crc8(data, strlen(data)), EOT
  );
}

void uartSendOAD(const char *argument, long num) {
  char data[12];
  snprintf(data, 12, "%ld", num);
  uartSendOAD(argument, data);
}
