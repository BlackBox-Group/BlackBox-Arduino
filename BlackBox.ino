/*
    Arduino-часть проекта BlackBox
    Схема:
      MOSI  -  ICSP-4
      MISO  -  ICSP-1   -> MISO от SD карты подключён через резистор в ~360 Ом
      SCK   -  ICSP-3

      SD-Card Reader:
        CS - 7

      RFID:
        RST -  13
        SS  -  10
        SDA -  9

    Создано BlackBox Group

                                    25.10.19
*/

#include <SD.h>
#include <RFID.h>
#include <aes.hpp>

// Выбранные цифровые выводы
#define SS_RFID   10
#define RST_RFID  9
#define CS_SD     7

// RFID ридер
const RFID rfid(SS_RFID, RST_RFID);

// Открытые файлы записываются сюда
File f;

// Дебажные выводы
/*bool debugMode = true;
  bool isNewLine = true;

  void //debugPrint(String s) {
  if (debugMode) {
    if (isNewLine) {
      Serial.print('#');
      isNewLine = false;
    }
    Serial.print(s);
  }
  }

  void //debugPrintln() {
  if (debugMode && isNewLine) Serial.println('#');
  isNewLine = true;
  }

  void //debugPrintln(String s) {
  if (debugMode) {
    if (isNewLine) {
      Serial.print('#');
    }
    Serial.println(s);
    isNewLine = true;
  }
  }*/

// Максимум 14 будет использовано, но сам массив - iv
const char salt[16] = "985e166be2e9d300";
const uint8_t* generateKey(byte* nuid, String* master) {
  uint8_t* key = new uint8_t[32];
  for (int i = 0; i < 4; i++)
    key[i] = nuid[i];

  byte mLen = master->length();
  for (int i = 0; i < mLen; i++)
    key[4 + i] = master->operator[](i);

  // Salt
  key[4 + mLen] = 25;
  key[5 + mLen] = 10;
  key[6 + mLen] = 20;
  key[7 + mLen] = 19;

  for (int i = 8 + mLen, j = 0; i < 32; i++)
    key[i] = salt[j++];

  return key;
}

void setup() {
  // Инициализация встроенного светодиода для отображения, что карта приложена
  pinMode(LED_BUILTIN, OUTPUT);

  // Запуск Serial и ожидание подключения
  Serial.begin(9600);
  while (!Serial) {
    ; // Необходимо для USB подключения, Arduino будет ждать открытия серийного монитора
  }

  // Запуск SPI и инициализация RFID
  //debugPrintln("Запуск SPI и инициализация RFID...");
  SPI.begin();
  rfid.init();

  //debugPrint("Инициализация SD карты... ");
  if (!SD.begin(CS_SD)) {
    //debugPrintln("Инициализация провалена! Выполнение программы остановлено.");
    Serial.println("# SD card init failed.");
    while (1);
  }
  //debugPrintln("Выполнено.");

  /* Тест записи и чтения */
  /*if (SD.exists("test.txt")) {
    //    //debugPrintln("Удаляем прошлый файл теста...");
    SD.remove("test.txt");
    }

    // Запись
    f = SD.open("test.txt", FILE_WRITE);
    if (f) {
    //debugPrint("Попытка проверить запись... ");

    f.print("testing 1, 2, 3.");
    f.close();
    } else {
    //debugPrintln("Ошибка открытия файла! Выполнение программы остановлено.");
    while (1);
    }

    // Чтение
    f = SD.open("test.txt");
    if (f) {
    //debugPrint("Чтение... ");

    char* c = new char[17];
    f.read(c, 16);

    if (strcmp("testing 1, 2, 3.", c) == 0) {
      f.close();
      //      SD.remove("test.txt");
      //debugPrintln("SD функционирует корректно.");
    }
    else {
      //debugPrintln();
      //debugPrint("Запись была выполнена некорректно, ожидалось: 'testing 1, 2, 3.', было записано: '");
      //debugPrint(c);
      //debugPrintln("'");
      while (1);
    }

    delete c;
    } else {
    //debugPrintln("Ошибка открытия файла! Выполнение программы остановлено.");
    while (1);
    }

    if (!SD.exists("cards.txt")) {
    //debugPrintln("Создаю файл cards.txt...");
    SD.open("cards.txt", FILE_WRITE).close();
    }*/

  if (!SD.exists("cards.txt")) {
    //debugPrintln("Создаю файл cards.txt...");
    SD.open("cards.txt", FILE_WRITE).close();
  }
  if (!SD.exists("usr/")) {
    //debugPrintln("Создаю директорию usr/...");
    SD.mkdir("usr");
  }

  //debugPrintln();
  //debugPrintln("Инициализация выполнена.");
  Serial.println("# OK.");
}

// Функция сравнения NUID в двух байтовых массивах
bool matchesNUID(byte nuid1[], byte nuid2[]) {
  for (byte i = 0; i < 4; i++)
    if (nuid1[i] != nuid2[i]) return false;

  return true;
}

// Копирование NUID из одного массива в другой
void copyNUID(byte origin[], byte dest[]) {
  for (int i = 0; i < 4; i++)
    dest[i] = origin[i];
}

String nuidToStr(byte nuid[]) {
  String buff;
  for (byte i = 0; i < 4; i++) buff += String(nuid[i], HEX) + ':';

  return buff;
}

//bool commandFinished = false;

bool readRFID() {
  // Ожидание, пока новая карта появится на сенсоре
  digitalWrite(LED_BUILTIN, HIGH);
  if (rfid.isCard()) {
    // Убеждаемся, что NUID было успешно прочитано
    if (rfid.readCardSerial()) {
      //debugPrintln("NUID прочитан");

      /*if ( matchesNUID(previousCardNUID, rfid.serNum) ) {
        //debugPrintln("Такая карта уже была приложена");
        }
        else {
        copyNUID(rfid.serNum, previousCardNUID);*/

      //debugPrint("Прочитанная карта: ");
      //debugPrintln( nuidToStr(rfid.serNum) );
      /*}*/

      // Пауза, чтобы человек успел убрать карту от сенсора
      // delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      return true;
    }
  }

  return false;
}

String fileReadUntil(char stp) {
  String s = "";
  while (f.available()) {
    char c = f.read();

    if (c != stp) s += c;
    else break;
  }

  return s;
}

uint8_t blockBuffer[16];
void fileReadBlock() {
  for (byte i = 0; i < 16; i++) {
    blockBuffer[i] = f.read();
  }
}

void rm(File dir, String tempPath) {
  while(true) {
    File entry =  dir.openNextFile();
    String localPath;

    if (entry) {
      if ( entry.isDirectory() )
      {
        localPath = tempPath + entry.name() + '/' + '\0';
        char folderBuf[localPath.length()];
        localPath.toCharArray(folderBuf, localPath.length() );
        rm(entry, folderBuf);

        SD.rmdir( folderBuf );
      } 
      else
      {
        localPath = tempPath + entry.name() + '\0';
        char charBuf[localPath.length()];
        localPath.toCharArray(charBuf, localPath.length() );

        SD.remove( charBuf );
      }
    } 
    else {
      // break out of recursion
      return;
    }
  }
}

void dumpBuffer(uint8_t buf[], size_t len) {
  Serial.print("'");
  Serial.write(buf, len);
  Serial.println("'");
}

void dumpBufferHex(uint8_t buf[], size_t len) {
  Serial.print("'");
  for (size_t i = 0; i < len; i++) {
    Serial.print(String(buf[i], HEX) + " ");
  }
  Serial.println("'");
}

String command;

bool isUsernameRequired = false,
     isMasterRequired   = false,
     isUserCreation     = false,
     isLoginProcess     = false;

struct AES_ctx ctx;
void loop() {
  // Рутины с таймером, например отсчитывания времени после прикладывания RFID

  // Обработка RFID карт
  // handleRFID();

  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    bool isCommandOK = true;
    if (command.startsWith("#")) {
      //debugPrint("echo: ");
      //debugPrintln(command);
      return;
    }

    if (command == "usercreate") {
      Serial.println("putRFID");
      while (!readRFID()) {
        ; // Ожидаем корректного ввода карты
      }
      //debugPrintln("Используем прочитанную карту, чтобы создать пользователя");

      f = SD.open("cards.txt");
      String cardID = nuidToStr(rfid.serNum);

      while (f.available()) {
        String nuid = fileReadUntil('\n');
        if (nuid == cardID) {
          Serial.println("cardexists");
          f.close();
          return;
        }
      }
      f.close();

      /*Serial.println("username?");
        isUsernameRequired = true;*/
      // TODO: username?
      Serial.println("masterpass?");
      isMasterRequired = true;
      isUserCreation = true;
    }
    /*else if (isUsernameRequired && command.startsWith("username")) {
      // TODO: 15 chars max
      username = command.substring(9);
      //debugPrintln(username);
      isUsernameRequired = false;

      Serial.println("masterpass?");
      isMasterRequired = true;
      }*/
    else if (isMasterRequired && command.startsWith("masterpass")) {
      // TODO: Check incoming password
      String m = command.substring(command.indexOf(' ') + 1);

      if (m.length() < 10 || m.length() > 16) {
        //debugPrintln("Пароль не удовлетворяет условиям (10 <= длина <= 16)");
        return;
      }

      //debugPrintln(m);

      if (isUserCreation) {
        auto key = generateKey(rfid.serNum, &m);

        Serial.print("# ");
        for (byte i = 0; i < 32; i++) {
          Serial.print(key[i]); Serial.print(' ');
        }
        Serial.println();

        f = SD.open("cards.txt", FILE_WRITE);
        f.print(nuidToStr(rfid.serNum)); f.print('\n');
        f.close();

        // Подсчёт существующих пользователей
        File directory = SD.open("usr/");
        byte maxN = 0;
        while (true) {
          f = directory.openNextFile();
          if (!f) {
            break;
          }

          if (!f.isDirectory()) {
            String n = f.name();
            if (n.endsWith(".USR")) {
              // 0 - если перевести не удалось
              byte num = n.substring(0, n.indexOf('.')).toInt();
              maxN = max(num, maxN);
            }
          }
        }
        directory.close();

        f.close();
        f = SD.open("usr/" + String(maxN + 1) + ".usr", FILE_WRITE);
        AES_init_ctx_iv(&ctx, key, salt);

        // CheckHeader
        uint8_t chkHeader[16] = {'h', 'f', 'i', 'l', 'e', 0, };

        // Username
        uint8_t usrBlock[16]  = {'B', 'l', 'a', 'c', 'k', 'B', 'o', 'x', 0,};

        // SERVICE HEADER
        uint8_t srvHeader[16] = {'h', 's', 'e', 'r', 'v', 'i', 'c', 'e', 0,};

        AES_CBC_encrypt_buffer(&ctx, chkHeader, 16);
        f.write(chkHeader, 16);

        AES_CBC_encrypt_buffer(&ctx, usrBlock, 16);
        f.write(usrBlock, 16);

        AES_CBC_encrypt_buffer(&ctx, srvHeader, 16);
        f.write(srvHeader, 16);

        f.close();
        isUserCreation = false;
        Serial.println("usercreated");

        delete key;
      }
      else if (isLoginProcess) {
        auto key = generateKey(rfid.serNum, &m);

        Serial.print("# ");
        for (byte i = 0; i < 32; i++) {
          Serial.print(key[i]); Serial.print(' ');
        }
        Serial.println();

        File directory = SD.open("usr/");
        bool isUserFound = false;
        while (true) {
          f = directory.openNextFile();
          if (!f) {
            Serial.println("Breaking...");
            break;
          }

          String n = f.name();

          AES_init_ctx_iv(&ctx, key, salt);
          if (!f.isDirectory() && n.endsWith(".USR")) {
            Serial.println("# " + n);
            if (f.available()) {
              fileReadBlock();
              AES_CBC_decrypt_buffer(&ctx, blockBuffer, 16);
              Serial.print("# "); dumpBuffer(blockBuffer, 16);

              if (strcmp(blockBuffer, "hfile") == 0) {
                isUserFound = true;
              }
            }
          }

          if (isUserFound) break;

          f.close();
        }
        directory.close();

        if (!isUserFound) {
          Serial.println("masterincorrect");
          delete key;
          return;
        }
        else {
          fileReadBlock();
          AES_CBC_decrypt_buffer(&ctx, blockBuffer, 16);
          Serial.print("username ");
          Serial.write(blockBuffer, 16);
          Serial.println();

          f.close();
        }

        delete key;
        isLoginProcess = false;
      }

      isMasterRequired = false;
    }
    else if (command == "userlogin") {
      Serial.println("putRFID");
      while (!readRFID()) {
        ; // Ожидаем корректного ввода карты
      }
      //debugPrintln("Используем прочитанную карту, чтобы авторизоваться");

      f = SD.open("cards.txt");
      String cardID = nuidToStr(rfid.serNum);

      bool cardExists = false;
      while (f.available()) {
        String nuid = fileReadUntil('\n');
        if (nuid == cardID) {
          cardExists = true;
          break;
        }
      }
      f.close();

      if (!cardExists) {
        Serial.println("nosuchcard");
        return;
      }

      isLoginProcess = true;
      Serial.println("masterpass?");
      isMasterRequired = true;
    }
    else if (command == "ping_blackbox") {
      Serial.println("pong_blackbox");
    }
    // DEBUG
    else if (command == "resetcards") {
      SD.remove("cards.txt");
      rm(SD.open("usr/"), "usr/");
      SD.rmdir("usr/");
    }
    else {
      Serial.println("# Command unknown / can't be accepted now");
      isCommandOK = false;
    }

    if (isCommandOK) Serial.println("# OK.");
  }
}
