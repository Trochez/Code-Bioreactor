/*
This module reads the temperature of the different temperature sensors.
The sensor is a dallas 1-wire ds18b20.

There is only one device per plug. Instead of searching for the 
adresses of the devices, we use the skip rom command which allow 
us to directly ask any device on the line without address.

The sequence is as follow: 

We ask the sensor to update its value :
1. Issue a Reset pulse and observe the Presence of the thermometer
2. Issue the Skip Rom command (0xCC)
3. Issue the Convert T command (0×44)

The conversion in 12 bits take 750ms, so we actually read the previous value :
1. Issue a Reset pulse and observe the Presence of the thermometer
2. Issue the Skip Rom command (0xCC)
3. Issue the Read Scratchpad command (0xBE)
4. Read the next two bytes which represent the temperature
*/

#ifdef TEMP_LIQ || TEMP_PLATE || TEMP_STEPPER
  #include <OneWire.h>
  
  void getTemperature(OneWire &ow, int parameter);
  
  
  NIL_WORKING_AREA(waThreadTemp, 70);
  NIL_THREAD(ThreadTemp, arg) {
    #ifdef TEMP_CTRL
  
      #ifdef TEMP_LIQ
        OneWire oneWire1(TEMP_LIQ);
      #endif
    
      #ifdef TEMP_PLATE
        OneWire oneWire2(TEMP_PLATE);
      #endif
   
    #endif
  
  
    #ifdef STEPPER_CTRL
    
      #ifdef TEMP_STEPPER
        OneWire oneWire3(TEMP_STEPPER);
      #endif
    
    #endif
    
    while(TRUE){
    #ifdef TEMP_LIQ
      getTemperature(oneWire1, PARAM_TEMP_LIQ);
    #endif
  
    #ifdef TEMP_PLATE
      getTemperature(oneWire2, PARAM_TEMP_PLATE);
    #endif
  
    #ifdef TEMP_STEPPER
      getTemperature(oneWire3, PARAM_TEMP_STEPPER); 
    #endif
    
    nilThdSleepMilliseconds(750);
    }
  }
  
  
<<<<<<< HEAD
  void getTemperature(DallasTemperature sensor, DeviceAddress address, int bus, int parameter){
    sensor.begin();
    //if (!sensor.getAddress(address, 0)) debugger(DEBUG_ONEWIRE_NODEVICE, bus); 
    sensor.setWaitForConversion(false); // we don't wait for conversion (otherwise may take 900mS)
    // set the resolution to 12 bit (Each Dallas/Maxim device is capable of several different resolutions)
    sensor.setResolution(address, 12);
    // following instruction takes 2ms
    sensor.requestTemperatures(); // Send the coThreadOneWiremmand to get temperatures
    // we should not forget that with 12 bits it takes over 600ms to get the result so in fact we
    // will get the result of the previous conversion ...
    // Following instruction takes 14ms
    setParameter(parameter,(int)(sensor.getTempC(address)*100));
=======
  void getTemperature(OneWire &ow, int parameter){
    byte data[2];
    ow.reset();
    ow.write(0xCC);
    ow.write(0x44);   
    byte present = ow.reset();
    ow.write(0xCC);
    ow.write(0xBE);
    data[0] = ow.read();
    data[1] = ow.read();
    int16_t raw = (data[1] << 8) | data[0];
    //float celsius = (float)raw / 16.0;
    setParameter(parameter, raw*62);
>>>>>>> 7240473b50128f4ab9b105be1f7d19b2a12587e2
  }  


#endif
