//setup of the plug classes given the Server set CONFIG words addresed to the card


NIL_WORKING_AREA(waThreadSetup, 70);      //check for memory allocation (enough // too much ?)
NIL_THREAD(ThreadSetup, arg) 
{
  resetConfig(); 
  while(TRUE)    
  {
    setConfig();
    nilSleepMilliseconds(15000);   //check config every 15s
  }
}



void resetConfig(){
  for(i=0;i<MAX_DEVICES;i++)
  {
   Device[i].config_number=3; //off the config table
   Device[i].type=NO_DEVICE;
   Device[i].port=0;
   Device[i].parameter=26; //  off the parameter table
   Device[i].state=0;
  }
  
}


//Read the Card configuration

void setConfig(){
  
  boolean config_modif=((getParameter(FLAG_VECTOR)&CONFIG_MODIF)==CONFIG_MODIF);
  
  // conditionnal test, config is reset only if a modification occurred
  if(config_modif==TRUE){
    
    setParameter(FLAG_VECTOR)=(getParameter(FLAG_VECTOR)||CONFIG_MODIF);
    int config_table[]={getParameter(CONFIG1),getParameter(CONFIG2),getParameter(CONFIG3)};
    char h=0;  // select device index in the Device[] object table
  
    for(char j=0;j<3;j++)
    {
      for(char i=0;i<4;i++)
      {
        if(h<MAX_DEVICES){
   
          Device[h].port=1+i+4*j;                // Allocated PORT
          Device[h].parameter=PARAM_PORT1+i+4*j; // Memory slot allocated to port i
//        Device[h].state=0;                     // Right to do that or not ???
          
          //sanity check for non-I2C device on PORT<=5
          if(Device[h].port<=5){
            switch (config_table[j]&(PORT_MASK<<(4*i)))
            {
              case  NO_DEVICE :
                Device[h].type=NO_DEVICE;
                Device[h].parameter=26;                 //  off the parameter table
                break;
              
              case  WGHT :
                Device[h].type=WGHT;
                break;
            
              case  TEMP :
                 Device[h].type=TEMP;
                 break;
            
              case  PH :
                Device[h].type=PH;
                break;
            
              case  STEPPER :
                Device[h].type=STEPPER;
                break;
              
              case  GAS_TAP :
                Device[h].type=GAS_TAP;
                break;
            
              default : 
                Device[h].type=NO_DEVICE;
                Device[h].parameter=26;
//              setParameter()            //possibly change configuration word but not necessary
                break;
            }
          }
          
          //sanity check for I2C devices
          else{
             switch (config_table[j]&(PORT_MASK<<(4*i))){
                case  NO_DEVICE :
                  Device[h].type=NO_DEVICE;
                  Device[h].parameter=26;                 
                  break;
              
                case  RELAY :
                  Device[h].type=RELAY;
                  break;
                  
                case  FLUX :
                  Device[h].type=FLUX;
                  break;        
          
              default : 
                Device[h].type=NO_DEVICE;
                Device[h].parameter=26;
//              setParameter()            //possibly change configuration word but not necessary
                break;
              }
              
          }
          
         h++;
         
        }
      }
    }  
  }
}
