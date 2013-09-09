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

#define MAX_INTEGER              65535

//Prototypes
//void setupParameters();
uint16_t getParameter(byte number);
uint16_t* getParametersTable();

void setParameter(byte number, uint16_t value);
void setAndSaveParameter(byte number, uint16_t value);

void printParameter(Print* output, byte number);
void printParameters(Print* output);

boolean sendError(uint8_t event, uint8_t value);


uint16_t parameters[MAX_PARAM];


void setupParameters() {
  //We copy all the value in the parameters table
  eeprom_read_block((void*)parameters, (const void*)EE_START_PARAM, MAX_PARAM*2);
  
  /*
  * DEFAULT PARAMETERS SET
  *
  * Set Initial value if not already defined
  *
  */
  
  if(getParameter(FLAG_VECTOR) == MAX_INTEGER) {
    setAndSaveParameter(FLAG_VECTOR,0);
  }
  
  #ifdef RELAY_PUMP
    //default pump parameters
    if(getParameter(PARAM_WAIT_TIME_PUMP_MOTOR) == MAX_INTEGER){
      setAndSaveParameter(PARAM_WAIT_TIME_PUMP_MOTOR,10);    //wait time of 10 seconds, to be changed then
    }   
    if(getParameter(PARAM_RELAY_PUMP) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_RELAY_PUMP,0);
    }
  #endif
  
  #ifdef WGHT
    if(getParameter(PARAM_LVL_MAX_WATER) == MAX_INTEGER) {    
      setAndSaveParameter(PARAM_LVL_MAX_WATER,420);
    }
      
    if(getParameter(PARAM_LVL_MIN_WATER) == MAX_INTEGER) {    
      setAndSaveParameter(PARAM_LVL_MIN_WATER,370); 
    }  
  #endif
 
  #ifdef TRANS_PID
    //Default Temperature parameters
    if(getParameter(PARAM_DESIRED_LIQUID_TEMP) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_DESIRED_LIQUID_TEMP,30000);
    }
    
    if(getParameter(PARAM_TEMP_MIN) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_TEMP_MIN,28000);
    }
    
    if(getParameter(PARAM_TEMP_MAX) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_TEMP_MAX,32000);
    }
  #endif
    
  #ifdef TAP_FOOD
    if(getParameter(PARAM_FOOD_PERIOD) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_FOOD_PERIOD,10);        //one pulse every 10 sec, default config
    }
  #endif
  
  #if defined(TAP_ACID) || defined(TAP_BASE)
    if(getParameter(PARAM_DESIRED_PH) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_DESIRED_PH,700);      //in fact corresponds to a PH of 7.0
    }
  #endif
  
  #ifdef TAP_GAS1    
    if(getParameter(PARAM_DESIRED_FLUX_GAS1) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS1,1210);  //set by default as 0 cc/min
    }
  #endif
  
  #ifdef TAP_GAS2    
    if(getParameter(PARAM_DESIRED_FLUX_GAS2) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS2,0);  //set by default as 0 cc/min
    }
  #endif
  
  #ifdef TAP_GAS3    
    if(getParameter(PARAM_DESIRED_FLUX_GAS3) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS3,0);  //set by default as 0 cc/min
    }
  #endif
  
  #ifdef TAP_GAS4    
    if(getParameter(PARAM_DESIRED_FLUX_GAS4) == MAX_INTEGER) {
      setAndSaveParameter(PARAM_DESIRED_FLUX_GAS4,0);  //set by default as 0 cc/min
    }  
  #endif

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
   eeprom_write_word((uint16_t*) EE_START_PARAM+number, value);
}

//Return true if success, false otherwise
boolean sendError(uint8_t event, uint8_t value){
  if((getParameter(FLAG_VECTOR) | EVENT_OCCURED) != 0){
     setParameter(FLAG_VECTOR, getParameter(FLAG_VECTOR) | EVENT_OCCURED);
     setParameter(PARAM_EVENT, event);
     setParameter(PARAM_EVENT_VALUE, value);
   return true;
  } else {
     return false; 
  }
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
