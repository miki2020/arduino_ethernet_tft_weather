
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "config.h"
#include "location.h"
#include "weather.h"
#include "http_stream.h"
#include "sd_image.h"
#include "display.h"

Weather wx;
bool haveData = false;

// wait, but return early when a new location arrives over Serial
static void waitFor(unsigned long ms) {
  unsigned long t = millis();
  while (millis() - t < ms) {
    if (locationPoll()) {
      haveData = false;                         // old data belongs to the old place
      return;
    }
  }
}

void setup() {

  Serial.begin(9600);
  locationLoad();

  // keep every SPI device deselected before starting anything
  pinMode(TFT_CS, OUTPUT); digitalWrite(TFT_CS, HIGH);
  pinMode(ETH_CS, OUTPUT); digitalWrite(ETH_CS, HIGH);
  pinMode(SD_CS,  OUTPUT); digitalWrite(SD_CS,  HIGH);

  displayInit();


  netBegin();
  delay(1000);

  if (!sdBegin()) {
    // displayMessage(F("SD CARD"), F("ERROR"));
    while (true) {}
  }

  locationHelp();
}

void loop() {

  if (weatherFetch(loc, wx)) {
    haveData = true;
    displayWeather(wx, loc);                    // redraws the scene picture and all values
    waitFor(REFRESH_MS);

  } else {
    if (!haveData) displayMessage(F("NO DATA"), F("RETRYING.."));
    else           displayStale();              // old screen stays, red mark in the corner
    waitFor(RETRY_MS);
  }
}
