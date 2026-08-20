/* CONFIGURAÇÕES BUZZER PASSIVO - VOLUME MÁXIMO / MODO RESGATE */

#define BUZZER_PIN 14


#define BEEP_FREQ_MAX 3500 
#define BEEP_TIME 300      
#define BEEP_PAUSE 150

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("Buzzer (Modo Resgate) configurado!");
}

void activateBuzzer() {
  tone(BUZZER_PIN, BEEP_FREQ_MAX);
  Serial.println("Buzzer ativado (Volume Max)");

}

void desactivateBuzzer() {
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("Buzzer desativado");

}

// --- 1. Sinal Sonoro de Inicialização (3 Bips Estridentes) ---
void tripleBuzzerBip() {
  Serial.println("--> AVISO: Sistema Ligado! Emitindo 3 bips no volume máximo...");
  
  for(int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, BEEP_FREQ_MAX);
    delay(BEEP_TIME);
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BEEP_PAUSE);
  }
}

// --- 2. Sinal de Busca/Resgate (Sirene de Localização) ---
// Chame essa função no loop() caso a aviônica detecte o pouso ou perda de sinal!
void beaconLostMode() {
  for(int freq = 2500; freq <= 4000; freq += 100) {
    tone(BUZZER_PIN, freq);
    delay(10);
  }
  for(int freq = 4000; freq >= 2500; freq -= 100) {
    tone(BUZZER_PIN, freq);
    delay(10);
  }
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}