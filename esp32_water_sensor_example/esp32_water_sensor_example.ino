#include <FastBot.h>
#include <datatypes.h>
#include <utils.h>
#include <HTTPClient.h>
#include <string> 

/*
  ESP32_Water_Sensor_Example for ESP32-WROOM
  version: 0.1.0
  date: 25.04.2024
  modified 25.04.2024
  Developer: Valeriy Chernobrovyi (chernobrovyivs)
*/

 // вводим имя и пароль точки доступа
const char* ssid = "Chernobrovyis Network";
const char* password = "09062021";

 #define POWER_PIN  14 // ESP32 pin GPIO17 connected to sensor's VCC pin
 #define SIGNAL_PIN 35 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin
 #define BOT_TOKEN "7036639733:AAFHdqEOIYFm4AlXVOf0w1rI1vQ56LEy9jY"
 #define CHAT_ID "27406386"

//FastBot bot;
FastBot bot(BOT_TOKEN); // с указанием токена

 int value = 0; // variable to store the sensor value

void setup() {
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);   // configure pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF

  // подключаемся к Wi-Fi сети
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to Wi-Fi..");
    }
    Serial.println("Connected to the Wi-Fi network");

  bot.attach(newMsg);   // подключаем обработчик сообщений
  bot.setChatID("27406386");
  bot.sendMessage("Hello, World!");
}

void loop() {
  // выполняем проверку подключения к беспроводной сети
    if ((WiFi.status() == WL_CONNECTED)) {
        // создаем объект для работы с HTTP
        HTTPClient http;
        // подключаемся к тестовому серверу с помощью HTTP
        http.begin("http://httpbin.org/");
        // делаем GET запрос
        int httpCode = http.GET();
        // проверяем успешность запроса
        if (httpCode > 0) {
            // выводим ответ сервера
            String payload = http.getString();
            Serial.println(httpCode);
            Serial.println(payload);
        }
        else {
            Serial.println("Error on HTTP request");
        }
        // освобождаем ресурсы микроконтроллера
        http.end();
    }
    delay(10000);
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("The water sensor value: ");
  Serial.println(value);

  if (value >= 1000){
    Serial.println("ALEARMA! WARNING!");
  } else if (value < 100){
    Serial.println("Ничего страшного :)");
  } else {
    Serial.println("Подтекает водичка, прям как ваша крыша..");
  }

  String val = (String)value;

  delay(1000);
  bot.tick();
  bot.sendMessage("The water sensor value: " + val);
  if (value >= 1000){
    bot.sendMessage("ALEARMA! WARNING!");
  } else if (value < 100){
    bot.sendMessage("Ничего страшного :)");
  } else {
    bot.sendMessage("Подтекает водичка, прям как ваша крыша..");
  }
}

void newMsg(FB_msg& msg) {
  Serial.print(msg.chatID);     // ID чата 
  Serial.print(", ");
  Serial.print(msg.username);   // логин
  Serial.print(", ");
  Serial.println(msg.text);     // текст

  bot.sendMessage(msg.text, msg.chatID);
}