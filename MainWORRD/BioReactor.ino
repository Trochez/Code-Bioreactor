


#define MAX_INTEGER              -1



void setSafeConditions() {
  // we will first store all the current paramters permanently
  writeLog(ARDUINO_SET_SAFE, 0);
  setParameter(PARAM_STATUS, 0);
}

void initParameterBioreactor() {

  // We turn off all the controls
  setParameter(PARAM_STATUS,0);


#ifdef FLAG_FOOD_CONTROL
setParameterBit(PARAM_STATUS, FLAG_FOOD_CONTROL);
#endif

#ifdef STEPPER
setParameterBit(PARAM_STATUS, FLAG_STEPPER_CONTROL);
#endif





#ifdef RELAY_PUMP
  //default pump parameters
  if(getParameter(PARAM_WAIT_TIME_PUMP_MOTOR) == MAX_INTEGER){
    setAndSaveParameter(PARAM_WAIT_TIME_PUMP_MOTOR,10);    //wait time of 10 seconds, to be changed then
  }   
#endif


#ifdef TRANS_PID
  //Default Temperature parameters
  if(getParameter(PARAM_TARGET_LIQUID_TEMP) == MAX_INTEGER) {
    setAndSaveParameter(PARAM_TARGET_LIQUID_TEMP,3000);
  }

  if(getParameter(PARAM_TEMP_MAX) == MAX_INTEGER) {
    setAndSaveParameter(PARAM_TEMP_MAX,3200);
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





