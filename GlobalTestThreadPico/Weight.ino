
// weight cell sensor with 12V supply and analog output 0-5V
// min and max value to be defined for the thresholds
// The thread control the max wolume
// If there is more than one device on a bus we need to specify the devices address. Otherwise we may just scan
/* pumping control, linked to weight thread */

#ifdef THR_WGHT


NIL_WORKING_AREA(waThreadWeight, 70);      //check for memory allocation (enough // too much ?)
NIL_THREAD(ThreadWeight, arg) 
{
  while(TRUE)    
  {
    getWeight();
    pumping();
  }
}


// weight cell reading

void getWeight() {                  //add condition that the weight module is present  (if {} else{nilThdSleepMilliseconds(60000)} for instance test performed on PARAM_DEVICES_1
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
 
  if(((getParameter(PARAM_FLAG) & FLAG_MOTOR_OFF)== TRUE))
  {  //condition compteur timer + flag motor
//    nilThdSleepMilliseconds(120000);                                      //2 min motor stop (change to 10 minutes) --> pb, no error routine served here !!!!
//    setParameter(PARAM_RELAY_1,getParameter(PARAM_RELAY_1)&(~(1<<3)));    //turns the 4th relay ON 
  
    if((getParameter(PARAM_WGHT)>getParameter(PARAM_LVL_MIN))&((getParameter(PARAM_FLAG)&(FLAG_PUMPING))==TRUE))  // pumping goes on as long as the
    {
      nilThdSleepMilliseconds(5000);                                        //5 sec pumping  
    }
  
    else
    {
      setParameter(PARAM_FLAG,getParameter(PARAM_FLAG)&(~(FLAG_PUMPING)));      //FLAG_MOTOR_OFF to false, all routines can start again
      setParameter(PARAM_RELAY_1,getParameter(PARAM_RELAY_1)||(1<<3));         //turns pump off (relay is off)
      nilThdSleepMilliseconds(120000);                                         //no control for 2min (or longer delay ???)
    }
  }
}


#endif



