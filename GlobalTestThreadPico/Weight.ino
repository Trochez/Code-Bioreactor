
// weight cell sensor with 12V supply and analog output 0-5V
// min and max value to be defined for the thresholds

// If there is more than one device on a bus we need to specify the devices address. Otherwise we may just scan
#ifdef THR_WGHT


NIL_WORKING_AREA(waThreadWeight, 70);
NIL_THREAD(ThreadWeight, arg) {
  getWeight();
}



void getWeight(){

  while (TRUE) {

    
    int weight= analogRead(THR_WGHT);
    setParameter(PARAM_WGHT,weight);
    
    nilThdSleepMilliseconds(1000);
  }
  
}

#endif



