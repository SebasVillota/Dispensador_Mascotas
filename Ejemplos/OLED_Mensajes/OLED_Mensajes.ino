#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
void setup() {
  Serial.begin(115200);
  display.begin(i2c_Address, true); // Address 0x3C default
  display.clearDisplay();
}

void loop() {
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("  Primera Prueba");
  display.println("--------------------");
  display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.println("  Calificacion:5.0 \r\n");
  display.println("");
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(2);
  display.print("  06/10/21");
  display.display();
  delay(5000);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("     Segunda Etapa");
  display.println("-------------------- \r\n");
  display.println("      PRIZE-PETS \r\n");
  display.print("    Modo:Automatico");
  display.display();
  delay(5000);
  display.clearDisplay();
}
