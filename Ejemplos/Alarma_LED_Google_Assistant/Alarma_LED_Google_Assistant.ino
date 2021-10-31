#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#define WLAN_SSID       "FAMILIA VILLOTA"
#define WLAN_PASS       "sandra1234"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  

#define AIO_USERNAME  "Sebas_Villota"
#define AIO_KEY       "aio_idfk99J7zuaWyXVkq4EEIuvYcTuo"

int output=2;
String formattedDate;
String dayStamp;
String timeStamp;

WiFiClient client;     // Create an ESP8266 WiFiClient class to connect to the MQTT server.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);        // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Subscribe LED_Control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED_Control");
Adafruit_MQTT_Subscribe Hora = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Hora");
Adafruit_MQTT_Subscribe Minuto = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Minuto");
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

pinMode(13,OUTPUT);

  display.begin(i2c_Address, true); // Address 0x3C default
  display.clearDisplay();
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
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  mqtt.subscribe(&LED_Control);
  mqtt.subscribe(&Hora);
  mqtt.subscribe(&Minuto);
  timeClient.begin();
  timeClient.setTimeOffset(-18000);
}
uint32_t x=0;
void loop() {
   MQTT_connect();
Adafruit_MQTT_Subscribe *subscription;
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT); //FECHA
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1); //HORA

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("--------------------");
  display.println("     PRIZE-PETS");
  display.println("--------------------");
  display.println("     Hora Actual    ");
  display.print("Fecha: ");
  display.println(dayStamp);
  display.print("Hora: ");
  display.println(timeStamp);
  display.println("       Alarma    ");
  display.print("       ");
  display.print((char *)Hora.lastread);
  display.print(":");
  display.println((char *)Minuto.lastread);
  display.display();
  display.clearDisplay();
  delay(100);
  
  while ((subscription = mqtt.readSubscription(500))) {
    if (subscription == &LED_Control) {
      Serial.print(F("Got: "));
      Serial.println((char *)LED_Control.lastread);
       if (!strcmp((char*) LED_Control.lastread, "ON"))
      {
        digitalWrite(13, HIGH);
        Serial.println((char *)Minuto.lastread);
      }
      else
      {
        digitalWrite(13, LOW);
        Serial.println((char *)Hora.lastread);
      }
    }
  }
}

void MQTT_connect() {
  int8_t ret;
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
