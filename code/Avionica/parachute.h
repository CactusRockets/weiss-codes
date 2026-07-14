#define SKIB1 13  
#define SAFE_MARGIN_ALTITUDE_ERROR 20 
 
// Em millisegundos 
#define SKIB_TIME 3000 
 
bool isDropping = false; 
 
bool parachute1Activated = false; 
 
double timeForStage1 = 0; 
 
bool alreadyDesactivatedBuzzer1 = false; 
 
void setupSkibPins() { 
  pinMode(SKIB1, OUTPUT);  
  Serial.println("Skib configurados!"); 
} 
 
void activateStage1() { 
  digitalWrite(SKIB1, HIGH); 
  Serial.println("Skib ativado!"); 
 
  timeForStage1 = millis(); 
  parachute1Activated = true; 
  if (allData.parachute < 1) 
    allData.parachute = 1; 
 
  activateBuzzer(); 
}  
 
void deactivateStage1() { 
  digitalWrite(SKIB1, LOW); 
  Serial.println("Skib desativado!"); 
  desactivateBuzzer(); 
} 
 
bool altitudeLessThan(double altitude1, double altitude2) { 
  return (altitude2 - altitude1 > SAFE_MARGIN_ALTITUDE_ERROR); 
} 
 
void activateParachutes() { 
  if (parachute1Activated == false) 
    activateStage1();  
 
  if(parachute1Activated && (millis() - timeForStage1) >= SKIB_TIME && !alreadyDesactivatedBuzzer1) { 
    alreadyDesactivatedBuzzer1 = true; 
    deactivateStage1(); 
  } 
} 
 
bool checkIsDropping() { 
  return altitudeLessThan(altitudeAtual, highestAltitude); 
} 
 
void checkApogee() { 
  isDropping = checkIsDropping() || isDropping; 
 
  if (ENABLE_SKIBS && isDropping) { 
      activateParachutes(); 
  } 
} 
 
void testActivations(int millisStage1, int millisStage2) { 
  Serial.println(millis()); 
  if(millis() > millisStage1 && parachute1Activated == false) { 
    activateStage1(); 
    delay(SKIB_TIME); 
    deactivateStage1(); 
  }
}
