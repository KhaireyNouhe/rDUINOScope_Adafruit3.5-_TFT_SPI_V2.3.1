#include <Nextion.h>  // Include the nextion library (the official one) https://github.com/itead/ITEADLIB_Arduino_Nextion
// Make sure you edit the NexConfig.h file on the library folder to set the correct serial port for the display.
// By default it's set to Serial1, which most arduino boards don't have.
// Change "#define nexSerial Serial1" to "#define nexSerial Serial" if you are using arduino uno, nano, etc.


int CurrentPage = 0;  // Create a variable to store which page is currently loaded

// Declare objects that we are going to read from the display. This includes buttons, sliders, text boxes, etc:
// Format: <type of object> <object name> = <type of object>(<page id>, <object id>, "<object name>");
/* ***** Types of objects:
   NexButton - Button
   NexDSButton - Dual-state Button
   NexHotspot - Hotspot, that is like an invisible button
   NexCheckbox - Checkbox
   NexRadio - "Radio" checkbox, that it's exactly like the checkbox but with a rounded shape
   NexSlider - Slider
   NexGauge - Gauge
   NexProgressBar - Progress Bar
   NexText - Text box
   NexScrolltext - Scroll text box
   NexNumber - Number box
   NexVariable - Variable inside the nextion display
   NexPage - Page touch event
   NexGpio - To use the Expansion Board add-on for Enhanced Nextion displays
   NexRtc - To use the real time clock for Enhanced Nextion displays
 * *****
*/

// Declare pages:

NexPage Boot = NexPage(0, 0, "Boot");  // Page added as a touch event
NexPage GPS = NexPage(1, 0, "GPS");  // Page added as a touch event

// Declare touch event objects to the touch event list:
// You just need to add the names of the objects that send a touch event.
// Format: &<object name>,

NexTouch *nex_listen_list[] =
{

  &Boot,  // Page added as a touch event
  &GPS,  // Page added as a touch event
  NULL  // String terminated
};  // End of touch event list

//----------------------------------------------
#define reverse_logic false //set true if the stepper drivers logic is "neglected enabled"
//#define serial_debug       // comment out to deactivate the serial debug mode

#define use_battery_level

// HERE GOES THE Mount, Gears and Drive information.
// ... used to calculate the HourAngle to microSteps ratio
// UPDATE THIS PART according to your SET-UP
// ---------------------------------------------
// NB: RA and DEC uses the same gear ratio (144 tooth in my case)!
//----------------------------------------------
#ifdef serial_debug
  int WORM = 144;
#else
  int WORM = 144;
#endif
int REDUCTOR = 4;      // 1:4 gear reduction
int DRIVE_STP = 200;   // Stepper drive have 200 steps per revolution
int MICROSteps = 16;   // I'll use 1/16 microsteps mode to drive sidereal - also determines the LOWEST speed.

// below variables are used to calculate the paramters where the drive works
#define ARCSEC_F_ROTAT 1296000   // ArcSeconds in a Full earth rotation;
#define SIDEREAL_DAY 86164.0905   // Sidereal day in seconds
float ArcSECstep;
int MicroSteps_360;
int RA_90;  // How much in microSteps the RA motor have to turn in order to make 6h = 90 degrees;
int DEC_90;   // How mich in microSteps the DEC motor have to turn in order to make 6h = 90 degrees;
int HA_H_CONST;
int HA_M_CONST;
int DEC_D_CONST;
int DEC_M_CONST;
int MIN_TO_MERIDIAN_FLIP = 2;   // This constant tells the system when to do the Meridian Flip. "= 2" means 2 minutes before 24:00h (e.g. 23:58h)
int MIN_SOUND_BEFORE_FLIP = 3;   // This constant tells the system to start Sound before it makes Meridian Flip
float mer_flp;                   // The calculateLST_HA() function depending on this timer will convert the HA and DEC to the propper ones to do the flip.
boolean MERIDIAN_FLIP_DO = false;
int Tracking_type = 1;  // 1: Sidereal, 2: Solar, 0: Lunar;
int Clock_Sidereal;  // Variable for the Interruptions. nterruption is initialized depending on the DATA above -in miliseconds
int Clock_Solar;  // Variable for the Interruptions. nterruption is initialized depending on the DATA above -in miliseconds
int Clock_Lunar;  // Variable for the Interruptions. nterruption is initialized depending on the DATA above -in miliseconds

////////////////////////////////////////////////
#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_HX8357.h"
#include "DHT.h"
#include <TinyGPS++.h>
#include <Time.h>
#include "TouchScreen.h"
#include <SD.h>
#include <DueTimer.h> // interruptions library0
#include <DS3231.h>
#include <math.h>
#include <TimeLib.h>
#include "defines.h"

// These are the four touchscreen analog pins
#define YP A10  // Y+ to A10   >>> must be an analog pin
#define XM A11  // X- to A11   >>> must be an analog pin
#define YM 44   // Y- to 44    >>> a digital pin
#define XP 42   // X+ to 42    >>> a digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 120  // touch Left margin  >>> Default value is 110 
#define TS_MINY 90   // touch up margin    >>> Default value is 80
#define TS_MAXX 850  // touch Right margin >>> Default value is 900  
#define TS_MAXY 930  // touch Down margin  >>> Default value is 940 

#define MINPRESSURE 5    // Minimum Pressure Touch Sensitivity >>> Default value is 10
#define MAXPRESSURE 1000 // Maximum Pressure Touch Sensitivity >>> Default value is 1000

// The display uses hardware SPI, plus #43 & #45
#define TFT_RST -1  // tie to arduino SPI RST 
#define TFT_DC 3    // khairey changed it from 43  to   3
#define TFT_CS 52

int sd_cs = 21;  // Khairey moved it from 4 to 41  // sd card chip select pin is Arduino digital pin 4
#define DHTPIN 43   // khairey changed it from  3  to   43
#define DHTTYPE DHT22




DS3231 rtc(A4, A5);           // (SDA-A4, SCL-A5) from the RTC board
DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;             // (Rx-16, Tx-17)

/** NIGHT MODE ANALOG pin */
#define DAY_NIGHT_PIN A6

//Joystick Pins//
int yPin = A0;
int xPin = A1;
int Joy_SW = A9;
int x_cal, y_cal = 0;

int FAN1 = A2;
int FAN2 = A3;

int speakerOut = DAC1;  // Was 2
int TFTBright = DAC0;

int POWER_DRV8825 = A8;

int RA_DIR = 5;
int RA_STP = 4;
int RA_MODE2 = 13;
int RA_MODE1 = 12;
int RA_MODE0 = 11;

int DEC_DIR = 7;
int DEC_STP = 6;
int DEC_MODE2 = 8;
int DEC_MODE1 = 9;
int DEC_MODE0 = 10;

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

const String FirmwareDate = "16 11 18";
const String FirmwareNumber = "v2.3.1 Boiana-EQ-Fayrouz-Solar_Sys";
const String FirmwareName = "rDUINOScope";
const String FirmwareTime = "12:00:00";
// Variables:
String Messier_Array[112];
String Treasure_Array[130];
String custom_Array[100];
String ObservedObjects[50];
String Iter_Stars[50];
int int_star_count = 0;       // Counter for how many stars are loaded into Iter_Stars[] array.... to be used with Pages, so that it does not show more pages than needed

int Observed_Obj_Count = 0;
int ALLIGN_STEP = 0;  // Using this variable to count the allignment steps - 1: Synchronize, 2: Allign and Center, 3:....
int ALLIGN_TYPE = 0;  // Variable to store the alignment type (0-Skip Alignment, 1-1 Star alignment, 2-2 Star alignment
float delta_a_RA = 0;
float delta_a_DEC = 0;
int Iterative_Star_Index = 0;
String Prev_Obj_Start;
int lastScreenUpdateTimer;
unsigned long Slew_timer, Slew_RA_timer = 0;
int OBJECT_Index;
String OBJECT_NAME;
String OBJECT_DESCR;
String OBJECT_DETAILS;
String BT_COMMAND_STR;
String START_TIME;
int STP_FWD = LOW;
int STP_BACK = HIGH;
float OBJECT_RA_H;
float OBJECT_RA_M;
float OBJECT_DEC_D;
float OBJECT_DEC_M;
float OBJECT_MAG;
float curr_RA_H, curr_RA_M, curr_RA_S, curr_DEC_D, curr_DEC_M, curr_DEC_S;    // Global variables to store Mount's current RA and DEC.
char curr_RA_lz[9], curr_DEC_lz[10], curr_HA_lz[9];                                                // Global variable to store current RA and DEC with Leading Zeroes and sign (RA: 00:00:00; DEC: +/-00*00:00)
int SELECTED_STAR = 0;
double DELTA_RA_ADJUST = 1; // cos RA
double DELTA_DEC_ADJUST = 1; // cos DEC
// Default values to load when CANCEL button is hit on the GPS screen
float OBSERVATION_LONGITUDE = 31.0004; // (23.3333* - Home)
float OBSERVATION_LATTITUDE = 30.7865; // (42.6378* - Home)
float OBSERVATION_ALTITUDE = 11.00; // Tanta, Egypt
int TIME_ZONE = 2;
// .............................................................
int GPS_iterrations = 0;
double LST, HAHour, HAMin, ALT, AZ;
double JD;
String BTs;

int last_button, MESS_PAGER, TREAS_PAGER, STARS_PAGER, CUSTOM_PAGER;
boolean IS_TFT_ON = true;
boolean IS_STEPPERS_ON = true;
boolean IS_OBJ_VISIBLE = false;
boolean IS_IN_OPERATION = false;  // This variable becomes True when Main screen appears
boolean IS_TRACKING = false;
boolean IS_NIGHTMODE;
boolean IS_OBJ_FOUND = true;
boolean IS_OBJECT_RA_FOUND = true;
boolean IS_OBJECT_DEC_FOUND = true;
boolean IS_MERIDIAN_PASSED = false;
boolean IS_POSIBLE_MERIDIAN_FLIP = true;
boolean IS_MERIDIAN_FLIP_AUTOMATIC = true;
boolean IS_BT_MODE_ON = false;
boolean IS_MANUAL_MOVE = false;
boolean IS_FAN1_ON = true;
boolean IS_FAN2_ON = true;
boolean IS_CUSTOM_MAP_SELECTED = false;
boolean IS_SOUND_ON = true;
int TFT_Brightness = 255;
int MAIN_SCREEN_MENU = 0;
int CURRENT_SCREEN = 0;
int LOAD_SELECTOR;   // selector to show which LOADING mechanism is used: 1 - Messier, 2 - File, 3 - NGCs
boolean TRACKING_MOON;
boolean sun_confirm = false;

String Fan1_State = "ON";
String Fan2_State = "ON";
String TFT_Time = "AL-ON";
String Sound_State = "ON";
String Stepper_State = "ON";
String Mer_Flip_State = "Auto";
String Tracking_Mode = "Celest";

int RA_microSteps, DEC_microSteps, rev_RA_microSteps, rev_DEC_microSteps;              // Current position of the motors in MicroSteps! - when movement occures, values are changed accordingly (manual, tracking or slew to);
int RA_mode_steps, DEC_mode_steps;
int SLEW_RA_microsteps, SLEW_DEC_microsteps;    // Where the mottors needs to go in order to point to the object
int RA_finish_last = 0;
int map_r = 0;    // Used to determine the StarMap Row ... image name (1-1.bmp; 1-2.bmp ....)
int map_c = 0;    // Ued to determine the StarMap Column .... image name


String old_t, old_d;
String Start_date;
int update_time, Tupdate_time, TFT_timeout;
unsigned long UPD_T, UPD_coord, DELAY_Slew, UPD_LST, TFT_Timer;
int RA_move_ending;
int w_DateTime[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // array to store date - as updated from updater screen - Wishing_Date
int dateEntryPos = 0;
int Summer_Time = 0;
int xPosition = 0;  // Joystick
int yPosition = 0;  // Joystick
float _temp, _Stemp;
float _humid, _Shumid;
int16_t texts, Button_State_ON, Button_State_OFF, Button_Title, l_text, d_text, btn_l_border, btn_d_border, btn_l_text, btn_d_text, btn_l_selection, title_bg, title_texts, messie_btn, file_btn, ngc_btn, MsgBox_bg, MsgBox_t; // defines string constants for the clor - Depending on the DAY/NIGHT modes
File roots;
File StarMaps;                    // bmp files

// Some variables used for Alignment procedure:
double Star_1_HA = 0;
double Star_1_DEC = 0;
double Star_2_HA = 0;
double Star_2_DEC = 0;
double err_RA = 0;
double err_DEC = 0;
double Eerr_RA = 0;
double Eerr_DEC = 0;
double err_ALT = 0;
double err_AZ = 0;
double det = 0;


void setup(void) {
#ifdef serial_debug
  Serial.begin(57600);
  while (!Serial) {}
#endif

  //////////////////////////// Nextion Setup ////////////////
  Serial1.begin(115200);  // Start serial comunication With Nextion at baud=115200

  // Register the event callback functions of each touch event:
  // You need to register press events and release events seperatly.
  // Format for press events: <object name>.attachPush(<object name>PushCallback);
  // Format for release events: <object name>.attachPop(<object name>PopCallback);

  Boot.attachPush(BootPushCallback);  // Page press event
  GPS.attachPush(GPSPushCallback);  // Page press event
  // End of registering the event callback functions

  ///////////////////////////// End Of Nextion Setup //////////////
  Serial2.begin(9600);  // Initialize GPS communication on PINs: 17 (RX) and 16 (TX)
  Serial3.begin(9600); // Bluetooth communication on PINs:  15 (RX) and 14 (TX)
  pinMode(speakerOut, OUTPUT);

  // below variables are used to calculate the paramters where the drive works
  int ww = WORM * REDUCTOR;
  int www = DRIVE_STP * MICROSteps;

  MicroSteps_360 = ww * www;
  RA_90 = MicroSteps_360 / 4;  // How much in microSteps the RA motor have to turn in order to make 6h = 90 degrees;
  DEC_90 = RA_90;   // How mich in microSteps the DEC motor have to turn in order to make 6h = 90 degrees;
  HA_H_CONST = MicroSteps_360 / 360;
  DEC_D_CONST = HA_H_CONST;

  Clock_Sidereal = 1000000 / (MicroSteps_360 / SIDEREAL_DAY); // This way I make the interruption occuer 2wice faster than needed - REASON: allow max time for Pin-UP, Pin-DOWN action
  Clock_Solar = 1000000 / (MicroSteps_360 / (SIDEREAL_DAY - 235.9095));
  Clock_Lunar = 1000000 / (MicroSteps_360 / (SIDEREAL_DAY - 2089.2292));

#ifdef serial_debug
  Serial.print("Clock_Sidereal = ");
  Serial.println(Clock_Sidereal, 6);
#endif

  analogWrite(TFTBright, 255);  // LCD Backlight brightness 255 = ~100%)

  rtc.begin();
  dht.begin();
  tft.begin(HX8357D);
  tft.setRotation(4);
  tft.fillScreen(BLACK);
  tft.setCursor(15, 10);

  // DRB8825 - drive mode pins (determine Steppping Modes 1/8, 1/16 and etc.
  pinMode(RA_MODE0, OUTPUT);
  pinMode(RA_MODE1, OUTPUT);
  pinMode(RA_MODE2, OUTPUT);

  pinMode(DEC_MODE0, OUTPUT);
  pinMode(DEC_MODE1, OUTPUT);
  pinMode(DEC_MODE2, OUTPUT);

  pinMode(RA_STP, OUTPUT); // RA Step
  pinMode(RA_DIR, OUTPUT); // RA Dir
  pinMode(DEC_STP, OUTPUT); // DEC Step
  pinMode(DEC_DIR, OUTPUT); // DEC Dir

  //digitalWrite(RA_DIR,HIGH); // Set Dir high
  //digitalWrite(RA_STP,LOW);
  //digitalWrite(DEC_DIR,HIGH); // Set Dir high
  //digitalWrite(DEC_STP,LOW);

  // Joystick
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  // Device 1 & 2 command Pins
  pinMode(FAN1, OUTPUT);
  pinMode(FAN2, OUTPUT);

  // Pin to define Power to the DRV8825 using MOSFET!
  pinMode(POWER_DRV8825, OUTPUT);


  // Set RA and DEC microstep position
  RA_microSteps = RA_90; //  --> point to North Sudereal Pole = -180 deg (-12h)
  DEC_microSteps = 0; //  --> Point to North Sudereal Pole = 90 deg

  Timer3.attachInterrupt(Sidereal_rate);
  //  Timer3.start(Clock_Sidereal); // executes the code every 62.329 ms.


  if (analogRead(DAY_NIGHT_PIN) < 800)
  {
    IS_NIGHTMODE = true;
    texts = Maroon;
    l_text = RED;
    d_text = Maroon;
    title_bg = RED;
    title_texts = BLACK;
    messie_btn = Maroon;
    btn_l_text = RED;
    btn_d_text = Maroon;
    btn_l_border = RED;
    btn_d_border = Maroon;
    btn_l_selection = RED;
    MsgBox_bg = RED;
    MsgBox_t = BLACK;
    Button_State_ON = BLACK;
    Button_State_OFF = BLACK;
    Button_Title = BLACK;
  } else {
    IS_NIGHTMODE = false;
    texts = LightGrey;
    l_text = WHITE;
    d_text = LightGrey;
    btn_l_text = GreenYellow;
    btn_d_text = DarkGreen;
    btn_l_border = GREEN;
    btn_d_border = DarkGreen;
    btn_l_selection = DarkGreen;
    title_bg = Orange;
    title_texts = BLACK;
    messie_btn = DarkGrey;
    MsgBox_bg = Purple;
    MsgBox_t = GreenYellow;
    Button_State_ON = DarkGreen;
    Button_State_OFF = RED;
    Button_Title = Purple;
  }


  // Draw initial screen - INITIALIZE
  // The below part cannot be removed form the code
  // You can add messages, but not remove!

  tft.setTextColor(title_bg);
  tft.setTextSize(4);
  tft.println("rDUINO SCope");
  tft.setTextColor(l_text);
  tft.setTextSize(2);
  tft.setCursor(50, 45);
  tft.setTextColor(l_text);
  tft.print("coded by <dEskoG>");
  tft.setCursor(40, 70);
  tft.print("Dessislav Gouzgounov");
  tft.setCursor(4, 90);
  tft.setTextSize(2);
  tft.print("rduinoscope.byethost24.com");
  tft.setCursor(6, 130);
  tft.setTextColor(d_text);
  tft.print("GNU General Public License");
  tft.setCursor(10, 160);
  tft.setTextColor(d_text);
  tft.println("Version: " + FirmwareNumber);

#ifdef serial_debug
  tft.setTextColor(title_bg);
  tft.println(" - DEBUG MODE");
  tft.setTextColor(d_text);
#endif

  tft.setCursor(0, 220);
  tft.setTextSize(1);

  // see if the card is present and can be initialized:
  char in_char;
  String items = "";
  int j = 0;
  int k = 0;
  MESS_PAGER = 0;
  TREAS_PAGER = 0;
  STARS_PAGER = 0;
  CUSTOM_PAGER = 0;
  
  volatile bool check = true;
  tft.print("--> Initializing touchscreen... ");
  Serial1.print("T_initialize.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial1.print("\"");  // Since we are sending text, and not a number, we need to send double quote before and after the actual text.
  Serial1.print("-> Initializing TouchScreen...");  // This is the text you want to send to that object and atribute mentioned before.
  Serial1.print("\"");  // Since we are sending text, and not a number, we need to send double quote before and after the actual text.
  Serial1.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial1.write(0xff);
  Serial1.write(0xff);
  tft.setTextColor(GREEN);
  tft.println("OK");
  Serial1.print("Touch_OK.txt=");
  Serial1.print("\"");
  Serial1.print("OK");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  tft.setTextColor(d_text);

  tft.print("--> Initializing DHT sensor... ");
  Serial1.print("DHT_initialize.txt=");
  Serial1.print("\"");
  Serial1.print("-> Initializing DHT Sensor....");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  if (isnan(dht.readTemperature()) || isnan(dht.readHumidity()))
  {
    tft.setTextColor(RED);
    tft.println("FAIL");
    Serial1.print("DHT_OK.txt=");
    Serial1.print("\"");
    Serial1.print("Fail");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
  }
  else
  {
    tft.setTextColor(GREEN);
    tft.println("OK");
    Serial1.print("DHT_OK.txt=");
    Serial1.print("\"");
    Serial1.print("OK");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
  }

  tft.print("--> Initializing RTC... ");
  Serial1.print("RTC_initialize.txt=");
  Serial1.print("\"");
  Serial1.print("-> Initializing RTC...........");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  int prev_mil = millis();
  if (isnan(rtc.getTemp()))
  {
    tft.setTextColor(RED);
    tft.println("FAIL");
    Serial1.print("RTC_OK.txt=");
    Serial1.print("\"");
    Serial1.print("Fail");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
    check = false;
  }
  else
  {
    tft.setTextColor(GREEN);
    tft.print("OK   ");
    Serial1.print("RTC_OK.txt=");
    Serial1.print("\"");
    Serial1.print("OK");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
#ifdef serial_debug
    tft.println(rtc.getTemp());
#else
    tft.println("");
#endif
    tft.setTextColor(d_text);
  }

  tft.print("--> Initializing SD card... ");
  Serial1.print("SD_initialize.txt=");
  Serial1.print("\"");
  Serial1.print("-> Initializing SD Card.......");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  for (int i = 0; i < 10 && !SD.begin(sd_cs); i++)
  {
    if (i == 9)
    {
      tft.setTextColor(RED);
      tft.println("ERROR: Card failed, or not present\n");
      tft.println("Try formatting the SD card to FAT32 and replace the files.");
      Serial1.print("SD_OK.txt=");
      Serial1.print("\"");
      Serial1.print("Fail");
      Serial1.print("\"");
      Serial1.write(0xff);
      Serial1.write(0xff);
      Serial1.write(0xff);
      tft.setTextColor(d_text);
      check = false;
    }
    delay(50);
  }
  //Debug or card initialized:
  tft.setTextColor(GREEN);
  tft.println("OK");
  Serial1.print("SD_OK.txt=");
  Serial1.print("\"");
  Serial1.print("OK");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  tft.setTextColor(d_text);
  //loadOptions_SD();
  //delay(100);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("messier.csv");

  // if the file is available, write to it:
  if (dataFile)
  {
    tft.print("--> loading Messier catalog... ");
    Serial1.print("Mes_initialize.txt=");
    Serial1.print("\"");
    Serial1.print("-> Loading Messier Catalog....");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    delay(100);
    while (dataFile.available())
    {
      in_char = dataFile.read();
      items += in_char;
      k = k + 1;
      if (in_char == '\n') {
        Messier_Array[j] = items;
        j = j + 1;
        //          Serial.print(items);
        items = "";
      }
    }
    tft.setTextColor(GREEN);
    tft.println("OK");
    Serial1.print("Messier_OK.txt=");
    Serial1.print("\"");
    Serial1.print("OK");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
    delay(100);

  }
  else
  {
    tft.setTextColor(RED);
    tft.println("ERROR opening: messier.csv");
    Serial1.print("Messier_OK.txt=");
    Serial1.print("\"");
    Serial1.print("Failed");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
  }

  dataFile.close();
  items = "";
  j = 0;
  k = 0;
  dataFile = SD.open("treasure.csv");


  // if the file is available, write to it:
  if (dataFile)
  {
    tft.print("--> loading Treasure catalog... ");
    Serial1.print("tre_initialize.txt=");
    Serial1.print("\"");
    Serial1.print("-> Loading Treasures Catalog..");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    delay(100);
    while (dataFile.available()) {
      in_char = dataFile.read();
      items += in_char;
      k = k + 1;
      if (in_char == '\n') {
        Treasure_Array[j] = items;
        j = j + 1;
        //          Serial.print(items);
        items = "";
      }
    }
    tft.setTextColor(GREEN);
    tft.println("OK");
    Serial1.print("Treasures_OK.txt=");
    Serial1.print("\"");
    Serial1.print("OK");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
    delay(100);

  }
  else
  {
    tft.setTextColor(RED);
    tft.println("ERROR opening: treasure.csv");
    Serial1.print("Treasures_OK.txt=");
    Serial1.print("\"");
    Serial1.print("Failed");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
  }
  dataFile.close();
  last_button = 0;
  LOAD_SELECTOR = 0;

  //tft.println("\n.................................\n");
  /////////////////////////////////////////////////////////////////////
  items = "";
  j = 0;
  k = 0;
  dataFile = SD.open("custom.csv");
  // if the file is available, write to it:
  if (dataFile)
  {
    tft.print("--> loading custom.csv... ");
    Serial1.print("cus_opt_load.txt=");
    Serial1.print("\"");
    Serial1.print("-> Loading Custom Options.....");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    delay(100);
    while (dataFile.available())
    {
      in_char = dataFile.read();
      items += in_char;
      k = k + 1;
      if (in_char == '\n')
      {
        custom_Array[j] = items;
        j = j + 1;
        //          Serial.print(items);
        items = "";
      }
    }
    tft.setTextColor(GREEN);        /// Edit the Nextion Code here
    tft.println("OK");
    tft.setTextColor(d_text);
    delay(100);
  }
  else
  {
    tft.setTextColor(RED);
    tft.println("ERROR opening: custom.csv");
    tft.setTextColor(d_text);
    //check = false;
  }
  dataFile.close();
  last_button = 0;
  LOAD_SELECTOR = 0;





  ////////////////////////////////////////////////////////////////
  tft.print("--> loading options...");
  if (SD.exists("options.txt"))
  {
    loadOptions_SD();
    tft.setTextColor(GREEN);
    tft.println("OK");
    Serial1.print("Options_OK.txt=");
    Serial1.print("\"");
    Serial1.print("OK");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
  }
  else
  {
    tft.setTextColor(RED);
    tft.println("FAIL");
    Serial1.print("Options_OK.txt=");
    Serial1.print("\"");
    Serial1.print("Fail");
    Serial1.print("\"");
    Serial1.write(0xff);
    Serial1.write(0xff);
    Serial1.write(0xff);
    tft.setTextColor(d_text);
  }
  tft.println("--> initializing BlueTooth");
  Serial1.print("BL_initialize.txt=");
  Serial1.print("\"");
  Serial1.print("-> Initializing Bluetooth.....");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  delay(300);
  Serial1.print("Blue_OK.txt=");
  Serial1.print("\"");
  Serial1.print("OK");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  delay(100);
  tft.println("--> initializing GPS");
  Serial1.print("GPS_initialize.txt=");
  Serial1.print("\"");
  Serial1.print("-> Initializing GPS...........");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  delay(300);
  Serial1.print("GPS_OK.txt=");
  Serial1.print("\"");
  Serial1.print("OK");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

#ifndef serial_debug
  if (check == false)  while (1); //don't do anything more
#endif

  Serial1.print("Joy_cal.txt=");
  Serial1.print("\"");
  Serial1.print("-> Calibrating Joystick.......");
  Serial1.print("\"");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  calibrateJoypad(&x_cal, &y_cal);

  // Draw Supporters Logos
  tft.setCursor(0, 360);   // was 370
  tft.setTextColor(l_text);
  //tft.println("SUPPORTERS:");
  bmpDraw("Desso.bmp", 160, 330);
  bmpDraw("hackaday.bmp", 0, 350);
  delay(200);

  // EMPIRIAL MARCH - if sounds everything was initialized well   :)
  if (IS_SOUND_ON)
  {
    SoundOn(note_f, 48);
    delay(100);
    SoundOn(note_f, 48);
    delay(100);
    SoundOn(note_f, 48);
    delay(100);
    SoundOn(note_cb, 32);
    delay(140);
    SoundOn(note_gb, 8);
    delay(50);
    SoundOn(note_f, 48);
    delay(100);
    SoundOn(note_cb, 32);
    delay(140);
    SoundOn(note_gb, 8);
    delay(50);
    SoundOn(note_f, 48);
  }

  delay(500);
  CURRENT_SCREEN = 0;
  drawGPSScreen();
  Serial1.print("page GPS");  // Sending this it's going to tell the nextion display to go to page GPS.
  Serial1.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial1.write(0xff);
  Serial1.write(0xff);
  UPD_T = millis();
  UPD_LST = millis();
  DELAY_Slew = millis();
  TFT_Timer = millis();
  //TFT_timeout = 0;
  RA_move_ending = 0;
  considerTempUpdates();

  digitalWrite(POWER_DRV8825, HIGH == !reverse_logic); // Switch on the Motor Driver Power!
}


void loop(void) {

  /////////////// Nextion Loop //////////////////////
  nexLoop(nex_listen_list);  // Check for any touch event
  //////////////////// End of Nextion Loop /////////////////////////
  // This is done in order to prevent multiple calculations of LST_HA per second (especially while SlewTo) and only
  // do it once the DEC SlewTo slows down, but before stopping OR once every 10 seconds (in order to do the Meridian Flip)
  if (RA_move_ending == 1) {
    calculateLST_HA();
  }

  if ((IS_MERIDIAN_FLIP_AUTOMATIC) && ((UPD_LST + 10000) <= millis()) && (IS_OBJ_FOUND == true)) {
    calculateLST_HA();
    UPD_LST = millis();
  }

  // Adding this delay to SLOW DOWN the Arduino so that the motors can catch up!
  // The delay is only needed when in full speed.... otherways the CalculateLST_HA() takes over and
  // slows down the arduino enough. CalculateLST_HA() when slewing only fires when the motors slows down
  // after they are very close to the Object Position.
  if ((DELAY_Slew + 1 <= millis()) && (IS_OBJ_FOUND == false)) {

    // If you wonder how I get to this delay - 800 uS
    // When I optimised the code for speed, the main delay was coming from calculateLST_HA() which back then was calculated on every Loop();
    // Once I optimized it to only calculate when the SlewTo stops (to fine tune after DEC stops) it turned out that
    // the code is too fast and the motors only "screemed" but not rotating - due to the low voltage/current.
    // This variable depends on How You Limit the Current to your motors and the Voltage you use!
    // I use 12V and 1.6A (70% in full step = 1.10A) to drive my NEMA 17 SY42STH47-1684B Motors.
    // Please note that Potentiometer does not really give consistent results for current on every restart (it drifted between 1.12A - 0.9A).

    // HINT: you can try to play with the Current/Voltage that powers the motors to get faster speeds.
    if (IS_STEPPERS_ON) {
      cosiderSlewTo();
    } else {
      IS_OBJECT_RA_FOUND = true;
      IS_OBJECT_DEC_FOUND = true;
      IS_OBJ_FOUND = true;
      RA_move_ending = 0;
    }
    DELAY_Slew = millis();
    // delayMicroseconds(800);
  }

  // The below part of the code makes sure that the system does NOT process any other inputs while SlweingTo!
  // Since both motors need every STEP to come from Arduino board, it needs it's entire power to run the motors in fastest possible way
  // The fastes possible from this board in the current state of the software is approx 3 turns/sec (600 steps/sec)
  // IS_OBJ_FOUND == true --> Means that SLEW command have completed
  //
  if (IS_OBJ_FOUND == true)
  {
    // BLUETOOTH Considerations ? ... if any
    if ((IS_BT_MODE_ON == true) && (Serial3.available() > 0) && (IS_MANUAL_MOVE == false))
    {
      BT_COMMAND_STR = Serial3.readStringUntil('#');
#ifdef serial_debug
      Serial.println(BT_COMMAND_STR);
#endif
      considerBTCommands();
    }


    // JOYSTICK Movements ? ... if any
    xPosition = analogRead(xPin);
    yPosition = analogRead(yPin);

    if ((xPosition < x_cal - 100) || (xPosition > x_cal + 100) || (yPosition < y_cal - 100) || (yPosition > x_cal + 100))
    {
#ifdef serial_debug
      Serial.print("xPin = ");
      Serial.println(xPosition);
      Serial.print("yPin = ");
      Serial.println(yPosition);
#endif
      IS_MANUAL_MOVE = true;
      if (IS_STEPPERS_ON)
      {
        consider_Manual_Move(xPosition, yPosition);
      }
    }
    else
    {
      IS_MANUAL_MOVE = false;
    }

    // This will take care of turning OFF the TFT's background light if the device is not used
    // for XXX amont of seconds and IS_IN_OPERATION = TRUE
    if ((TFT_timeout > 0) && (millis() - TFT_Timer > TFT_timeout) && (IS_TFT_ON) && (IS_IN_OPERATION))
    {
      analogWrite(TFTBright, 0);
      IS_TFT_ON = false;
    }

    // TOUCH SCREEN Inputs ? ... if any

    TSPoint p = ts.getPoint();

    // Scale from ~0->1000 to tft.width using the calibration #'s

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

    int tx, ty;

    tx = (p.x - 0);
    ty = (p.y - 0);

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!

    if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
      return;
    }

    //Useful to debug touch:
#ifdef serial_debug
    Serial.print(" -> Touched: ");
    Serial.print(tx);
    Serial.print(", y = ");
    Serial.println(ty);
#endif
    considerTouchInput(tx, ty);


    // OTHER UPDATES ?  ... if any
    // Happens every 2 seconds
    if (((millis() - UPD_T) > 2000) && (IS_MANUAL_MOVE == false))
    {
      calculateLST_HA();  // Make sure it Updates the LST! used on Main Screen and When Calculating current Coords.
      considerTimeUpdates();
      considerDayNightMode();
      considerTempUpdates();
      // I need to make sure the Drives are not moved to track the stars,
      // if Object is below horizon ALT < 0 - Stop tracking.
      if ((ALT <= 0) && (IS_TRACKING == true) && (IS_IN_OPERATION == true))
      {
        IS_TRACKING = false;
        Timer3.stop();
        drawMainScreen();
      }
      UPD_T = millis();
    }
  }
}
