#ifdef TEMP_LIQ||TEMP_PLATE||TEMP_STEPPER

  #ifdef TEMP_CTRL
  
    #ifdef TEMP_LIQ
      OneWire oneWire1(TEMP_LIQ);
      DallasTemperature sensors1(&oneWire1);
      DeviceAddress oneWireAddress1;
    #endif
  
    #ifdef TEMP_PLATE
      OneWire oneWire2(TEMP_PLATE);
      DallasTemperature sensors2(&oneWire2);
      DeviceAddress oneWireAddress2;
    #endif
  
  #endif
  
  
  #ifdef STEPPER_CTRL
  
    #ifdef TEMP_STEPPER
      OneWire oneWire3(TEMP_STEPPER);
      DallasTemperature sensors3(&oneWire3);
      DeviceAddress oneWireAddress3;
    #endif
    
  #endif
  
  
  NIL_WORKING_AREA(waThreadTemp, 70);
  NIL_THREAD(ThreadTemp, arg) {
    
    while(TRUE){
    #ifdef TEMP_LIQ
      getTemperature(sensors1, oneWireAddress1, TEMP_LIQ, TEMP_LIQ);
    #endif
  
    #ifdef TEMP_PLATE
      getTemperature(sensors2, oneWireAddress2, TEMP_PLATE, TEMP_PLATE);
    #endif
  
    #ifdef TEMP_STEPPER
      getTemperature(sensors3, oneWireAddress3, TEMP_STEPPER, TEMP_STEPPER); 
    #endif
    
    nilThdSleepMilliseconds(500);
    }
  }
  
  
  
void getTemperature(DallasTemperature sensor, DeviceAddress address, int bus, int parameter){
   sensor.begin();
  if (!sensor.getAddress(address, 0)) debugger(DEBUG_ONEWIRE_NODEVICE, bus); 
  sensor.setWaitForConversion(false); // we don't wait for conversion (otherwise may take 900mS)
  // set the resolution to 12 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensor.setResolution(address, 12);
    // following instruction takes 2ms
    sensor.requestTemperatures(); // Send the coThreadOneWiremmand to get temperatures
    // we should not forget that with 12 bits it takes over 600ms to get the result so in fact we
    // will get the result of the previous conversion ...
    // Following instruction takes 14ms
    setParameter(parameter,(int)(sensor.getTempC(address)*100));
  
}

#endif

