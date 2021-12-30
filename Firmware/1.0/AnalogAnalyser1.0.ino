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
  Version History
  1.0   Webinterface included on first core while rest runs on second core. This way WIFI low signal doesn's freeze up the program
        VU meter sensitivity is linked to Sense potmeter like with the overal sensitivity
        Webserver setup can be forced on startup by holding down the mode button
*/

#define numBands 10  // number of bands
#define VERSION     "V1.0"

//libraries
#include <FastLED_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <pixeltypes.h>
#include <EasyButton.h>

// included dependency files
#include "Webstuf.h"
#include "LEDDRIVER.H"
#include "Settings.h"
#include "PatternsLedstrip.h"
#include "fire.h"

//libaries for webinterface
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>
#include <WiFiManager.h>

// no need to change any of the parameters below, all you can change is in settings.h
#define   up                  1                         // just to make Ardduino life easier
#define   down                0                         // just to make Ardduino life easier
int       PeakDirection =     0;                        // peaks can be displayed falling or floating away. 
long      LastDoNothingTime = 0;                        // only needed for screensaver
int       DemoModeMem =       0;                        // to remember what mode we are in when going to demo, in order to restore it after wake up
bool      AutoModeMem =       false;                    // same story
bool      DemoFlag =          false;                    // we need to know if demo mode was manually selected or auto engadged.
bool      webtoken =          false;                    // this is a flag so that the webserver noise when the other core has new data
int       noise =             0;                        // default, overruled by setuploop
int       VUadc =             0;                        // we'll need it don't change
int       SENSE =             60;                       // Sensitivity. overruled by potmeter 
int       gVU       =         0;                        // Instantaneous read of VU value
int       oldVU     =         0;                        // Previous read of VU value
int FreqBins[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   // these will contain the amplitude values for the frequency columns
int FreqBinsOld[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};// these will contain the amplitude values for the frequency columns
int FreqBinsNew[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};// these will contain the amplitude values for the frequency columns
int oldBarHeights[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // so they are set to 11
char PeakFlag[numBands];                                // the top peak delay needs to have a flag because it has different timing while floating compared to falling to the stack
int PeakTimer[numBands];                                // counter how many loops to stay floating before falling to stack

TaskHandle_t WebserverTask;
TaskHandle_t LogoTask;
//char LCDPrintBuf[30];

// Running a web server
WebServer server(80);
WiFiManager wm;
// Adding a websocket to the server
WebSocketsServer webSocket = WebSocketsServer(81);

//************************************************************************
//************ Button related stuff **************************************
//** see also 'Attach callback to buttons' in setup function          //**
// Two buttons                                                        //**
EasyButton ModeBut(MODE_BUTTON_PIN);                                  //**
EasyButton SelBut(SELECT_BUTTON_PIN);                                 //**
                                                                      //**
// Mode button 1 short press                                          //**
void onPressed() {                                                    //**
  Serial.println("Mode Button has been pressed!");                    //**
  buttonPushCounter = (buttonPushCounter + 1) % NumberOfModes; //%6   //**
  FastLED.clear();                                                    //**
  //SetInput(MIC);                                                    //**
}                                                                     //**
// Select button 1 short press                                        //**
void onPressed2() {                                                   //**
  Serial.println("Select Button has been pressed!");                  //**
  InputLine = !InputLine;                                             //**
  SetInput(InputLine);                                                //**
  Serial.printf("inputline set to %d", InputLine);                    //**
}                                                                     //**
//called when mode button is pressed 3 times within 600ms             //**
void VUMeterToggle() {                                                //**
  VUMeter = !VUMeter;                                                 //**
  Serial.println("Select Button has been pressed 3x!");               //**
}                                                                     //**
//called when select button is pressed 2 times within 500ms           //**
void ChangePeakDirection() {                                          //**
  PeakDirection = !PeakDirection;                                     //**
}                                                                     //**
//************ END Button related stuff **********************************
//************************************************************************

void setup() {

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  // this will run the webinterface datatransfer.
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "WebserverTask",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    4,           /* priority of the task */
    &WebserverTask,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);



  
  //WiFi.begin(ssid, password);
  Serial.begin(115200);
  delay(500);
  Serial.print("Main program is  running on core ");
  Serial.println(xPortGetCoreID());
  Serial.print("Version#:");
  Serial.println(VERSION);
  // Initialize the button.
  ModeBut.begin();
  SelBut.begin();
  
  // Attach callback to buttons
  ModeBut.onPressed(onPressed);
  SelBut.onPressed(onPressed2);
  SelBut.onPressedFor(LONG_PRESS_MS, startAutoMode);
  ModeBut.onSequence(3, 600, VUMeterToggle);
  SelBut.onSequence(2, 500, ChangePeakDirection);
  
  //set input pins / output pins
  pinMode(InputSelPin, OUTPUT);  // Pin for input-selector U11
  pinMode(S0, OUTPUT);  // Pin for S0 Multitplexor
  pinMode(S1, OUTPUT);  // Pin for S1 Multitplexor
  pinMode(S2, OUTPUT);  // Pin for S2 Multitplexor
  pinMode(MP_E, OUTPUT);  // Pin for Enable Multiplexor When High muliplexor=disabled
  pinMode(S3, OUTPUT);  // Pin for S3 Multitplexor



  //reset saved settings is mode button is pressed and hold during startup
  if (digitalRead(MODE_BUTTON_PIN) == 0) {
    Serial.println("button pressed on startup, WIFI settings will be reset");
    wm.resetSettings();
  }
  //Try to connect WiFi, then create AP but if no success then don't block the program
  wm.setConfigPortalBlocking(false);
  wm.autoConnect("ESP32_AP", "");

  Serial.println("ESP32 is connected to Wi-Fi network, Don't forget to reboot if you just reconfigured the network settings.");
  Serial.println("You can reconfigure WIFI settings by pressing and holding the mode button when rebooting.\n");
  Serial.println("Original program written by Mark Donners, aka The Electronic Engineer.");
  Serial.println("www.theelectronicengineer.nl");
  Serial.println("email: Mark.Donners@judoles.n");

 // this will load the actual html webpage to be displayed
  server.on("/", []() {
    server.send_P(200, "text/html", webpage);
  });

  // now start the server
  server.begin();
  Serial.println("HTTP server started");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  // setup the ledstrip
  SetupLEDSTRIP();
  // setup the correct input line or mic
  SetInput(InputLine); // select input source on startup
}


void loop() {
  // wm.process();
  size_t bytesRead = 0;
  int TempADC = 0;

  //############  Handle user interface ##################

  // read potmeters and process
  PEAKDELAY = map(analogRead(PEAKDELAYPOT), 0, 4095, 100, Fallingspeed);
  BRIGHTNESS = map(analogRead(BRIGHTNESSPOT), 0, 4095, BRIGHTNESSMAX, BRIGHTNESSMIN);
  SENSE = map(analogRead(SENSEPOT), 0, 4095, 1, InputBoost);
  //int tempfilter= map(analogRead(PEAKDELAYPOT), 0, 4095, 100,1);
  FastLED.setBrightness(BRIGHTNESS);
  // Continuously update the button state.
  ModeBut.read();
  SelBut.read();

  // now get latest band data 
  Analyser_ALL(); // call function to read all channels to fill FreqBinsNew[]
  
  // Process the read data
  // compare to previous values. If gone up then oldvalue is overwritten with new value
  // if gone down then newvalue is oldvalue-speedfilter ( this will reduce noise glitches and smooths out the display)
  float averageSum = 0;
  for (int cnt = 0; cnt < numBands; cnt++) {
    if (FreqBinsNew[cnt] < FreqBinsOld[cnt]) {
      FreqBins[cnt] = max(FreqBinsOld[cnt] - Speedfilter, FreqBinsNew[cnt]);
    }
    else if (FreqBinsNew[cnt] > FreqBinsOld[cnt]) {
      FreqBins[cnt] = FreqBinsNew[cnt];
    }
    FreqBinsOld[cnt] = FreqBins[cnt];
    // using this same loop to do some processing
    // like scaling the freqbins to matrix height
    // if VUMeter is displayed then matrix height -1
    //   Serial.printf("%d,",FreqBins[cnt]);
    if (VUMeter == 1)   FreqBins[cnt] = map(FreqBins[cnt], 0, SENSE, 0, kMatrixHeight - 1);
    else FreqBins[cnt] = map(FreqBins[cnt], 0, SENSE, 0, kMatrixHeight);
    //   if(FreqBins[cnt]>gVU)gVU=FreqBins[cnt];

  }

  // Value for VU meter processing
  gVU = (gVU * 400) / SENSE; // this makes the VU scale depend on the Sense potmeter
  if (VUadc < oldVU)gVU--;
  else gVU = VUadc;
  oldVU = VUadc;
  if (abs(gVU) > DemoTreshold)LastDoNothingTime = millis(); // if there is signal in any off the bands[>treshhold] then no demo mode


  // Process the BINdata into bar heights
  for (int band = 0; band < numBands; band++) {
    int barHeight = FreqBins[band];
    if (VUMeter == 1) {
      if (barHeight > TOP - 1) barHeight = TOP - 1; // don't want to mess up the VU meter
    }
    // Small amount of averaging between frames
    barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

    // Move peak up
    if (barHeight > peak[band]) {
      peak[band] = min(TOP, barHeight);
      PeakFlag[band] = 1;
    }


    // if there hasn't been much of a input signal for a longer time ( see settings ) go to demo mode
    if ((millis() - LastDoNothingTime) > DemoAfterSec && DemoFlag == false)
    { 
      DemoFlag = true;
      // first store current mode so we can go back to it after wake up
      DemoModeMem = buttonPushCounter;
      AutoModeMem = autoChangePatterns;
      autoChangePatterns = false;
      buttonPushCounter = 12;
      Serial.println("Automode is turned of because of demo");
    }
    // Wait,signal is back? then wakeup!
    else if (DemoFlag == true &&   (millis() - LastDoNothingTime) < DemoAfterSec   )
    { 
      // while in demo the democounter was reset due to signal on one of the bars.
      // So we need to exit demo mode.
      buttonPushCounter = DemoModeMem; // restore settings
      Serial.printf ("automode setting restored to: %d", AutoModeMem);
      autoChangePatterns = AutoModeMem; // restore settings
      DemoFlag = false;
      FastLED.clear();
    }

    #if BottomRowAlwaysOn
       if (barHeight == 0)barHeight = 1; // make sure there is always one bar that lights up
    #endif
    // Now visualize those bar heights
    switch (buttonPushCounter) {
      case 0:
        changingBarsLS(band, barHeight);
        break;
      case 1:
        TriBarLS(band, barHeight);
        TriPeakLS(band);
        break;
      case 2:
        rainbowBarsLS(band, barHeight);
        NormalPeakLS(band, PeakColor1);
        break;
      case 3:
        purpleBarsLS(band, barHeight);
        NormalPeakLS(band, PeakColor2);
        break;
      case 4:
        SameBarLS(band, barHeight);
        NormalPeakLS(band, PeakColor3);
        break;
      case 5:
        SameBar2LS(band, barHeight);
        NormalPeakLS(band, PeakColor3);
        break;
      case 6:
        centerBarsLS(band, barHeight);
        break;
      case 7:
        centerBars2LS(band, barHeight);
        break;
      case 8:
        centerBars3LS(band, barHeight);
        break;
      case 9:
        BlackBarLS(band, barHeight);
        outrunPeakLS(band);
        break;
      case 10:
        BlackBarLS(band, barHeight);
        NormalPeakLS(band, PeakColor5);
        break;
      case 11:
        BlackBarLS(band, barHeight);
        TriPeak2LS(band);
        break;
      case 12:
        matrix->fillRect(0, 0, matrix->width(), 1, 0x0000); // delete the VU meter
        make_fire();
        break;
    }
    // Save oldBarHeights for averaging later
    oldBarHeights[band] = barHeight;
  }

  if (VUMeter == true) {
    if (buttonPushCounter != 12) DrawVUMeter(0); // Draw it when not in screensaver mode
  }


  // Decay peak
  EVERY_N_MILLISECONDS(Fallingspeed) {

    for (byte band = 0; band < numBands; band++) {
      if (PeakFlag[band] == 1) {
        PeakTimer[band]++;
        if (PeakTimer[band] > PEAKDELAY) {
          PeakTimer[band] = 0;
          PeakFlag[band] = 0;
        }
      }
      else if ((peak[band] > 0) && (PeakDirection == up)) {
        peak[band] += 1;
        if (peak[band] > (kMatrixHeight + 10))peak[band] = 0;
      } // when to far off screen then reset peak height
      else if ((peak[band] > 0) && (PeakDirection == down)) {
        peak[band] -= 1;
      }
    }
    colorTimer++;
  }

  EVERY_N_MILLISECONDS(10)colorTimer++; // Used in some of the patterns

  EVERY_N_SECONDS(SecToChangePattern) {
    if (autoChangePatterns) {
      buttonPushCounter = (buttonPushCounter + 1) % 12;
    }
  }

  delay(1);                         // needed to give fastled a minimum recovery time
  FastLED.show();
  webtoken = true;                  // set marker so that other core can process data

} // loop end

void DrawVUPixels(int i, int yVU, int fadeBy = 0) {
  CRGB VUC;
  if (i > (PANE_WIDTH - 3)) {
    VUC.r = 255;
    VUC.g = 0;
    VUC.b = 0 ;
  }
  else if (i > (PANE_WIDTH / 5)) {
    VUC.r = 255;
    VUC.g = 255;
    VUC.b = 0;
  }
  else { // green
    VUC.r = 0;
    VUC.g = 255;
    VUC.b = 0;
  }
  if (DoubleVU == true) {
    // dbgprint("i=%d", i);
    if (i > ((PANE_WIDTH / 2) - 2)) { // little overrule
      VUC.r = 255;
      VUC.g = 0;
      VUC.b = 0 ;
    }
    int xHalf = matrix->width() / 2;
    matrix->drawPixel(xHalf - i - 1, yVU, CRGB(VUC.r, VUC.g, VUC.b).fadeToBlackBy(fadeBy));
    matrix->drawPixel(xHalf + i,   yVU, CRGB(VUC.r, VUC.g, VUC.b).fadeToBlackBy(fadeBy));
  }
  else {
    matrix->drawPixel( i,   yVU, CRGB(VUC.r, VUC.g, VUC.b).fadeToBlackBy(fadeBy));
  }
}


void DrawVUMeter(int yVU) {
  static int iPeakVUy = 0;                                  // size (in LED pixels) of the VU peak
  static unsigned long msPeakVU = 0;                        // timestamp in ms when that peak happened so we know how old it is
  const int MAX_FADE = 256;
  matrix->fillRect(0, yVU, matrix->width(), 1, 0x0000);
  if (iPeakVUy > 1) {
    int fade = MAX_FADE * (millis() - msPeakVU) / (float) 1000;
    DrawVUPixels(iPeakVUy,   yVU, fade);
  }
  int xHalf ;
  if (DoubleVU == true)xHalf = (PANE_WIDTH / 2); // - 1;
  else xHalf = PANE_WIDTH;
  int bars  = map(gVU, 0, MAX_VU, 1, xHalf);
  bars = min(bars, xHalf);
  //  bars = min(bars, xHalf*2);
  if (bars > iPeakVUy) {
    msPeakVU = millis();
    iPeakVUy = bars;
  }
  else if (millis() - msPeakVU > 1000)iPeakVUy = 0;
  for (int i = 0; i < bars; i++)DrawVUPixels(i, yVU);
}


//**************************************************************************************************
//                                          D B G P R I N T                                        *
//**************************************************************************************************
// Send a line of info to serial output.  Works like vsprintf(), but checks the DEBUG flag.        *
// Print only if DEBUG flag is true.  Always returns the formatted string.                         *
// Usage dbgprint("this is the text you want: %d", variable);
//**************************************************************************************************
void dbgprint(const char * format, ...) {
  if (DEBUG) {
    static char sbuf[DEBUG_BUFFER_SIZE]; // For debug lines
    va_list varArgs; // For variable number of params
    va_start(varArgs, format); // Prepare parameters
    vsnprintf(sbuf, sizeof(sbuf), format, varArgs); // Format the message
    va_end(varArgs); // End of using parameters
    if (DEBUG) // DEBUG on?
    {
      Serial.print("Debug: "); // Yes, print prefix
      Serial.println(sbuf); // and the info
    }
    // return sbuf; // Return stored string
  }
}



// inputSelect  use: SetInput(MIC) or SetInput(LINE)
void SetInput(int inputpin) {
  digitalWrite(InputSelPin, inputpin);
  if (inputpin==MIC)noise=noisemic;
  else noise= noiseline;
}

// Muliplexor channel select
void SetMultiPlexTo(int channelnumber) {
  digitalWrite(MP_E, 1); // multiplex disabled
  digitalWrite(S0, channelnumber & 0b0001);
  digitalWrite(S1, channelnumber & 0b0010);
  digitalWrite(S2, channelnumber & 0b0100);
  digitalWrite(S3, channelnumber & 0b1000);
  digitalWrite(MP_E, 0); // Multiplex enabled
}

void Analyser_ALL() {
  int tempADC = 0;
  //gVUalt=0;
  for (int channelcnt = 0; channelcnt < numBands; channelcnt++) {
    digitalWrite(MP_E, 1); // multiplex disabled
    digitalWrite(S0, channelcnt & 0b0001);
    digitalWrite(S1, channelcnt & 0b0010);
    digitalWrite(S2, channelcnt & 0b0100);
    digitalWrite(S3, channelcnt & 0b1000);
    digitalWrite(MP_E, 0); // Multiplex enabled
    tempADC = analogRead(ADC_IN);
    FreqBinsNew[channelcnt] = max(tempADC - ADC_OFFSET - noise, 1); // remove offset
    //gVUalt+= FreqBinsNew[channelcnt];
  }
  // gVUalt=gVUalt/10;
  // now read the audio level for VU
  digitalWrite(MP_E, 1); // multiplex disabled
  digitalWrite(S0, 10 & 0b0001);
  digitalWrite(S1, 10 & 0b0010);
  digitalWrite(S2, 10 & 0b0100);
  digitalWrite(S3, 10 & 0b1000);
  digitalWrite(MP_E, 0); // Multiplex enabled
  tempADC = analogRead(ADC_IN);
  //  VUadc=max(tempADC-ADC_OFFSET,1);

  VUadc = abs(VU_OFFSET - tempADC);
  VUadc = max(VUadc - noise, 0);

  //dbgprint("%d",gVUalt);
}


void startAutoMode() {
  autoChangePatterns = true;
  Matrix_Flag();                  //this is to show user that automode was engaged. It will show dutch flag for 2 seconds
  delay(5000);
  Serial.println(" Patterns will change after few seconds ");
  Serial.println(" You can reset by pressing the mode button again");
}

void Matrix_Flag() {

  int ledcounter = 0;
  for (int i = 0; i < kMatrixWidth / 2; i++) {
    for (int j = 0; j < kMatrixHeight / 3; j++) {
      leds[ledcounter] = CRGB::Blue;
      ledcounter++;
    }
    for (int j = 0; j < kMatrixHeight / 3; j++) {
      leds[ledcounter] = CRGB::White;
      ledcounter++;
    }
    for (int j = 0; j < kMatrixHeight / 3; j++) {
      leds[ledcounter] = CRGB::Red;
      ledcounter++;
    }
    // if number of rows can not be devided by 3 then compensate
    for (int i = 0; i < (kMatrixHeight - (kMatrixHeight / 3) * 3); i++) {
      leds[ledcounter] = CRGB::Red;
      ledcounter++;
    }
    // if number of rows can not be devided by 3 then compensate
    for (int i = 0; i < (kMatrixHeight - (kMatrixHeight / 3) * 3); i++) {
      leds[ledcounter] = CRGB::Red;
      ledcounter++;
    }

    for (int j = 0; j < kMatrixHeight / 3; j++) {
      leds[ledcounter] = CRGB::Red;
      ledcounter++;
    }
    for (int j = 0; j < kMatrixHeight / 3; j++) {
      leds[ledcounter] = CRGB::White;
      ledcounter++;
    }
    for (int j = 0; j < kMatrixHeight / 3; j++) {
      leds[ledcounter] = CRGB::Blue;
      ledcounter++;
    }




  }
  FastLED.show();
}

void getData() {

  String json = "[";
  for (int i = 0; i < 10; i++) {
    if (i > 0) {
      json += ", ";
    }
    json += "{\"bin\":";
    json += "\"" + labels[i] + "\"";
    json += ", \"value\":";
    json += String(FreqBins[i]);
    json += "}";
  }
  json += "]";
  webSocket.broadcastTXT(json.c_str(), json.length());
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // Do something with the data from the client
  if (type == WStype_TEXT) {

  }
}


//Task1code: webserver runs on separate core so that WIFI low signal doesn't freeze up program on other core
void Task1code( void * pvParameters ) {
  delay(3000);
  Serial.print("Webserver task is  running on core ");
  Serial.println(xPortGetCoreID());
  int gHue=0;
  for (;;) {
    wm.process();
    webSocket.loop();
    server.handleClient();
    if (webtoken == true) {
      getData(); // webbrowser
      webtoken = false;
    }

    // also update the logo
    EVERY_N_MILLISECONDS( 50 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    fill_rainbow( Logo,N_PIXELS_LOGO , gHue, HueSpread);
  }
}
