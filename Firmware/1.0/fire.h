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


/*  This part of the code ( the fire part) has been adapted and heavly alterted from
  Patrick Rigney (https://www.toggledbits.com/)
  Github: https://github.com/toggledbits/MatrixFireFast */


#pragma once
#include "Settings.h"
#define MAT_W   kMatrixWidth          /* Size (columns) of entire matrix */
#define MAT_H   kMatrixHeight          /* and rows */
const uint16_t rows = MAT_H;
const uint16_t cols = MAT_W;
const uint16_t xorg = 0;
const uint16_t yorg = 0;
uint8_t pix[rows][cols];
const uint8_t NCOLORS = (sizeof(colors) / sizeof(colors[0]));
uint8_t nflare = 0;
uint32_t flare[maxflare];
const uint8_t phy_h = MAT_W;
const uint8_t phy_w = MAT_H;
unsigned long t = 0; /* keep time */



uint16_t pos( uint16_t col, uint16_t row ) {
  uint16_t phy_x = xorg + (uint16_t) row;
  uint16_t phy_y = yorg + (uint16_t) col;
  return phy_x + phy_y * phy_w;
}

uint32_t isqrt(uint32_t n) {
  if ( n < 2 ) return n;
  uint32_t smallCandidate = isqrt(n >> 2) << 1;
  uint32_t largeCandidate = smallCandidate + 1;
  return (largeCandidate * largeCandidate > n) ? smallCandidate : largeCandidate;
}

// Set pixels to intensity around flare
void glow( int x, int y, int z ) {
  int b = z * 10 / flaredecay + 1;
  for ( int i = (y - b); i < (y + b); ++i ) {
    for ( int j = (x - b); j < (x + b); ++j ) {
      if ( i >= 0 && j >= 0 && i < rows && j < cols ) {
        int d = ( flaredecay * isqrt((x - j) * (x - j) + (y - i) * (y - i)) + 5) / 10;
        uint8_t n = 0;
        if ( z > d ) n = z - d;
        if ( n > pix[i][j] ) { // can only get brighter
          pix[i][j] = n;
        }
      }
    }
  }
}

void newflare() {
  if ( nflare < maxflare && random(1, 101) <= flarechance ) {
    int x = random(0, cols);
    int y = random(0, flarerows);
    int z = NCOLORS - 1;
    flare[nflare++] = (z << 16) | (y << 8) | (x & 0xff);
    glow( x, y, z );
  }
}

void make_fire() {
  uint16_t i, j;

  if (t > millis()) return;
  t = millis() + (1000 / FPS);

  // First, move all existing heat points up the display and fade

  for (i = rows - 1; i > 0; --i) {
    for (j = 0; j < cols; ++j) {
      uint8_t n = 0;
      if (pix[i - 1][j] > 0)
        n = pix[i - 1][j] - 1;
      pix[i][j] = n;
    }
  }

  // Heat the bottom row
  for (j = 0; j < cols; ++j) {
    i = pix[0][j];
    if (i > 0) {
      pix[0][j] = random(NCOLORS - 6, NCOLORS - 2);
    }
  }

  // flare
  for (i = 0; i < nflare; ++i) {
    int x = flare[i] & 0xff;
    int y = (flare[i] >> 8) & 0xff;
    int z = (flare[i] >> 16) & 0xff;
    glow(x, y, z);
    if (z > 1) {
      flare[i] = (flare[i] & 0xffff) | ((z - 1) << 16);
    } else {
      // This flare is out
      for (int j = i + 1; j < nflare; ++j) {
        flare[j - 1] = flare[j];
      }
      --nflare;
    }
  }
  newflare();

  // Set and draw
  for (i = 0; i < rows; ++i) {
    for (j = 0; j < cols; ++j) {
      // matrix -> drawPixel(j, rows - i, colors[pix[i][j]]);
      CRGB COlsplit = colors[pix[i][j]];
      matrix -> drawPixel(j, rows - i, colors[pix[i][j]]);
    }
  }
}
