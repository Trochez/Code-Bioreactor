
#if defined(TAP_GAS1_2) || defined(TAP_GAS3_4) || defined(TAP_ACID_BASE) || defined(TAP_FOOD)



NIL_WORKING_AREA(waThreadTap, 50);
NIL_THREAD(ThreadTap, arg) { 
  
  #define OPEN  1
  #define CLOSE 0
  
  #ifdef TAP_FOOD
    unsigned long previous_time_food=now();
    boolean food_state=CLOSE;
    setParameter(FOOD_PERIOD,10000);        //one pulse every 10 sec, default config
  #endif
  
  
//  unsigned long previous_time_ph=now();
//  unsigned long previous_time_gas=now();
  
  while(true){
  
    
/***********
 FOOD SUPPLY
************/

    #ifdef TAP_FOOD
    
      if( ((now()-previous_time_food) >= getParameter(FOOD_PERIOD)) & (food_state==CLOSE))
      {
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<8)); //open the tap
        previous_time_food=now();
        food_state=OPEN;
      }
      
      else if( ((now()-previous_time_food) >= OPENING_TIME) & (food_state==OPEN))
      {
        setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<0)); //close the tap
        food_state=CLOSE;
      }
      
    #endif
    
    /*PH implementation 
      1) read sensor, 
      2) adjust according to desired PH
      3) wait for a certain delay before to recheck the PH
    */
    
    /*
    #ifdef TAP_ACID
      setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<8));
    #endif 
    
    #ifdef TAP_ACID
      setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<8));
    #endif 
    */
    
    #ifdef  TAP_GAS1
    #endif
    
    #ifdef  TAP_GAS2
    #endif
    
    #ifdef  TAP_GAS3
    #endif
    
    #ifdef  TAP_GAS4
    #endif
  }
  
}

#endif
