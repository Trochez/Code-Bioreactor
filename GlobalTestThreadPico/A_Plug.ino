//define the general structure of the data used by the program

#define MAX_DEVICES  9

class Plug{

// 0-9: temperature, 10-19: relay,
public:
	Plug Plug();
	uint8_t type;
	uint8_t port; 
	uint8_t parameter; 
	uint8_t state;             //OK, unplugged, disable
	void checkStateSensor();	
        unit8_t io();
        unit8_t pwm();
}

Plug Device[9] = {Plug(),Plug(),Plug(),Plug(),Plug(),Plug(),Plug(),Plug(),Plug()};


// is it useful ? possibly to set a global flag
void checkStateSensor()
{
  
}

unit8_t io(){
  switch (this.port){
    
    case  1:
      return IO1;
    case  2:
      return IO2;
    case  3:
      return IO3;
    case  4:
      return IO4;
    case  5:
      return IO5;
      
    default:
      break;           
  } 
}


unit8_t pwm(){
  switch (this.port){
    
    case  1:
      return PWM1;
    case  2:
      return PWM2;
    case  3:
      return PWM3;
    case  4:
      return PWM4;
    case  5:
      return PWM5;
      
    default:
      break;           
  } 
}
