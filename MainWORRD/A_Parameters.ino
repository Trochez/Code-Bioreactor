/*********************************************
  This file is used to declare the parameters
  table used by the program.

  We use the EEPROM for saving the parameters 
  changed by the user during the functionment 
  of the Bioreactor.
  
  The parameter are loaded during the boot.
  
  All change to important parameters are saved 
  to the EEPROM
*********************************************/

#include <avr/eeprom.h>


#define MAX_PARAM 26   // If the MAX_PARAM change you need to change the pointer in the EEPROM

#define EE_START_PARAM           0 // We save the parameter from byte 0 of EEPROM
#define EE_LAST_PARAM            (MAX_PARAM*2-1) // The last parameter is stored at byte 50-51

#define EEPROM_MIN_ADDR            0
#define EEPROM_MAX_ADDR          511




int parameters[MAX_PARAM];


void setupParameters() {
  //We copy all the value in the parameters table
  eeprom_read_block((void*)parameters, (const void*)EE_START_PARAM, MAX_PARAM*2);
}

int getParameter(byte number) {
  return parameters[number];
}

int* getParametersTable(){
   return parameters; 
}

void setParameter(byte number, int value) {
   parameters[number]=value;
}

/*
This will take time, around 4 ms
This will also use the EEPROM that is limited to 100000 writes
*/
void setAndSaveParameter(byte number, int value) {
   parameters[number]=value;
   //The address of the parameter is given by : EE_START_PARAM+number*2
   eeprom_write_word((uint16_t*) EE_START_PARAM+number, value);
}


void printParameter(Print* output, byte number){
  output->print(number);
  output->print("-");
  output->print((char)(number + 65));
  output->print(": ");
  output->println(parameters[number]);
}

void printParameters(Print* output) {
  for (int i = 0; i < MAX_PARAM; i++) {
    printParameter(output, i);
  }
}
