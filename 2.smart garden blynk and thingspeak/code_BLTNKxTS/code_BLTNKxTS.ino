/***************************************************************************************************************
 * smart garden v1.0
 * referensi https://www.hackster.io/mjrobot/from-blink-to-blynk-an-iot-journey-on-the-wings-of-nodemcu-764aa3
 * dikembangkan oleh dafa wijaya
 * github.com/dafawijaya

 ********************************************************************************************************************************/
#define SW_VERSION "Version Blynk&TS" // SW version will appears at innitial LCD Display

/* ESP */
#include <ESP8266WiFi.h>
WiFiClient client;

/* Blynk */
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <BlynkSimpleEsp8266.h>
char ssid[] = "Kelg_Hadie"; // isi nama network
char pass[] = "Vitri1972" ; // isi pass wifi, kalau open network isi ""
char auth[] = "SL5_Bqv9HskOcnwT0oPrGFAyVn0KCofa";


/* Thinkspeak*/
const char* TS_SERVER = "api.thingspeak.com";
String TS_API_KEY ="GSVWEUR63PJV0ING";



/* DHT22*/
#include "DHT.h"
#define DHTPIN 0  
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
float hum = 0;
float temp = 0;

/* TIMER */
#include <SimpleTimer.h>
SimpleTimer timer;

/* Soil Moister */
#define soilMoisterPin A0
#define soilMoisterVcc D4
int soilMoister = 0;

void setup()
{
  pinMode(soilMoisterVcc, OUTPUT);
  Serial.begin(115200);
  delay(10);
  Blynk.begin(auth, ssid, pass);
  
  dht.begin();
  timer.setInterval(2000L, getDhtData);
  timer.setInterval(7000L, getSoilMoisterData);
  timer.setInterval(5000L, sendUptime);
  timer.setInterval(19000L, sendDataTS);
  digitalWrite (soilMoisterVcc, LOW);
}

void loop()
{
  
  timer.run(); // Initiates SimpleTimer
  Blynk.run();
}

/***************************************************
 *  DHT data
 **************************************************/
void getDhtData(void)
{
  float tempIni = temp;
  float humIni = hum;
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(hum) || isnan(temp))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    temp = tempIni;
    hum = humIni;
    return;
  }
}

/***************************************************
 *  Soil Moister Sensor data
 **************************************************/
void getSoilMoisterData(void)
{
  soilMoister = 0;
  digitalWrite (soilMoisterVcc, HIGH);
  delay (500);
  int N = 3;
  for(int i = 0; i < N; i++) // read sensor "N" times and get the average
  {
    soilMoister += analogRead(soilMoisterPin);   
    delay(150);
  }
  digitalWrite (soilMoisterVcc, LOW);
  soilMoister = soilMoister/N; 
  Serial.println(soilMoister);
  soilMoister = map(soilMoister, 380, 0, 0, 100); 
}

/***************************************************
 * Send data ke blynk
 **************************************************/
void sendUptime()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(6, temp); //virtual pin V10
  Blynk.virtualWrite(5, hum); // virtual pin V11
  Blynk.virtualWrite(7, soilMoister); // virtual pin V12
}




/***************************************************
 * Sending Data ke Thinkspeak 
 **************************************************/
void sendDataTS(void)
{
   if (client.connect(TS_SERVER, 80)) 
   { 
     String postStr = TS_API_KEY;
     postStr += "&field1=";
     postStr += String(temp);
     postStr += "&field2=";
     postStr += String(hum);
     postStr += "&field3=";
     postStr += String(soilMoister);
     postStr += "\r\n\r\n";
   
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: " + TS_API_KEY + "\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     delay(1000); 
   }
   client.stop();
}
