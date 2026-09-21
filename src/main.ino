#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Irisoled.h>
#include <IrisoledAnimation.h>

#include "logboot.h"
#include "eyes.h"
#include "logo.h"

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
bool animacionActiva = false;

unsigned long previousMillis = 0;
int pasoSecuencia = 0;

const unsigned long INTERVALO_ANIMACION = 1800;

const unsigned char* scanFrames[] = {
  Irisoled::normal,
  Irisoled::look_left,
  Irisoled::normal,
  Irisoled::look_right,
  Irisoled::normal,
  Irisoled::look_up,
  Irisoled::normal,
  Irisoled::look_down,
  Irisoled::normal
};

const uint16_t scanDelays[] = {
  300,
  280,
  220,
  280,
  220,
  280,
  220,
  280,
  400
};

IrisoledAnimation scanAnimation(
  scanFrames,
  sizeof(scanFrames) / sizeof(scanFrames[0]),
  scanDelays,
  250,
  true
);

const unsigned char* blinkFrames[] = {
  Irisoled::normal,
  Irisoled::blink_up,
  Irisoled::blink,
  Irisoled::blink_down,
  Irisoled::blink,
  Irisoled::blink_up,
  Irisoled::normal
};

const uint16_t blinkDelays[] = {
  100,
  45,
  45,
  55,
  45,
  45,
  100
};

IrisoledAnimation blinkAnimation(
  blinkFrames,
  sizeof(blinkFrames) / sizeof(blinkFrames[0]),
  blinkDelays,
  60,
  false
);

void detenerAnimaciones() {
  animacionActiva = false;
  scanAnimation.stop();
  blinkAnimation.stop();
}

void mostrarExpresion(EyeState estado) {
  detenerAnimaciones();

  currentState = estado;

  switch (estado) {
    case STATE_NORMAL:
      drawEyeExpression(display, eye_normal);
      break;

    case STATE_HAPPY:
      drawEyeExpression(display, eye_happy);
      break;

    case STATE_ALERT:
      drawEyeExpression(display, eye_alert);
      break;

    case STATE_SLEEPY:
      drawEyeExpression(display, eye_sleepy);
      break;

    case STATE_BLINK:
      drawEyeExpression(display, eye_blink);
      break;

    case STATE_LOOK_LEFT:
      drawEyeExpression(display, eye_look_left);
      break;

    case STATE_LOOK_RIGHT:
      drawEyeExpression(display, eye_look_right);
      break;

    case STATE_EXCITED:
      drawEyeExpression(display, eye_excited);
      break;
  }
}

void iniciarScanning() {
  scanAnimation.reset();
  scanAnimation.start();
  animacionActiva = true;
}

void iniciarParpadeo() {
  blinkAnimation.reset();
  blinkAnimation.start();
  animacionActiva = true;
}

void debugEyesSerial() {
  while (Serial.available() > 0) {
    char cmd = Serial.read();

    if (cmd == '\r' || cmd == '\n' || cmd == ' ') {
      continue;
    }

    modoAutonomo = false;
    detenerAnimaciones();

    switch (cmd) {
      case '1':
      case 'N':
      case 'n':
        mostrarExpresion(STATE_NORMAL);
        Serial.println(F("[SERIAL DEBUG] Expresion: NORMAL"));
        break;

      case '2':
      case 'H':
      case 'h':
        mostrarExpresion(STATE_HAPPY);
        Serial.println(F("[SERIAL DEBUG] Expresion: FELIZ"));
        break;

      case '3':
      case 'A':
      case 'a':
        mostrarExpresion(STATE_ALERT);
        Serial.println(F("[SERIAL DEBUG] Expresion: ALERTA"));
        break;

      case '4':
      case 'S':
      case 's':
        mostrarExpresion(STATE_SLEEPY);
        Serial.println(F("[SERIAL DEBUG] Expresion: REPOSO"));
        break;

      case '5':
      case 'B':
      case 'b':
        iniciarParpadeo();
        Serial.println(F("[SERIAL DEBUG] Animacion: PARPADEO"));
        break;

      case '6':
      case 'L':
      case 'l':
        mostrarExpresion(STATE_LOOK_LEFT);
        Serial.println(F("[SERIAL DEBUG] Expresion: IZQUIERDA"));
        break;

      case '7':
      case 'R':
      case 'r':
        mostrarExpresion(STATE_LOOK_RIGHT);
        Serial.println(F("[SERIAL DEBUG] Expresion: DERECHA"));
        break;

      case '8':
      case 'E':
      case 'e':
        mostrarExpresion(STATE_EXCITED);
        Serial.println(F("[SERIAL DEBUG] Expresion: EMOCIONADO"));
        break;

      case '9':
        iniciarScanning();
        Serial.println(F("[SERIAL DEBUG] Animacion: SCANNING"));
        break;

      case '0':
      case 'M':
      case 'm':
        modoAutonomo = true;
        detenerAnimaciones();
        pasoSecuencia = 0;
        mostrarExpresion(STATE_NORMAL);
        previousMillis = millis();
        Serial.println(F("[SERIAL DEBUG] Modo AUTONOMO activado"));
        break;

      default:
        Serial.print(F("[SERIAL DEBUG] Comando desconocido: "));
        Serial.println(cmd);
        break;
    }
  }
}

void ejecutarSecuenciaAutonoma() {
  detenerAnimaciones();

  switch (pasoSecuencia) {
    case 0:
      mostrarExpresion(STATE_NORMAL);
      break;

    case 1:
      iniciarParpadeo();
      break;

    case 2:
      iniciarScanning();
      break;

    case 3:
      mostrarExpresion(STATE_HAPPY);
      break;

    case 4:
      mostrarExpresion(STATE_NORMAL);
      break;

    case 5:
      mostrarExpresion(STATE_ALERT);
      break;

    case 6:
      mostrarExpresion(STATE_NORMAL);
      break;

    case 7:
      mostrarExpresion(STATE_SLEEPY);
      break;

    case 8:
      mostrarExpresion(STATE_NORMAL);
      break;

    case 9:
      drawEyeExpression(display, eye_wink_left);
      break;

    case 10:
      drawEyeExpression(display, eye_wink_right);
      break;

    case 11:
      mostrarExpresion(STATE_EXCITED);
      break;

    case 12:
      mostrarExpresion(STATE_NORMAL);
      break;
  }

  pasoSecuencia++;

  if (pasoSecuencia > 12) {
    pasoSecuencia = 0;
  }

  previousMillis = millis();
}

void setup() {
  Serial.begin(115200);

  while (!Serial && millis() < 1000);

  if (!initDiagnostics(display)) {
    Serial.println(F("[FALLO CRITICO] Error al inicializar pantalla OLED."));

    while (true) {
      delay(100);
    }
  }

  mostrarLogo(display);
  delay(1800);

  runSystemPOST(display);

  Serial.println(F(""));
  Serial.println(F("======================================================="));
  Serial.println(F("SISTEMA EMBEBIDO ESP32 - TELEMETRIA Y CONTROL DE OJOS"));
  Serial.println(F("======================================================="));
  Serial.println(F("1/N NORMAL"));
  Serial.println(F("2/H FELIZ"));
  Serial.println(F("3/A ALERTA"));
  Serial.println(F("4/S REPOSO"));
  Serial.println(F("5/B PARPADEO"));
  Serial.println(F("6/L IZQUIERDA"));
  Serial.println(F("7/R DERECHA"));
  Serial.println(F("8/E EMOCIONADO"));
  Serial.println(F("9 SCANNING"));
  Serial.println(F("0/M AUTONOMO"));
  Serial.println(F("======================================================="));
  Serial.println(F(""));

  mostrarExpresion(STATE_NORMAL);

  previousMillis = millis();
}

void loop() {
  debugEyesSerial();

  if (animacionActiva) {
    if (scanAnimation.isRunning()) {
      scanAnimation.update(
        display,
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
      );
    } else if (blinkAnimation.isRunning()) {
      blinkAnimation.update(
        display,
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
      );
    } else {
      animacionActiva = false;
      mostrarExpresion(STATE_NORMAL);
      previousMillis = millis();
    }
  }

  if (modoAutonomo && !animacionActiva) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= INTERVALO_ANIMACION) {
      ejecutarSecuenciaAutonoma();
    }
  }
}
