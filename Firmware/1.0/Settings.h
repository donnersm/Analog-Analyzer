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

// Ledstrip / matrix settings
#define CHIPSET             WS2812B                 // LED strip type
#define LED_PIN             27                      // LED strip data
#define COLOR_ORDER         GRB                     // If colours look wrong, play with this
#define LED_VOLTS           5                       // Usually 5 or 12
#define MAX_MILLIAMPS       2000                    // Careful with the amount of power here if running off USB port
#define offsetBar   0                               //if you need the whole display to start on a different column, you can give an offset. 
const int kMatrixWidth =    10; //128;              // Matrix width --> number of columns in your led matrix
const int kMatrixHeight =   18 ;//64;                // Matrix height --> number of leds per column

// Logo
#define LED_PIN_LOGO        14                      // NeoPixel LED strand is connected to this pin 
#define N_PIXELS_LOGO       12                       // Number of pixels in Logo strand
long    firstPixelHue =     0;                      // logo only
#define HueSpread           5                       // Hue spread between two pixels: 255/N_PIXELS_LOGO to spread the total hue over
                                                    // your leds other options: you can also put down any number 0-255
                                                    

//Options Change to your likings
// first the options related to visualisation
#define BottomRowAlwaysOn   1                       // if set to 1, bottom row is always on. Setting only applies to LEDstrip not HUB75
#define Fallingspeed        30                      // Falling down factor that effects the speed of falling tiles
int     PEAKDELAY =         60;                     // Delay before peak falls down to stack. Overruled by PEAKDEALY Potmeter
#define SecToChangePattern  10                      // number of seconds that pattern changes when auto change mode is enabled
boolean VUMeter =           1;                      // if 1 then VU is visible, if 0 then it's off. can be overrules by tripple press on mode button
boolean DoubleVU =          true;                  // use a double vu meter from center to L and R, of when false, use single from L to R
#define Speedfilter         20                      // slowdown factor for columns to look less 'nervous' The higher the quicker
#define BRIGHTNESSMAX       100                     // Max brightness of the leds...carefull...to bright might draw to much amps!
int     BRIGHTNESSMIN =     1;                      // Min brightness
int     BRIGHTNESS =        50;                     // Default brightness, however, overruled by the Brightness potmeter

//amplification and signal related stuff
#define MAX_VU              1500                    // How high our VU could max out at.  Arbitarily tuned.
#define InputBoost          1000                    // this is the SEnse potmeter amplification. Higher number for low level input
int     noisemic =          300;
int     noiseline =         300;
#define ADC_OFFSET          1680
#define VU_OFFSET           2200
// other stuff
int     buttonPushCounter = 0;                      // This number defines what pattern to start after boot (0 to 12)
bool    autoChangePatterns =false;                  // After boot, the pattern will not change automatically.
#define DemoAfterSec        6000                    // if there is no input signal during this number of milli seconds, the unit will go to demo mode
#define DemoTreshold        200                     // this defines the treshold that will get the unit out of demo mode
#define NumberOfModes       13                      // this is the number of modes. If you add modes, increase this number also.
#define LONG_PRESS_MS       3000                    // Number of ms to count as a long press on the switch


// Debug features should default be off!
int     DEBUG =             0 ;                     // When debug=1, extra information is printed to serial port. Turn of if not needed--> DEBUG=0
#define DEBUG_BUFFER_SIZE   100                     // Debug buffer size

// Label for Frequency columns in webbrowser
String labels[] = {"30", "60", "125", "250", "500", "1K", "2K", "4K", "8K", "16K"}; 



//****************************************************************************************************
//***********   HARDWARE  don't change unless you are using your own hardware design *****************
// Arduino pin number where the button is connected.                                              //**
                                                                                                  //**
#define MODE_BUTTON_PIN     25                                                                    //**
#define SELECT_BUTTON_PIN   26                                                                    //**
                                                                                                  //**
// onboard multiplexor for channel input                                                          //**
#define S0                  15                                                                    //**
#define S1                  16                                                                    //**
#define S2                  18                                                                    //**
#define S3                  17                                                                    //**
#define MP_E                19                                                                    //**
                                                                                                  //**
// Potmeters                                                                                      //**
#define BRIGHTNESSPOT       34                                                                    //**
#define PEAKDELAYPOT        39                                                                    //**
#define SENSEPOT            35                                                                    //**
                                                                                                  //**
//Input select pin                                                                                //**
#define InputSelPin         21                                                                    //**
#define MIC                 1                                                                     //**
#define LINE                0                                                                                    //**
boolean InputLine =         LINE; // defauft                                                      //**
                                                                                                  //**
// audio ADC to uController                                                                       //**
#define ADC_IN              32                                                                    //**
//************END OF HARDWARE                                                                     //**
//****************************************************************************************************






 
/****************************************************************************
   Colors of bars and peaks in different modes, changeable to your likings
 ****************************************************************************/
 uint8_t colorTimer = 0; // don't change
// Colors mode 0
#define ChangingBar_Color   y * (255 / kMatrixHeight) + colorTimer, 255, 255
// no peaks

// Colors mode 1 These are the colors from the TRIBAR when using Ledstrip
#define TriBar_Color_Top      0 , 255, 255    // Red CHSV
#define TriBar_Color_Bottom   95 , 255, 255   // Green CHSV
#define TriBar_Color_Middle   45, 255, 255    // Yellow CHSV

#define TriBar_Color_Top_Peak      0 , 255, 255    // Red CHSV
#define TriBar_Color_Bottom_Peak   95 , 255, 255   // Green CHSV
#define TriBar_Color_Middle_Peak   45, 255, 255    // Yellow CHSV

// Colors mode 1 These are the colors from the TRIBAR when using HUB75
#define TriBar_RGB_Top      255 , 0, 0    // Red CRGB
#define TriBar_RGB_Bottom   0 , 255, 0   // Green CRGB
#define TriBar_RGB_Middle   255, 255, 0    // Yellow CRGB

#define TriBar_RGB_Top_Peak      255 , 0, 0    // Red CRGB
#define TriBar_RGB_Bottom_Peak   0 , 255, 0   // Green CRGB
#define TriBar_RGB_Middle_Peak   255, 255, 0    // Yellow CRGB

// hub 75 center bars
#define Center_RGB_Edge      255 , 0, 0    // Red CRGB
#define Center_RGB_Middle   255, 255, 0    // Yellow CRGB
// hub 75 center bars 2
#define Center_RGB_Edge2      255 , 0, 0    // Red CRGB
#define Center_RGB_Middle2   255, 255, 255    // Yellow CRGB

// Colors mode 2
#define RainbowBar_Color  (x / BAR_WIDTH) * (255 / numBands), 255, 255
#define PeakColor1  0, 0, 255       // white CHSV

// Colors mode 3
#define PeakColor2  0, 0, 255       // white CHSV
DEFINE_GRADIENT_PALETTE( purple_gp ) {
  0,   0, 212, 255,   //blue
  255, 179,   0, 255
}; //purple
CRGBPalette16 purplePal = purple_gp;


// Colors mode 4
#define SameBar_Color1      0 , 255, 255      //red  CHSV
#define PeakColor3  160, 255, 255   // blue CHSV

// Colors mode 5
#define SameBar_Color2      160 , 255, 255    //blue  CHSV
#define PeakColor4  0, 255, 255   // red CHSV

// Colors mode 6
DEFINE_GRADIENT_PALETTE( redyellow_gp ) {
  0,   200, 200,  200,   //white
  64,   255, 218,    0,   //yellow
  128,   231,   0,    0,   //red
  192,   255, 218,    0,   //yellow
  255,   200, 200,  200
}; //white
CRGBPalette16 heatPal = redyellow_gp;
// no peaks

// Colors mode 7
DEFINE_GRADIENT_PALETTE( outrun_gp ) {
  0, 141,   0, 100,   //purple
  127, 255, 192,   0,   //yellow
  255,   0,   5, 255
};  //blue
CRGBPalette16 outrunPal = outrun_gp;
// no peaks

// Colors mode 8
DEFINE_GRADIENT_PALETTE( mark_gp2 ) {
  0,   255,   218,    0,   //Yellow
  64,   200, 200,    200,   //white
  128,   141,   0, 100,   //pur
  192,   200, 200,    200,   //white
  255,   255,   218,    0,
};   //Yellow
CRGBPalette16 markPal2 = mark_gp2;

// Colors mode 9
// no bars only peaks
DEFINE_GRADIENT_PALETTE( mark_gp ) {
  0,   231,   0,    0,   //red
  64,   200, 200,    200,   //white
  128,   200, 200,    200,   //white
  192,   200, 200,    200,   //white
  255,   231, 0,  0,
};   //red
CRGBPalette16 markPal = mark_gp;

// Colors mode 10
// no bars only peaks
#define PeakColor5  160, 255, 255   // blue CHSV

// These are the colors from the TRIPEAK mode 11
// no bars
#define TriBar_Color_Top_Peak2      0 , 255, 255    // Red CHSV
#define TriBar_Color_Bottom_Peak2   95 , 255, 255   // Green CHSV
#define TriBar_Color_Middle_Peak2   45, 255, 255    // Yellow CHSV



/******************************************************************
  Setting below this point are only related to the demo Fire mode
*******************************************************************/

#define FPS 25              /* Refresh rate 15 looks good*/

/* Flare constants */
const uint8_t flarerows = 8;  //8  /* number of rows (from bottom) allowed to flare */
const uint8_t maxflare = 50;//4;     /* max number of simultaneous flares */
const uint8_t flarechance = 50; /* 50chance (%) of a new flare (if there's room) */
const uint8_t flaredecay = 14;  /* decay rate of flare radiation; 14 is good */

/* This is the map of colors from coolest (black) to hottest. Want blue flames? Go for it! */
const uint32_t colors[] = {
  0x000000,
  0x100000,
  0x300000,
  0x600000,
  0x800000,
  0xA00000,
  0xC02000,
  0xC04000,
  0xC06000,
  0xC08000,
  0x807080
};
