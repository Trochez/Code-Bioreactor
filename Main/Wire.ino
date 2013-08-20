/*
   MCP23008:
 - B00100000 - Start address(32)
   relay box
 - B00100100 - First relay box
 - B00100101 - Second relay box
   LCD display
 - B00100111 - 2 x 16 LCD display
 - B00100110 - 4 x 20 LCD display
   8 OUTPUT EXTENSION
 - B00100010 - EXT_1 (34)
 - B00100011 - EXT_2 (35)
 */

#define PUMP_BYTE 0
#define PID_BYTE  1

#define WIRE_MAX_DEVICES 10
byte numberI2CDevices=0;
byte wireDeviceID[WIRE_MAX_DEVICES];

NIL_WORKING_AREA(waThreadWire, 150);
NIL_THREAD(ThreadWire, arg) {
  byte aByte=0;
  byte* wireFlag32=&aByte;
  unsigned int wireEventStatus=0;
  //boolean relayInitialized = false;
  // TODO: PLUG IN / OUT CRASH THE SYSTEM !!! {
  Wire.begin();
  
  #ifdef I2C_LCD
    LiquidCrystal lcd(I2C_LCD);
  #endif

  while(TRUE) {
    
    wireEventStatus++;

    if (wireEventStatus%10==5) {
      wireUpdateList();
    }


    /*****************
      EXT DEVICE (We don't use it)
    *****************/
    
    /*if (wireDeviceExists(WIRE_EXT_1)) {
      // Serial.println("Device exists");
      if (wireEventStatus%2==1) {
        wireWrite(WIRE_EXT_1,B01010101);
      }
      if (wireEventStatus%2==0) {
        wireWrite(WIRE_EXT_1,B10101010);
      }
    } 
    if (wireDeviceExists(WIRE_EXT_2)) {
      // Serial.println("Device exists");
      if (wireEventStatus%2==1) {
        wireWrite(WIRE_EXT_2,B11110000);
      }
      if (wireEventStatus%2==0) {
        wireWrite(WIRE_EXT_2,B00001111);
      }
    }*/
    
    
    /*********
      RELAY
    *********/
    //
    #ifdef PARAM_RELAY_PUMP
      sendRelay(I2C_RELAY, getParameter(PARAM_RELAY_PUMP), wireFlag32);
    #endif
    
    #ifdef PARAM_FLUX
      sendRelay(I2C_FLUX, getParameter(PARAM_FLUX), wireFlag32);
    #endif
    
    
    /*****************
      LCD I2C Module
    *****************/
    
    #ifdef I2C_LCD
      if (wireEventStatus%10==0) {
        if (wireDeviceExists(I2C_LCD)) {
          if (! wireFlagStatus(wireFlag32, I2C_LCD)) {
            // we should be able to dynamically change the LCD I2C bus address
            setWireFlag(wireFlag32, I2C_LCD);
            lcd.begin(16,2);
            // Print a message to the LCD.
            lcd.setCursor(0,0);
            lcd.print(F("IO1:"));
            lcd.setCursor(0,1);
            lcd.print(F("IO2:"));
          }
          /* To be replaced with the information we want
          lcd.setCursor(4,0);
          lcd.print(((float)getParameter(PARAM_TEMP1))/100);
          lcd.print(F("C "));
          lcd.setCursor(4,1);
          lcd.print(getParameter(PARAM_DISTANCE));
          lcd.print(F("mm  "));
          
          lcd.setCursor(12,1);
          lcd.print(getParameter(PARAM_IRCODE));
          lcd.print(F("   "));
          */
        } 
        else {
          clearWireFlag(wireFlag32, I2C_LCD); 
        }
      }
    
    /*
    if (wireEventStatus%10==5) {
      if (wireDeviceExists(WIRE_LCD_20_4)) {
        if (! wireFlagStatus(wireFlag32, WIRE_LCD_20_4)) {
          // we should be able to dynamically change the LCD I2C bus address
          setWireFlag(wireFlag32, WIRE_LCD_20_4);
          lcd.begin(20, 4);
          // Print a message to the LCD.
          lcd.setCursor(0,0);
          lcd.print(F("Temperature A1!"));
          lcd.setCursor(0,2);
          lcd.print(F("Distance A2!"));
        }
        lcd.setCursor(0,1);
        lcd.print(((float)getParameter(PARAM_TEMP1))/100);
        lcd.print(F(" C   "));
        lcd.setCursor(0,3);
        lcd.print(getParameter(PARAM_DISTANCE));
        lcd.print(F(" mm    "));
      } 
      else {
        clearWireFlag(wireFlag32, WIRE_LCD_20_4); 
      }
    }*/
    #endif


    /*************
      PH METER
    ************/
    /* No device exist yet
    if (wireDeviceExists(WIRE_PHMETER_ID)) {
      wireWrite(WIRE_PHMETER_ID, 0b00010000); // initialize A/D conversion with 5th bit
      setParameter(REGISTER_PH_METER_READOUT, wireReadTwoBytesToInt(WIRE_PHMETER_ID)); // save pH value into 
    }
    */
    nilThdSleepMilliseconds(100);

  }
}
