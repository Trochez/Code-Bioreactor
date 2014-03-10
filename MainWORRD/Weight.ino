#ifdef WGHT

//TO BE DONE
//implement a conversion to give the volume rather than analog read.
//hard coded safety value, TO BE CHANGED ONCE THE SENSOR IS CALIBRATED and conversion performed automatically !!!!!!!!!

#define MIN_ABSOLUTE_WGHT          150
#define MAX_ABSOLUTE_WGHT          600

NIL_WORKING_AREA(waThreadWeight, 0);    
NIL_THREAD(ThreadWeight, arg) {

  int previous_weight;
  int weight = analogRead(WGHT);

  byte weight_status=0;

#define WEIGHT_STATUS_NORMAL    0
#define WEIGHT_STATUS_WAITING   1
#define WEIGHT_STATUS_EMPTYING  2
#define WEIGHT_STATUS_FILLING   3
#define WEIGHT_STATUS_ERROR     255



  unsigned long lastEventTime=now(); // when was the last food cycle

  while(true){ 

    //sensor read 
    previous_weight=weight;
    weight = 0.9*previous_weight+0.1*analogRead(WGHT);

    if (weight<MIN_ABSOLUTE_WGHT || weight>MAX_ABSOLUTE_WGHT) {
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

    setParameter(PARAM_WGHT, weight);      

#ifdef RELAY_PUMP
    switch (weight_status) {
    case WEIGHT_STATUS_NORMAL:
      if((now()-lastEventTime)>=getParameter(PARAM_MIN_FILLED_TIME)){
        weight_status=WEIGHT_STATUS_WAITING;
        lastEventTime=now(); 
        // TURN OFF ROTATION
        writeLog(MOTOR_STOP, 0);
        bitClear(PARAM_STATUS, FLAG_STEPPER_CONTROL);
      }
      break;
    case WEIGHT_STATUS_WAITING:
      if((now()-lastEventTime)>=getParameter(PARAM_WAIT_TIME_PUMP_MOTOR)){
        weight_status=WEIGHT_STATUS_EMPTYING;
        // START EMPTYING PUMP
        writeLog(PUMPING_EMPTYING_START, 0);
        bitSet(PARAM_STATUS, FLAG_EMPTYING);

        // TURN OFF ROTATION
        writeLog(MOTOR_STOP, 0);
        bitClear(PARAM_STATUS, FLAG_STEPPER_CONTROL);
      }
      break;
    case WEIGHT_STATUS_EMPTYING:
      if (weight<=getParameter(PARAM_WEIGHT_MIN)) {
        // turn off the emptying pump
        writeLog(PUMPING_EMPTYING_STOP, 0);
        bitClear(PARAM_STATUS, FLAG_FILLING);

        // TURN ON ROTATION
        writeLog(MOTOR_START, 0);
        bitSet(PARAM_STATUS, FLAG_STEPPER_CONTROL);
        // turn on filling pump
        writeLog(PUMPING_FILLING_START, 0);
        bitSet(PARAM_STATUS, FLAG_FILLING);

        weight_status=WEIGHT_STATUS_FILLING;
      }
      break;
    case WEIGHT_STATUS_FILLING:
      if (weight>=getParameter(PARAM_WEIGHT_MAX)) {
        // turn off the pump
        writeLog(PUMPING_FILLING_STOP, 0);
        bitClear(PARAM_STATUS, FLAG_FILLING);

        // when finished we specialy the last cycle
        lastEventTime=now(); 
        weight_status=WEIGHT_STATUS_NORMAL;
      }
      break;
    default: 
      // we turn off the pump
    }
#endif



#ifdef RELAY_PUMP

    //set the condition to initiate pumping sequence
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) | FLAG_STEPPER_OFF));

    //enable pumping

    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) | FLAG_PUMPING));
    setParameter(PARAM_RELAY_PUMP,(getParameter(PARAM_RELAY_PUMP)|8));


    if( (weight<=getParameter(PARAM_LVL_MIN_WATER)) & ((getParameter(FLAG_VECTOR)&(FLAG_STEPPER_OFF))==(FLAG_STEPPER_OFF))){
      //set the condition to disable the pumping sequence and re-anable stepper motor mixing
      setParameter(PARAM_RELAY_PUMP, (getParameter(PARAM_RELAY_PUMP)&~8));
      setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) & ~(FLAG_PUMPING)));
      setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR) & ~(FLAG_STEPPER_OFF)));
    }

#endif

    nilThdSleepMilliseconds(500);  //refresh every second
  }
}

#endif










