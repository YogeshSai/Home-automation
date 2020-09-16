// Credits to be given if used in commercial spaces

#include "BluetoothSerial.h"  // including blutooth 
#include <WiFi.h>  // including wifi
#include "Adafruit_MQTT.h"  // including And calling ADAFRUIT, login https://io.adafruit.com/ here and create your triggers
#include "Adafruit_MQTT_Client.h" // intializing Adafruit web server as a client

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run
`make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int bluedata;  // Data recieved from the bluetooth app which I linked
int relay1 = 12;  // connecting relay 1 to GPIO 12 of ESP32
int relay2 = 2;  // Connecting relay 2 to GPIO 2 of ESP32

//I have called these relay triggers twice because top one is for Bluetooth and the below one is for MQTT server 

#define Relay2  2
#define Relay1 12

// You can add many output triggers in view of the output pins

#define WLAN_SSID       "Your SSID"
#define WLAN_PASS       "Your wifi password"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Your Adafruit username"           // You will find this in your profile
#define AIO_KEY         "aio_xxxxxxxxxxxxxxxxxxxxx"        // You will find this in your profile

// Calling Adafruit MQTT functions

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish Light = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/light");
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/xxxxxx"); // Change the trigger names in the x's
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/xxxxxx"); // Change the trigger names in the x's

void MQTT_connect(); // Refer down for this function

void setup() {
  Serial.begin(9600);

  btStart(); // Turning ON bluetooth in ESP32
  Serial.println("Bluetooth On");

  SerialBT.begin("ESP32_Bluetooth"); //Bluetooth device name, Even you can name your bluetooth device
  Serial.println("The device started, now you can pair it with bluetooth!");
  delay(1000);
  
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);

  Serial.println(F("Adafruit MQTT"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  
  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Light1); 
  mqtt.subscribe(&Light2);
  
}


void loop() {
  if (SerialBT.available())
  {

    Bluetooth_handle();

  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Not Connected");
    delay(5000);
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    delay(1000);
  }
  else
  {
   MQTT_connect();
   Adafruit_MQTT_Subscribe *subscription;
   while ((subscription = mqtt.readSubscription(20000))) {
     if (subscription == &Light1) {
       Serial.print(F("Got: "));
       Serial.println((char *)Light1.lastread);
       int Light1_State = atoi((char *)Light1.lastread);
       digitalWrite(Relay1, Light1_State);
       }
    if (subscription == &Light2) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light2.lastread);
      int Light2_State = atoi((char *)Light2.lastread);
      digitalWrite(Relay2, Light2_State);
      }
  } 
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
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
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void Bluetooth_handle()
{
  //char bluedata;
  if (SerialBT.available())
  {
    Serial.println("BLUETOOTH CONNECTED");
  }
  bluedata = SerialBT.parseInt();
  //Serial.println(bluedata);
  delay(20);
  if (1 == bluedata) {
    digitalWrite(relay1, HIGH);
    SerialBT.println("relay1 on");
    Serial.print("Relay1 on\n");
  }
  else if (2 == bluedata) {
    digitalWrite(relay1, LOW);
    SerialBT.println("relay1 off");
    Serial.print("Relay1 off\n");
  }
  else if (3 == bluedata) {
    digitalWrite(relay2, HIGH);
    SerialBT.println("relay2 on");
    Serial.print("Relay2 on\n");
  }
  else if (4 == bluedata) {
    digitalWrite(relay2, LOW);
    SerialBT.println("relay2 off");
    Serial.print("Relay2 off\n");
  }
  else
  {
  }
}
