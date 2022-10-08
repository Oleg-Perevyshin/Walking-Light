

// ================================================================================ //
// WIFI МОДУЛЬ WalkingLight                                                         //
// Ядро https://github.com/esp8266/Arduino                                          //
// Загрузчик FS https://github.com/esp8266/arduino-esp8266fs-plugin                 //
// JSON https://github.com/bblanchon/ArduinoJson                                    //
// ESPAsyncWebServer https://github.com/me-no-dev/ESPAsyncWebServer                 //
// ESPAsyncTCP https://github.com/me-no-dev/ESPAsyncTCP                             //
// ESPAsyncUDP https://github.com/me-no-dev/ESPAsyncUDP                             //
// ================================================================================ //


#include <ESP8266WiFi.h>          // Ядро WiFi
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncUDP.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <StreamString.h>

// СЛУЖЕБНЫЕ ПАРАМЕТРЫ
#define STATUS_LED_PIN      12                // D6
#define STATUS_LED_PIN      2                 // D4
#define MCU_RESTART_PIN     5                 // D1
#define DEVICE_ID           "28"              // Device ID                (Идентификатор устройства по каталогу, представление в HEX)
#define DEVICE_TYPE         "Lighting"        // Device Type              (Тип устройства)
#define DEVICE_MODEL        "WalkingLight"    // Device Model             (Модель устройства)
#define VERSION             0                 // Version FW               (Версия текущей прошивки)
#define VENDOR              "SOI Tech"
#define VENDOR_URL          "http://soi-tech.com"
#define DEVICE_URL          "http://soi-tech.com/walkinglight/"
#define OTA_HOST            "updates.soi-tech.com"
#define OTA_PATH            "/" DEVICE_MODEL ".txt"
#define OTA_URL             "https://updates.soi-tech.com/" DEVICE_MODEL ".txt"
#define OTA_FP              "6A E3 57 B5 1E 03 70 99 A5 44 B3 81 B2 D8 25 13 0B B9 D7 63"
#define CONFIG_PATH         "/config.json"
#define MCU_HEX_PATH        "/mcu.hex"

// ИДЕНТИФИКАЦИЯ
char dn[33];                                  // Device Name              (Пользовательское имя устройства)
char dc[4];                                   // Device Color             (Цветовая зона устройства)
char dl[3];                                   // Device Language          (Язык интерфейса, как международное сокращение в 2 символа)
char dls[31];                                 // Device Languages         (Список языков интерфейса, как массив)
int  dz;                                      // Device Zone              (Часовой пояс GTM)
char dv[2];                                   // Device Visual Design     (Тема визуального оформления интерфейса)
char du[17];                                  // Device User              (Имя пользователя для доступа к закрытым областям)
char dp[17];                                  // Device Password          (Пароль для доступа к закрытым областям)

// РЕЖИМ STA
char staSSID[33];                             // Station SSID             (Точка доступа)
char staPSK[65];                              // Station PSK              (Пароль Точки доступа)
byte staIPMode;                               // Station IP Mode          (Режим параметров сети, статические или динамисеские)
char staIP[16];                               // Station IP               (IP адрес при статических параметрах сети)
char staMS[16];                               // Station Mask             (Маска подсети при статических параметрах сети)
char staGW[16];                               // Station GateWay          (IP адрес шлюза при статических параметрах сети)
#define IP_DHCP             0                 // DHCP                     (Признак использования динамических параметров сети)
#define IP_STATIC           1                 // Static                   (Признак использования статических параметров сети)

// РЕЖИМ AP
char apSSID[33];                             // Access Point SSID        (Идентификатор устройства в режиме Точка доступа)
char apPSK[65];                              // Access Point PSK         (Пароль устройства в режиме Точка доступа)
char apIP[16];                               // Access Point IP          (IP адрес устройства в режиме Точка доступа)
char apMS[16];                               // Access Point Mask        (Маска подсети устройства в режиме Точка доступа)
char apGW[16];                               // Access Point GateWay     (IP адрес шлюза в режиме Точка доступа)

// РАДИО КАНАЛ
char da[7];                                   // Device Address           (Адрес устройства в Радио сети)
char rch[3];                                 // Device Radio Channel     (Частотный канал в радио сети)

// UART
#define UART_NORMAL         false             // UART Normal              (Штатный режим работы UART)
#define UART_PROG           true              // UART Programming         (UART в режиме программирования MCU)
char inChar;                                  // UART Byte                (Текущий Байт для работы с UART)
char uartData[1024] =      "";                // UART Data                (Буфер хранения данных для UART)
File mcuHex;                                  // MCU Hex                  (Переменная для хранения файла пришивки MCU в процессе загрузки)
int mcuFlashTries =         0;                // MCU Flash Tries          (Счетчик попыток записи во флешь MCU)
#define SOH                 0x01              // SOH                      (Начало пакета)
#define STX                 0x02              // STX                      (Начало данных в пакете)
#define ETX                 0x03              // ETX                      (Конец данных в пакете)
#define EOT                 0x04              // EOT                      (Конец пакета)
#define US                  0x1F              // US                       (Разделитель заголовка и аргумента в пакете)
#define BOOT_MCU_START      0x3E              // BOOT_MCU_START           (Старт загрузчика MCU)
#define BOOT_MCU_XON        0x11              // Flow Control xOn         (Управление потоком, передача разрешена)
#define BOOT_MCU_XOFF       0x13              // Flow Control xOff        (Управление потоком, передача запрещена)
#define BOOT_MCU_ECRC       0x21              // BOOT_MCU_ECRC            (Ошибка контрольной суммы)
#define BOOT_MCU_EFLASH     0x23              // BOOT_MCU_EFLASH          (Ошибка записи во Flash)
#define BOOT_MCU_END        0x3C              // BOOT_MCU_END             (Успешная прошивка MCU)

// UDP + SCANNER
AsyncUDP udp;
String devsListTemp = "";
String devsList = "";
String * ipList = new String [100];

// СЕРВЕР
AsyncWebServer server(80);                    // Создаем http сервер на порту 80
AsyncWebSocket ws("/ws");                     // Создаем WebSocket Server (ws://ip/ws)
AsyncEventSource events("/events");
AsyncWebSocketClient * client;                // Создаем WebSocket Client
static AsyncClient * aClient = NULL;          // Готовим асинхронный клиент
long int latVer = VERSION;                    // Начальное значение последней версии прошивки
char espMD5[33] = "";                         // MD5 последней версии прошивки ESP
char ffsMD5[33] = "";                         // MD5 последней версии прошивки FS
char mcuMD5[33] = "";                         // MD5 последней версии прошивки MCU

// ДИСПЕТЧЕР ЗАДАЧ
Ticker updateTicker;                          // Задача проверки наличия обновления на сервере
Ticker wifiTicker;                            // Задача переключения AP/STA в зависмости от сотояния подключения
Ticker udpTicker;                             // Задача запуска сканирования устройств в сети
Ticker waitScan;                              // Формирование задержки при сканировании сети
Ticker waitMCUProg;                           // Ожидание готовности MCU к прошивке
Ticker ledBlinkTicker;                        // Задача для моргания светодиодом

// ФЛАГИ ОБРАБОТЧИКОВ
bool uartMode =             UART_NORMAL;      // Режим работы UART, штатный либо программирования MCU
bool uartFlowCtrl =         false;            // Управление потоком в режиме прошивки MCU
bool flagHandlePacket =     false;            // Чтение пакетов принимаемых по широковещательному запросу
bool flagReboot =           false;            // Перезагрузки после обновления
bool flagCheckUpdate =      false;            // Запуска проверки наличия обновления на сервере
bool flagWConnect =         false;            // Проверки состояния подключения

ADC_MODE(ADC_VCC);                            // Инициализация ADC

void setup() {
  pinMode(MCU_RESTART_PIN, OUTPUT);           // Пин для сброса MCU на Выход
  digitalWrite(MCU_RESTART_PIN, HIGH);        // Начальное состояние 1
  pinMode(STATUS_LED_PIN, OUTPUT);            // Пин светодиода СТАТУС на Выход
  digitalWrite(STATUS_LED_PIN, LOW);          // Начальное состояние 0

  uartInit(115200);                           // Инициализация UART
  configLoad();                               // Загрузка параметров
  wifiInit();                                 // Инициализация WiFi
  udpInit();                                  // Инициализация UDP
  serverInit();                               // Инициализация Web сервера

  wifiTicker.attach(30, flagWiFi);            // Проверка статуса подключения каждые 30 сек.
  udpTicker.attach(600, findDevices);         // Поиск устройств в сети каждые 10 минут
  updateTicker.attach(10800, flagUpdate);     // Прверка обновлений на сервере каждые 3 часа

  Serial.println(ESP.getFreeHeap());
}

void loop() {
  uartRead();
  checkConnect();
  if (flagReboot) {
    delay(100);
    uartSendHAD("SYS", "restart", NULL);
  }
//  if (flagCheckUpdate) {
//    otaAvailable();
//  }
}

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ //
int getVCC() {
  return ESP.getVcc();
}
