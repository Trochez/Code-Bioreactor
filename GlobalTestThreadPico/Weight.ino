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
  
  //ADD condition to set flag motor off
  
}  

#endif


