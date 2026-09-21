#ifndef LOGO_H
#define LOGO_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

inline void mostrarLogo(Adafruit_SSD1306 &display) {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(3);
  display.setCursor(34, 4);
  display.print(F("UETS"));

  display.drawRoundRect(39, 34, 50, 25, 6, SSD1306_WHITE);

  display.fillCircle(53, 45, 4, SSD1306_WHITE);
  display.fillCircle(75, 45, 4, SSD1306_WHITE);

  display.drawLine(53, 53, 75, 53, SSD1306_WHITE);

  display.drawLine(64, 27, 64, 34, SSD1306_WHITE);
  display.fillCircle(64, 25, 2, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(42, 59);
  display.print(F("ROBOT EYES"));

  display.display();
}

#endif
