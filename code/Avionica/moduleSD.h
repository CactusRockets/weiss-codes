#include <SD.h>

/* CONFIGURAÇÕES CARTÃO MICROSD */
#define CS_SDPIN 5

File myFile;

void verifySD() {
  setupSDFlag = SD.begin(CS_SDPIN);
}

void setupSd() {
  println("Inicializando o cartão SD...");

  // Cria arquivo data.txt e abre
  myFile = SD.open("/data.txt", FILE_APPEND);

  // Escreve dados no arquivo
  if (myFile) {
    println("Gravando dados iniciais!");
    myFile.println("Informations");
    myFile.close();
  } else {
    println("Error ao abrir data.txt");
  }
}

void writeOnSD(String str) {
  myFile = SD.open("/data.txt", FILE_APPEND);

  if (myFile) {
    println("(Dados Gravados!)");
    myFile.println(str);
    myFile.close();
  } else {
    println("Error ao gravar em data.txt");
  }
}

