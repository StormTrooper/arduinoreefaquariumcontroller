/*
Arduino Aquarium controller for the Mega2560
Based on the code from:
    Jarduino - https://code.google.com/p/jarduino-aquarium-controller/
    Stilo - https://code.google.com/p/stilo/
    
The below libraries were also used:    
DS1307 v1.1  http://henningkarlsen.com/electronics/library.php?id=34
http://www.hacktronics.com/Tutorials/arduino-1-wire-tutorial.html
http://www.geeetech.com/wiki/index.php/3.2TFT_LCD
https://github.com/jcw/ethercard
http://milesburton.com/Dallas_Temperature_Control_Library

NTP Code:
https://github.com/thiseldo/EtherCardExamples/blob/master/EtherCard_ntp/EtherCard_ntp.ino
TimeZone
https://github.com/JChristensen/Timezone


Hardware
Arduino Mega 2560
3.2 TFT LCD
Sensors: pH Sensor connected to A0
DS18B20 Temperature sensors conncted to pin 9
DS1307 RTC
LCD brightness connected to pin 11
ENC28J60 module sents data to Thingspeak

*/

#include <avr/wdt.h>
#include <UTFT.h>
#include <UTouch.h>
#include <UTFT_Buttons.h>
#include <DS1307.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EtherCard.h>
#include <EEPROM.h>
#include "writeAnything.h"
#include <Time.h>
#include <Timezone.h>   
#include "key.h"
#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
#else
    #define DEBUG_PRINT(x)
#endif

#ifdef DEBUG
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINTLN(x)
#endif

//----------------------------------------------------------------------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------------------------------------------------------------------

//#define APIKEY "your api key here"

#define SECS_YR_1900_2000  (3155673600UL)							// Number of seconds between 1-Jan-1900 and 1-Jan-1970, unix time starts 1970 and ntp time starts 1900.
#define GETTIMEOFDAY_TO_NTP_OFFSET 2208988800UL
#define	EPOCH_YR	1970									//(24L * 60L * 60L)
#define	SECS_DAY	86400UL  
#define	LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define	YEARSIZE(year)	(LEAPYEAR(year) ? 366 : 365)

#define ONE_WIRE_BUS 9

// Find list of servers at http://support.ntp.org/bin/view/Servers/StratumTwoTimeServers
// Please observe server restrictions with regard to access to these servers.
// This number should match how many ntp time server strings we have
#define NUM_TIMESERVERS 2

#define LARGE true
#define SMALL false


//----------------------------------------------------------------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------------------------------------------------------------

//UK Time Zone
TimeChangeRule myDST = {"Day", Last, Sun, Mar, 1, +60};    //Daylight time
TimeChangeRule mySTD = {"Std", First, Sun, Oct, 2, 0};     //Standard time
Timezone myTZ(myDST, mySTD);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev
time_t utc, local;


boolean RECOM_RCD = true;            //For Mean Well drivers change "true" to "false"
boolean CLOCK_SCREENSAVER = true;    //For a Clock Screensaver "true" / Blank Screen "false"
                                    //You can turn the Screensaver ON/OFF in the pogram

//Define the PWM PINS for the LEDs
const int ledPinSump = 14;            //PowerLed Shield pin 10
const int ledPinBlue = 15;            //PowerLed Shield pin 5
const int ledPinWhite = 16;           //PowerLed Shield pin 3
const int ledPinRoyBlue = 17;        //PowerLed Shield pin 6
const int ledPinRed = 18;            //PowerLed Shield pin 9
const int ledPinUV = 19;             //PowerLed Shield pin 11
const int ledPinMoon = 13;           //PowerLed Shield pin 13 (Modification to Shield & NOT controlled by an array)
const int tempAlarmPin = 7;         //Buzzer Alarm for Temperature offsets


byte mymac[] = { 0xD4, 0x2A, 0xB3, 0xFF, 0xC9, 0xF9 };
char website[] PROGMEM = "api.thingspeak.com";

byte Ethernet::buffer[700];
uint32_t timer;

static int currentTimeserver = 0;


// Create an entry for each timeserver to use
prog_char ntp0[] PROGMEM = "0.uk.pool.ntp.org";
prog_char ntp1[] PROGMEM = "1.uk.pool.ntp.org";



// Now define another array in PROGMEM for the above strings
prog_char *ntpList[] PROGMEM = { ntp0, ntp1 };
uint8_t clientPort = 123;

uint32_t lastNTPUpdate = 0;
uint32_t timeLong;

char tbuff[30];

// ph, fan, water, heatsink, outside
char cbuff[]= "?key=" APIKEY "&field1=00.00&field2=0000&field3=00.00&field4=00.00&field5=00.00";

float tempW = 0;                     //Water temperature values
float tempH = 0;                     //Heatsink temperature
float tempS = 0;                     //Sump heatsink temperature
int TempToBeginHoodFanInDegC = 29;   //Temperature to Turn on Hood Fans (in Degrees C)
int TempToBeginSumpFanInDegC = 29;   //Temperature to Turn on Sump Fan (in Degrees C)
float FanOn = 0.2;                   //Starts Fan(s) at 20% Duty Cycle
int HoodTempInterval = 0;            //Used for PWM Duty calculations
int SumpTempInterval = 0;            //Used for PWM Duty calculations
float HoodFanSpeedIncrease = 0;      //Used for PWM Duty calculations
float SumpFanSpeedIncrease = 0;      //Used for PWM Duty calculations
float HoodPWM = 0;                   //Used for PWM Duty calculations
float SumpPWM = 0;                   //Used for PWM Duty calculations

DeviceAddress WaterThermometer = { 0x28, 0x33, 0x33, 0x81, 0x04, 0x00, 0x00, 0xA8 };      //Waterproof 
DeviceAddress HeatsinkThermometer = { 0x28, 0xDB, 0x35, 0xDF, 0x04, 0x00, 0x00, 0x71 };     //Temp 2
DeviceAddress SumpThermometer = { 0x28, 0x78, 0x9E, 0x9C, 0x04, 0x00, 0x00, 0x5F };    //Temp 5

int dispScreen=0;                    //0-Main Screen, 1-Menu, 2-Clock Setup, 3-Temp Control, 
                                     //4-LED Test Options, 5-Test LED Arrays, 6-Test Individual
                                     //LED Colors, 7-Choose LED Color, 8-View Selected Color
                                     //Values, 9-Change Selected Color Array Values
                                     //10-Wavemaker, 11-Wavemaker Settings, 12-General
                                     //Settings, 13-Automatic Feeder, 14-Set Feeder Timers,
                                     //15-About
                                     
int x, y;                            //touch coordinates

float setTempC = 0.0;                //Desired Water Temperature (User input in program)
float setTempF = 0.0;
float offTempC = 0.0;                //Desired Water Temp. Offsets for Heater & Chiller (User input in program)
float offTempF = 0.0;
float alarmTempC = 0.0;              //Temperature the Alarm will sound (User input in program)
float alarmTempF = 0.0;
boolean tempCoolflag = 0;            //1 if cooling on
boolean tempHeatflag = 0;            //1 if heating on
boolean tempAlarmflag = 0;           //1 if alarm on
long intervalAlarm = 1000 * 30;      //Interval to beep the Alarm (1000 * seconds)
float temp2beS;                      //Temporary Temperature Values
float temp2beO;                      //Temporary Temperature Values
float temp2beA;                      //Temporary Temperature Values
int setTempScale = 0;                //Celsius=0 || Fahrenheit=1 (change in prog)
String degC_F;                       //Used in the Conversion of Celsius to Fahrenheit 

boolean UseEthernet = true;	     // False if there is no ethernet module
String time;
String date;
String day_string; 
Time mytime;
Time rtcSet;
float LC = 29.53059;                 //1 Lunar Cycle = 29.53059 days
String LP;                           //LP = Lunar Phase - variable used to print out Moon Phase
double AG;
int MI, tMI;                         //Maximum Illumination of Moon (User Defined/Set in Prog. -- Default = 0)
int MoonLow = 43;                    //Highest Value (0-255) at which Led is Still OFF, or the Value

//DIMMING VALUES can be changed below BUT the EEPROM must be cleared first.
//To CLEAR EEPROM, use arduino-0022\libraries\EEPROM\examples\eeprom_clear\eeprom_clear.pde
//and change the 512 to 4096 before Upload.  After the LED comes on indicating the EEPROM
//has been cleared, it is now ok to change DIMMING VALUES below & Upload the sketch.
//SUMP Dimming Values 8pm to 8am
byte sled[96] = {
  192, 200, 200, 200, 200, 200, 200, 200,   //0 - 1
  200, 200, 200, 200, 200, 200, 200, 200,   //2 - 3
  197, 195, 192, 190, 175, 175, 175, 175,   //4 - 5
  140, 140, 140, 140, 105, 95, 85, 75,      //6 - 7
  0, 0, 0, 0, 0, 0, 0, 0,                   //8 - 9
  0, 0, 0, 0, 0, 0, 0, 0,                   //10 - 11
  0, 0, 0, 0, 0, 0, 0, 0,                   //12 - 13
  0, 0, 0, 0, 0, 0, 0, 0,                   //14 - 15
  0, 0, 0, 0, 0, 0, 0, 0,                   //16 - 17
  0, 0, 0, 0, 0, 0, 0, 0,                   //18 - 19
  75, 85, 95, 105, 140, 140, 140, 140,      //20 - 21
  175, 175, 175, 175, 190, 192, 195, 197    //22 - 23
};  
//REGULAR BLUE Dimming Values
byte bled[96] = {
  0, 0, 0, 0, 0, 0, 0, 0,                   //0 - 1
  0, 0, 0, 0, 0, 0, 0, 0,                   //2 - 3
  0, 0, 0, 0, 0, 0, 0, 0,                   //4 - 5
  0, 0, 0, 0, 0, 0, 28, 28,                 //6 - 7
  30, 30, 32, 55, 70, 70, 75, 80,           //8 - 9
  80, 85, 90, 110, 120, 125, 130, 135,      //10 - 11
  140, 145, 150, 160, 160, 160, 165, 170,   //12 - 13
  175, 180, 185, 190, 195, 195, 195, 195,   //14 - 15
  195, 195, 195, 195, 190, 185, 180, 175,   //16 - 17
  170, 165, 160, 160, 160, 150, 145, 140,   //18 - 19
  135, 130, 125, 115, 100, 75, 60, 30,      //20 - 21
  30, 30, 28, 28, 0, 0, 0, 0                //22 - 23
};  
//WHITE Dimming Values (White LED array in RAM)
byte wled[96] = {
  0, 0, 0, 0, 0, 0, 0, 0,                   //0 - 1
  0, 0, 0, 0, 0, 0, 0, 0,                   //2 - 3
  0, 0, 0, 0, 0, 0, 0, 0,                   //4 - 5
  0, 0, 0, 0, 0, 0, 0, 0,                   //6 - 7
  0, 0, 0, 0, 0, 0, 0, 0,                   //8 - 9
  28, 32, 38, 50, 70, 90, 90, 90,           //10 - 11
  95, 95, 95, 95, 105, 105, 105, 105,       //12 - 13
  120, 120, 120, 120, 125, 125, 125, 125,   //14 - 15
  125, 125, 125, 125, 120, 120, 120, 120,   //16 - 17
  115, 115, 115, 110, 100, 100, 95, 95,     //18 - 19
  70, 50, 38, 32, 28, 0, 0, 0,              //20 - 21
  0, 0, 0, 0, 0, 0, 0, 0                    //22 - 23
};
//ROYAL BLUE Dimming Values
byte rbled[96] = {
  0, 0, 0, 0, 0, 0, 0, 0,                   //0 - 1
  0, 0, 0, 0, 0, 0, 0, 0,                   //2 - 3
  0, 0, 0, 0, 0, 0, 0, 0,                   //4 - 5
  0, 0, 0, 0, 0, 0, 35, 40,                 //6 - 7
  40, 43, 47, 55, 65, 75, 80, 85,           //8 - 9
  90, 95, 95, 100, 110, 110, 115, 120,      //10 - 11
  125, 130, 135, 145, 145, 145, 150, 155,   //12 - 13
  160, 165, 170, 175, 180, 180, 180, 180,   //14 - 15
  180, 180, 180, 180, 175, 170, 165, 160,   //16 - 17
  155, 150, 145, 145, 145, 135, 130, 125,   //18 - 19
  120, 115, 110, 110, 100, 75, 65, 50,      //20 - 21
  40, 35, 33, 28, 0, 0, 0, 0                //22 - 23
};  
//RED Dimming Values
byte rled[96] = {
  0, 0, 0, 0, 0, 0, 0, 0,                   //0 - 1
  0, 0, 0, 0, 0, 0, 0, 0,                   //2 - 3
  0, 0, 0, 0, 0, 0, 0, 0,                   //4 - 5
  0, 0, 0, 0, 0, 0, 0, 0,                   //6 - 7
  0, 0, 0, 0, 0, 0, 0, 0,                   //8 - 9
  0, 0, 0, 0, 0, 0, 0, 0,                   //10 - 11
  30, 30, 40, 40, 50, 50, 60, 60,           //12 - 13
  60, 60, 50, 50, 40, 40, 30, 30,           //14 - 15
  0, 0, 0, 0, 0, 0, 0, 0,                   //16 - 17
  0, 0, 0, 0, 0, 0, 0, 0,                   //18 - 19
  0, 0, 0, 0, 0, 0, 0, 0,                   //20 - 21
  0, 0, 0, 0, 0, 0, 0, 0    
};  
//ULTRA VIOLET (UV) Dimming Values
byte uvled[96] = {
  0, 0, 0, 0, 0, 0, 0, 0,                   //0 - 1
  0, 0, 0, 0, 0, 0, 0, 0,                   //2 - 3
  0, 0, 0, 0, 0, 0, 0, 0,                   //4 - 5
  0, 0, 0, 0, 0, 0, 0, 0,                   //6 - 7
  0, 0, 0, 0, 0, 0, 0, 0,                   //8 - 9
  0, 0, 0, 0, 20, 25, 30, 35,               //10 - 11
  40, 45, 50, 55, 60, 65, 70, 70,           //12 - 13
  65, 60, 55, 50, 45, 40, 35, 30,           //14 - 15
  25, 20, 0, 0, 0, 0, 0, 0,                 //16 - 17
  0, 0, 0, 0, 0, 0, 0, 0,                   //18 - 19
  0, 0, 0, 0, 0, 0, 0, 0,                   //20 - 21
  0, 0, 0, 0, 0, 0, 0, 0    
};
byte tled[96];     //Temporary Array to Hold changed LED Values

int rtcSet2, AM_PM, yTime;           //Setting clock stuff
int timeDispH, timeDispM, 
    xTimeH, xTimeM10, xTimeM1, 
    xTimeAMPM, xColon;




/**************************** CHOOSE OPTION MENU BUTTONS *****************************/
const int tanD[]= {10, 29, 155, 59};        //"TIME and DATE" settings
const int temC[]= {10, 69, 155, 99};        //"H2O TEMP CONTROL" settings
//const int wave[]= {10, 109, 155, 139};      //"Wavemaker CONTROL" settings
const int gSet[]= {10, 109, 155, 139};      //"GENERAL SETTINGS" page
const int tesT[]= {165, 29, 310, 59};       //"LED TESTING OPTIONS" menu
const int ledChM[]= {165, 69, 310, 99};     //"CHANGE LED VALUES" menu
//const int aFeed[]= {165, 109, 310, 139};    //"AUTOMATIC FEEDER" menu
const int about[]= {165, 109, 310, 139};    //"ABOUT" program information
/**************************** TIME AND DATE SCREEN BUTTONS ***************************/
const int houU[]= {110, 22, 135, 47};       //hour up
const int minU[]= {180, 22, 205, 47};       //min up
const int ampmU[]= {265, 22, 290, 47};      //AM/PM up
const int houD[]= {110, 73, 135, 96};       //hour down
const int minD[]= {180, 73, 205, 96};       //min down
const int ampmD[]= {265, 73, 290, 96};      //AM/PM down
const int dayU[]= {110, 112, 135, 137};     //day up
const int monU[]= {180, 112, 205, 137};     //month up
const int yeaU[]= {265, 112, 290, 137};     //year up
const int dayD[]= {110, 162, 135, 187};     //day down
const int monD[]= {180, 162, 205, 187};     //month down
const int yeaD[]= {265, 162, 290, 187};     //year down
/*************************** H2O TEMP CONTROL SCREEN BUTTONS *************************/
const int temM[]= {90, 49, 115, 74};        //temp. minus
const int temP[]= {205, 49, 230, 74};       //temp. plus
const int offM[]= {90, 99, 115, 124};       //offset minus
const int offP[]= {205, 99, 230, 124};      //offset plus
const int almM[]= {90, 149, 115, 174};      //alarm minus
const int almP[]= {205, 149, 230, 174};     //alarm plus
/**************************** LED TESTING MENU BUTTONS *******************************/
const int tstLA[] = {40, 59, 280, 99};      //"Test LED Array Output" settings
const int cntIL[] = {40, 109, 280, 149};    //"Control Individual Leds" settings
/********************** TEST LED ARRAY OUTPUT SCREEN BUTTONS *************************/
const int stsT[]= {110, 105, 200, 175};     //start/stop
const int tenM[]= {20, 120, 90, 160};       //-10s
const int tenP[]= {220, 120, 290, 160};     //+10s
/******************** TEST INDIVIDUAL LED VALUES SCREEN BUTTONS **********************/
//These Buttons are made within the function
/****************** CHANGE INDIVIDUAL LED VALUES SCREEN BUTTONS **********************/
//These Buttons are made within the function
/************************* CHANGE LED VALUES MENU BUTTONS ****************************/
const int btCIL[]= {5, 188, 90, 220};       //back to Change Individual LEDs Screen
const int ledChV[]= {110, 200, 210, 220};   //LED Change Values
const int eeprom[]= {215, 200, 315, 220};   //Save to EEPROM (Right Button)
const int miM[]= {90, 115, 115, 140};       //MI minus
const int miP[]= {205, 115, 230, 140};      //MI plus


                                     //you wish the New Moon to Shine (it will increase from here to MI)

unsigned int *MoonPic;               //Pointer to the Lunar Phase Pics
extern unsigned int                  //Lunar Phase Pics
  New_Moon[0x9C4],
  Waxing_Crescent[0x9C4],
  First_Quarter[0x9C4],
  Waxing_Gibbous[0x9C4],
  Full_Moon[0x9C4],
  Waning_Gibbous[0x9C4],
  Last_Quarter[0x9C4],
  Waning_Crescent[0x9C4];


//int setTempScale = 0;                //Celsius=0 || Fahrenheit=1 (change in prog)

int LedChangTime = 0;                //LED change page, time and values

int min_cnt;                         //Used to determine the place in the color arrays

boolean LEDtestTick = false;         //for testing leds and speed up clock

int whiteLed, blueLed, rblueLed,     //previous LED output values
    redLed, uvLed, sumpLed;
int bled_out, wled_out, rbled_out,   //current LED output values
    rled_out, uvled_out, sled_out,
    moonled_out, colorled_out;

int COLOR=0, WHITE=1, BLUE=2,        //Used to Determine View/Change Color LED Values
    ROYAL=3, RED=4, ULTRA=5, 
    SUMP=6, MOON=7;

boolean colorLEDtest = false;        //To test individual color LEDs
int Rgad=0, Ggad=0, Bgad=0,          //Used in the Test Ind. Color LEDs Gadget
    Rfont=0, Gfont=0, Bfont=0,
    Rback=0, Gback=0, Bback=0,
    Rline=0, Gline=0, Bline=0;
int CL_1=0, CL_10=0, CL_100=0,       //Used in the Test Ind. Color LEDs Gadget
    cl_1, cl_10, cl_100;
int bcol_out, wcol_out, rbcol_out,   //Current LED output values for Test Ind. Color LEDs
    rcol_out, uvcol_out, scol_out,
    mooncol_out;
int x1Bar=0, x2Bar=0,                //Used in LED Output Chart on Test Ind. LEDs Screen
    xValue=0, yValue=0, 
    LEDlevel, yBar; 
int setAutoStop = 2;                //ON=1 ||  OFF = 2 (change in prog)

int returnTimer = 0;                //Counter for Screen Retuen
boolean SCREEN_RETURN = true;        //Auto Return to mainScreen() after so long of inactivity

//int setScreenSaverTimer = (20)*12;   //how long in (minutes) before Screensaver comes on  
int setScreenSaverTimer = 30;   //how long in (minutes) before Screensaver comes on  

int setReturnTimer = setScreenSaverTimer * .75;       //Will return to main screen after 75% of the amount of
                                     //time it takes before the screensaver turns on
    
int setScreensaver = 1;              //ON=1 || OFF=2 (change in prog)
int screenSaverTimer = 0;            //counter for Screen Saver
                              
                                     
long previousMillisLED = 0;          //Used in the Test LED Array Function
long previousMillisWave = 0;         //Used in the WaveMaker Function wave_output()
long previousMillisFive = 0;         //Used in the Main Loop (Checks Time,Temp,LEDs,Screen)
long previousMillisAlarm = 0;        //Used in the Alarm

const byte LCDbrightPin = 11;    //pin to controll LCD brightness
//dont use pin 10 - for some reason it causes ethernet module to crash
byte LCDbright = 70; 

int Fanspeed = 5321;

int pH_AnalogPin = 0;
int TX_led = 12;
int pH_SensorValue;        //Analog value of pin

float pH = 0;
//float WaterTemp = 0;
//float HeatsinkTemp = 0;
//float OutsideTemp = 0;

int freeCount;
int Counter=0;

int setCalendarFormat = 0;           //DD/MM/YYYY=0 || Month DD, YYYY=1 (change in prog)
int setTimeFormat = 0;               //24HR=0 || 12HR=1 (change in prog)

//--------------------buttons------------------------
const int canC[]= {215,200,315,220};       //standard cancel
//const int prOK[]= {2,194,158,225};         //standard ok
const int back[]= {5, 200, 105, 220};       //BACK
const int prSAVE[]= {110, 200, 210, 220};   //SAVE


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Init DS1307
DS1307 rtc(20,21);

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t BigFont[];
extern uint8_t Dingbats1_XL[];

//                  RS,WR,CS, Reset
UTFT myGLCD(ITDB32S,38,39,40,41);   // Remember to change the model parameter to suit your display module!
UTouch        myTouch(6,5,4,3,2);

// Finally we set up UTFT_Buttons :)
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

int i=1;
//Time t;
//int Xsize;

//----------------------------------------------------------------------------------------------------------------------------------
// EEPROM FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------

struct config_t
{
  int tempset;
  int tempFset;
  int tempoff;
  int tempFoff;
  int tempalarm;
  int tempFalarm;
} tempSettings;

struct config_m
{
  int MI_t;
} MIsettings;

//struct config_w
//{
//  int waveMode;
//  int altPump1m;
//  int altPump1s;  
//  int altPump2m;
//  int altPump2s;
//  int synchMode;
//  int synchPumpOnM;
//  int synchPumpOnS; 
//  int synchPumpOffM;  
//  int synchPumpOffS;
//} WAVEsettings;

struct config_g
{
  int calendarFormat;
  int timeFormat;
  int tempScale;
  int SCREENsaver;  
  int autoStop;
} GENERALsettings;

//struct config_f
//{
//  int feedFish1h;
//  int feedFish1m;
//  int feedFish2h;
//  int feedFish2m;
//  int feedFish3h;
//  int feedFish3m;
//  int feedFish4h;
//  int feedFish4m;
//  int feedTime1;
//  int feedTime2;
//  int feedTime3;
//  int feedTime4;  
//} FEEDERsettings;

void SaveLEDToEEPROM()
{
  EEPROM.write(0, 123);         //to determine if data available in EEPROM
  for (int i=1; i<97; i++)  
    {
     EEPROM.write(i+(96*0), wled[i]);
     EEPROM.write(i+(96*1), bled[i]);
     EEPROM.write(i+(96*2), rbled[i]);
     EEPROM.write(i+(96*3), rled[i]);
     EEPROM.write(i+(96*4), sled[i]);
     EEPROM.write(i+(96*5), uvled[i]);    
    } 
}

void SaveMoonLEDToEEPROM()
{
  MIsettings.MI_t = int(MI);
  EEPROM_writeAnything(600, MIsettings);
}


void SaveTempToEEPROM()
{
  tempSettings.tempset = int(setTempC*10);
  tempSettings.tempFset = int(setTempF*10);  
  tempSettings.tempoff = int(offTempC*10);
  tempSettings.tempFoff = int(offTempF*10);  
  tempSettings.tempalarm = int(alarmTempC*10);
  tempSettings.tempFalarm = int(alarmTempF*10);
  EEPROM_writeAnything(620, tempSettings);
}

void SaveGenSetsToEEPROM()
{
  GENERALsettings.calendarFormat = int(setCalendarFormat);
  GENERALsettings.timeFormat = int(setTimeFormat);
  GENERALsettings.tempScale = int(setTempScale);
  GENERALsettings.SCREENsaver = int(setScreensaver);
  GENERALsettings.autoStop = int(setAutoStop);
  EEPROM_writeAnything(640, GENERALsettings);
}



void(* resetFunc) (void) = 0; //declare reset function @ address 0

/************************************* Month length for NTP *************************************/
uint8_t monthlen(uint8_t isleapyear,uint8_t month){
  const uint8_t mlen[2][12] = {
    { 
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    ,
    { 
      31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
  };
  return(mlen[isleapyear][month]);
}


/************************************* LED LEVELS *************************************/
void LED_levels_output()
{
  int sector, sstep, t1, t2 ;
  int s_out, b_out, w_out, rb_out, r_out, uv_out, moon_out;
 
  if (min_cnt>=1440) {min_cnt=1;}   // 24 hours of minutes 
  sector = min_cnt/15;              // divided by gives sector -- 15 minute
  sstep = min_cnt%15;               // remainder gives add on to sector value 
 
  t1 =sector;
  if (t1==95) {t2=0;}
   else {t2 = t1+1;}
  
  
  if (colorLEDtest) 
    {
     sled_out = scol_out; 
     rled_out = rcol_out; 
     wled_out = wcol_out; 
     bled_out = bcol_out; 
     rbled_out = rbcol_out; 
     uvled_out = uvcol_out; 
     moonled_out = mooncol_out;
    } else 
        {
         if (sstep==0) 
           {
            sled_out = sled[t1];
            bled_out = bled[t1];
            wled_out = wled[t1];
            rbled_out = rbled[t1];
            rled_out = rled[t1];
            uvled_out = uvled[t1];
           } else 
               {
                sled_out = check(&sled[t1], &sled[t2], sstep);
                bled_out = check(&bled[t1], &bled[t2], sstep);
                wled_out = check(&wled[t1], &wled[t2], sstep);
                rbled_out = check(&rbled[t1], &rbled[t2], sstep);
                rled_out = check(&rled[t1], &rled[t2], sstep);
                uvled_out = check(&uvled[t1], &uvled[t2], sstep);
               }  
         float lunarCycle = moonPhase(mytime.year, mytime.mon, mytime.date); //get a value for the lunar cycle
         moonled_out = MoonLow *(1 - lunarCycle) +            
                       MI * lunarCycle + 0.5;                  //MaximumIllumination * % of Full Moon (0-100)
        }
  
  if (RECOM_RCD) {
    s_out = sled_out;
    b_out = bled_out;
    w_out = wled_out;
    rb_out = rbled_out;
    r_out = rled_out;
    uv_out = uvled_out;
    moon_out = moonled_out;
  } else {
    s_out = 255 - sled_out;
    b_out = 255 - bled_out;
    w_out = 255 - wled_out;
    rb_out = 255 - rbled_out;
    r_out = 255 - rled_out;
    uv_out = 255 - uvled_out; 
    moon_out = 255 - moonled_out;
  }
    
  analogWrite(ledPinSump, s_out);
  analogWrite(ledPinBlue, b_out);
  analogWrite(ledPinWhite, w_out);
  analogWrite(ledPinRoyBlue, rb_out);
  analogWrite(ledPinRed, r_out);
  analogWrite(ledPinUV, uv_out); 
  analogWrite(ledPinMoon, moon_out);
}

int check( byte *pt1, byte *pt2, int lstep)
{
  int result;
  float fresult;
   
  if (*pt1==*pt2) {result = *pt1;}     // No change
    else if (*pt1<*pt2)                //Increasing brightness
    { fresult = ((float(*pt2-*pt1)/15.0) * float(lstep))+float(*pt1);
     result = int(fresult);
     }
     //Decreasing brightness
    else {fresult = -((float(*pt1-*pt2)/15.0) * float(lstep))+float(*pt1);
     result = int(fresult);                     
    } 
    return result;  
}
/********************************* END OF LED LEVELS **********************************/

void printLedChangerP(char* text, int x1, int y1, int x2, int y2, boolean fontsize = false)
{
  int stl = strlen(text);
  int fx, fy;
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect (x1, y1, x2, y2);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (x1, y1, x2, y2);
 
  myGLCD.setBackColor(255, 255, 255);
       myGLCD.setColor(255, 0, 0);
  if (fontsize) {
    myGLCD.setFont(BigFont); 
    fx = x1+(((x2 - x1+1)-(stl*16))/2);
    fy = y1+(((y2 - y1+1)-16)/2);
    myGLCD.print(text, fx, fy);
    }
  else {
    myGLCD.setFont(SmallFont); 
    fx = x1+(((x2 - x1)-(stl*8))/2);
    fy = y1+(((y2 - y1-1)-8)/2);
    myGLCD.print(text, fx, fy);
    }
}


void printLedChangerM(char* text, int x1, int y1, int x2, int y2, boolean fontsize = false)
{
  int stl = strlen(text);
  int fx, fy;
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect (x1, y1, x2, y2);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect (x1, y1, x2, y2);
 
  myGLCD.setBackColor(255, 255, 255);
       myGLCD.setColor(0, 0, 255);
  if (fontsize) {
    myGLCD.setFont(BigFont); 
    fx = x1+(((x2 - x1+1)-(stl*16))/2);
    fy = y1+(((y2 - y1+1)-16)/2);
    myGLCD.print(text, fx, fy);
    }
  else {
    myGLCD.setFont(SmallFont); 
    fx = x1+(((x2 - x1)-(stl*8))/2);
    fy = y1+(((y2 - y1-1)-8)/2);
    myGLCD.print(text, fx, fy);
    }
}

void ReadFromEEPROM()
{
  int k = EEPROM.read(0);
 // char tempString[3];
  
  if (k==123) {
    for (int i=1; i<97; i++)  {
      wled[i] = EEPROM.read(i+(96*0));
      bled[i] = EEPROM.read(i+(96*1));
     rbled[i] = EEPROM.read(i+(96*2));
      rled[i] = EEPROM.read(i+(96*3));
      sled[i] = EEPROM.read(i+(96*4));
     uvled[i] = EEPROM.read(i+(96*5));   
    }  
   }

  EEPROM_readAnything(600, MIsettings);  
  MI = MIsettings.MI_t;
  
   
  EEPROM_readAnything(620, tempSettings);  
  setTempC = tempSettings.tempset;
  setTempC /=10;
  setTempF = tempSettings.tempFset;
  setTempF /=10;  
  offTempC = tempSettings.tempoff;
  offTempC /=10;
  offTempF = tempSettings.tempFoff;
  offTempF /=10;  
  alarmTempC = tempSettings.tempalarm;
  alarmTempC /= 10;
  alarmTempF = tempSettings.tempFalarm;
  alarmTempF /= 10;
  
  EEPROM_readAnything(640, GENERALsettings);
  setCalendarFormat = GENERALsettings.calendarFormat;
  setTimeFormat = GENERALsettings.timeFormat;
  setTempScale = GENERALsettings.tempScale;
  setScreensaver = GENERALsettings.SCREENsaver;  
  setAutoStop = GENERALsettings.autoStop;

}


//----------------------------------------------------------------------------------------------------------------------------------
// RTC Functions
//----------------------------------------------------------------------------------------------------------------------------------
void SaveRTC()
{
  //int year=rtcSet.year - 2000;
      
  //rtc.stop();            //RTC clock setup 
  rtc.setTime(rtcSet.hour, rtcSet.min, rtcSet.sec);      // Set time

  rtc.setDate(rtcSet.date, rtcSet.mon, rtcSet.year);     // Set Date
  delay(10);
  //RTC.start();
  //delay(10);
//  for(int i=0; i<56; i++)
 //   { rtc.writeBuffer(65,i);   } 
  //delay(50);
}

//----------------------------------------------------------------------------------------------------------------------------------
// Date & Time Bar
//----------------------------------------------------------------------------------------------------------------------------------

void TimeDateBar(boolean refreshAll=false)
{
  String oldVal, rtc1, rtc2, ampm, month;  
  
  if ((mytime.min>=0) && (mytime.min<=9)) 
    { rtc1= '0' + String(mytime.min);}               //adds 0 to minutes 
  else { rtc1= String(mytime.min); }
    
  if (setTimeFormat==1) 
    {
     if (mytime.hour==0) { rtc2 = "12"; }                //12 HR Format
       else {
         if (mytime.hour>12) { rtc2= String(mytime.hour-12); }
           else { rtc2= String(mytime.hour); }
            }
    } 

  if(mytime.hour < 12){ ampm= " AM  "; }              //Adding the AM/PM sufffix
    else { ampm= " PM  "; }
  
  oldVal = time;                                 //refresh time if different
  if (setTimeFormat==1) 
    { time= rtc2 + ':' + rtc1 + ampm;} 
 else {time= " " + String(mytime.hour) + ':' + rtc1 +"      ";}
  if ((oldVal!=time) || refreshAll)
    {
     char bufferT[9];
     time.toCharArray(bufferT, 9);               //String to Char array 
     setFont(SMALL, 255, 255, 0, 64, 64, 64);
     myGLCD.print(bufferT, 215, 227);            //Display time
    }


  if (mytime.mon==1)  { month= "JAN "; }             //Convert the month to its name
  if (mytime.mon==2)  { month= "FEB "; }
  if (mytime.mon==3)  { month= "MAR "; }
  if (mytime.mon==4)  { month= "APR "; }
  if (mytime.mon==5)  { month= "MAY "; }
  if (mytime.mon==6)  { month= "JUN "; }
  if (mytime.mon==7)  { month= "JLY "; }
  if (mytime.mon==8)  { month= "AUG "; }
  if (mytime.mon==9)  { month= "SEP "; }
  if (mytime.mon==10) { month= "OCT "; }
  if (mytime.mon==11) { month= "NOV "; }
  if (mytime.mon==12) { month= "DEC "; }
    
  oldVal = date;                                 //refresh date if different
  
  if (setCalendarFormat==0)
    { date= "  " + String(mytime.date) + '/' + String(mytime.mon) + '/' + String(mytime.year) + "   ";}
  else
    { date= "  " + month + String(mytime.date) + ',' + ' ' + String(mytime.year);}
  if ((oldVal!=date) || refreshAll)
    {
     char bufferD[15];
     date.toCharArray(bufferD, 15);              //String to Char array
     setFont(SMALL, 255, 255, 0, 64, 64, 64);
     myGLCD.print(bufferD, 20, 227);             //Display date  
    }
}

//----------------------------------------------------------------------------------------------------------------------------------
// Temperature Functions
//----------------------------------------------------------------------------------------------------------------------------------

void checkTempC()
{ 
  
   sensors.requestTemperatures();            //Read DS18B20 Sensors
  //WaterTemp = sensors.getTempC(WaterThermometer);
  // HeatsinkTemp = sensors.getTempC(HeatsinkThermometer);
  // OutsideTemp = sensors.getTempC(OutsideThermometer);

   pH_SensorValue = analogRead(pH_AnalogPin);
   
   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
   pH = (14.0 / 3.148) * pH_SensorValue * (5.0 / 1023.0);
   
   
  tempW = (sensors.getTempC(WaterThermometer));  //read water temperature
  
  
  tempH = (sensors.getTempC(HeatsinkThermometer));   //read hood's heatsink temperature
  tempS = (sensors.getTempC(SumpThermometer));   //read Outside temperature

  if (tempW<(setTempC+offTempC+alarmTempC) && tempW>(setTempC-offTempC-alarmTempC)) {
    tempAlarmflag = false;
    digitalWrite(tempAlarmPin, LOW);                           //turn off alarm
    }
  if (tempW<(setTempC+offTempC) && tempW>(setTempC-offTempC))  //turn off chiller/heater
      {
       tempCoolflag=false;
       tempHeatflag=false;
       //digitalWrite(tempHeatPin, LOW);
     //  digitalWrite(tempChillPin, LOW);   
       }
  if (offTempC>0)
    {
     if (tempW >=(setTempC+offTempC))            //turn on chiller
       {
        tempCoolflag=true;
       // digitalWrite(tempChillPin, HIGH);
       }
     if (tempW<=(setTempC-offTempC))             //turn an heater
       {
        tempHeatflag=true;
      //  digitalWrite(tempHeatPin, HIGH);
       }
    }
  if (alarmTempC>0)                              //turn on alarm
    {
     if ((tempW>=(setTempC+offTempC+alarmTempC)) || (tempW<=(setTempC-offTempC-alarmTempC)))
       {
        tempAlarmflag=true;
        unsigned long cMillis = millis();
        if (cMillis - previousMillisAlarm > intervalAlarm)
          {
           previousMillisAlarm = cMillis;
           digitalWrite(tempAlarmPin, HIGH);
           delay(1000);
           digitalWrite(tempAlarmPin, LOW);      
          }
       }
    }
     
  //Fan Controller for Hood
  HoodTempInterval = (tempH - TempToBeginHoodFanInDegC);   //Sets the interval to start from 0
  HoodFanSpeedIncrease = HoodTempInterval*0.1;   //Fan's speed increases 10% every degree over set temperature
//  digitalWrite(HoodFansTranzPin, HIGH);  
  if (tempH < TempToBeginHoodFanInDegC)          //If Temp's less than defined value, leave fan off
    { HoodPWM = 0;
 //     digitalWrite(HoodFansTranzPin, LOW);
 }
  if ((tempH  >= TempToBeginHoodFanInDegC) && (HoodFanSpeedIncrease < 1))   //For every degree over defined value, increase by 10%
    { HoodPWM = FanOn + HoodFanSpeedIncrease;}
  if (HoodFanSpeedIncrease >= 1)                 //If the temperature is 10 or more degrees C higher than user 
    { HoodPWM = 1;}                              //defined value to start, leave it at 100%
 
  //Fan Controller for Sump
  SumpTempInterval = (tempS - TempToBeginSumpFanInDegC);   //Sets the interval to start from 0
  SumpFanSpeedIncrease = SumpTempInterval*0.1;   //Fan's speed increases 10% every degree over set temperature
//  digitalWrite(SumpFanTranzPin, HIGH);  
  if (tempS < TempToBeginSumpFanInDegC)          //If Temp's less than defined value, leave fan off
    { SumpPWM = 0;
 //     digitalWrite(SumpFanTranzPin, LOW);
 }
  if ((tempS  >= TempToBeginSumpFanInDegC) && (SumpFanSpeedIncrease < 1))   //For every degree over defined value, increase by 10%
    { SumpPWM = FanOn + SumpFanSpeedIncrease;}
  if (SumpFanSpeedIncrease >= 1)                 //If the temperature is 10 or more degrees C higher than user 
    { SumpPWM = 1;}                              //defined value to start, leave it at 100%   
}


//----------------------------------------------------------------------------------------------------------------------------------
// Lunar Phase
//----------------------------------------------------------------------------------------------------------------------------------
float moonPhase(int moonYear, int moonMonth, int moonDay)
{ 
  float phase;
  double IP; 
  long YY, MM, K1, K2, K3, JulianDay; 
  
  YY = moonYear - floor((12 - moonMonth) / 10); 
  MM = moonMonth + 9;
  if (MM >= 12)
    { MM = MM - 12; }
  
  K1 = floor(365.25 * (YY + 4712));
  K2 = floor(30.6 * MM + 0.5);
  K3 = floor(floor((YY / 100) + 49) * 0.75) - 38;
  JulianDay = K1 + K2 + moonDay + 59;

  if (JulianDay > 2299160)
    { JulianDay = JulianDay - K3; }
  IP = MyNormalize((JulianDay - 2451550.1) / LC);
  AG = IP*LC;
  phase = 0; 

  //Determine the Moon Illumination %
  if ((AG >= 0) && (AG <= LC/2))             //FROM New Moon 0% TO Full Moon 100%
    { phase = (2*AG)/LC; }
  if ((AG > LC/2) && (AG <= LC))             //FROM Full Moon 100% TO New Moon 0%
    { phase = 2*(LC-AG)/LC; }

  //Determine the Lunar Phase
  if ((AG >= 0) && (AG <= 1.85))             //New Moon; ~0-12.5% illuminated
    { LP = "    New Moon   "; 
      MoonPic = New_Moon; }
  if ((AG > 1.85) && (AG <= 5.54))           //New Crescent; ~12.5-37.5% illuminated
    { LP = "Waxing Crescent";
      MoonPic = Waxing_Crescent; }
  if ((AG > 5.54) && (AG <= 9.23))           //First Quarter; ~37.5-62.5% illuminated
    { LP = " First Quarter ";
      MoonPic = First_Quarter; }
  if ((AG > 9.23) && (AG <= 12.92))          //Waxing Gibbous; ~62.5-87.5% illuminated
    { LP = "Waxing Gibbous ";
      MoonPic = Waxing_Gibbous; }
  if ((AG > 12.92) && (AG <= 16.61))         //Full Moon; ~87.5-100-87.5% illuminated
    { LP = "   Full Moon   ";
      MoonPic = Full_Moon; }    
  if ((AG > 16.61) && (AG <= 20.30))         //Waning Gibbous; ~87.5-62.5% illuminated
    { LP = "Waning Gibbous ";
      MoonPic = Waning_Gibbous; }
  if ((AG > 20.30) && (AG <= 23.99))         //Last Quarter; ~62.5-37.5% illuminated
    { LP = " Last Quarter  ";
      MoonPic = Last_Quarter; }
  if ((AG > 23.99) && (AG <= 27.68))         //Old Crescent; ~37.5-12.5% illuminated
    { LP = "Waning Crescent";
      MoonPic = Waning_Crescent;
    }
  if ((AG >= 27.68) && (AG <= LC))           //New Moon; ~12.5-0% illuminated
    { LP = "    New Moon   ";
      MoonPic = New_Moon; }
        
  return phase;

}

double MyNormalize(double v) 
{ 
  v = v - floor(v);
  if (v < 0)
    v = v + 1;
  return v;
} 

//----------------------------------------------------------------------------------------------------------------------------------
// Main Screen
//----------------------------------------------------------------------------------------------------------------------------------
void mainScreen(boolean refreshAll=false)
{
  int ledLevel, t;
  String oldval, deg;  
  
  mytime = rtc.getTime();
  TimeDateBar(true);
  
  oldval = day_string;                                  //refresh day if different
  day_string = rtc.getDOWStr();        //String(rtc[4]);

  if ((oldval!=day_string) || refreshAll)
    {
     myGLCD.setColor(64, 64, 64);                //Draw Borders & Dividers in Grey
     myGLCD.drawRect(0, 0, 319, 239);            //Outside Border
     myGLCD.drawRect(158, 14, 160, 226);         //Vertical Divider
     myGLCD.drawRect(160, 125, 319, 127);        //Horizontal Divider
     myGLCD.fillRect(0, 0, 319, 14);             //Top Bar
     myGLCD.setColor(0, 0, 0);
     myGLCD.drawLine(159, 126, 161, 126);        //Horizontal Divider Separator     
     setFont(SMALL, 255, 255, 0, 64, 64, 64);
     myGLCD.print("Aquarium Controller v.1.0", CENTER, 1);
     setFont(SMALL, 255, 255, 255, 0, 0, 0);
     myGLCD.print("LED ARRAY", 52, 20);
     myGLCD.print("LUNAR PHASE", 196, 20); 
     myGLCD.print("MONITORS & ALERTS", 174, 133);
     
     myGLCD.drawBitmap(211, 35, 50, 50, MoonPic, 1);    //Moon Phase Picture (middle 240,64)
     setFont(SMALL, 176, 176, 176, 0, 0, 0);            
     char bufferLP[16];
     LP.toCharArray(bufferLP, 16);
     myGLCD.print(bufferLP, 180, 96);            //Print Moon Phase Description to LCD
    float lunarCycle = moonPhase(mytime.year, mytime.mon, mytime.date); //get a value for the lunar cycle
     
     if ((lunarCycle*100) < 1)                   //Print % of Full to LCD
       { myGLCD.print(" 0.0", 188, 108); }
       else { myGLCD.printNumF(lunarCycle*100, 1, 188, 108);}
     myGLCD.print("% of Full", 220, 108);     
    }
     drawBarGraph();    
  if ((sumpLed!=sled_out) || refreshAll)         //refresh red led display
   {
    sumpLed = sled_out;
    ledLevel = LedToPercent(sled_out);
    oldval = "SUMP:   " + String(ledLevel) + "%  " + "  ";
    char bufferS[13];
    oldval.toCharArray(bufferS, 13);
    t= 136 - sled_out*.39;
      
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(40, t, 52, 36);              //hide end of last bar
    myGLCD.setColor(0, 150, 0);
    setFont(SMALL, 0, 150, 0, 0, 0, 0);
    myGLCD.print(bufferS, 45, 147);              //display SUMP LEDs % output
    myGLCD.drawRect(40, 136, 52, 135);           //SUMP %bar place holder
    myGLCD.fillRect(40, 136, 52, t);             //SUMP percentage bar
   }    

  if ((redLed!=rled_out) || refreshAll)          //refresh red led display
   {
    redLed = rled_out;
    ledLevel = LedToPercent(rled_out);
    oldval = "Red:    " + String(ledLevel) + "%  " + "  ";
    char bufferR[13];
    oldval.toCharArray(bufferR, 13);
    t= 136 - rled_out*.39;
      
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(57, t, 69, 36);              //hide end of last bar
    myGLCD.setColor(255, 0, 0);
    setFont(SMALL, 255, 0, 0, 0, 0, 0);
    myGLCD.print(bufferR, 45, 159);              //display red LEDs % output
    myGLCD.drawRect(57, 136, 69, 135);           //red %bar place holder
    myGLCD.fillRect(57, 136, 69, t);             //red percentage bar
   }    
        
  if ((whiteLed!=wled_out) || refreshAll)        //refresh white led display
   {
    whiteLed = wled_out;
    ledLevel = LedToPercent(wled_out);
    oldval = "White:  " + String(ledLevel) + "%  " + "  ";
    char bufferW[13];
    oldval.toCharArray(bufferW, 13);
    t= 136 - wled_out*.39;
      
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(74, t, 86, 36);              //hide end of last bar
    myGLCD.setColor(255, 255, 255);
    setFont(SMALL, 255, 255, 255, 0, 0, 0);
    myGLCD.print(bufferW, 45, 171);              //display white LEDs % output
    myGLCD.drawRect(74, 136, 86, 135);           //white %bar place holder
    myGLCD.fillRect(74, 136, 86, t);             //white percentage bar
   }
     
  if ((blueLed!=bled_out) || refreshAll)         //refresh blue led displays
   {
    blueLed = bled_out;
    ledLevel = LedToPercent(bled_out);
    oldval = "Blue:   " + String(ledLevel) + "%" + "  ";
    char bufferB[13];
    oldval.toCharArray(bufferB, 13);
    t= 136 - bled_out*.39;
     
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(91, t, 103, 36);             //hide end of last bar 
    myGLCD.setColor(9, 184, 255);
    setFont(SMALL, 99, 184, 255, 0, 0, 0);
    myGLCD.print(bufferB, 45, 183);              //display blue LEDs % output
    myGLCD.drawRect(91, 136, 103, 135);          //blue %bar place holder
    myGLCD.fillRect(91, 136, 103, t);            //blue percentage bar
   }

  if ((rblueLed!=rbled_out) || refreshAll)       //refresh royal blue led display
   {
    rblueLed = rbled_out;
    ledLevel = LedToPercent(rbled_out);
    oldval = "Royal:  " + String(ledLevel) + "%  " + "  ";
    char bufferRB[13];
    oldval.toCharArray(bufferRB, 13);
    t= 136 - rbled_out*.39;
      
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(108, t, 120, 36);            //hide end of last bar
    myGLCD.setColor(58, 95, 205);
    setFont(SMALL, 58, 95, 205, 0, 0, 0);
    myGLCD.print(bufferRB, 45, 195);             //display royal blue LEDs % output
    myGLCD.drawRect(108, 136, 120, 135);         //royal %bar place holder
    myGLCD.fillRect(108, 136, 120, t);           //royal percentage bar
   }
    
  if ((uvLed!=uvled_out) || refreshAll)          //refresh UV led display
   {
    uvLed = uvled_out;
    ledLevel = LedToPercent(uvled_out);
    oldval = "Ultra:  " + String(ledLevel) + "%  " + "  ";
    char bufferUV[13];
    oldval.toCharArray(bufferUV, 13);
    t= 136 - uvled_out*.39;
      
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(125, t, 137, 36);            //hide end of last bar
    myGLCD.setColor(224, 102, 255);
    setFont(SMALL, 224, 102, 255, 0, 0, 0);
    myGLCD.print(bufferUV, 45, 207);             //display UV LEDs % output
    myGLCD.drawRect(125, 136, 137, 135);         //UV %bar place holder
    myGLCD.fillRect(125, 136, 137, t);           //UV percentage bar
   }

  if (setTempScale==1) {deg ="F";}               //Print deg C or deg F
    else {deg = "C";}
  degC_F=deg;
  char bufferDeg[2];
  degC_F.toCharArray(bufferDeg,2);
                                  
  if (refreshAll)                                //draw static elements
   {
    setFont(SMALL, 0, 255, 0, 0, 0 , 0);
    myGLCD.print("Water Temp:", 169, 148);
    myGLCD.drawCircle(304, 150, 1);              
    myGLCD.print(bufferDeg, 309, 148);

    myGLCD.print("Hood Temp:", 169 , 161);
    myGLCD.drawCircle(304, 163, 1);              
    myGLCD.print(bufferDeg, 309, 161);
     
    myGLCD.print("Sump Temp:", 169, 174);
    myGLCD.drawCircle(304, 176, 1);              
    myGLCD.print(bufferDeg, 309, 174); 

    myGLCD.print("pH:", 169, 187);
   // myGLCD.drawCircle(304, 189, 1);              
   //myGLCD.print(bufferDeg, 309, 187);

    //myGLCD.print("Fanspeed:", 169, 200);
    //myGLCD.drawCircle(304, 202, 1);              
   // myGLCD.print(bufferDeg, 309, 200);

   }
  
  myGLCD.setColor(0, 0, 0);                      //clear cooler/heater & alarm notices
  myGLCD.fillRect(200, 189, 303, 203);
  myGLCD.fillRect(182, 203, 315, 221);

  if ((tempW>50) || (tempW<10))                  //range in deg C no matter what
    { setFont(SMALL, 255, 0, 0, 0, 0, 0);
      myGLCD.print("Error", 260, 148);}
  else 
    {
     if (setTempScale==1) 
       { tempW = ((tempW*1.8) + 32.05);}         //C to F with rounding
     if (tempCoolflag==true)                     //Water temperature too HIGH
       {
        setFont(SMALL, 255, 0, 0, 0, 0, 0);
        myGLCD.printNumF( tempW, 1, 260, 148);
        setFont(SMALL, 255, 255, 0, 0, 0, 0);
        myGLCD.print("Chiller ON", 200, 191);}
        else 
          if (tempHeatflag==true)                      //Water temperature too LOW
            { setFont(SMALL, 0, 0, 255, 0, 0, 0);
              myGLCD.printNumF( tempW, 1, 260, 148);
              setFont(SMALL, 255, 255, 0, 0, 0, 0);
              myGLCD.print("Heater ON", 203, 191);}
          else 
            { setFont(SMALL, 0, 255, 0, 0, 0, 0);
              myGLCD.printNumF( tempW, 1, 260, 148);} 
     if ((tempW<100) && (tempW>=0))
       { myGLCD.setColor(0, 0, 0);
         myGLCD.fillRect(292, 148, 300, 160);}
    }          
     
  if ((tempH>50) || (tempH<10))
    { setFont(SMALL, 255, 0, 0, 0, 0, 0);
      myGLCD.print("Error", 260, 161);}
  else
    {
     if (setTempScale==1) 
       { tempH = ((tempH*1.8) + 32.05);}      
     setFont(SMALL, 0, 255, 0, 0, 0, 0);
     myGLCD.printNumF( tempH, 1, 260, 161);            //Hood temperature (No Flags)
     if ((tempH<100) && (tempH>=0)) 
       { myGLCD.setColor(0, 0, 0);
         myGLCD.fillRect(292, 161, 300, 173);}
    }     
         
  if ((tempS>50) || (tempS<10))
    { setFont(SMALL, 255, 0, 0, 0, 0, 0);
      myGLCD.print("Error", 260, 174);}
  else
    {
     if (setTempScale==1) 
       { tempS = ((tempS*1.8) + 32.05); }
     setFont(SMALL, 0, 255, 0, 0, 0, 0);
     myGLCD.printNumF( tempS, 1, 260, 174);            //Sump temperature (No Flags)         
     if ((tempS<100) && (tempS>=0))
       { myGLCD.setColor(0, 0, 0);
         myGLCD.fillRect(292, 174, 300, 186);}
    }
   
  //write ph and fanspeed to lcd here
  
  if ((tempAlarmflag==true)&&(tempHeatflag==true))     //Alarm: H20 temp Below offsets
    { setFont(LARGE, 0, 0, 255, 0, 0, 0);
      myGLCD.print("ALARM!!", 185, 204);}    
  if ((tempAlarmflag==true)&&(tempCoolflag==true))     //Alarm: H20 temp Above offsets
    { setFont(LARGE, 255, 0, 0, 0, 0, 0);
      myGLCD.print("ALARM!!", 185, 204);}
}

void screenReturn()                                    //Auto Return to MainScreen()
{
  if (SCREEN_RETURN==true)
  {
    if (dispScreen!=0)
    {
      if (myTouch.dataAvailable())
      { processMyTouch();} 
      else { returnTimer++; }
      if (returnTimer>setReturnTimer) 
      {
        returnTimer=0;
        LEDtestTick = false;       
        colorLEDtest = false;           
   //     ReadFromEEPROM();
        dispScreen=0;
        clearScreen();
        mainScreen(true);
      }
    }
  }
}

/******************************** END OF MAIN SCREEN **********************************/



int LedToPercent (int Led_out)                   //returns LED output in %
{
  int result;
 
  if (Led_out==0) {
    result = 0; }
  else {
    result = map(Led_out, 1, 255, 1, 100);  }
  
  return result; 
}


void drawBarGraph()
{
  myGLCD.setColor(255, 255, 255);                //LED Chart
  setFont(SMALL, 255, 255, 255, 0, 0, 0);     
   
  myGLCD.drawRect(30, 137, 148, 138);            //x-line
  myGLCD.drawRect(30, 137, 31, 36);              //y-line

  for (int i=0; i<5; i++)                        //tick-marks
    { myGLCD.drawLine(31, (i*20)+36, 35, (i*20)+36); }  
  myGLCD.print("100", 4, 30);  
  myGLCD.print("80", 12, 50);       
  myGLCD.print("60", 12, 70);
  myGLCD.print("40", 12, 90); 
  myGLCD.print("20", 12, 110);
  myGLCD.print("0", 20, 130);
  
  myGLCD.setColor(0, 150, 0);
  myGLCD.drawRect(40, 136, 52, 135);             //SUMP %bar place holder
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRect(57, 136, 69, 135);             //red %bar place holder
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect(74, 136, 86, 135);             //white %bar place holder
  myGLCD.setColor(9, 184, 255);
  myGLCD.drawRect(91, 136, 103, 135);            //blue %bar place holder
  myGLCD.setColor(58, 95, 205);
  myGLCD.drawRect(108, 136, 120, 135);           //royal %bar place holder
  myGLCD.setColor(224, 102, 255);  
  myGLCD.drawRect(125, 136, 137, 135);           //UV %bar place holder    
}
  

void drawBarandColorValue()
{
  colorled_out = CL_100 + CL_10 + CL_1;
  setFont(SMALL, Rback, Gback, Bback, Rback, Gback, Bback);
  myGLCD.print("    ", xValue, yValue);                  //fill over old   
  setFont(SMALL, Rfont, Gfont, Bfont, Rback, Gback, Bback);
  myGLCD.printNumI(colorled_out, xValue, yValue);
                           
  yBar = 136 - colorled_out*.39;
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(x1Bar, yBar, x2Bar, 36);       //hide end of last bar
  myGLCD.setColor(Rgad, Ggad, Bgad);
  myGLCD.fillRect(x1Bar, 136, x2Bar, yBar);      //color percentage bar
  if (COLOR==SUMP) {scol_out=colorled_out;}
  if (COLOR==RED) {rcol_out=colorled_out;}
  if (COLOR==WHITE) {wcol_out=colorled_out;}
  if (COLOR==BLUE) {bcol_out=colorled_out;}
  if (COLOR==ROYAL) {rbcol_out=colorled_out;}
  if (COLOR==ULTRA) {uvcol_out=colorled_out;}
  if (COLOR==MOON) {mooncol_out=colorled_out;}
}


void ledChangerGadget()
{
  myGLCD.setColor(Rgad, Ggad, Bgad);
  myGLCD.fillRoundRect(199, 25, 285, 132);       //Gadget Color
  myGLCD.setColor(0, 0, 0); 
  myGLCD.fillRect(204, 50, 280, 74);             //Black Background of Numbers
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(199, 25, 285, 132);       //Outline Gadget
  myGLCD.setColor(Rline, Gline, Bline);          //Line Color
  myGLCD.drawLine(199, 46, 285, 46);
  setFont(LARGE, Rfont, Gfont, Bfont, Rback, Gback, Bback); 
  if (COLOR==0){
    myGLCD.print("COLOR", 202, 28);} 
  if (COLOR==1){
    myGLCD.print("WHITE", 202, 28);} 
  if (COLOR==2){
    myGLCD.print("BLUE", 210, 28);} 
  if (COLOR==3){
    myGLCD.print("ROYAL", 202, 28);}    
  if (COLOR==4){
    myGLCD.print("RED", 218, 28);} 
  if (COLOR==5){
    myGLCD.print("ULTRA", 202, 28);} 
  if (COLOR==6){
    myGLCD.print("SUMP", 210, 28);} 
  if (COLOR==7){
    myGLCD.print("LUNAR", 202, 28);} 
  for (int b=0; b<3; b++)
    {
     printLedChangerP("+", (b*27)+204, 78, (b*27)+226, 100, LARGE);  //Press Increase Number
     printLedChangerM("-", (b*27)+204, 105, (b*27)+226, 127, LARGE); //Press Decrease Number
    }
  for (int c=0; c<3; c++)
    {
     myGLCD.setColor(Rline, Gline, Bline);
     myGLCD.drawRect((c*27)+204, 78, (c*27)+226, 100);
     myGLCD.drawRect((c*27)+204, 105, (c*27)+226, 127);
    }  
  setFont(LARGE, 255, 255, 255, 0, 0, 0);
  myGLCD.printNumI(cl_100, 214, 54);
  myGLCD.printNumI(cl_10, 234, 54);
  myGLCD.printNumI(cl_1, 255, 54);  
}


void TimeSaver(boolean refreshAll=false)
{
  if (setTimeFormat==0)                                 //24HR Format
    {
     myGLCD.setColor(0, 0, 255);
     myGLCD.setBackColor(0, 0, 0);  
     myGLCD.setFont(SevenSegNumFont);
     if ((mytime.hour>=0) && (mytime.hour<=9))                    //Display HOUR
       { myGLCD.setColor(0, 0, 0);
         myGLCD.fillRect(80, 95, 111, 145);
         myGLCD.setColor(0, 0, 255);
         myGLCD.printNumI(mytime.hour, 112, 95);}
     else { myGLCD.printNumI(mytime.hour, 80, 95);}
    }    

  if (setTimeFormat==1)                                 //12HR Format
    {
     myGLCD.setColor(0, 0, 255);
     myGLCD.setBackColor(0, 0, 0);  
     myGLCD.setFont(SevenSegNumFont);
     if (mytime.hour==0)                                     //Display HOUR
       { myGLCD.print("12", 80, 95);}
     if ((mytime.hour=1) && (mytime.hour<=9))
       { myGLCD.setColor(0, 0, 0);
         myGLCD.fillRect(80, 95, 111, 145);
         myGLCD.setColor(0, 0, 255);
         myGLCD.printNumI(mytime.hour, 112, 95);}
     if ((mytime.hour>=10) && (mytime.hour<=12))
       { myGLCD.printNumI(mytime.hour, 80, 95);}
     if ((mytime.hour>=13) && (mytime.hour<=21))
       { myGLCD.setColor(0, 0, 0);
         myGLCD.fillRect(80, 95, 111, 145);
         myGLCD.setColor(0, 0, 255);
         myGLCD.printNumI(mytime.hour-12, 112, 95);}
     if (mytime.hour>=22)
       { 
       myGLCD.printNumI(mytime.hour-12, 80, 95);
       }
      
     if ((mytime.hour>=0) && (mytime.hour<=11))                   //Display AM/PM
       { setFont(LARGE, 0, 0, 255, 0, 0, 0);
         myGLCD.print("AM", 244, 129);}
     else
       { setFont(LARGE, 0, 0, 255, 0, 0, 0);
         myGLCD.print("PM", 244, 129);} 
    }

  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);  
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.fillCircle(160, 108, 4);
  myGLCD.fillCircle(160, 132, 4);  
  if ((mytime.min>=0) && (mytime.min<=9))                       //Display MINUTES
    { myGLCD.print("0", 176, 95);
      myGLCD.printNumI(mytime.min, 208, 95);}
  else { myGLCD.printNumI(mytime.min, 176, 95);} 
}


void screenSaver()                               //Make the Screen Go Blank after so long
{
  if ((setScreensaver==1) && (tempAlarmflag==false))
    {
     if (myTouch.dataAvailable())
       { processMyTouch();} 
     else { screenSaverTimer++;}
     if (screenSaverTimer==setScreenSaverTimer) 
       { dispScreen=0;
         DEBUG_PRINTLN("Screen saver on");
         myGLCD.clrScr(); }
     if (CLOCK_SCREENSAVER==true)
       { if (screenSaverTimer>setScreenSaverTimer) 
           { dispScreen=0;
             TimeSaver(true); }
       } 
    }
}



void genSetSelect()
{
  if (setCalendarFormat==0)                      //Calendar Format Buttons
    { myGLCD.setColor(0, 255, 0);
      myGLCD.fillRoundRect(185, 19, 305, 39);
      setFont(SMALL, 0, 0, 0, 0, 255, 0);  
      myGLCD.print("DD/MM/YYYY", 207, 23);
      myGLCD.setColor(0, 0, 255);
      myGLCD.fillRoundRect(185, 45, 305, 65);    
      setFont(SMALL, 255, 255, 255, 0, 0, 255);  
      myGLCD.print("MTH DD, YYYY", 199, 49);   
    }
    else 
      { myGLCD.setColor(0, 0, 255);
        myGLCD.fillRoundRect(185, 19, 305, 39);
        setFont(SMALL, 255, 255, 255, 0, 0, 255);  
        myGLCD.print("DD/MM/YYYY", 207, 23);
        myGLCD.setColor(0, 255, 0);
        myGLCD.fillRoundRect(185, 45, 305, 65);    
        setFont(SMALL, 0, 0, 0, 0, 255, 0);  
        myGLCD.print("MTH DD, YYYY", 199, 49);   
      }
  if (setTimeFormat==0)                          //Time Format Buttons
    { myGLCD.setColor(0, 0, 255);
      myGLCD.fillRoundRect(195, 76, 235, 96);
      setFont(SMALL, 255, 255, 255, 0, 0, 255);  
      myGLCD.print("12HR", 201, 80);    
      myGLCD.setColor(0, 255, 0);
      myGLCD.fillRoundRect(255, 76, 295, 96);      
      setFont(SMALL, 0, 0, 0, 0, 255, 0);  
      myGLCD.print("24HR", 261, 80);   
    }
    else 
      { myGLCD.setColor(0, 255, 0);
        myGLCD.fillRoundRect(195, 76, 235, 96);
        setFont(SMALL, 0, 0, 0, 0, 255, 0);  
        myGLCD.print("12HR", 201, 80);    
        myGLCD.setColor(0, 0, 255);
        myGLCD.fillRoundRect(255, 76, 295, 96);    
        setFont(SMALL, 255, 255, 255, 0, 0, 255);  
        myGLCD.print("24HR", 261, 80); 
      }
  if (setTempScale==0)                           //Temperature Scale Buttons
    { myGLCD.setColor(0, 255, 0);
      myGLCD.fillRoundRect(195, 107, 235, 127);
      setFont(SMALL, 0, 0, 0, 0, 255, 0);  
      myGLCD.print("C", 215, 111);   
      myGLCD.setColor(0, 0, 255);
      myGLCD.fillRoundRect(255, 107, 295, 127);    
      setFont(SMALL, 255, 255, 255, 0, 0, 255);  
      myGLCD.print("F", 275, 111);
      myGLCD.setColor(0, 0, 0);   
      myGLCD.drawCircle(210, 113, 1);              
      myGLCD.setColor(255, 255, 255);
      myGLCD.drawCircle(270, 113, 1);
    }
    else 
      { myGLCD.setColor(0, 0, 255);
        myGLCD.fillRoundRect(195, 107, 235, 127);
        setFont(SMALL, 255, 255, 255, 0, 0, 255);  
        myGLCD.print("C", 215, 111);    
        myGLCD.setColor(0, 255, 0);
        myGLCD.fillRoundRect(255, 107, 295, 127);      
        setFont(SMALL, 0, 0, 0, 0, 255, 0);  
        myGLCD.print("F", 275, 111);
        myGLCD.setColor(255, 255, 255);
        myGLCD.drawCircle(210, 113, 1);              
        myGLCD.setColor(0, 0, 0);  
        myGLCD.drawCircle(270, 113, 1);
      }  
  if (setScreensaver==1)                         //Screensaver Buttons
    { myGLCD.setColor(0, 255, 0);
      myGLCD.fillRoundRect(195, 138, 235, 158);
      setFont(SMALL, 0, 0, 0, 0, 255, 0);  
      myGLCD.print("ON", 209, 142);    
      myGLCD.setColor(0, 0, 255);
      myGLCD.fillRoundRect(255, 138, 295, 158);    
      setFont(SMALL, 255, 255, 255, 0, 0, 255);  
      myGLCD.print("OFF", 265, 142); 
    }
    else 
      { myGLCD.setColor(0, 0, 255);
        myGLCD.fillRoundRect(195, 138, 235, 158);
        setFont(SMALL, 255, 255, 255, 0, 0, 255);  
        myGLCD.print("ON", 209, 142);    
        myGLCD.setColor(0, 255, 0);
        myGLCD.fillRoundRect(255, 138, 295, 158);      
        setFont(SMALL, 0, 0, 0, 0, 255, 0);  
        myGLCD.print("OFF", 265, 142); 
     }  
}    


//--------------------------------------------------------------------------------------------------------------------
//                                                          SETUP
//--------------------------------------------------------------------------------------------------------------------

void setup()
{
  Serial.begin(57600);
  DEBUG_PRINTLN("Setup");

  pinMode(TX_led, OUTPUT);								//Setup Data TX Led pin as output
  
  DEBUG_PRINT("Free Ram is: ");
  DEBUG_PRINTLN(freeRam());
  DEBUG_PRINTLN("Setup Ethernet");
  
 //Setup the LCD
  byte bout = map(LCDbright, 0, 100, 0, 255);          //Set LCD brightness
  analogWrite(LCDbrightPin, bout);
  
  myGLCD.InitLCD();										//Init LCD Screeb
  myGLCD.setFont(SmallFont);
  myGLCD.clrScr();
  
  if (UseEthernet = true)
     {
       printHeader("Initializing ethernet - please wait...");
     }
  
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  
  myButtons.setTextFont(BigFont);
  myButtons.setSymbolFont(Dingbats1_XL);
  
  delay(500);  //Give everything a chance to settle

   lastNTPUpdate = millis();
  
  
  if (ether.begin(sizeof Ethernet::buffer, mymac,8 ) == 0) 
    DEBUG_PRINTLN( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    {
    DEBUG_PRINTLN("DHCP failed");
    myGLCD.clrScr();
    printHeader("Failed to get DHCP"); 
    while(1)                              // Coulndt get DHCP - stop!
      {}
    }
    
  ether.printIp("IP:  ", ether.myip);
  //ether.printIp("GW:  ", ether.gwip);  
  //ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    ether.printIp("SRV: ", ether.hisip);
    DEBUG_PRINTLN("Ethernet Okay");

     // Start up the library
    sensors.begin();
    // set the resolution to 10 bit (good enough?)
    sensors.setResolution(WaterThermometer, 10);
    sensors.setResolution(HeatsinkThermometer, 10);
    sensors.setResolution(SumpThermometer, 10);

    myGLCD.setColor(64, 64, 64);
    myGLCD.fillRect(0, 226, 319, 239);                     //Bottom Bar
    min_cnt= (mytime.hour*60)+mytime.min;
    ReadFromEEPROM();
    LED_levels_output();
    mainScreen(true);
    
    // Set the clock to run-mode
  rtc.halt(false);
  
  wdt_enable(WDTO_8S);										//Enable wdt for 8 secs in case code crashes
  
}

//----------------------------------------------------------------------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------------------------------------------------------------------

void loop()
{  
    wdt_reset();
    ether.packetLoop(ether.packetReceive());  

if ((myTouch.dataAvailable()) && (screenSaverTimer>=setScreenSaverTimer))  
    {
     LEDtestTick = false;
     myGLCD.setColor(64, 64, 64);
     myGLCD.fillRect(0, 226, 319, 239);                     //Bottom Bar    
     screenSaverTimer=0;
     clearScreen();     
     mainScreen(true);
     delay(1000);
     dispScreen=0;
    } 
  else 
    {
     if (myTouch.dataAvailable())  
       { 
         DEBUG_PRINTLN("Loc1");
       processMyTouch();
     }
    }


unsigned long currentMillis = millis();
  if (currentMillis - previousMillisFive > 5000)   //check time, temp and LED levels every 5s
    {
      previousMillisFive = currentMillis;  
     // RTC.get(rtc,true);
     mytime = rtc.getTime();
      
      //feedingTimeOutput();
      
      if (screenSaverTimer<setScreenSaverTimer)
        { TimeDateBar(); }
      
      checkTempC();
   //   OCR5A = 16000000.0 / (2*25000.0);            //25kHz PWM - above our audible range so fans are quiet
   //   OCR5B = 16000000.0 / (2*25000.0) * SumpPWM;  //"SumpPWM" is the % duty cycle for pin 45
   //   OCR5C = 16000000.0 / (2*25000.0) * HoodPWM;  //"HoodPWM" is the % duty cycle for pin 44
      
      min_cnt= (mytime.hour*60)+mytime.min;
      LED_levels_output();

      screenReturn();
      screenSaver();

      if ((dispScreen == 0) && (screenSaverTimer<setScreenSaverTimer)) 
        { mainScreen();}
    }
    
    
  const unsigned long oneMinute = 60 * 1000UL;
  static unsigned long lastSampleTime = 0 - oneMinute;
  unsigned long now = millis();
  

  if (now - lastSampleTime >= oneMinute)    //Upload data to Thingspeak every minute
    {
     //DEBUG_PRINT("freeMemory()=");
     //DEBUG_PRINTLN(availableMemory());
     lastSampleTime += oneMinute;
     DEBUG_PRINTLN("Transmit Data");
     TransmitData();
    // DEBUG_PRINTLN("Transmit data complete");
   }
 
	
      if ((lastNTPUpdate + 15*60000L < millis()) and (UseEthernet = true)) 
   
    {                      //Check NTP Time every 15 mins
       DEBUG_PRINTLN("Get NTP");
       getNTP();
       DEBUG_PRINTLN("Got NTP");
    }
 }
  
