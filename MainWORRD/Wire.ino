#if defined(GAS_CTRL) || defined(STEPPER_CTRL) || defined(I2C_LCD) || defined(PH_CTRL)

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


void wireWrite(uint8_t address, uint8_t _data );
void wireWrite(uint8_t address, uint8_t reg, uint8_t _data );
byte wireRead(uint8_t address);
int wireReadTwoBytesToInt(uint8_t address);
int wireReadFourBytesToInt(uint8_t address);
void wireInfo(Print* output);
void wireUpdateList();
void wireRemoveDevice(byte id);
void wireInsertDevice(byte id, byte newDevice);
boolean wireDeviceExists(byte id);
void sendRelay(byte id, byte value, byte* flag);
void setWireFlag(byte *aByte, byte address);
void clearWireFlag(byte *aByte, byte address);

boolean wireFlagStatus(byte *aByte, byte address);


#define WIRE_MAX_DEVICES 5
byte numberI2CDevices=0;
byte wireDeviceID[WIRE_MAX_DEVICES];

NIL_WORKING_AREA(waThreadWire, 128);
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
    if (wireEventStatus%25==5) {
      wireUpdateList();
    }
 
    
    /*********
      RELAY
    *********/
    //
    #ifdef PARAM_RELAY_PUMP  
      sendRelay(I2C_RELAY,getParameter(PARAM_RELAY_PUMP), wireFlag32);
    #endif
    
    #ifdef PARAM_RELAY_TAP
      sendRelay(I2C_RELAY_TAP,(getParameter(PARAM_RELAY_TAP)>>8), wireFlag32);
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
    
    #ifdef PARAM_PH
    
      
      Serial.print("valeur du PARAM_PH ");     
      Serial.println(getParameter(PARAM_PH));
      delay (6);
   
      //setParameter(PARAM_PH,1); quand pourrait-on définir le parametre à 1?
      
      int valueph = getParameter(PARAM_PH);
      //Serial.print("valueph (PARAM_PH): ");     DEBUG
      //Serial.println(valueph);                  DEBUG
      //delay (1500);                             DEBUG
      
      int phquatre ;
      int phquatretot = 0;
      int phquatremoy ;
      int phsept ;
      int phsepttot = 0;
      int phseptmoy ;
      int factora ;
      int factorb ;
         
      if(valueph == 4)
      {
        
      Serial.println("1ere calibration (pH 4)");
      //Serial.print("verif valueph (4) :");      DEBUG
      //Serial.println(valueph);                  DEBUG
      delay (10000);
      
      
          int i ; 
      
          for (i=0; i<10; i++)
         {           
          phquatre = wireReadFourBytesToInt(PH);
          phquatretot += phquatre;
          Serial.print("valeur calibration pH 4 :");
          Serial.println(phquatre);
          delay (1500);
         }
     
         phquatremoy = (phquatretot / 10);
         Serial.print("valeur calibration pH 4 MOYENNE :");
         Serial.println(phquatremoy);
         delay (8000);
      }
      else
      
      if(valueph == 7)
      {
        
        Serial.println("2e calibration (pH 7)");
        //Serial.print("verif valueph (4) :");      DEBUG
        //Serial.println(valueph);                  DEBUG
        delay (10000);
      
      
          int j ; 
      
          for (j=0; j<10; j++)
         {           
          phsept = wireReadFourBytesToInt(PH);
          phsepttot += phsept;
          Serial.print("valeur calibration pH 7:");
          Serial.println(phsept);
          delay (1500);
         }
     
         phseptmoy = (phsepttot / 10);
         Serial.print("valeur calibration pH 7 MOYENNE :");
         Serial.println(phseptmoy);
         delay (8000);
      
      }
      else
      
      
      
       if(valueph == 2)
      {      
      Serial.println("c'est parti pour la lecture du pH !!");
      //Serial.println("verif valueph (2) :");               DEBUG
      //Serial.println(valueph);                             DEBUG
      delay (4000);
      
      int phlecture = wireReadFourBytesToInt(PH);
      Serial.print("valeur brute du pH :");
      Serial.println(phlecture);
      delay (6);
      
      factora = ((300*20) / (phquatremoy - phseptmoy ));
      Serial.print("facteur a :");
      Serial.println(factora);
      delay (6);
      
      factorb = (((phseptmoy*factora)/20) + 700 );
      Serial.print("facteur b :");
      Serial.println(factorb);
      delay (6);
      
      int phfinal = (factorb - ((factora*phlecture)/20));
      Serial.print("valeur finale du pH :");
      Serial.println(phfinal);
      delay (3000);
      
      setParameter(PARAM_PH,wireReadFourBytesToInt(PH));
      
      }
      else
      
      if(valueph > 10)
      {  
      Serial.print("facteur a :");
      Serial.println(factora);
      
      Serial.print("facteur b :");
      Serial.println(factorb);
        
      int phlecture = wireReadFourBytesToInt(PH);
      Serial.print("valeur brute du pH :");
      Serial.println(phlecture);
      delay (6);
      
      
      int phfinal = (factorb - ((factora*phlecture)/20));
      Serial.print("valeur finale du pH :");
      Serial.println(phfinal);
      delay (3000);
      
      setParameter(PARAM_PH,wireReadFourBytesToInt(PH));
     
      }
      else
      
      Serial.println("Entrez C4, ou C7, pour commencer la calibration");
      //Serial.print("verif valueph initiale (1) :");                        DEBUG
      //Serial.println(valueph);                                             DEBUG
      delay (500);
      
      
      
      
      
      
      //Serial.print("entrer une valeur pour le parametre C (1, 4 ou 7): ");
      //setParameter(PARAM_PH,wireReadFourBytesToInt(PH));
      //delay (10000);
      
      //setParameter(PARAM_PH,wireReadFourBytesToInt(PH));
      //setParameter(PARAM_PH,wireReadFourBytesToInt(PH));
      
      /*delay (5000);
      Serial.print("valeur du ph: ");
      Serial.println(PARAM_PH);
      delay (5000);
      
      */
      
      
       
      //Serial.print("valeur pH 4 bytes: ");
      //Serial.println(wireReadFourBytesToInt(PH));
      //delay (1000);
      
     
      /*Serial.print("valeur pH 2 bytes : ");
      Serial.println(wireReadTwoBytesToInt(PH));
      delay (1000);*/
      
      
    #endif 
    
    
    /*************
      ANEMOMETER
    ************/
    
    
    // check if a conditionnal test on the ready bit of the config word is mandatory or not (indicate end of conversion) voir fonction wireReadFourBytesToInt
    // results given in mV
  
    #ifdef  PARAM_FLUX_GAS1
      wireWrite(ANEMOMETER_WRITE,0b10010000);
      delay(6);
      setParameter(PARAM_FLUX_GAS1,wireReadFourBytesToInt(ANEMOMETER_READ));
    #endif
   
    #ifdef  PARAM_FLUX_GAS2
      wireWrite(ANEMOMETER_WRITE,0b10110000);
      delay(6);
      setParameter(PARAM_FLUX_GAS2,wireReadFourBytesToInt(ANEMOMETER_READ));
    #endif
    
    #ifdef  PARAM_FLUX_GAS3
      wireWrite(ANEMOMETER_WRITE,0b11010000);
      delay(6);
      setParameter(PARAM_FLUX_GAS3,wireReadFourBytesToInt(ANEMOMETER_READ));
    #endif
    
    #ifdef  PARAM_FLUX_GAS4
      wireWrite(ANEMOMETER_WRITE,0b11110000);
      delay(6);
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
    
    nilThdSleepMilliseconds(200);

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
  uint8_t byteWithADD;
  uint8_t byteWithMSB;
  uint8_t byteWithLSB;
  uint8_t byteWithCFG;

  Wire.requestFrom(address, (uint8_t)4);
  while(Wire.available()) {
    if (i > 4) return 0; // security mechanism, see if sufficient or not (give false info about the FLUX if the case !!!!)
    else i++;
    byteWithMSB = Wire.read();
    byteWithLSB = Wire.read();
    byteWithCFG = Wire.read();
    byteWithADD = Wire.read();
    _data = (byteWithMSB<<8) | byteWithLSB;
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
      } 
      else if (currentPosition<numberI2CDevices && wireDeviceID[currentPosition]<i) { // some device(s) disappear, we need to delete them
        wireRemoveDevice(currentPosition);
        i--;
      } 
      else if (currentPosition>=numberI2CDevices || wireDeviceID[currentPosition]>i) { // we need to add a device
        //Serial.print("add: ");        DEBUG POUR CONNAITRE L'ADRESSE DE L'I2C !!!!!!!!
        //Serial.println(i);
        wireInsertDevice(currentPosition, i);
        currentPosition++;
      }
      nilThdSleepMilliseconds(1);
    }
  }
  while (currentPosition<numberI2CDevices) {
    wireRemoveDevice(currentPosition);
  }
}

void wireRemoveDevice(byte id) {
  for (byte i=id; i<numberI2CDevices-1; i++) {
    wireDeviceID[i]=wireDeviceID[i+1];
  }
  numberI2CDevices--;
}

void wireInsertDevice(byte id, byte newDevice) {
  //Serial.println(id);
  
  if (numberI2CDevices<WIRE_MAX_DEVICES) {
    for (byte i=id+1; i<numberI2CDevices-1; i++) {
      wireDeviceID[i]=wireDeviceID[i+1];
    }
    wireDeviceID[id]=newDevice;
    numberI2CDevices++;
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


