/////////////////////////////////////////////////////////////////////////////////////////////////
//
// development facility for a simple I2C LCD for the V3 3d Printer using a modified version of 
// original Sprinter firmware to test the display and relevent code
// designed to hook into existing code exploiting existing variabled.
// 
// by Suusi Malcolm-Brown
//
/////////////////////////////////////////////////////////////////////////////////////////////////
//Wiring
 
//Connect SDA to SDA (A4) (Nano pin 27)
//Connect SCL to SCL (A5) (Nano pin 28)
//Connect GND to GND
//Connect VCC to VCC

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library
// download the repository from here and put it in your documents/arduino/libraries folder and restart your ide 
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
/*
LiquidCrystal_I2C_ByVac.h

change from 
#include <Arduino.h>
to
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
*/

#include "I2C_lcd.h"

#define STATUS_OK 0
#define STATUS_SD 1
#define STATUS_ERROR 2

#define ERROR_CODE_NO_ERROR 0
#define ERROR_CODE_HOTEND_TEMPERATURE 1
#define ERROR_CODE_BED_TEMPERATURE 2

#define NUM_AXIS 4

int tt    = 0 ;                               // hook to Extruder temperature in C
int bt    = 0 ;                               // hook to Heated Bed temperature in C
int ett   = 0 ;                               // hook to Extruder target temperature in C
int btt   = 0 ;                               // hook to Heated Bed target temperature in C
bool bFanOn = false;                          // hook to fan status

int status = STATUS_OK;                       // hook to printer status: 0=Ok, 1=SD, 2=Errror
int error_code = ERROR_CODE_NO_ERROR;         // hook to error status: 0=Nothing, 1=Heater thermistor error, 2=bed thermistor error

float current_position[NUM_AXIS] = { 120.9, 1.0, 2.1, 0.0};
const char* error_code_str[]     = { "No Error", "Hotend", "Bed" };
const char* status_str[]         = { "Ok", "SD", "Error"};


/////////////////////////////////////////////////////////////////////////////////////////////////
//
// local function declarations
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManageHeatedBed();
void ManageExtruder() ;
void ManualTest();

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void setup()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  Wire.begin();

  SplashScreen();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void loop()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  ManualTest() ;

  StatusScreen();
  ManageHeatedBed();

  if(bt==btt) {
      ManageExtruder() ;
  }
  delay(1000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void ManageHeatedBed()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManageHeatedBed() {
  if( bt != btt ) {
    (bt < btt) ? bt++ : bt-- ;
  }

}/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void ManageExtruder()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManageExtruder() {
  if( tt != ett ) {
    (tt < ett) ? tt++ : tt-- ;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void ManualTest()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManualTest() {
  char szT[40];
  int iNextChar = 0;
  // read from serial port
  if (Serial.available()) {
    switch( Serial.read()) {
      case 'F':
      case 'f':
        // toggle fan
        iNextChar = Serial.read();
        iNextChar = Serial.read();
        if( iNextChar == 'O' || iNextChar == 'o') {
          iNextChar = Serial.read();
          if( iNextChar == 'N' || iNextChar == 'n') {
            Serial.print("Fan:");
            Serial.println("On");
            bFanOn = true;
          } else {
            Serial.print("Fan:");
            Serial.println("Off");
            bFanOn = false;
          }
        }
        break;
      case 'X':
      case 'x':
        // set x axis value
        current_position[0]=Serial.parseFloat();
        Serial.print("X Axis:");
        Serial.println(current_position[0]);
        break;
      case 'Y':
      case 'y':
        // set error
        current_position[1]=Serial.parseFloat();
        Serial.print("Y Axis:");
        Serial.println(current_position[1]);
        break;
      case 'Z':
      case 'z':
        // set error
        current_position[2]=Serial.parseFloat();
        Serial.print("Z Axis:");
        Serial.println(current_position[2]);
        break;
      case 'B':
      case 'b':
        // set the hot end target temparature
        btt = Serial.parseInt();
        sprintf( szT, "Heated Bed Target Temperature = %d", btt);
        Serial.println(szT);
        break;
      case 'E':
      case 'e':
        // set error or hot end target temperature
        iNextChar = Serial.read();
        if( (iNextChar == 'X') || (iNextChar == 'x') ) {
          // set the hot end target temparature
          ett = Serial.parseInt();
          sprintf( szT, "Hot End Target Temperature = %d", ett);
          Serial.println(szT);
        } else {
          // it must be error code setting then
          iNextChar = Serial.parseInt();
          switch(iNextChar){
            case ERROR_CODE_NO_ERROR:
              status = STATUS_OK;
              error_code = ERROR_CODE_NO_ERROR;
              sprintf( szT, "Sprinter:%s", status_str[status]);
              Serial.println(szT);
              break;
            case ERROR_CODE_HOTEND_TEMPERATURE:
              status = STATUS_ERROR;
              error_code = ERROR_CODE_HOTEND_TEMPERATURE;
              sprintf( szT, "Error: %s", error_code_str[error_code] );
              Serial.println(szT);
              break;
            case ERROR_CODE_BED_TEMPERATURE:
              status = STATUS_ERROR;
              error_code = ERROR_CODE_BED_TEMPERATURE;
              sprintf( szT, "Error: %s", error_code_str[error_code] );
              Serial.println(szT);
              break;
          }
        }
        break;
    }
  }
}

