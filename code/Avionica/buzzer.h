/* CONFIGURAÇÕES BUZZER */

#define BUZZER_PIN 14
#define BEEP_TIME 300

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Buzzer configurado!");
}

void activateBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  flash_up();
  Serial.println("Buzzer ativado!");
}

void desactivateBuzzer() {
  digitalWrite(BUZZER_PIN, LOW);
  flash_down();
  Serial.println("Buzzer desativado!");
}

void tripleBuzzerBip() {
  activateBuzzer();
  delay(BEEP_TIME);
  desactivateBuzzer();
  delay(BEEP_TIME);

  activateBuzzer();
  delay(BEEP_TIME);
  desactivateBuzzer();
  delay(BEEP_TIME);
  
  activateBuzzer();
  delay(BEEP_TIME);
  desactivateBuzzer();
  delay(BEEP_TIME);
}

