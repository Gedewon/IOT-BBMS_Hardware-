
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>


// conncet to firebase 
#define FIREBASE_HOST "iot-bbms-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "8Kehg3t6FFuhN5u55vYmmZWZ19T0TjZ0TaYmpydP"
#define WIFI_SSID "hp"
#define WIFI_PASSWORD "eSnbJaBp"

//define pin for temprature and humidity 
#define DHTPIN D0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

String fireStatus = "";                                                     // led status received from firebase
int led = 1;  
                                                              
void setup() 
{
  Serial.begin(9600);
  delay(1000);    
  pinMode(led, OUTPUT);                 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                               
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                  // connect to firebase
  Firebase.setString("LED_STATUS", "OFF");                       //send initial string of led status
  dht.begin();
}
 
void loop() 
{
// defination for sensoring temprature and humidity //
  float humidity = dht.readHumidity();                                 // Read Humidity
  float temperature = dht.readTemperature();                              // Read temperature
  
  if (isnan(humidity) || isnan(temperature))                                     // Checking sensor working
  {                                   
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  } 
  Serial.print("Humidity: ");  
  Serial.print(humidity);
  String fireHumid = String(humidity) + String("%");                   //Humidity integer to string conversion
  
  Serial.print("%  Temperature: ");  
  Serial.print(temperature);  
  Serial.println("°C ");
  String fireTemp = String(temperature) + String("°C");                  //Temperature integer to string conversion
  delay(5000);
 
 
  Firebase.pushString("/DHT/Humidity", fireHumid);            //setup path to send Humidity readings
  Firebase.pushString("/DHT/Temperature", fireTemp);         //setup path to send Temperature readings
    if (Firebase.failed()) 
    {
 
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error()); 
      return;
  }

// end of temprature and humidity//
  
  fireStatus = Firebase.getString("LED_STATUS");                                      // get ld status input from firebase
  if (fireStatus == "ON") 
  {                                                          // compare the input of led status received from firebase
    Serial.println("Led Turned ON");                                                        
    digitalWrite(led, HIGH);                                                         // make external led ON
  } 
  else if (fireStatus == "OFF") 
  {                                                  // compare the input of led status received from firebase
    Serial.println("Led Turned OFF");
    digitalWrite(led, LOW);                                                         // make external led OFF
  }
  else 
  {
    Serial.println("Command Error! Please send ON/OFF");
  }
}
