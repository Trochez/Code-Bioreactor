
/* !!! reading of all the analog sensor inputs !!! */

NIL_WORKING_AREA(waThreadAnalog, 70);      //check for memory allocation (enough // too much ?)
NIL_THREAD(ThreadAnalog, arg) 
{
  while(TRUE)    
  {
    getWeight();
    getTemperature();
    pumping();
  }
}


// weight cell reading

void getWeight() {            


  //add condition that the weight module is present  (if {} else{nilThdSleepMilliseconds(60000)} for instance test performed on PARAM_DEVICES_1
    int weight= analogRead(THR_WGHT);
    setParameter(PARAM_WGHT,weight);
}
// control of maximum level, adress the motor flag

void pumping(){                        //add condition that the pump module is present  (if {} else{nilThdSleepMilliseconds(60000)} for instance

  if((getParameter(PARAM_LVL_MAX)<getParameter(PARAM_WGHT))&                // if level max reached and FLAG_MOTOR_OFF to 0, flag set to 1
     ((getParameter(PARAM_FLAG) & FLAG_MOTOR_OFF)== TRUE))
  {  
    setParameter(PARAM_FLAG,getParameter(PARAM_FLAG)||(FLAG_MOTOR_OFF));
    int delay_counter=0;                                                    // delay_counter linked to timer, no pumping for 30 min
  }
}


