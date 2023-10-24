
// ИНИЦИАЛИЗАЦИЯ РЕЖИМА ПРОГРАММИРОВАНИЯ MCU
void mcuUpdateInit() {
  ledBlinkTicker.attach(0.1, blinkLed);             // Запускаем моргание светодиодом - признак прошивки MCU
  waitMCUProg.attach(3, finishUpdateMCU);           // Запускаем таймер ожидание готовности MCU к прошивке
  if (mcuFlashTries > 2) {                          // Проверяем счетчик попыток
    uartSendEA("Failed Programming MCU");
    finishUpdateMCU();
    return;
  }
  uartFlowCtrl = false;                             // Запрещаем передачу
  if (mcuHex)                                       // Проверяем, открыт ли файл прошивки
    mcuHex.close();                                 // Закрываем файл
  if (!SPIFFS.exists(MCU_HEX_PATH)) {               // Проверяем наличие файла прошивки по пути MCU_HEX_PATH
    Serial.printf("MCU Firmware File \"%s\" doesn't exist", MCU_HEX_PATH);
    Serial.println();
    finishUpdateMCU();
    return;
  } else
    mcuHex = SPIFFS.open(MCU_HEX_PATH, "r");
  mcuHex.seek(0, SeekSet);
  uartMode = UART_PROG;
  restartMcu();
}

// ПЕРЕДАЧА ПРОШИВКИ В MCU С КОНТРОЛЕМ ПОТОКА
void mcuProg() {
  if (mcuHex.available()) {
    if (Serial.available()) {
      inChar = (char)Serial.read();                 // Читаем очередной байт из буфера UART
      switch (inChar) {                             // Сравниваем
        case BOOT_MCU_START:                        // Старт загрузчика MCU
        case BOOT_MCU_XON:                          // XON
          uartFlowCtrl = true;                      // Разрешаем передачу данных
          break;
        case BOOT_MCU_XOFF:                         // XOFF
          uartFlowCtrl = false;                     // Запрещаем передачу данных
          break;
        case BOOT_MCU_ECRC:                         // Ошибка контрольной суммы
        case BOOT_MCU_EFLASH:                       // Ошибка записи Flash
          mcuFlashTries++;                          // Увеличиваем счетчик попыток
          mcuUpdateInit();                          // Заново инициализируем режима прошивки MCU
          break;
        case BOOT_MCU_END:                          // Запись успешно завершена
          finishUpdateMCU();                        // Завершающие действия после прошивки
          break;
      }
    }
  }
  if (uartFlowCtrl) {
    Serial.write(mcuHex.read());                    // Читаем очередной байт из файла и передаем в UART
    waitMCUProg.attach(3, finishUpdateMCU);         // Обновляем таймер ожидание готовности MCU к прошивке
    delay(2);
  }
}

// ФИНАЛИЗАЦИЯ РАБОТЫ С ПРОШИВКОЙ MCU
void finishUpdateMCU() {
  waitMCUProg.detach();                             // Снимаем таймер ожидание готовности MCU к прошивке
  ledBlinkTicker.detach();                          // Выключаем светодиод статуса
  uartData[0] = '\0';                               // Готовим буфер UART для штатного режима
  uartMode = UART_NORMAL;                          // Переводим UART в штатный режим
  mcuFlashTries = 0;                                // Устанавливаем счетчик попыток записи в исходное состояние
  if (mcuHex)                                       // Проверяем, открыт ли файл прошивки
    mcuHex.close();                                 // Закрываем файл
  SPIFFS.remove(MCU_HEX_PATH);                      // Удаляем прошивку из файловой системы
  restartMcu();                                     // Перезагружаем MCU
}

// ПЕРЕЗАГРУЗКА MCU
void restartMcu() {
  digitalWrite(MCU_RESTART_PIN, LOW);
  delay(100);
  digitalWrite(MCU_RESTART_PIN, HIGH);
}
