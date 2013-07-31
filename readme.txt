Arduino Aquarium Controller Project v1.0
========================================

This project is based off the code from the Jarduino project and the work from Stilo.

Jarduino
code.google.com/p/jarduino-aquarium-controller/

Stilo
code.google.com/p/stilo/

I've reused a lot of code from both of these projects and then added in a few new routines.

This is still a work in progress as I haven't had much time to do full testing. 

So far I've tested the temperature sensors, ethernet and pH sensors. 

I don't yet have my LED's so can't fully test them, but as the LCD code is from the Jarduino. I dont think there will be any problems.


Hardware
========

THe hardware used in this project is:

- Arduino Mega 2560
- DS1307 RTC
- DS18B20 Temperature Sensors
- ENC28J60 ethernet module
- 3.2 TFT LCD Screen
- pH Sensor
- pH Amplifier

Libraries Used
==============

DS1307
http://henningkarlsen.com/electronics/library.php?id=34

TFT/UButtons
http://henningkarlsen.com/electronics/library.php?id=51

ENC28J20 Ethernet Module
https://github.com/jcw/ethercard

Dallas Temperature Sensors
http://milesburton.com/Dallas_Temperature_Control_Library

One Wire tutorial
http://www.hacktronics.com/Tutorials/arduino-1-wire-tutorial.html

Wiring up the TFT
http://www.geeetech.com/wiki/index.php/3.2TFT_LCD


Software
========

I've added in the ENC28J60 ethernet module which logs data to ThingSpeak. (www.thingspeak.com)

There is also a NTP function which syncs the clock against 2 NTP Servers.

If you are using the ethernet module you will need to create a key.h file which needs to contain the API key used for ThingSpeak.

The file needs to contain the following:

#define APIKEY "use you api key here"

