#ifdef WGHT

NIL_WORKING_AREA(waThreadSensor, 70);      //check for memory allocation ?
NIL_THREAD(ThreadSensor, arg) 
{
    getWeight();
    nilSleepMilliseconds(500);  //refresh every 500ms
}


void getWeight(char i){
  int weight= analogRead(WGHT);                       
  setParameter(PARAM_WGHT,weight);
  if(weight>=PARAM_LVL_MAX){
    //set the condition to initiate pumping sequence
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_STEPPER_OFF));
  }
  if(weight<=PARAM_LVL_MIN){
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_STEPPER_OFF)));
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)&~(FLAG_PUMPING)));
  }
  
}  

#endif


