
/* !!! reading of all the analog sensor inputs !!! */

NIL_WORKING_AREA(waThreadAnalog, 70);      //check for memory allocation
NIL_THREAD(ThreadAnalog, arg) 
{
  while(TRUE)                                    //condition sur un timer ? pour éviter la répétition du test ?
  {
    getWeight();
  }
}


void getWeight()                                // on aimerait éviter de réeffectuer le test à chaque passage dans la boucle de lecture 
{            
    int weight= analogRead(wght_port());
    setParameter(PARAM_WGHT,weight);           //modifier le paramètre à setter (param_port ... etc)     
}


int wght_port()                                //moyen d'éviter la répétition des tests conditionnels ?
{
  int wght_port
  if((getParameter(CONFIG1)&PORT1)==(WGHT<<0))
  {
      wght_port=IO1;
      return wght_port;
  }
    
  if((getParameter(CONFIG1)&PORT2)==(WGHT<<4))
  {
      wght_port=IO2;
      return wght_port;
  }
    
  if((getParameter(CONFIG1)&PORT3)==(WGHT<<8))
  {
      wght_port=IO3;
      return wght_port;
  }
    
  if((getParameter(CONFIG1)&PORT4)==(WGHT<<12))
  {
      wght_port=IO4;
      return wght_port;
  }
    
//  if((getParameter(CONFIG2)&PORT5)==(WGHT<<0))    //IO5 undefined
//  {
//      wght_port=IO5;
//      return wght_port;
//  }
    
  //ajouter si port I2_C ?
}


