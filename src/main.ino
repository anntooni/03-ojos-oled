#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "logboot.h"
#include "eyes.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum EyeState {
  STATE_NORMAL,
  STATE_HAPPY,
  STATE_ALERT,
  STATE_SLEEPY,
  STATE_BLINK,
  STATE_LOOK_LEFT,
  STATE_LOOK_RIGHT,
  STATE_EXCITED
};

EyeState currentState = STATE_NORMAL;
bool modoAutonomo = true;
unsigned long previousMillis = 0;
const unsigned long INTERVALO_ANIMACION = 2500;
int pasoSecuencia = 0;

void debugEyesSerial() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    if (cmd == '\r' || cmd == '\n' || cmd == ' ') return;

    modoAutonomo = false;

    switch (cmd) {
      case '1':
      case 'N':
      case 'n':
        currentState = STATE_NORMAL;
        drawEyeExpression(display, eye_normal);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: NORMAL"));
        break;

      case '2':
      case 'H':
      case 'h':
        currentState = STATE_HAPPY;
        drawEyeExpression(display, eye_happy);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: FELIZ"));
        break;

      case '3':
      case 'A':
      case 'a':
        currentState = STATE_ALERT;
        drawEyeExpression(display, eye_alert);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: ALERTA"));
        break;

      case '4':
      case 'S':
      case 's':
        currentState = STATE_SLEEPY;
        drawEyeExpression(display, eye_sleepy);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: REPOSO (Sleepy)"));
        break;

      case '5':
      case 'B':
      case 'b':
        currentState = STATE_BLINK;
        drawEyeExpression(display, eye_blink);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: PARPADEO (Blink)"));
        break;

      case '6':
      case 'L':
      case 'l':
        currentState = STATE_LOOK_LEFT;
        drawEyeExpression(display, eye_look_left);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: MIRADA IZQUIERDA"));
        break;

      case '7':
      case 'R':
      case 'r':
        currentState = STATE_LOOK_RIGHT;
        drawEyeExpression(display, eye_look_right);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: MIRADA DERECHA"));
        break;

      case '8':
      case 'E':
      case 'e':
        currentState = STATE_EXCITED;
        drawEyeExpression(display, eye_excited);
        Serial.println(F("[SERIAL DEBUG] Expresión cambiada a: EMOCIONADO (Excited)"));
        break;

      case '0':
      case 'M':
      case 'm':
        modoAutonomo = true;
        previousMillis = millis();
        Serial.println(F("[SERIAL DEBUG] Modo Autónomo reactivado (Animación FSM activa)"));
        break;

      default:
        Serial.print(F("[SERIAL DEBUG] Comando desconocido: "));
        Serial.println(cmd);
        break;
    }
  }
}

void ejecutarSecuenciaAutonoma() {
  pasoSecuencia = (pasoSecuencia + 1) % 6;

  switch (pasoSecuencia) {
    case 0:
      currentState = STATE_NORMAL;
      drawEyeExpression(display, eye_normal);
      break;

    case 1:
      currentState = STATE_BLINK;
      drawEyeExpression(display, eye_blink);
      break;

    case 2:
      currentState = STATE_LOOK_LEFT;
      drawEyeExpression(display, eye_look_left);
      break;

    case 3:
      currentState = STATE_NORMAL;
      drawEyeExpression(display, eye_normal);
      break;

    case 4:
      currentState = STATE_LOOK_RIGHT;
      drawEyeExpression(display, eye_look_right);
      break;

    case 5:
      currentState = STATE_HAPPY;
      drawEyeExpression(display, eye_happy);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 1000);

  if (!initDiagnostics(display)) {
    Serial.println(F("[FALLO CRÍTICO] Error al inicializar pantalla OLED."));
    while (true) delay(100);
  }

  runSystemPOST(display);

  Serial.println(F("\n======================================================="));
  Serial.println(F("🤖 SISTEMA EMBEBIDO ESP32 — TELEMETRÍA Y CONTROL DE OJOS"));
  Serial.println(F("======================================================="));
  Serial.println(F("Comandos Serial interactivos (Debug / Control de IA):"));
  Serial.println(F("  '1' o 'N' -> Ojos Normales (Neutro)"));
  Serial.println(F("  '2' o 'H' -> Ojos Felices (Empatía)"));
  Serial.println(F("  '3' o 'A' -> Ojos Alerta (Atención/Peligro)"));
  Serial.println(F("  '4' o 'S' -> Ojos Reposo (Sleepy)"));
  Serial.println(F("  '5' o 'B' -> Parpadeo (Blink)"));
  Serial.println(F("  '6' o 'L' -> Mirar Izquierda"));
  Serial.println(F("  '7' o 'R' -> Mirar Derecha"));
  Serial.println(F("  '8' o 'E' -> Ojos Emocionados (Excited)"));
  Serial.println(F("  '0' o 'M' -> Alternar Modo Autónomo (FSM millis)"));
  Serial.println(F("=======================================================\n"));

  drawEyeExpression(display, eye_normal);

  previousMillis = millis();
}

void loop() {
  debugEyesSerial();

  if (modoAutonomo) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= INTERVALO_ANIMACION) {
      previousMillis = currentMillis;
      ejecutarSecuenciaAutonoma();
    }
  }
}
