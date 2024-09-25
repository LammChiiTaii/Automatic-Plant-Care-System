#include <SoftwareSerial.h>

SoftwareSerial ArduinoSerial(10, 11); // RX, TX

uint64_t timer = 0;

const int s[3] = {A3, A5, A7};
const int b[3] = { 2,  3,  4};
const int v    =   5;
const int w    =  A0;
int soil[3]    = { 0,  0,  0};
int nuoc       = 0;
String data_send = "";

int nguongdoam = 50 ;
int nguongnuoc = 400;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  ArduinoSerial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(s[i], INPUT) ;
    pinMode(b[i], OUTPUT);
  }
  pinMode(w, INPUT) ;
  pinMode(v, OUTPUT);
}

void loop() { // run over and over
  for (int i = 0; i < 3; i++){
    soil[i] = map(analogRead(s[2-i]), 0, 1023, 100, 0);
    if (soil[i] < nguongdoam) digitalWrite(b[i], HIGH);
    else digitalWrite(b[i], LOW);
  }

  nuoc = analogRead(w);
  if(nuoc < nguongnuoc) digitalWrite(v, HIGH);
  else digitalWrite(v, LOW);

  if (millis() - timer >= 1500){
      for(int i = 0; i < 3; i++){
        data_send += String(soil[i]);
        if (i<2) data_send += ";";
      }
      ArduinoSerial.println(data_send);
      Serial.println(data_send);
      data_send = "";
    timer = millis();
  }

}
