#include <DHT.h>
#include "SdsDustSensor.h"

// Set DHT pin:
#define DHTPIN 1
#define DHTTYPE DHT11  
// Initialize DHT sensor for normal 16mhz Arduino:
DHT dht = DHT(DHTPIN, DHTTYPE);

//Set up DustSensor variables
SdsDustSensor sds(Serial1);
long interval = 30000;

void setup() {
  // Setup sensor:
  dht.begin();
  Serial.begin(9600);
  delay(2000);
  Serial.begin(9600);
  delay(1000);
  sds.begin();
  delay(1000);
}

void loop() {

  // Read the humidity in %:
  float h = dht.readHumidity();
  // Read the temperature as Celsius:
  float t = dht.readTemperature();
  //Calc Dewpt
  float d = log(h/100) + (17.62 * t) / (243.12 + t);
  d = 243.12 * d / (17.62 - d);
  // Print the temperature and the humidity on the Serial:
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("DewPoint is: ");
  Serial.print(d);
  Serial.println("");


  sds.wakeup();
  Serial.println("Fan ON for 30 seconds");
  delay(30000);
  PmResult pm = sds.queryPm();
  delay(1000);
  
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25);
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10);
    if(pm.pm25<=50){
    Serial.println("High Visibility");
  }  
  else {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }
  
  WorkingStateResult state = sds.sleep();
  delay(1000);
  
  if (state.isWorking()) {
    Serial.println("Problem with sleeping the sensor.");
  } 
  else {
  Serial.print("Sensor sleeps for ");
  Serial.print(interval / 1000);
  Serial.println(" seconds");

  //Smog (smoke+fog) detection and visibility estimation
  if((abs(t - d) <= 2.5) && (pm.pm25>50)){
    Serial.println("Smog has begun");
    if(pm.pm25<=100){
      Serial.println("Basic Smog. Visibility below average");
      }
    if((pm.pm25>100) && (pm.pm25 < 200) ){
      Serial.println("Medium Smog. Poor Visibility");
      }
    if(pm.pm25>200){
      Serial.println("High Smog. Extreme Low Visibility");
      }
  }
  //Haze (Only pollutant particles) detection and visibility estimation
  else if(pm.pm25>100){
  if(pm.pm25<200)
  Serial.println("Basic Haze. Visibility below average");
  if(pm.pm25>200)
  Serial.println("Medium/High Haze. Poor Visibility");     
  }
  }
  delay(interval);
  }  
}
