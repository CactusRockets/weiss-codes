#include <SPI.h>
#include <Wire.h>

#define BUTTON_PIN 4
#define EXPECTED_PACKET_SIZE 31

int valueButton;

String solo_message = "";
String telemetry_message = "";

bool parachuteOpened = false;

#include "telemetry.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Serial inicializada!");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);

  setupTelemetry();
}

void loop() {
  telemetry_message.clear();
  if(LoRaSerial.available() > 0) {
    receiveString();
    // Imprime somente os pacotes sem erros
    // if(telemetry_message.length() == EXPECTED_PACKET_SIZE) {
    //   Serial.println(telemetry_message);
    // }
    Serial.println(telemetry_message);
  }

  // valueButton = digitalRead(BUTTON_PIN);
  // if(valueButton == LOW) {
  //   Serial.print("Botão Apertado");

  //   parachuteOpened = true;
  //   solo_message = "1";

  //   transmitString(solo_message);
  // }
}

