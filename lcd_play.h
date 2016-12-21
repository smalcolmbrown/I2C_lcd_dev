//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// lcd_play.h 
//
// Date:      2016/12/19
// By:        Suusi Malcolm-Brown
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LCD_PLAY_H
  #define LCD_PLAY_H

  enum StatusCodes { 
    STATUS_OK = 0, 
    STATUS_SD, 
    STATUS_ERROR,
    STATUS_FINISHED,
    STATUS_PAUSE,
    STATUS_ABORT 
  };

  enum ErrorCodes { 
    ERROR_CODE_NO_ERROR = 0, 
    ERROR_CODE_HOTEND_TEMPERATURE, 
    ERROR_CODE_BED_TEMPERATURE 
  };

  #define NUM_AXIS 4

#endif
