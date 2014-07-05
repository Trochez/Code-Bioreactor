#define MAX_MULTI_LOG 20

void printGeneralParameters(Print* output){
  output->print(F("EPOCH:"));
  output->println(now());
  output->print(F("millis:"));
  output->println(millis());
}

void printResult(char* data, Print* output) {
  boolean theEnd=false;
  // The maximal length of a parameter value. It is a int so the value must be between -32768 to 32767
#define MAX_PARAM_VALUE_LENGTH 12
  char paramValue[MAX_PARAM_VALUE_LENGTH];
  byte paramValuePosition=0;
  byte i=0;

  while (!theEnd) {
    byte inChar=data[i];
    i++;
    if (inChar=='\0' || i==SERIAL_BUFFER_LENGTH) theEnd=true;
    if (inChar=='p') { // show settings
      printGeneralParameters(output);
    }
    else if (inChar=='d') { // show settings
      for (byte i=0; i<sizeof(DOORS); i++) {
        output->println(DOORS[i]); 
      }
    } 
    else if (inChar=='h') {
      printHelp(output);
    }
    else if (inChar=='f') { // show settings
      printFreeMemory(output);
    } 
    else if (theEnd) {
      if (data[0]=='e') {
        if (paramValuePosition>0) {
          setTime(atol(paramValue));
        } 
        else {
          output->println(now());
        }
      }
      else if (data[0]=='i') {
        if (paramValuePosition>0) {
          pushID(atol(paramValue));
        } 
        else {
          printIDs(output);
        }
      }
      else if (data[0]=='l') {
        printLogs(output);
      }
      else if (data[0]=='m') {
        if (paramValuePosition>0) {
          getLogAfter(atol(paramValue), output);
        }
        else {
          output->println("LAST LOG EPOCH");
        }
      }
    }
    else if ((inChar>47 && inChar<58) || inChar=='-') {
      if (paramValuePosition<MAX_PARAM_VALUE_LENGTH) {
        paramValue[paramValuePosition]=inChar;
        paramValuePosition++;
        if (paramValuePosition<MAX_PARAM_VALUE_LENGTH) {
          paramValue[paramValuePosition]='\0';
        }
      }
    } 
  }
}

void printHelp(Print* output) {
  //return the menu
  output->println(F("(d)oors list"));
  output->println(F("(e)poch"));
  output->println(F("(f)ree"));
  output->println(F("(h)elp"));
  output->println(F("(i)ds"));
  output->println(F("(l)og"));
  output->println(F("(m)ultiple log"));
  output->println(F("(p)arameters"));
}


static void printFreeMemory(Print* output)
{
  nilPrintUnusedStack(output);
}

















