// We plan to have 16 parameters that are int
// The parameters should be saved in the EEPROM
// And could be modified using a serial port.

/*********************************************
  This file is used to declare the parameters
  table used by the program.

  We use the EEPROM for saving the parameters 
  changed by the user during the functionment 
  of the Bioreactor
*********************************************/

#include <avr/eeprom.h>


#define MAX_PARAM 26   // If the MAX_PARAM change you need to change the pointer in the EEPROM

#define EE_START_PARAM           0 // We save the parameter from byte 0 of EEPROM
#define EE_LAST_PARAM            (MAX_PARAM*2-1) // The last parameter is stored at byte 50-51

#define EEPROM_MIN_ADDR            0
#define EEPROM_MAX_ADDR          511

#define MAX_INTEGER              65535


uint16_t parameters[MAX_PARAM];

void setupParameters() {
  //We copy all the value in the parameters table
  eeprom_read_block ((void*)&parameters, (const void*)EE_START_PARAM, MAX_PARAM*2);
}

uint16_t getParameter(byte number) {
  return parameters[number];
}

uint16_t* getParametersTable(){
   return parameters; 
}

void setParameter(byte number, uint16_t value) {
   parameters[number]=value;
}

/*
This will take time, around 4 ms
This will also use the EEPROM that is limited to 100000 writes
*/
void setAndSaveParameter(byte number, uint16_t value) {
   parameters[number]=value;
   //The address of the parameter is given by : EE_START_PARAM+number*2
   eeprom_write_word((uint16_t*) EE_START_PARAM+number*2, value);
}

void printParameter(Print* output, byte number){
  output->println(F("Parameter:<br/>"));
  output->print(number);
  output->print("-");
  output->print((char)(number + 65));
  output->print(": ");
  output->print(parameters[number]);
  output->println("<br/>");
}

void printParameters(Print* output) {
  output->println(F("Current settings:<br/>"));
  for (int i = 0; i < MAX_PARAM; i++) {
    output->print(i);
    output->print("-");
    output->print((char)(i + 65));
    output->print(": ");
    output->print(parameters[i]);
    output->println(F("<br/>"));
  }
}
