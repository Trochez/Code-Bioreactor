//define the general structure of the data used by the program


class Plug{

// 0-9: temperature, 10-19: relay,
public:
	Plug Plug();
        unit8_t config_number;
	uint8_t type;
	uint8_t port; 
	uint8_t parameter; 
	uint8_t state;             //OK, unplugged, disable
	void checkStateSensor();	
}

/* Temp inheritance for Dallas 1-Wire */

class Temp : public Plug{
 
  public:
  
    OneWire oneWire1;
    DallasTemperature sensors;
    DeviceAddress oneWireAddress; 
    void initTemp()
}
  

void initTemp(){
    this.type = TEMP;  //...
    this.onWire1=oneWire1(this.port);
    this.sensors=sensors(&oneWire1);
    this.oneWireAddress=oneWireAddress; 
}


Plug Device[9] = {Plug(),Plug(),Plug(),Plug(),Plug(),Plug(),Plug(),Plug(),Plug()};


void checkStateSensor()
{
  
}
