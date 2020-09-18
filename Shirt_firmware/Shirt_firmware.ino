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
int beat_old = 0;
float beats[500];  // Used to calculate average BPM
int beatIndex;
int send_time;
int gps_time;
int heart_time;
boolean belowThreshold = true;
String payload;
int test_bpm = 0;
float threshold = 10.0;


unsigned int average_value[60];
int i = 0;
long lastDebounceTime = 0;
long debounceDelay = 300;

static void calculateBPM ()
{
  BPM = test_bpm * 4;
  test_bpm = 0;
}

static void read_heart_rate(void)
{
  pinMode(HEART_RATE_SENSOR, INPUT);
  int cur_time = millis();
  //int randNumber = random(50, 200);
  //payload.concat(randNumber);
  //payload.concat(":");

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
      test_bpm++;

      belowThreshold = false;
    }
    else if (heart_rate < threshold)
    {
      belowThreshold = true;
    }
  }
  if ((cur_time - heart_time) >= 15000)
  {
    Serial.println("Going to BPM calc");
    calculateBPM();
    heart_time = cur_time;
  }
}

static void send_data(void)
{
  int cur_time = millis();
  //Serial.println(payload);
  if ((cur_time - send_time) >= 1000)
  {
    payload = "";
    payload.concat(BPM);
    payload.concat(":");
    payload.concat(gps.location.lat());
    payload.concat(":");
    payload.concat(gps.location.lng());
    SerialBT.println(payload);
    Serial.println(payload);
    send_time = cur_time;
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
