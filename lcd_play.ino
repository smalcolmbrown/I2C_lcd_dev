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
#if (ARDUINO <  100)
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif
*/

#include "lcd_play.h"
#include "I2C_lcd.h"



int tt    = 0 ;                               // hook to Extruder temperature in C
int bt    = 0 ;                               // hook to Heated Bed temperature in C
int ett   = 0 ;                               // hook to Extruder target temperature in C
int btt   = 0 ;                               // hook to Heated Bed target temperature in C
bool bFanOn = false;                          // hook to fan status

int status = STATUS_OK;                       // hook to printer status: 0=Ok, 1=SD, 2=Errror
int error_code = ERROR_CODE_NO_ERROR;         // hook to error status: 0=Nothing, 1=Heater thermistor error, 2=bed thermistor error

float current_position[NUM_AXIS] = { 120.9, 1.0, 2.1, 0.0};
const char* error_code_str[]     = { "No Error", "Hotend", "Bed" };
const char* status_str[]         = { "Ok", "SD", "Error", "Job done", "Pause", "Job Killed" };
const char* pszFirmware[]        = { "Sprinter", "https://github.com/smalcolmbrown/V3-Sprinter-Melzi_1_00/", "1.01", "Vector 3 3D Printer", "1" };
const char* uuid                 = "Sn009128-V300-0000-0000-000000000000";

//SerialMgr.cur()->print("FIRMWARE_NAME:Sprinter FIRMWARE_URL:http%%3A/github.com/kliment/Sprinter/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:Mendel EXTRUDER_COUNT:1 UUID:");

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
  ManageHeaters();
  delay(1000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void ManageHeaters()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManageHeaters() {
   ManageHeatedBed();
   ManageExtruder() ;
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
    switch( toupper(Serial.read())) {
      case 'X':
        // set X Axis Position
        current_position[0]=Serial.parseFloat();
        Serial.print("X Axis:");
        Serial.println(current_position[0]);
        break;
      case 'Y':
        // set Y Axis Position
        current_position[1]=Serial.parseFloat();
        Serial.print("Y Axis:");
        Serial.println(current_position[1]);
        break;
      case 'Z':
        // set Z Axis Position
        current_position[2]=Serial.parseFloat();
        Serial.print("Z Axis:");
        Serial.println(current_position[2]);
        break;
      case 'E':
        // set error
        switch(Serial.parseInt()){
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
        break;
      case 'G':
        switch(Serial.parseInt()) {
          case 0:
          case 1:
            while(Serial.available()) {
              switch(toupper(Serial.read())) {
                case 'X':
                  // set X Axis Position
                  current_position[0]=Serial.parseFloat();
                  Serial.print("X Axis:");
                  Serial.println(current_position[0]);
                  break;
                case 'Y':
                  // set Y Axis Position
                  current_position[1]=Serial.parseFloat();
                  Serial.print("Y Axis:");
                  Serial.println(current_position[1]);
                  break;
                case 'Z':
                  // set Z Axis Position
                  current_position[2]=Serial.parseFloat();
                  Serial.print("Z Axis:");
                  Serial.println(current_position[2]);
                  break;
                case 'E':
                  // The amount to extrude between the starting point and ending point
                  Serial.print("Extruedr:");
                  Serial.println(Serial.parseFloat());
                  break;
                case 'F':
                  // The feedrate per minute of the move between the starting point and ending point (if supplied)
                  Serial.print("Feedrate/min:");
                  Serial.println(Serial.parseFloat());
                  break;
                case 'S':
                  // Flag to check if an endstop was hit (S1 to check, S0 to ignore, S2 see note, default is S0)
                  Serial.print("Endstop:");
                  Serial.println(Serial.parseInt());
                  break;
              }
            break;
          }
        }
        break;
      case 'M':
        switch(Serial.parseInt()) {
          case 109:                     // M104 Set Extruder Temperature
          case 104:                     // M104 Set Extruder Temperature
            Serial.read();
            Serial.read();
            ett = Serial.parseInt();
            sprintf( szT, "Hot End Target Temperature = %d", ett);
            Serial.println(szT);
            break;
          case 105:                     // M105 Get Extruder Temperature
            sprintf( szT, "ok T:%d B:%d", tt, bt );
            break;
          case 106:                     // M106 Fan On
            Serial.println("Fan:On");
            bFanOn = true;
            break;
          case 107:                     // M107 Fan Off
            Serial.print("");
            Serial.println("Fan:Off");
            bFanOn = false;
            break;
          case 115:                     // M115 Get Firmware Version and Capabilities
            Serial.print("FIRMWARE_NAME:");
            Serial.print(pszFirmware[FIRMWARE_NAME]);
            Serial.print(" FIRMWARE_URL:");
            Serial.print(pszFirmware[FIRMWARE_URL]);
            Serial.print(" PROTOCOL_VERSION:");
            Serial.print(pszFirmware[FIRMWARE_VERSION]);
            Serial.print(" MACHINE_TYPE:");
            Serial.print(pszFirmware[FIRMWARE_MACHINENAME]);
            Serial.print(" EXTRUDER_COUNT:");
            Serial.print(pszFirmware[FIRMWARE_EXTRUDERS]);
            Serial.print("UUID:");
            Serial.println(uuid);
            break;
          case 140:                     // M140 Set Heated Bed Temperature
          case 190:                     // M190 Set Heated Bed Temperature
            Serial.read();
            Serial.read();
            btt = Serial.parseInt();
            sprintf( szT, "Heated Bed Target Temperature = %d", btt);
            Serial.println(szT);
            break;

        }
    }
  }
}

