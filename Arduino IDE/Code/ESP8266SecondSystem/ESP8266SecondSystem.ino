#define DEFAULT_MQTT_HOST "mqtt1.eoh.io"

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "355a07af-aea5-4fff-be57-a147b60fdef8"

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <BH1750FVI.h>

// Khai báo các chân RX và TX cho espsoftwareserial
#define RX_PIN D3 // D2
#define TX_PIN D4 // D1

const char ssid[] = "American Study HD";
const char pass[] = "66668888";

// Tạo một đối tượng espsoftwareserial với tốc độ baud là 9600
SoftwareSerial mySerial(RX_PIN, TX_PIN);

Adafruit_SHT31 sht31 = Adafruit_SHT31();
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

ERaTimer timer;
float temp = 0;
float hum = 0;
uint16_t lux = 0;
int soil[4] = {0, 1, 2, 3};

/* This function print uptime every second */
void timerEvent() {
    ERa.virtualWrite(V7, (ERaMillis() / 1000L / 3600L));
    ERa.virtualWrite(V8, ((ERaMillis() / 1000L / 60L) - int(ERaMillis() / 1000L / 3600L) * 60L));
    ERa.virtualWrite(V9, (ERaMillis() / 1000L) % 60L);
    ERa.virtualWrite(V1 , soil[1]);
    ERa.virtualWrite(V2 , soil[2]);
    ERa.virtualWrite(V3 , soil[3]);
    ERa.virtualWrite(V4 , temp);
    ERa.virtualWrite(V5 , hum );
    ERa.virtualWrite(V6 , lux );
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
      if (data.charAt(i) == separator || i == maxIndex) {
          found++;
          strIndex[0] = strIndex[1] + 1;
          strIndex[1] = (i == maxIndex) ? i+1 : i;
      }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
    /* Setup debug console */
    Serial.begin(9600);
    mySerial.begin(9600);
    ERa.begin(ssid, pass);
    /* Setup timer called function every second */
    timer.setInterval(1000L, timerEvent);
    Wire.begin(D2, D1);
    if (!sht31.begin(0x44)) {  // Địa chỉ I2C của cảm biến (0x44 hoặc 0x45)
      Serial.println("Không tìm thấy SHT31");
      while (1) delay(1);
    }
    LightSensor.begin(); 
}

void loop() {
    temp = sht31.readTemperature();
    hum = sht31.readHumidity();
    lux = LightSensor.GetLightIntensity();

    if (mySerial.available()) {
      String c = mySerial.readString();
      String value1 = getValue(c, ';', 0);
      String value2 = getValue(c, ';', 1);
      String value3 = getValue(c, ';', 2);

      soil[1] = value1.toInt();
      soil[2] = value2.toInt();
      soil[3] = value3.toInt();

  }

    ERa.run();
    timer.run();
    
}

