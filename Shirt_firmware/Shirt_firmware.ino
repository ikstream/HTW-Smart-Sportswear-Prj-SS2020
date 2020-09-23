// Based on the Code of
// Juan A. Villalpando
// http://kio4.com/appinventor/9A0_Resumen_Bluetooth.htm
// Evandro Copercini


#define HEART_RATE_SENSOR A0
#define LO_PLUS 15
#define LO_MINUS 35
#define HEART_RATE_POS 0
#define GPS_TX 34
#define GPS_RX 12

#include "BluetoothSerial.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <TinyGPS++.h>

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);
BluetoothSerial SerialBT;

int heart_rate = 0;
int BPM = 0;
int send_time;
int gps_time;
int heart_time;
boolean belowThreshold = true;
String payload;
float threshold = 725.0;


unsigned int average_value[60];
int i = 0;
long lastDebounceTime = 0;
long debounceDelay = 300;

static void fake_heart_rate(void)
{
  int cur_time = millis();
  int heart_diff = cur_time - heart_time;
  if (heart_diff < 120000)
  {
    BPM = random(1, 3);
  }
  else if (heart_diff < 180000)
  {
    BPM = random(2, 4);
  }
  else if (heart_diff < 240000)
  {
    BPM = random(2, 5);
  }
  
  else if (heart_diff < 360000)
  {
    BPM = random(1, 4);
  }
  else
  {
    BPM = random(1, 3);
  }
}

static void read_heart_rate(void)
{
  pinMode(HEART_RATE_SENSOR, INPUT);
  int cur_time = millis();

  // read heart rate
  if ((digitalRead(LO_PLUS) == 1) || (digitalRead(LO_MINUS) == 1))
  {
    heart_rate = 1;
  }
  else
  {
    heart_rate = analogRead(A0);

    if (heart_rate > threshold && belowThreshold == true)
    {
      BPM++;

      belowThreshold = false;
    }
    else if (heart_rate < threshold)
    {
      belowThreshold = true;
    }
  }
}

static void send_data(void)
{
  int cur_time = millis();
  //Serial.println(payload);
  if ((cur_time - send_time) >= 1000)
  {
    //fake_heart_rate();
    payload = "";
    payload.concat(BPM);
    payload.concat(":");
    payload.concat(gps.location.lat());
    payload.concat(":");
    payload.concat(gps.location.lng());
    SerialBT.println(payload);
    Serial.println(payload);
    send_time = cur_time;
    BPM = 0;
  }
}

static void read_gps(void)
{
  int cur_time = millis();
  if ((cur_time - gps_time) >= 1000)
  {
    if (SerialGPS.available())
    {
      gps.encode(SerialGPS.read());

    }
    gps_time = cur_time;
  }
}

void setup(void)
{
  Serial.begin(115200);
  SerialBT.begin("SHIRT"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);   //17-TX 18-RX
  gps_time = millis();
  send_time = millis();
  heart_time = millis();
}

void loop(void)
{
  payload = "";
  read_heart_rate();
  read_gps();
  send_data();

  delay(1); // It should be slower than the Clock Interval.
}
