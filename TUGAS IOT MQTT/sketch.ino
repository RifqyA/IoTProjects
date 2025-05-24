#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define ssid "Wokwi-GUEST"     // Sudah pasti untuk simulasi Wokwi
#define password ""

#define THINGSBOARD_SERVER "thingsboard.cloud"      // Alamat server Thingsboard
#define DEVICE_TOKEN "Oxzxe6681fAUGOYRXLSt"        

WiFiClient espClient;
PubSubClient client(espClient);

const int trigpin  = 18;      
const int echopin = 19; 
const float SOUND_SPEED = 0.0343;
#define DHTTYPE DHT22  
const int DHTpin = 4;

DHT dht(DHTpin, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(""); 
  Serial.print("WiFi connected to: ");
  Serial.println(ssid); 
  Serial.println("IP address: ");     
  Serial.println(WiFi.localIP());
  delay(2000);
  client.setServer(THINGSBOARD_SERVER, 1883);
}

void loop() {
  delay(2000);
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  float duration = pulseIn(echopin, HIGH);
  
  // Calculate the distance
  float distanceCm = duration * SOUND_SPEED/2;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

   while (!client.connected()) {
    if (client.connect("ESP32Client", DEVICE_TOKEN, NULL)) {
      break;
    } else {
      Serial.print("Gagal terhubung, akan mencoba dalam 3 detik...");
      delay(3000);
    }
  }
}

