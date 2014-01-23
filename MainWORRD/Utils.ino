void printHardCodedParameters(Print* output){
  output->println(F("Hardcoded:")); 
  output->print(F("IP:"));
  printIP(output, ip, 4, DEC);
  output->print(F("MAC:"));
  printIP(output, mac, 6, HEX);
  output->print(F("NTP:"));
  printIP(output, (uint8_t*) alix, 4, DEC);
#ifdef RELAY_PUMP
  output->print(F("I2C relay:"));
  output->println(I2C_RELAY); 
#endif
#ifdef FLUX
  output->print(F("I2C Flux:"));
  output->println(I2C_FLUX); 
#endif
}

void printIP(Print* output, uint8_t* tab, uint8_t s, byte format){
  for(int i=0; i<s; i++){
    output->print(tab[i], format);
    output->print(' ');
  }
  output->println("");
}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 
 This method will mainly set/read the parameters:
 Uppercase + number + CR ((-) and 1 to 5 digit) store a parameter (0 to 25 depending the letter)
 example: A100, A-1
 -> Many parameters may be set at once
 example: C10,20,30,40,50
 Uppercase + CR read the parameter
 example: A
 -> Many parameters may be read at once
 example: A,B,C,D
 s : read all the parameters
 h : help
 l : show the log file
 */




void printResult(char* data, Print* output) {
  boolean theEnd=false;
  byte paramCurrent=0; // Which parameter are we defining
  // The maximal length of a parameter value. It is a int so the value must be between -32768 to 32767
#define MAX_PARAM_VALUE_LENGTH 6
  char paramSerialValue[MAX_PARAM_VALUE_LENGTH];
  byte paramSerialValuePosition=0;
  byte i=0;

  while (!theEnd) {
    byte inChar=data[i];
    i++;
    if (inChar=='\0' || i==SERIAL_BUFFER_LENGTH) theEnd=true;
    if (inChar=='p') { // show settings
      printHardCodedParameters(output);
    } 
    else if (inChar=='h') {
      printHelp(output);
    }
    else if (inChar=='l') {
#ifdef THR_LINEAR_LOGS
      printIndexes(output); 
#else
      noThread(output);
#endif
    }
    else if (inChar=='i') { // show i2c (wire) information
#if defined(GAS_CTRL) || defined(STEPPER_CTRL) || defined(I2C_LCD)
      //wireInfo(&Serial); TODO
#else  //not elsif !!
      noThread(output);
#endif
    } 

    else if (inChar=='o') { // show oneWire information
#if defined(TEMP_LIQ) || defined(TEMP_PLATE) || defined(TEMP_STEPPER)
      //oneWireInfo(&Serial); TODO
#else
      noThread(output);
#endif
    } 
    else if (inChar=='s') { // show settings
      printParameters(output);
    } 
    else if (inChar=='f') { // show settings
      printFreeMemory(output);
    } 
    else if (inChar==',') { // store value and increment
      if (paramCurrent>0) {
        if (paramSerialValuePosition>0) {
          setAndSaveParameter(paramCurrent-1,atof(paramSerialValue));
        } 
        else {
          Serial.println(parameters[paramCurrent-1]);
        }
        if (paramCurrent<=MAX_PARAM) {
          paramCurrent++;
          paramSerialValuePosition=0;
          paramSerialValue[0]='\0';
        }
      }
    }
    else if (theEnd) {
      // this is a carriage return;
      if (paramCurrent>0) {
        if (paramSerialValuePosition>0) {
          setAndSaveParameter(paramCurrent-1,atof(paramSerialValue));
        } 
        else {
          Serial.println(parameters[paramCurrent-1]);
        }
      }
    }
    else if ((inChar>47 && inChar<58) || inChar=='-') {
      if (paramSerialValuePosition<MAX_PARAM_VALUE_LENGTH) {
        paramSerialValue[paramSerialValuePosition]=inChar;
        paramSerialValuePosition++;
        if (paramSerialValuePosition<MAX_PARAM_VALUE_LENGTH) {
          paramSerialValue[paramSerialValuePosition]='\0';
        }
      }
    } 
    else if (inChar>64 && inChar<(66+MAX_PARAM)) { // a character so we define the field
      paramCurrent=inChar-64;
    } 
  }
}

void printHelp(Print* output) {
  //return the menu
  output->println(F("(f)free"));
  output->println(F("(h)help"));
  output->println(F("(i)2c"));
  output->println(F("(l)og"));
  output->println(F("(o)1-wire"));
  output->println(F("(p)param"));
  output->println(F("(s)settings"));
}


static void printFreeMemory(Print* output)
{
  nilPrintUnusedStack(output);
}






