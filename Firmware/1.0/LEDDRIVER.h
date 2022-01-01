/********************************************************************************************************************************************************
*                                                                                                                                                       *
   Project:         Analog 10 Channel Spectrum Analyzer
   Target Platform: ESP32
*                                                                                                                                                       *
   Version: 1.0
   Hardware setup: See github
   Spectrum analyses done with analog discrete components. AKA Filters
*                                                                                                                                                       *
   Mark Donners
   The Electronic Engineer
   Website:   www.theelectronicengineer.nl
   facebook:  https://www.facebook.com/TheelectronicEngineer
   youtube:   https://www.youtube.com/channel/UCm5wy-2RoXGjG2F9wpDFF3w
   github:    https://github.com/donnersm
*                                                                                                                                                       *
*********************************************************************************************************************************************************

*/

#pragma once
#include "Settings.h"

// some definitions to make Arduino life a little easier - Don't Change
#define PANE_WIDTH kMatrixWidth
#define BAR_WIDTH  (kMatrixWidth /(numBands ))  // If width >= 8 light 1 LED width per bar, >= 16 light 2 LEDs width bar etc
#define TOP            (kMatrixHeight - 0)         // Don't allow the bars to go offscreen
#define NeededWidth (BAR_WIDTH * numBands)           // we need this to see if all bands fit or that we have left over space
#define NUM_LEDS   (kMatrixWidth * kMatrixHeight)   // Total number of LEDs

CRGB leds[NUM_LEDS];
CRGB Logo[N_PIXELS_LOGO];
// See manual if you need to change these settings
// FastLED_NeoMaxtrix - see https://github.com/marcmerlin/FastLED_NeoMatrix for Tiled Matrixes, Zig-Zag and so forth
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, kMatrixWidth, kMatrixHeight,
    NEO_MATRIX_BOTTOM        + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS       + NEO_MATRIX_ZIGZAG +
    NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS);

/* // this one is used if you are using a ledstrip setup simular to the one from the acryllic spectrum analyzer
  FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, kMatrixWidth, kMatrixHeight,
                                  NEO_MATRIX_BOTTOM        + NEO_MATRIX_LEFT +
                                  NEO_MATRIX_COLUMNS       + NEO_MATRIX_PROGRESSIVE +
                                  NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS);
*/



void SetupLEDSTRIP(void) {

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_LOGO, COLOR_ORDER>(Logo, N_PIXELS_LOGO).setCorrection(TypicalSMD5050);
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTS, MAX_MILLIAMPS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();

}
