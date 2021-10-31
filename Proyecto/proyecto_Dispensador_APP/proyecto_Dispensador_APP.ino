#include <FirebaseESP32.h>
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

//DEFINICIÓN DE PARAMETROS PARA OLED
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

FirebaseData firebaseData; //Creacion de la funcion de la base de datos

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
String banderaA = "";
String banderaP = "";
String banderaC = "";
String banderaM = "";
String tipo_BotA;
String tipo_BotP;
String tipo_BotC;
String tipo_BotM;
String H_0,H_1,M_0,M_1,S_0,S_1;
int H,M,S,M_band=0;
int Hora_A;
int Minuto_A;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
Preferences preferences;

void setup() {
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(23,OUTPUT);
  Serial.begin(115200);
  delay(10);
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
  timeClient.begin();
  timeClient.setTimeOffset(-18000);

  Firebase.begin("https://dispensador-mascotas-default-rtdb.firebaseio.com/", "gmg1kyS7moNuINfkmBp9e1XMSwNvUwJTkThdPeHn");
  Firebase.setString(firebaseData, "/ESP32/botonA", "N");
  Firebase.setString(firebaseData, "/ESP32/botonP", "N");
  Firebase.setString(firebaseData, "/ESP32/botonC", "N");
  Firebase.setString(firebaseData, "/ESP32/botonM", "N");
}

void loop() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
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
  display.print("HORA");
  display.print(":");
  display.println("MINUTO");
  display.display();
  display.clearDisplay();
  preferences.begin("myfile",false);
  Hora_A = preferences.getInt("T_alarma",0);
  Minuto_A = preferences.getInt("T_Hora",0);
  preferences.end();
  H_0=timeStamp[0];
  H_1=timeStamp[1];
  M_0=timeStamp[3];
  M_1=timeStamp[4];
  S_0=timeStamp[6];
  S_1=timeStamp[7];
  H_0.concat(H_1);
  M_0.concat(M_1);
  S_0.concat(S_1);
  H=H_0.toInt();
  M=M_0.toInt();
  S=S_0.toInt();
  Serial.print(H);
  Serial.print(":");
    Serial.print(M);
    Serial.print(":");
    Serial.println(S);
    Serial.print(Hora_A);
    Serial.println(Minuto_A);
  if(Hora_A == H && Minuto_A == M){
    if(M != M_band){ 
      M_band = M;
      Serial.println("ENTRO A ALARMA");
      digitalWrite(23,HIGH);
      delay(3000);
      digitalWrite(23,LOW);
  }
  }
  if (Firebase.getString(firebaseData, "/ESP32/botonA")){
    tipo_BotA = firebaseData.stringData();
    
    if(tipo_BotA != banderaA){
      banderaA = tipo_BotA;
      Serial.println(banderaA);
      if(banderaA.indexOf('A') != -1){Modo_Alarma();}
    }
  }
  if (Firebase.getString(firebaseData, "/ESP32/botonP")){
    tipo_BotP = firebaseData.stringData();
    if(tipo_BotP != banderaP){
      banderaP = tipo_BotP;
      Serial.println(banderaP);
      if(banderaP.indexOf('P') != -1){Modo_Peso();}
    }
  }
  if (Firebase.getString(firebaseData, "/ESP32/botonC")){
    tipo_BotC = firebaseData.stringData();
    if(tipo_BotC != banderaC){
      banderaC = tipo_BotC;
      Serial.println(banderaC);
      if(banderaC.indexOf('C') != -1){Modo_Camara();}
    }
  }
  if (Firebase.getString(firebaseData, "/ESP32/botonM")){
    tipo_BotM = firebaseData.stringData();
    if(tipo_BotM != banderaM){
      banderaM = tipo_BotM;
      Serial.println(banderaM);
      if(banderaM.indexOf('M') != -1){Modo_Manual();}
    }
  }
}

void Modo_Alarma(){
  int band = 1;
  String alarm="";
  String band_alarma="",B;
  char Buf[9];
  int cont, num[6], Hora_A,Minuto_A, Tipo_A;
  banderaA=' ';
  digitalWrite(13,HIGH);
  Firebase.setString(firebaseData, "/ESP32/HOMEA", "NO");
  while(band == 1){
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("--------------------");
    display.println("     PRIZE-PETS");
    display.println("--------------------");
    display.println(" ");
    display.setTextSize(2);
    display.println("    Modo");
    display.println("   Alarma");
    display.display();
    display.clearDisplay();
    if(Firebase.getString(firebaseData,"/ESP32/ALARMA")){
      alarm = firebaseData.stringData();
      if(alarm != band_alarma){
        band_alarma = alarm;
        Serial.println(band_alarma);
        cont=0;
        for(int i=2; i<7; i++ ){
          B=band_alarma[i];
          num[cont]=B.toInt();
          cont++;
        }
        Tipo_A=num[0];
        Hora_A=num[1]*10+num[2];
        Minuto_A=num[3]*10+num[4];
        preferences.begin("myfile",false);
        preferences.putInt("T_alarma",Hora_A);
        int T_A = preferences.getInt("T_alarma",0);
        Serial.print("El valor de la Hora de la alarma es =");
        Serial.println(T_A);
        preferences.putInt("T_Hora",Minuto_A);
        int T_M = preferences.getInt("T_Hora",0);
        Serial.print("El valor del tipo de los minutos es =");
        Serial.println(T_M);
        preferences.end();
      }
    }
    if (Firebase.getString(firebaseData, "/ESP32/HOMEA")){
      tipo_BotA = firebaseData.stringData();
      if(tipo_BotA != banderaA){
        banderaA = tipo_BotA;
        Serial.println(banderaA);
      if(banderaA.indexOf("EXIT") != -1){
          digitalWrite(13,LOW);
          band = 0;
          Firebase.setString(firebaseData, "/ESP32/botonA", "N");
          return;
        }
      }
    }
    
  }

}

void Modo_Peso(){
  int band = 1;
  banderaP=' ';
  digitalWrite(12,HIGH);
  Firebase.setString(firebaseData, "/ESP32/HOMEP", "NO");
  while(band == 1){
    
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("---------------------");
    display.println("     PRIZE-PETS");
    display.println("---------------------");
    display.println(" ");
    display.setTextSize(2);
    display.println(" Modo Peso");
    display.display();
    display.clearDisplay();
    
    if (Firebase.getString(firebaseData, "/ESP32/HOMEP")){
      tipo_BotP = firebaseData.stringData();
      if(tipo_BotP != banderaP){
        banderaP = tipo_BotP;
        Serial.println(banderaP);
      if(banderaP.indexOf("EXIT") != -1){
          digitalWrite(12,LOW);
          band = 0;
          Firebase.setString(firebaseData, "/ESP32/botonP", "N");
        }
      }
    }
  }
  return;
}

void Modo_Camara(){

  int band = 1;
  banderaC=' ';
  digitalWrite(14,HIGH);
  Firebase.setString(firebaseData, "/ESP32/HOMEC", "NO");
  while(band == 1){
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("--------------------");
    display.println("     PRIZE-PETS");
    display.println("--------------------");
    display.println(" ");
    display.setTextSize(2);
    display.println("    Modo");
    display.println("   Camara");
    display.display();
    display.clearDisplay();
    
    if (Firebase.getString(firebaseData, "/ESP32/HOMEC")){
      tipo_BotC = firebaseData.stringData();
      if(tipo_BotC != banderaC){
        banderaC = tipo_BotC;
        Serial.println(banderaC);
      if(banderaC.indexOf("EXIT") != -1){
          digitalWrite(14,LOW);
          band = 0;
          Firebase.setString(firebaseData, "/ESP32/botonC", "N");

        }
      }
    }
  }
  return;
}

void Modo_Manual(){

  int band = 1;
  banderaM=' ';
  digitalWrite(27,HIGH);
  Firebase.setString(firebaseData, "/ESP32/HOMEM", "N");
  while(band == 1){
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("--------------------");
    display.println("     PRIZE-PETS");
    display.println("--------------------");
    display.println(" ");
    display.setTextSize(2);
    display.println("    Modo");
    display.println("   Manual");
    display.display();
    display.clearDisplay();
    if (Firebase.getString(firebaseData, "/ESP32/HOMEM")){
      tipo_BotM = firebaseData.stringData();
      if(tipo_BotM != banderaM){
        banderaM = tipo_BotM;
        Serial.println(banderaM);
      if(banderaM.indexOf("EXIT") != -1){
          digitalWrite(27,LOW);
          band = 0;
          Firebase.setString(firebaseData, "/ESP32/botonM", "N");
        }
      }
    }
  }
  return;
}
