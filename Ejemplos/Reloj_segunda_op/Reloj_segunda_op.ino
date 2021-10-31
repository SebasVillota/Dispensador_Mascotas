#include <NTPtimeESP.h>
#define DEBUG_ON

NTPtime NTPch("ch.pool.ntp.org");
char *ssid ="FAMILIA VILLOTA";
char *password = "sandra1234";
strDateTime dateTime;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");
  Serial.println("Connecting to Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi Connected");
}

void loop() {
  // put your main code here, to run repeatedly:
  dateTime = NTPch.getNTPtime(-5.0,1);
  if(dateTime.valid){
    NTPch.printDateTime(dateTime);

    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualSecond = dateTime.second;
    int actualyear = dateTime.year;
    byte actualMoth = dateTime.month;
    byte actualDay = dateTime.day;
    byte actualDayofWeek = dateTime.dayofWeek;
  }
   
}
