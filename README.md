# Analog-Analyzer
Analog Analyzer with ESP32 

## Disclaimer and safety
I, Mark Donners, The Electronics Engineer, may or may not endorse various Do-It-Yourself (DIY) projects and all DIY projects are purely “at your own risk”. As with any DIY project, unfamiliarity with the tools and process can be dangerous. Posts should be construed as theoretical advice only.
If you are at all uncomfortable or inexperienced working on these projects (especially but not limited to electronics and mechanical), please reconsider doing the job yourself. It is very possible (but not likely) on any DIY project to damage belongings or void your property insurance, create a hazardous condition, or harm or even kill yourself or others. 
I will not be held responsible for any injury due to the misuse or misunderstanding of any DIY project.
By using the information provided by me, (Website, YouTube, Facebook and other social media), you agree to indemnify me, affiliates, subsidiaries and their related companies for any and all claims, damages, losses and causes of action arising out of your breach or alleged breach of this agreement(disclaimer).
The materials on this site are distributed “as is” and appear on the site without express or implied warranties of any kind, except those required by the relevant legislation. In particular, I make no warranty as to the accuracy, quality, completeness or applicability of the information provided.
The information provided is for entertainment and promotional purposes only. You may not rely on any information and opinions expressed in it for any other purpose. 
Disclaimer short version:
This is a DIY project, use any provided information and/or materials at your own risk! I am not responsible for what you do with it!

#	About this project
This document is related to the 10 channel Analog Spectrum analyser. The analogue back end uses 10 band pass filters to divide the input signal into 10 frequency bands. 
A ESP32 controllers reads the amplitude of each band and visualizes it using a Pixel LED Matrix. On top of that, the ESP32 runs a webserver to display a live graph of the current analysis. This enables you to visualize your spectrum analyser on any mobile device or PC within your network. 
You can connect your audio signal by using the audio input or you can use the microphone input to connect a small condenser microphone. Although using the microphone will limit the frequency response because of its limitations.
The input sensitivity can by adjusted just like brightness and peak hold time.  When it does not receive any input signal, after a while, it will go to fire mode in which some leds/display will light up like a fire.
The PCB can be purchased at my Tindie web shop. The firmware (Arduino Sketch ) is open source and you can modify it to your needs.

#	Operation
You can use the microphone in to connect a small condenser microphone or you can connect your audio device to the line input connectors. Although the signal from the microphone is amplified on the PCB, it might not be strong enough. Depending on your microphone, you can adjust resistor R52; decreasing it’s value will amplify the signal more. In my prototype I replaced it with a resistor of 0 Ohm ( I shorted it). However, when using a different mic, I had to increase it again to 20K. So it all depends on your mic. 

Mode button
The mode button has 3 functions:
Short press: change pattern(mode), there are 12 available patterns from which the last one is a fire screensaver.
Fast triple press: The VU meter that is displayed on the top row can be disabled/enabled
Pressed/ hold while booting: This will reset your stored WIFI settings. In case you need to change your WIFI settings or in case your system keeps rebooting, this is where to start!

Select Button
The select button has 3 functions:
Short press: Toggle between line-in and microphone input.
Long press: Press for 3 seconds to toggle “the auto change patterns” mode. When enabled, the pattern that is shown changes every few seconds. Also, when the button is pressed long enough, the Dutch national Flag will be shown. That’s how you know you’ve pressed long enough!
Double press: The direction of the falling peak will change.

Brightness Potmeter
You can use this to adjust the overall brightness of all leds / display. WARNING:Make sure you use a power supply to match the current for the brightness that you set. For sure, the ESP32 onboard regulator cannot handle all leds at full brightness. It is best to use an external powersupply that can handle 4 to 6 A. If you are using the USB cable that is connected to the ESP32, you might end up with a burning sensation coming from the ESP32 Board.

Peak Delay Potmeter
You can use this to adjust the time it takes for a peak to fall down to / rise up from the stack

Sensitivity Potmeter
You can adjust to the input level.

Serial Monitor
The serial monitor is your friend, it displays all info on booting, including your web server IP address.

Webserver
After uploading your sketch and booting for the first time, the ESP32 will serve as an access point. Use your labtop, cellphone etc, to connect to this wifi access point. You will be directed to the webpage of the WIFI manager to setup your WIFI access. It will be remembered on your next boot. If you need to change it, see text ‘Mode button’.
When set up properly, you can access the webpage with a live view of your spectrum analyzer. You can use the Serial monitor to find the ip-address, it is shown in the boot log. 
