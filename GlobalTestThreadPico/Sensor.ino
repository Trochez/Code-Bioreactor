// the code addresses all non-I2C sensors


//!!!!!!!!!  TO BE DONE  !!!!!!!!!!!
// add sanity checks, the sensor values should be in a reasonnable range (chack if pluged for analogic case) What is reasonable ? 
// modify the oneWire library so we don't need to get the address to read the sensor

NIL_WORKING_AREA(waThreadSensor, 70);      //check for memory allocation ?
NIL_THREAD(ThreadSensor, arg) 
{
  while(TRUE)    
  {
    getSensor();
    nilSleepMilliseconds(500);  //refresh every 500ms
  }
}


void getSensor() {
  
    for(char i=0;i<MAX_DEVICES;i++)
    {
      switch(Device[i].type)
      {
        case WGHT :
          getWeight(i);
          break;
        case PH :
          getPh(i);
          break;
        case TEMP :       
          getTemp(i);
          break;
        default :
          break;
      }
    }
}
 

void getWeight(char i){
  
  int weight= analogRead(Device[i].io());                       
  setParameter(Device[i].parameter,weight);
  if(weight>=getParameter(PARAM_LVL_MAX)){
    setParameter(FLAG_VECTOR,(getParameter(FLAG_VECTOR)||FLAG_STEPPER_OFF));                       //STEPPER_OFF FLAG !!, enable pumping sequence
    }
  }
  
}  

void getPh(char i){
  
  // to be implemented
  
}


void getTemp(char i){
  
OneWire oneWire1(Device[i].io());                                     
DallasTemperature sensors1(&oneWire1);
DeviceAddress oneWireAddress1;
getTemperature(sensors1, oneWireAddress1, Device[i].port, Device[i].parameter);
  
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



