#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

// Definitie van de gebruikte pinnen
#define DIN_PIN D7
#define CS_PIN D8
#define CLK_PIN D5

// Aantal 8x8 modules in de keten
#define MAX_DEVICES 4
#define MATRIX_WIDTH (MAX_DEVICES * 8) // Breedte van de matrix in pixels

// Instantieer het Parola-object
MD_Parola matrix = MD_Parola(MD_MAX72XX::FC16_HW, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Buffer voor seriële input
#define BUF_SIZE 100
char inputBuffer[BUF_SIZE] = "";
bool newMessageAvailable = false;

void setup() {
  Serial.begin(115200); // Start seriële communicatie
  while (!Serial) {
    ; // Wacht op seriële verbinding
  }

  // Initialiseer de matrix
  matrix.begin();
  matrix.setIntensity(5); // Helderheid (0-15)
  matrix.displayClear();
  matrix.print("Welkom!"); // Toon een welkomstbericht
}

void loop() {
  // Check op nieuwe seriële input
  if (Serial.available()) {
    readSerialInput();
  }

  // Toon nieuwe tekst als beschikbaar
  if (newMessageAvailable) {
    matrix.displayClear();

    // Schat de breedte van de tekst
    uint16_t textWidth = strlen(inputBuffer) * 6; // 6 pixels per karakter

    if (textWidth <= MATRIX_WIDTH) {
      // Tekst past in de matrix -> statisch weergeven
      matrix.displayText(inputBuffer, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    } else {
      // Tekst past niet -> scrol weergeven
      matrix.displayScroll(inputBuffer, PA_CENTER, PA_SCROLL_LEFT, 100);
    }

    newMessageAvailable = false;
  }

  // Update de matrix
  if (matrix.displayAnimate()) {
    matrix.displayReset();
  }
}

void readSerialInput() {
  static uint8_t index = 0;

  while (Serial.available()) {
    char c = Serial.read();

    // Check op einde van invoer
    if (c == '\n' || c == '\r') {
      inputBuffer[index] = '\0'; // Voeg stringterminator toe
      index = 0;                 // Reset bufferindex
      newMessageAvailable = true;
    } else if (index < BUF_SIZE - 1) {
      inputBuffer[index++] = c; // Voeg karakter toe aan buffer
    }
  }
}
