//setup of the plug classes given the Server set CONFIG words

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
  for(i=0;i<9;i++)
  {
   Device[i].config_number=3; //off the config table
   Device[i].type=NO_DEVICE;
   Device[i].port=0;
   Device[i].parameter=26; //  off the parameter table
   Device[i].state=0;
  }
  
}


//add conditionnal tests to avoid a bad setup (analog on I2C port or the contrary)
void setConfig(){
  
  int config_table[]={getParameter(CONFIG1),getParameter(CONFIG2),getParameter(CONFIG3)};

  char h=0;  // char h check for free class  
  char l=0;  // compteur pour définir h
  
  for(char j=0;j<3;j++)
  {
    for(char i=0;i<4;i++)
    {
    
      // find the first empty class "Device" in the Device table
      while((l<9)||(h==0))
      {
        if (Device[l].type==NO_DEVICE)
        {
          h=l;
          l=0;
        }
      
        l++;
      }
     
      switch (config_table[j]&(PORT1<<(4*i)))
      {
        case  NO_DEVICE :
          Device[h].config_number=j;
          Device[h].type=NO_DEVICE;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=26;         //  off the parameter table
          Device[h].state=0;
          break;
        
        case  WGHT :
          Device[h].config_number=j;
          Device[h].type=WGHT;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=PARAM_PORT1+i+4*j; //  Memory slot allocated to port i
          Device[h].state=0;
          break;
      
        case  TEMP :
          Device[h].config_number=j;
          Device[h].type=TEMP;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=PARAM_PORT1+i+4*j;
          Device[h].state=0;
          break;
      
        case  PH :
          Device[h].config_number=j;
          Device[h].type=PH;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=PARAM_PORT1+i+4*j; 
          Device[h].state=0;
          break;
      
        case  STEPPER :
          Device[h].config_number=j;
          Device[h].type=STEPPER;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=PARAM_PORT1+i+4*j; 
          Device[h].state=0;
          break;
        
        case  GAS_TAP :
          Device[h].config_number=j;
          Device[h].type=GAS_TAP;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=PARAM_PORT1+i+4*j; 
          Device[h].state=0;
          break;
      
        case  RELAY :
          Device[h].config_number=j;
          Device[h].type=RELAY;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=PARAM_PORT1+i+4*j; 
          Device[h].state=0;
          break;
          
        case  FLUX :
          Device[h].config_number=j;
          Device[h].type=FLUX;
          Device[h].port=(PORT1<<(4*i));
          Device[h].parameter=PARAM_PORT1+i+4*j; 
          Device[h].state=0;
          break;   
       
      
        case  default :
          break;
      }
    }
  }  
}
