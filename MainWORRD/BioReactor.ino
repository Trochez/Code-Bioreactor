
// Definition of all events to be logged
#define CARD_BOOT              1
#define PUMPING_START          2
#define PUMPING_STOP           3
#define MOTOR_START            4
#define MOTOR_STOP             5
#define NO_ANSWER_NTP_SERVER   6
#define NO_ANSWER_SERVER       7
#define SENSORS_ERROR          8
#define PUMPING_FAILURE        9
#define SET_MODE_1             16
#define SET_MODE_2             17
#define SET_MODE_3             18
#define TEMP_LIQ_FAILED        20
#define TEMP_PLATE_FAILED      21
#define TEMP_STEPPER_FAILED    22

#define WGHT_FAILURE           129
#define WGHT_BACK_TO_NORMAL    130
#define ERROR_ERASE_SECTOR     140
//When parameters are changed an event is recorded (200-225 : A-Z)
#define PARAMETER_SET          200
#define ERROR_NOT_FOUND_ENTRY_N  150



#define MAX_INTEGER              -1

void initParameterBioreactor() {
  
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
      setAndSaveParameter(PARAM_LVL_MAX_WATER,350);
    }
      
    if(getParameter(PARAM_LVL_MIN_WATER) == MAX_INTEGER) {    
      setAndSaveParameter(PARAM_LVL_MIN_WATER,260); 
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


