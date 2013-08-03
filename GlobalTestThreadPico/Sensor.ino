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
  
  int weight= analogRead(Device[i].io());                        //change because it is not directly 'PORT' that should be used
  setParameter(Device[i].parameter,weight);
  if(weight>=getParameter(PARAM_LVL_MAX)){
      setParameter();
      setParameter();
    }
    setParameter(

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



/*
// function to print a oneWire address
void printOneWireAddress(DeviceAddress deviceAddress, Print* output)
{
    if (*deviceAddress < 16) output->print("0");
    else output->print(*deviceAddress, HEX);
}

/*Does not work at the moment
void oneWireInfo(Print* output) {
  output->println("One wire");
  // Loop through each device, print out address
  for(int i=0;i<sensors1.getDeviceCount(); i++)
  {
    // Search the wire for address
    if (sensors1.getAddress(oneWireAddress, i))
    {
      output->print(i);
      output->print(": ");
      printOneWireAddress(oneWireAddress, output);
      output->println();
    }
  }
}
*/


