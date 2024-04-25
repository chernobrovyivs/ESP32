/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-water-sensor
 */

 #define POWER_PIN  14 // ESP32 pin GPIO17 connected to sensor's VCC pin
 #define SIGNAL_PIN 35 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin

int value = 0; // variable to store the sensor value

void setup() {
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);   // configure pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
}

void loop() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("The water sensor value: ");
  Serial.println(value);

  delay(1000);
}