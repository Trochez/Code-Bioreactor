#if defined(GAS_CTRL) || defined(STEPPER_CTRL) || defined(I2C_LCD)

#include <Wire.h>
/*
   MCP23008 (DEFAULT RELAY):
 - B00100XXX - Start address(32)
   relay box
 - B00100100 - First relay box
 - B00100101 - Second relay box
   LCD display
 - B00100111 - 2 x 16 LCD display
 - B00100110 - 4 x 20 LCD display
   8 OUTPUT EXTENSION
 - B00100010 - EXT_1 (34)
 - B00100011 - EXT_2 (35)
   FLUX
 - B1011 XXX R/W  (XXX is the user defined address and R/W the read/write byte)
   PH METER
 - B????????
 */
 
//  #define ANEMOMETER_WRITE 0b10110000
  #define ANEMOMETER_WRITE 104 
//  #define ANEMOMETER_READ  0b10110001
  #define ANEMOMETER_READ 104
  

#define WIRE_MAX_DEVICES 10
byte numberI2CDevices=0;
byte wireDeviceID[WIRE_MAX_DEVICES];

NIL_WORKING_AREA(waThreadWire, 150);
NIL_THREAD(ThreadWire, arg) {
  
  
  byte aByte=0;
  byte* wireFlag32=&aByte;
  unsigned int wireEventStatus=0;
  //boolean relayInitialized = false;
  //TODO: PLUG IN / OUT CRASH THE SYSTEM !!! {
  Wire.begin();

  
  #ifdef I2C_LCD
    LiquidCrystal lcd(I2C_LCD);
  #endif

  while(true) {
    
    wireEventStatus++;
    if (wireEventStatus%10==5) {
      wireUpdateList();
    }
 
    
    /*********
      RELAY
    *********/
    //
    #ifdef PARAM_RELAY_PUMP  
        sendRelay(I2C_RELAY,getParameter(PARAM_RELAY_PUMP), wireFlag32);
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
          lcd.print(F("   "));(
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
    
    
    /*************
      ANEMOMETER
    ************/
    
    
    // check if a conditionnal test on the ready bit of the config word is mandatory or not (indicate end of conversion) voir fonction wireReadFourBytesToInt
    // results given in mV

    
    #ifdef  PARAM_FLUX_GAS1
      wireWrite(ANEMOMETER_WRITE,0b10010000);
      setParameter(PARAM_FLUX_GAS1,wireReadFourBytesToInt(ANEMOMETER_READ));
    #endif
    
    #ifdef  PARAM_FLUX_GAS2
      wireWrite(ANEMOMETER_WRITE,0b10110000);
      setParameter(PARAM_FLUX_GAS2,wireReadFourBytesToInt(ANEMOMETER_READ));
    #endif
    
    #ifdef  PARAM_FLUX_GAS3
      wireWrite(ANEMOMETER_WRITE,0b11010000);
      setParameter(PARAM_FLUX_GAS3,wireReadFourBytesToInt(ANEMOMETER_READ));
    #endif
    
    #ifdef  PARAM_FLUX_GAS4
      wireWrite(ANEMOMETER_WRITE,0b11110000);
      setParameter(PARAM_FLUX_GAS4,wireReadFourBytesToInt(ANEMOMETER_READ));
    #endif
    
    
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
    
    nilThdSleepMilliseconds(100);

  }
}


/********************
  Utilities functions 
**********************/

void wireWrite(uint8_t address, uint8_t _data ) {
  Wire.beginTransmission(address);
  Wire.write(_data);
  Wire.endTransmission();
}

void wireWrite(uint8_t address, uint8_t reg, uint8_t _data ) // used by 4-relay
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(_data);
  Wire.endTransmission();
}

byte wireRead(uint8_t address) {
  byte _data = 0;
  Wire.requestFrom(address, (uint8_t)1);
  if(Wire.available()) {
    _data = Wire.read();
  }
  return _data;
}


int wireReadTwoBytesToInt(uint8_t address) {
  int i = 0;
  int _data = 0;
  int byteWithMSB;
  int byteWithLSB;

  Wire.requestFrom(address, (uint8_t)2);
  while(Wire.available()) {
    if (i > 2) return 0; // security mechanism
    else i++;
    byteWithMSB = Wire.read();
    byteWithLSB = Wire.read();
    _data = (byteWithMSB<<8) | byteWithLSB;
  }
  return _data;
}


int wireReadFourBytesToInt(uint8_t address) {
  int i = 0;
  unsigned int _data = 0;
  int byteWithADD;
  int byteWithMSB;
  int byteWithLSB;
  int byteWithCFG;

  Wire.requestFrom(address, (uint8_t)4);
  while(Wire.available()) {
    if (i > 4) return 0; // security mechanism, see if sufficient or not (give false info about the FLUX if the case !!!!)
    else i++;
    byteWithADD = Wire.read();
    Serial.print("address: ");
    Serial.println(byteWithADD);
    byteWithMSB = Wire.read();
    Serial.print("MSB: ");
    Serial.println(byteWithMSB);
    byteWithLSB = Wire.read();
    Serial.print("LSB: ");
    Serial.println(byteWithLSB);
    byteWithCFG = Wire.read();
    Serial.print("CFG: ");
    Serial.println(byteWithADD);
    _data = (byteWithMSB<<8) | byteWithLSB;
    Serial.print("data: ");
    Serial.println(_data);
  }
  return _data;
}

void wireInfo(Print* output) {
  //wireUpdateList();
  output->println("I2C");

  for (byte i=0; i<numberI2CDevices; i++) {
    output->print(i);
    output->print(": ");
    output->print(wireDeviceID[i]);
    output->print(" - ");
    output->println(wireDeviceID[i],BIN);
  }
}


void wireUpdateList() {
  // 16ms
  byte _data;
  byte currentPosition=0;
  // I2C Module Scan, from_id ... to_id
  for (byte i=0; i<=127; i++)
  {
    Wire.beginTransmission(i);
    Wire.write(&_data, 0);
    // I2C Module found out!
    if (Wire.endTransmission()==0)
    {
      // there is a device, we need to check if we should add or remove a previous device
      if (currentPosition<numberI2CDevices && wireDeviceID[currentPosition]==i) { // it is still the same device that is at the same position, nothing to do
        currentPosition++;
//        Serial.print("ok: ");
//        Serial.println(i);
      } 
      else if (currentPosition<numberI2CDevices && wireDeviceID[currentPosition]<i) { // some device(s) disappear, we need to delete them
        Serial.print("delete: ");
        Serial.println(wireDeviceID[currentPosition]);
        wireRemoveDevice(currentPosition);
        i--;
      } 
      else if (currentPosition>=numberI2CDevices || wireDeviceID[currentPosition]>i) { // we need to add a device
        Serial.print("add: ");
        Serial.println(i);
        wireInsertDevice(currentPosition, i);
        currentPosition++;
      }
      nilThdSleepMilliseconds(1);
    }
  }
  while (currentPosition<numberI2CDevices) {
    Serial.print("delete: ");
    Serial.println(wireDeviceID[currentPosition]);
    wireRemoveDevice(currentPosition);
  }
}

void wireRemoveDevice(byte id) {
  debugger(DEBUG_WIRE_REMOVED_DEVICE,wireDeviceID[id]);
  for (byte i=id; i<numberI2CDevices-1; i++) {
    wireDeviceID[i]=wireDeviceID[i+1];
  }
  numberI2CDevices--;
}

void wireInsertDevice(byte id, byte newDevice) {
  //Serial.println(id);
  debugger(DEBUG_WIRE_ADDED_DEVICE,newDevice);
  if (numberI2CDevices<WIRE_MAX_DEVICES) {
    for (byte i=id+1; i<numberI2CDevices-1; i++) {
      wireDeviceID[i]=wireDeviceID[i+1];
    }
    wireDeviceID[id]=newDevice;
    numberI2CDevices++;
  } 
  else {
    logger(LOGGER_I2C_CAPACITY_EXCEEDED);
  }
}

boolean wireDeviceExists(byte id) {
  for (byte i=0; i<numberI2CDevices; i++) {
    if (wireDeviceID[i]==id) return true;
  }
  return false; 
}


void sendRelay(byte id, byte value, byte* flag) {
  if (wireDeviceExists(id)) {
    if (!wireFlagStatus(flag, id))
    {
      setWireFlag(flag, id);
      wireWrite(id, 0x05, 0b00000100); // initialize CONFREG (0x05)
      wireWrite(id, 0x00, 0b00000000); // initialize IOREG (0x00)
    }
    wireWrite(id, 0x0A, value); // pin control
  }
  else
  {
    clearWireFlag(flag, id);
  }
}


// We will combine flags in a byte. Using pointer does not seems to improve
// memory size so we don't use pointer
void setWireFlag(byte *aByte, byte address) {
  *aByte |= (1 << (address & 0b00000111));
}

void clearWireFlag(byte *aByte, byte address) {
  *aByte &= ~(1 << (address & 0b00000111));
}

boolean wireFlagStatus(byte *aByte, byte address) {
  return *aByte & (1 << (address & 0b00000111));
}


#endif

