
#if defined(TAP_GAS1_2) || defined(TAP_GAS3_4) || defined(TAP_ACID_BASE) || defined(TAP_FOOD)



NIL_WORKING_AREA(waThreadTap, 50);
NIL_THREAD(ThreadTap, arg) { 
  
  #ifdef TAP_FOOD
    
  #endif
  
  #ifdef
  #endif
  
  #ifdef
  #endif
  
  while(true){
 
    #ifdef  TAP_GAS1
    #endif
    
    #ifdef  TAP_GAS2
    #endif
    
    #ifdef  TAP_GAS3
    #endif
    
    #ifdef  TAP_GAS4
    #endif
  
    #ifdef TAP_FOOD
      
      setParameter(PARAM_RELAY_TAP,getParameter(PARAM_RELAY_TAP)|(8<<8)); 
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
  }
  
}

#endif
