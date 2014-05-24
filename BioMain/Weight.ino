#ifdef WEIGHT

//TO BE DONE
//implement a conversion to give the volume rather than analog read.



#ifdef RELAY_PUMP
int cycleCounter=0;
#define PUMP_SLOWDOWN_RATIO          10  // this would open for 5s and then wait the ratio
#endif

NIL_WORKING_AREA(waThreadWeight, 32);    // minimum of 32 !
NIL_THREAD(ThreadWeight, arg) {
  nilThdSleepMilliseconds(500);
  int previous_weight;
  int weight = analogRead(WEIGHT);

  byte weight_status=0;

#define WEIGHT_STATUS_NORMAL    0
#define WEIGHT_STATUS_WAITING   1
#define WEIGHT_STATUS_EMPTYING  2
#define WEIGHT_STATUS_FILLING   3
#define WEIGHT_STATUS_STANDBY   4
#define WEIGHT_STATUS_ERROR     7

  unsigned long sinceLastEvent=0; // when was the last food cycle
  unsigned long lastCycleMillis=millis(); // when was the last food cycle

  if (getParameter(PARAM_WEIGHT_STATUS)!=-1) {
    weight_status=(((uint16_t)getParameter(PARAM_WEIGHT_STATUS)) >> 13);
Serial.println(weight_status);
    
    sinceLastEvent=(((uint16_t)getParameter(PARAM_WEIGHT_STATUS))&0b0001111111111111)*60000;
    Serial.println(sinceLastEvent);
  }

  while(true){ 
    //sensor read 
    previous_weight=weight;
    weight = 0.9*previous_weight+0.1*analogRead(WEIGHT);


    if(readParameterBit(PARAM_STATUS, FLAG_FOOD_CONTROL)==0) {
      weight_status=WEIGHT_STATUS_STANDBY;
    } 
    else {
      if (weight<MIN_ABSOLUTE_WEIGHT || weight>MAX_ABSOLUTE_WEIGHT) {
        if (weight_status!=WEIGHT_STATUS_ERROR) {
          writeLog(WEIGHT_FAILURE,0);
          weight_status=WEIGHT_STATUS_ERROR;
        } 
      } 
      else {
        if (weight_status==WEIGHT_STATUS_ERROR) {
          writeLog(WEIGHT_BACK_TO_NORMAL,0);
          weight_status=WEIGHT_STATUS_NORMAL;
        }
      }
    }


    setParameter(PARAM_WEIGHT, weight);      
    setParameter(PARAM_WEIGHT_STATUS, (((uint16_t)(sinceLastEvent/60000)) | ((uint16_t)(weight_status<<13))));

#ifdef RELAY_PUMP

    switch (weight_status) {
    case WEIGHT_STATUS_NORMAL:
      if(( (uint16_t)(sinceLastEvent/60000))>=getParameter(PARAM_MIN_FILLED_TIME)){
        weight_status=WEIGHT_STATUS_WAITING;
        sinceLastEvent=0; 
        writeLog(PUMPING_WAITING, 0);
        // TURN OFF ROTATION
        writeLog(MOTOR_STOP, 0);
        clearParameterBit(PARAM_STATUS, FLAG_STEPPER_CONTROL);
      }
      break;
    case WEIGHT_STATUS_WAITING:
      if(( (uint16_t)(sinceLastEvent/60000))>=getParameter(PARAM_SEDIMENTATION_TIME)){
        weight_status=WEIGHT_STATUS_EMPTYING;
        // START EMPTYING PUMP
        writeLog(PUMPING_EMPTYING_START, 0);
        setParameterBit(PARAM_STATUS, FLAG_RELAY_EMPTYING);
         sinceLastEvent=0; 
      }
      break;
    case WEIGHT_STATUS_EMPTYING:
      if (weight<=getParameter(PARAM_WEIGHT_MIN)) {
        // turn off the emptying pump
        writeLog(PUMPING_EMPTYING_STOP, 0);
        clearParameterBit(PARAM_STATUS, FLAG_RELAY_EMPTYING);

        // TURN ON ROTATION
        writeLog(MOTOR_START, 0);
        setParameterBit(PARAM_STATUS, FLAG_STEPPER_CONTROL);

        // turn on filling pump
        writeLog(PUMPING_FILLING_START, 0);
        setParameterBit(PARAM_STATUS, FLAG_RELAY_FILLING);

        weight_status=WEIGHT_STATUS_FILLING;
         sinceLastEvent=0; 
      }
      break;
    case WEIGHT_STATUS_FILLING:
      if (weight>=getParameter(PARAM_WEIGHT_MAX)) {
        // turn off the pump
        writeLog(PUMPING_FILLING_STOP, 0);
        clearParameterBit(PARAM_STATUS, FLAG_RELAY_FILLING);

        // when finished we specialy the last cycle
        sinceLastEvent=0; 
        weight_status=WEIGHT_STATUS_NORMAL;
      }
      break;
    case WEIGHT_STATUS_ERROR:
      clearParameterBit(PARAM_STATUS, FLAG_RELAY_FILLING);
      clearParameterBit(PARAM_STATUS, FLAG_RELAY_EMPTYING);
    }
#endif

    nilThdSleepMilliseconds(500);  //refresh every second

    sinceLastEvent+=millis()-lastCycleMillis;
    lastCycleMillis=millis();
  }
}

#endif




















