void wrapperSetupBuzzer() {
  setupBuzzer();
}

void wrapperSetupTelemetry() {
  setupTelemetry();
}

void wrapperSetupSD() {
  verifySD();
  if (!setupSDFlag) {
    println("[ATENÇÃO] O cartão SD não foi inicializado");
  } else {
    setupSd();
  }
}

void wrapperSetupMPU() {
  verifyMPU();
  if (!setupMPUFlag) {
    Serial.print("[ATENÇÃO] Erro ao iniciar MPU");
  } else {
    setupMPU();
  }
}

void wrapperSetupBMP() {
  verifyBMP();
  if(!setupBMPFlag) {
    Serial.println("[ATENÇÃO] Erro ao iniciar BMP");
  } else {
    setupBMP();
  }
}

void wrapperSetupSKIBS() {
  setupSkibPins();
}

void wrapperSetupGPS() {
  verifyGPS();
  if (!setupGPSFlag) {
    Serial.println("[ATENÇÃO] Erro ao iniciar GPS");
  } else {
    setupGPS();
  }
}

void setupComponents() {
  if(ENABLE_BUZZER)    { wrapperSetupBuzzer();    }
  if(ENABLE_TELEMETRY) { wrapperSetupTelemetry(); }
  if(ENABLE_SD)        { wrapperSetupSD();        }
  if(ENABLE_MPU)       { wrapperSetupMPU();       }
  if(ENABLE_BMP)       { wrapperSetupBMP();       }
  if(ENABLE_SKIBS)     { wrapperSetupSKIBS();     }
  if(ENABLE_GPS)       { wrapperSetupGPS();       }
}

void getSensorsMeasures() {
  // Medições BMP390
  if (ENABLE_BMP) {
    readBMP();
  }

  // Medições MPU6050
  if (ENABLE_MPU) {
    verifyMPU();
    if (setupMPUFlag) {
      readMPU();
    } else {
      wrapperSetupMPU();
    }
  }

  //Medições GPS
  if (ENABLE_GPS) {
    updateGPSData();
  }
}

void resetStructs() {
  allData = {
    { 0,0 },        // time, parachute
    { 0,0,0 },      // temperature, pressure, altitude
    { 0,0,0 },      // accX, accY, accZ
    { "", "", 0,0 }         // latitude, longitude
  };
  soloData = { 0 }; // openParachute
}