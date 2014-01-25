#if defined(TEMP_LIQ) || defined(TEMP_PLATE) || defined(TEMP_STEPPER)

/*******************************************
*  THREAD TEMPERATURE
*  This module reads the temperature of the different temperature sensors.
*  The sensor is a dallas 1-wire ds18b20.
*  
*  There is only one device per plug. Instead of searching for the 
*  adresses of the devices, we use the skip rom command which allow 
*  us to directly ask any device on the line without address.
*  
*  The sequence is as follow: 
*  
*  We ask the sensor to update its value :
*  1. Issue a Reset pulse and observe the Presence of the thermometer
*  2. Issue the Skip Rom command (0xCC)
*  3. Issue the Convert T command (0×44)
*  
*  The conversion in 12 bits take 750ms, so we actually read the previous value :
*  1. Issue a Reset pulse and observe the Presence of the thermometer
*  2. Issue the Skip Rom command (0xCC)
*  3. Issue the Read Scratchpad command (0xBE)
*  4. Read the next two bytes which represent the temperature
*********************************************/

#include <OneWire.h>


void getTemperature(OneWire &ow, int parameter, byte * loggedErrorTemp, uint16_t logNumber);
  
  
NIL_WORKING_AREA(waThreadTemp, 150);
NIL_THREAD(ThreadTemp, arg) {
  nilThdSleepMilliseconds(1000);
  #ifdef TEMP_CTRL

    #ifdef TEMP_LIQ
      OneWire oneWire1(TEMP_LIQ);
      byte errorTempLiq = false;
    #endif
  
    #ifdef TEMP_PLATE
      OneWire oneWire2(TEMP_PLATE);
      byte errorTempPlate = false;
    #endif
 
  #endif


  #ifdef STEPPER_CTRL
  
    #ifdef TEMP_STEPPER
      OneWire oneWire3(TEMP_STEPPER);
      byte errorTempStepper = false;
    #endif
  
  #endif
  
  while(true){
    #ifdef TEMP_LIQ
      getTemperature(oneWire1, PARAM_TEMP_LIQ, &errorTempLiq, TEMP_LIQ_FAILED);
    #endif
  
    #ifdef TEMP_PLATE
      getTemperature(oneWire2, PARAM_TEMP_PLATE, &errorTempPlate, TEMP_PLATE_FAILED);
    #endif
  
    #ifdef TEMP_STEPPER
      getTemperature(oneWire3, PARAM_TEMP_STEPPER, &errorTempStepper, TEMP_STEPPER_FAILED); 
    #endif
    
    nilThdSleepMilliseconds(5000);
  }
}
  
  
void getTemperature(OneWire &ow, int parameter, byte * loggedErrorTemp, uint16_t logNumber){
  byte data[2];
  ow.reset();
  ow.write(0xCC);
  ow.write(0x44);   

  //We use the return of the reset function to check if the device is present
  byte present = ow.reset();
  // if(present == 0) => one error occured
  // if(*errorTemp) == false => The error has not been logged
  
  //if error & non logged
  if (present == 0){
    //We log the error if not done before
    if((*loggedErrorTemp) == 0){
      if(sendError(logNumber, 0)){
        (*loggedErrorTemp) = 1; 
      }
    }
  }
  //no error
  else {
    //we can log new error again
    if ((*loggedErrorTemp) == 1){
     (*loggedErrorTemp) = 0; 
    }
    //We get the new temperature
    ow.write(0xCC);
    ow.write(0xBE);
    data[0] = ow.read();
    data[1] = ow.read();
    int16_t raw = (data[1] << 8) | data[0];
    //float celsius = (float)raw / 16.0;
    setParameter(parameter, ((long)raw*625)/100);
  }
}  


#endif
