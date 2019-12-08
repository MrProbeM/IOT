//............Library.............//
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_SERVERPORT 1883
#define MQTT_USERNAME "sourav"
#define MQTT_KEY "11225633"

#include "EmonLib.h"
// Include Emon Library
EnergyMonitor emon1;
// Create an instance


//.....................MQTT_topic.....................//

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);
Adafruit_MQTT_Publish Digital = Adafruit_MQTT_Publish(&mqtt, "dsviot1");

//.........Global...........//
//int D5=0;
//int D6=0;
//int analogPin = A0;
int sensorValue;
double Irms;

unsigned long lastMsg = 0;

//int D3=0; //digital 3
int counter = 0;
int state;
int laststate = HIGH;

int count;
long prev_time;
//long interval = 60*1000ul; // time in ms
long interval = 60000ul; // time in ms

float ratio = 30;

//..........main_setup..........//

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  pinMode(D3, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  emon1.current(A0, 111.1);             // Current: input pin, calibration.
}


//............Analog Current Sensor..............//
int analog_sensor_data() {
  Irms = emon1.calcIrms(1480);  // Calculate Irms only
  return Irms;
}


//..........................Counter Data..............................//
int counter_data(){
  prev_time = millis();
  counter = 0;
  Serial.println("inside counter");
  while((millis() - prev_time) < interval){
    int state = digitalRead(D3);
       if ( state != laststate) {        
       //was motion detected
 //    Serial.println("start!!");
       counter=counter+1;   
      }
  laststate = state;
    delay(0);
  }
  return counter;
}

//..........................WiFi_setup..............................//

void set_wifi() {
  int tryCount = 0;
  Serial.println("");
  Serial.println("Connecting to WiFi");
  WiFi.begin("DataSoft_WiFi", "support123");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    delay(1000);
    Serial.println("Retrying to Connect WiFi..");
    tryCount++;
    if (tryCount == 10) return loop(); //exiting loop after trying 10 times
  }

  if (tryCount != 0 && tryCount < 10) {
    Serial.println("Connected");
    green_led();
    Serial.println(WiFi.localIP());
    tryCount = 0;
  }
}

//.............................MQTT_connection...............................//

void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  green_led();
}

//..............................mqtt_publish............................//

void publish_data() {
  String msg2 = "";
  msg2 = msg2 + "{\"device_id\":" + 3 + ", " + "\"analog_sensor\":" + Irms  +", " + "\"counter\":" + counter + "}";
  //msg2= msg2 + "test4";
  Serial.println(msg2);
  delay(1000);

  if (! Digital.publish(msg2.c_str())) { 
    Serial.println(F("MQTT transmit Failed"));
    delay(50);
    red_led();
  }
  else
  {
    Serial.println(F("MQTT transmit OK!"));
//    delay(200);
    green_led();
  }
  WiFi.disconnect();
  Serial.println("WiFi disconnected");
//  delay(200);
  red_led();
  delay(1000);
  //deep_sleep();
}

void green_led() {
  digitalWrite(D5, HIGH);
  delay(1000);
  digitalWrite(D5, LOW);
}

void red_led() {
  digitalWrite(D6, HIGH);
  delay(1000);
  digitalWrite(D6, LOW);
}


void loop() {
  Serial.println("awake!!");
  //delay(40*1000ul);
  Serial.println("start!!");

  Serial.println("");
  counter_data();
  analog_sensor_data();
  set_wifi();
  if (WiFi.status() == WL_CONNECTED) {
    MQTT_connect();
    publish_data();
    delay(1000);
  }
}
