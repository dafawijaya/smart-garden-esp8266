#define BLYNK_PRINT Serial

#ifdef ESP32 // ESP32 libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#else // ESP8266 libraries
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#endif
#include <DHT.h>

int MODE = 0;


#define DHTPIN 0          // (d3)
#define relay 16          // (d0)
#define buzzer 2          // (d4)
#define led 14            // (d5)

const int soilpin = A0;   //analog pin
//kalibrasi nilai sensor
const int dry = 765; // value sensor kering
const int wet = 385; //value sensor basah

int soilmoisture = 0;
int persentase = 0;


#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

char ssid[] = "Kelg_Hadie"; // isi nama network
char pass[] = "Vitri1972" ; // isi pass wifi, kalau open network isi ""
char auth[] = "SL5_Bqv9HskOcnwT0oPrGFAyVn0KCofa";

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // atau dht.readTemperature(true) untuk Fahrenheit

  soilmoisture = analogRead(soilpin);
  //sensor di berikan range value yg telah di kalibrasi
  persentase = map(soilmoisture, wet, dry, 100, 0);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (isnan(t)) {
    return;
  }
  if (t > 35) {
    Blynk.notify(String("Temperature terlalu tinggi : ") + t);
  }

  if (persentase < 20) // jika kurang dari 10% maka akan notifikasi blynk
  {
    Serial.println("tanah kering");
    
   
  }



  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V7, soilmoisture);
  Blynk.virtualWrite(V8, persentase);
}

void checkBlynk() { // memanggil setiap 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    MODE = 1;
    digitalWrite(led, LOW);

  }
  if (isconnected == true) {
    MODE = 0;
    digitalWrite(led, HIGH);

  }
}


void setup()
{

  Serial.begin(9600);
  pinMode (relay , OUTPUT);
  pinMode (buzzer , OUTPUT);
  pinMode (led , OUTPUT);
  Blynk.begin(auth, ssid, pass);
  // Specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  dht.begin();

  // Setup  function untuk send sensor setiap 1000ms dan check koneksi 
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(3000L, checkBlynk);
}
void loop()
{
  Blynk.run();
  timer.run(); // Initiates SimpleTimer

}
