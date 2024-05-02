#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"

const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

String ssid;
String pass;
String ip;
String gateway;

AsyncWebServer server(80); //Инициализируем сервер
IPAddress localIP;
IPAddress localGateway;
IPAddress subnet(255, 255, 0, 0); //Маска

unsigned long previousMillis = 0;
const long interval = 10000; //Сколько ждем перед открытием портала
const int ledPin = 2;
String ledState;

void setup() {
  Serial.begin(115200); //Открываем порт
  initSPIFFS(); //Инициализируем spiffs
  pinMode(ledPin, OUTPUT); //Светодиод на выход
  digitalWrite(ledPin, LOW);

  //Загружаем в переменные данные из SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  ip = readFile(SPIFFS, ipPath);
  gateway = readFile (SPIFFS, gatewayPath);
  //Если есть wifi
  if(initWiFi()) {
    // Открываем успешную страницу index.html
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/index.html", "text/html");
    });
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){ //Подключаем стили
      request->send(SPIFFS, "/bootstrap.min.css", "text/css");
    });
    server.begin();
  }
  //Иначе
  else {
    Serial.println("Setting AP (Access Point)"); //Раздаем точку доступа
    WiFi.softAP("ESP-CONNECT", NULL); // Создаем открытую точку доступа с именем ESP-CONNECT

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { //Запускаем сервер с формой конфигурации
      request->send(SPIFFS, "/conf.html", "text/html");
    });

    server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){ //Подключаем стили
      request->send(SPIFFS, "/bootstrap.min.css", "text/css");
    });
    
    server.serveStatic("/", SPIFFS, "/");
    
    //Получаем данные из формы, если пришел запрос
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          if (p->name() == PARAM_INPUT_1) { // Получаем имя сети из формы
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          if (p->name() == PARAM_INPUT_2) { // Получаем пароль из формы
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
          if (p->name() == PARAM_INPUT_3) { // Получаем POST запрос про IP
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(SPIFFS, ipPath, ip.c_str());
          }
          if (p->name() == PARAM_INPUT_4) { // Получаем POST запрос про Gateway путь
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(SPIFFS, gatewayPath, gateway.c_str());
          }
        }
      }
      request->send(200, "text/plain", "Успешно, esp перезагрузиться и получит адрес:" + ip);
      delay(3000);
      ESP.restart(); //Перезагружаем esp
    });
    server.begin(); //Запускаем сервер в любом случае
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

String readFile(fs::FS &fs, const char * path){ //Чтение файла из spiffs
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){ //Функция записи файла в spiffs
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
}

bool initWiFi() { //Функция инициализации wifi
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());


  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}