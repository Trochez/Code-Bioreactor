
// weight cell sensor with 12V supply and analog output 0-5V
// min and max value to be defined for the thresholds
// The thread control the max wolume
// If there is more than one device on a bus we need to specify the devices address. Otherwise we may just scan


/* pumping control, linked to weight thread */

#ifdef THR_WGHT


NIL_WORKING_AREA(waThreadWeight, 70);      //check for memory allocation (enough // too much ?)
NIL_THREAD(ThreadWeight, arg) {
  while(TRUE){
  getWeight();
  level_ctrl();
  }
}


// weight cell reading

void getWeight() {   
    int weight= analogRead(THR_WGHT);
    setParameter(PARAM_WGHT,weight);
}

// control of maximum level, adress the motor flag

void level_ctrl(){

  /* In case we want to add a refill pump
  if(getParameter(PARAM_WGHT)<getParameter(PARAM_LVL_MIN)){
  setParameter(PARAM_RELAY_1,getParameter(PARAM_RELAY_1) & (~(1<<0))); // turns the 1st relay on
  nilThdSleepMilliseconds(3000);                                       //duration to be tuned
  }
  */
  
  if(getParameter(PARAM_LVL_MAX)<getParameter(PARAM_WGHT)){
    
  setParameter(PARAM_FLAG,getParameter(PARAM_FLAG)||(FLAG_MOTOR_OFF));  //FLAG_MOTOR_OFF to  TRUE send to the server and other modules all feeding modules to be stopped
  nilThdSleepMilliseconds(120000);                                      //2 min motor stop
  setParameter(PARAM_RELAY_1,getParameter(PARAM_RELAY_1)&(~(1<<3)));    //turns the 4th relay ON  
  nilThdSleepMilliseconds(30000);                                       //30 sec pumping
  setParameter(PARAM_FLAG,getParameter(PARAM_FLAG)&(~(FLAG_MOTOR_OFF)));//FLAG_MOTOR_OFF to false, all routines can start again
  
  }
    
  setParameter(PARAM_RELAY_1,getParameter(PARAM_RELAY_1)||((1<<3)||(1<<0)); //turns both pump off (relays off)
  nilThdSleepMilliseconds(60000);                                           //no control for 1min (or longer delay ???)
}

#endif



