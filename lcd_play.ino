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


// comment out if no I2C display
#define I2C_DISPLAY

#ifdef I2C_DISPLAY
  // comment out the option that is not used
    #define PCF8574
//    #define PCF8574A

  #ifdef PCF8574
    #define I2C_ADDR    0x27  // Define I2C Address for PCF8574 port expander
  #endif
  #ifdef PCF8574A
    #define I2C_ADDR    0x3f  // Define I2C Address for PCF8574A port expander
  #endif
  #define BACKLIGHT_PIN  3
  #define En_pin  2
  #define Rw_pin  1
  #define Rs_pin  0
  #define D4_pin  4
  #define D5_pin  5
  #define D6_pin  6
  #define D7_pin  7

  #define  LED_OFF  0
  #define  LED_ON  1
#endif


#ifdef I2C_DISPLAY
  LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
#endif

#define STATUS_OK 0
#define STATUS_SD 1
#define STATUS_ERROR 2
const char* status_str[] = { "Ok", "SD", "Error"};

#define ERROR_CODE_NO_ERROR 0
#define ERROR_CODE_HOTEND_TEMPERATURE 1
#define ERROR_CODE_BED_TEMPERATURE 2
const char* error_code_str[] = { "No Error", "Hotend", "Bed" };



int status = STATUS_OK; //
int error_code = ERROR_CODE_NO_ERROR; //0=Nothing, 1=Heater thermistor error


#define NUM_AXIS 4
#define NUM_DIGITS 7
#define NUM_PRECISION 1
float current_position[NUM_AXIS] = { 120.9, 1.0, 2.1, 0.0};

// analog2temp(current_raw);
// analog2tempBed(current_bed_raw);

int target_raw        = 0 ;
int current_raw       = 0 ;
int target_bed_raw    = 0 ;
int current_bed_raw   = 0 ;


int ert = 0 ;
int ett = 0 ;
int brt = 0 ;
int btt = 0 ;


bool bFanOn = false;
char  szTemp[81];         // temp work aria for sprintf

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// local function declarations
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManageHeatedBed();
void ManageExtruder() ;
void ManualTest();
void ConvertleadingSpacesToZeros( char* source );
void SplashScreen();
void StatusScreen();

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcd.begin (20,4);  // initialize the lcd
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
  lcd.clear();
  lcd.home();
  lcd.setCursor(0,0); 

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

  if(brt==btt) {
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
  if( brt != btt ) {
    (brt < btt) ? brt++ : brt-- ;
  }

}/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void ManageExtruder()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManageExtruder() {
  if( ert != ett ) {
    (ert < ett) ? ert++ : ert-- ;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void ConvertleadingSpacesToZeros( char* source )
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ConvertleadingSpacesToZeros( char* source ) {
  while(*source++ ) {
    if (isWhitespace(*source)) {
      *source = '0';
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void StatusScreen()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void StatusScreen(){
  char szT[40] ;    // workspace
  lcd.clear();
  lcd.home();
  
  // do first line 
  
  lcd.setCursor( 0, 0 ); 
  sprintf( szTemp, "%c%d/%d%c", 0x5c, ert, ett, 0xdf );
  lcd.print( szTemp );

  lcd.setCursor( 10, 0 ); 
  sprintf( szTemp, "%c%d/%d%c", 0xfc, brt, btt, 0xdf );
  lcd.print( szTemp );

  // do second line
  
  lcd.setCursor( 0, 1 );
  ConvertleadingSpacesToZeros( dtostrf( current_position[0], NUM_DIGITS, NUM_PRECISION, szT) );
  sprintf( szTemp, "X%s", szT+2 );
  lcd.print( szTemp );
  
  lcd.setCursor(7,1);
  ConvertleadingSpacesToZeros( dtostrf( current_position[1], NUM_DIGITS, NUM_PRECISION, szT) );
  sprintf( szTemp, "Y%s", szT+2 );
  lcd.print(szTemp);
  
  lcd.setCursor(14,1);
  ConvertleadingSpacesToZeros( dtostrf( current_position[2], NUM_DIGITS, NUM_PRECISION, szT) );
  sprintf( szTemp, "Z%s", szT+2 );
  lcd.print(szTemp);

  // do third line
  
  lcd.setCursor( 0, 2 ); 
  sprintf( szTemp, "Fan %s", (bFanOn)? "On" : "Off" );
  lcd.print( szTemp );

  // do fourth line
  
  lcd.setCursor(0,3);
  if( status == STATUS_ERROR ){
    if(error_code){
      sprintf( szTemp, "Error: %s", error_code_str[error_code] );
      lcd.print( szTemp );
    }
  } else {
    // no error
    sprintf( szTemp, "Sprinter: %s", status_str[status]);
    lcd.print( szTemp );
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void SplashScreen()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void SplashScreen(){
  Wire.begin();
  lcd.begin (20,4);  // initialize the lcd
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
  lcd.clear();
  lcd.home();
  lcd.setCursor(0,0); 
  lcd.print("Vector 3 3D Printer");
  lcd.setCursor(3,1);
  lcd.print("Version  V1.01");
  lcd.setCursor(6,3);
  lcd.print("Sprinter");
  delay(4000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// void ManualTest()
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void ManualTest() {
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
        sprintf( szTemp, "Heated Bed Target Temperature = %d", btt);
        Serial.println(szTemp);
        break;
      case 'E':
      case 'e':
        // set error or hot end target temperature
        iNextChar = Serial.read();
        if( (iNextChar == 'X') || (iNextChar == 'x') ) {
          // set the hot end target temparature
          ett = Serial.parseInt();
          sprintf( szTemp, "Hot End Target Temperature = %d", ett);
          Serial.println(szTemp);
        } else {
          // it must be error code setting then
          iNextChar = Serial.parseInt();
          switch(iNextChar){
            case ERROR_CODE_NO_ERROR:
              status = STATUS_OK;
              error_code = ERROR_CODE_NO_ERROR;
              sprintf( szTemp, "Sprinter:%s", status_str[status]);
              Serial.println(szTemp);
              break;
            case ERROR_CODE_HOTEND_TEMPERATURE:
              status = STATUS_ERROR;
              error_code = ERROR_CODE_HOTEND_TEMPERATURE;
              sprintf( szTemp, "Error: %s", error_code_str[error_code] );
              Serial.println(szTemp);
              break;
            case ERROR_CODE_BED_TEMPERATURE:
              status = STATUS_ERROR;
              error_code = ERROR_CODE_BED_TEMPERATURE;
              sprintf( szTemp, "Error: %s", error_code_str[error_code] );
              Serial.println(szTemp);
              break;
          }
        }
        break;
    }
  }
}

