#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>


  /*
     Define Wi-Fi Access point username/PW
     Define Firebase server 
     Define IFTT Server --> this will be part of our second semister implemtaion
  */ 
#define FIREBASE_HOST "iot-bbms-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "8Kehg3t6FFuhN5u55vYmmZWZ19T0TjZ0TaYmpydP"
#define WIFI_SSID "hp"
#define WIFI_PASSWORD "eSnbJaBp"

  /*
  Define NodeMcu.GPIO pin's for Realy bord for switching actuators
      * swing motor 
      * fan
      * musical toy
  */
#define realyInput_fun    2     //D4
#define realyInput_motor 12     //D6
#define realyInput_music 15     //D8


  /*
  Define NodeMcu.GPIO pin's for sensor's
      * Temperature and Humidity
      * sound   
  */ 
  /*Temperature and Humidity*/
  
#define DHTPIN 5  //D1 pin in NODEMCU
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

   /*sound */

#define sensor A0



  /* 
    *Assigne the treshold value's for 
    *temprature 
    *humidity
    *sound
  */
float TEMTH =28.0 ; //Temprature treshold 
float HTH = 70.0 ; //Humidity treshold 
int STH = 50 ;   //sound treshold
                                                
/*CRIE DETECTION*/
String Sound_Detection ="OFF";
                                                              
void setup() 
{
  Serial.begin(9600);
  delay(1000);                     
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
 
  dht.begin();

  // assign pin of realy out's 
  pinMode(realyInput_fun,OUTPUT);
  pinMode(realyInput_motor,OUTPUT);
  pinMode(realyInput_music,OUTPUT);
  
  // set them initialy to zero
  digitalWrite(realyInput_fun,LOW);
  digitalWrite(realyInput_motor,LOW);
  digitalWrite(realyInput_music,LOW);
  
  
}
 
void loop() 
{  
    /*get the sensoring value of sound */
    
    int sound_value = analogRead(sensor);
    Serial.println(sound_value);
//    
   delay(1000);
  /*get the sensoring value of temprature and humidity*/
  
  float humidity = dht.readHumidity();                                 // Read Humidity
  float temperature = dht.readTemperature();                           // Read temperature
  
  if (isnan(humidity) || isnan(temperature))                           // Checking sensor working
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
  delay(100);
  
 
    checkForSound(sound_value);

   ReadfromFirebase();
 
    checkForTempartureandHumidity(temperature,humidity);
    
  
    uploadDataToServer(fireHumid,fireTemp,Sound_Detection);

    ReadfromFirebase();
  


 
}
void ReadfromFirebase(){

//Read data from firebase 

  String motor_status = Firebase.getString("Motor");
  String Fun_status = Firebase.getString("Fan");
  String Music_status = Firebase.getString("Music");
  
//    check for fun 
  if(motor_status == "ON"){
    digitalWrite(realyInput_motor,HIGH);
    
  }else if(motor_status == "OFF"){
    digitalWrite(realyInput_motor,LOW);
    
  }else if (Fun_status == "ON"){
    digitalWrite(realyInput_fun,HIGH);
    
  }else if (Fun_status == "OFF"){
     digitalWrite(realyInput_fun,LOW);
     
  }else if (Music_status == "ON"){
     digitalWrite(realyInput_music,HIGH);
     
  }else if (Music_status == "OFF"){
    digitalWrite(realyInput_music,LOW);
  }

  
  }
void uploadDataToServer(String fireHumid,String fireTemp,String Sound_Detection){

  Firebase.setString("/Sound Detection",Sound_Detection);
  Firebase.setString("/Status/Humidity", fireHumid);            //setup path to send Humidity readings
  Firebase.setString("/Status/Temperature", fireTemp);         //setup path to send Temperature readings
    if (Firebase.failed()) 
    {
 
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error()); 
      return;
  }
 }

void checkForSound(int sound_value){

    /* check if sound
      * is greater than the threashold value's
      * and turn on/off 
      * Swing Motor 
      * Switch on Musical toy
      */

  if (sound_value > STH){
            Serial.println("swing Motor Turned ON");
            Firebase.setString("/Motor", "ON");
            
            Serial.println("Music toy Turned ON");
            Firebase.setString("/Music", "ON");

                
           /* Notify parent's via IFTTT 
            * this will part of our second semister implemntation
            */

            Sound_Detection ="ON";
  }else if(sound_value <STH){
    Sound_Detection ="OFF";
  }
  
}
 
void checkForTempartureandHumidity(float temperature,float humidity){
  
     
     /* check if temperature and humidity 
      * is greater than the threashold value's
      * and turn on/off the fun
      */
      if (temperature >TEMTH || humidity >HTH ) {
          Serial.println("Fun Turned ON"); 
          Firebase.setString("/Fan", "ON");  

     /* Notify parent's via IFTTT 
      * this will part of our second semister implemntation
      */
      
      }else{
            Serial.println("Fun Turned OFF");
            Firebase.setString("/Fan", "OFF");
            
            Serial.println("swing Motor Turned OFF");
            Firebase.setString("/Motor", "OFF");
            
            Serial.println("Music toy Turned OFF");
            Firebase.setString("/Music", "OFF");
      }
     
  
  }