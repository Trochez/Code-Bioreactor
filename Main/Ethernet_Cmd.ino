/*--------------------------------

This thread will take care of the Ethernet communications

-Answer to request from the server (get logs, get parameters)
-Get state updates from the server / other modules

TODO:
-Add parsing for the new commands
!! Need to set the flag for CONFIG_MODIF in FLAG_VECTOR

--------------------------------*/
#ifdef THR_ETHERNET

void ethernetSetup();
void ethernetPushLog(char *logString);
void ethernetPushStatus();
void ethernetReadCommand();

#define DEBUG 0
#define MAX_HTTP_STRING_LENGTH 2048    // in bytes; max. http return string to read  
// (reserve space for the http header !AND! the JSON command string)
#define MAX_COMMAND_STRING_LENGTH 400 // in bytes; max. JSON return string to read 

// THIS SHOULD BE DEFINED GLOBALLY SO THREADS CAN ACCESS IT 
// Enter a MAC address and IP address for the Arduino controller below.
// The IP address is reserved on the routher for this Arduino controller.
// CAUTION
// Each different boards should have a different IP in the range 172.17.0.100 - 172.17.0.200
// and a different MAC address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = { 172,17,0,100}; // reserved IP adress of the Arduino

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
unsigned int localPort = 8888;      // local port to listen for UDP packets
EthernetServer server(80);

IPAddress alix_server(172,17,0,10); // local NTP server 

/*---------------------------
  Ethernet Thread
---------------------------*/

NIL_WORKING_AREA(waThreadEthernet, 50); //change memoy allocation
NIL_THREAD(ThreadEthernet, arg) {
  
  // Initializate the connection with the server
  ethernetSetup();
  
  while (TRUE) {
    
     // listen for client request
    EthernetClient client = server.available();
    if (client) {
    
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          ethernetSendLog();
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
  }  
    nilThdSleepMilliseconds(1000);
  }
}

/*----------------------------
  Ethernet related functions
----------------------------*/

void ethernetSetup() {
  // the ethernet pin has been initialized in the main setup() from Bioreactor
  Ethernet.begin(mac,ip);
  server.begin();
}  

/*
//kept here as an example
void ethernetPushStatus() {
  if(DEBUG)Serial.println("PUSH STATUS: Connecting to server...");
  ethernetOpenConnection80();
  if (client.connected()) {

    char pushUrl[50];
    strcpy(pushUrl,"GET /bioReacTor/setStatus.php?");
    char mode[3];
    itoa(0 ,mode,10); //write state vector (replace 0 with the correct bytes)
    strcat(pushUrl,mode);
    strcat(pushUrl," HTTP/1.0\n\n");

    if (DEBUG) Serial.print("Push URL: ");
    if (DEBUG) Serial.println(pushUrl);
    client.println(pushUrl);
    if (DEBUG) Serial.println("The status has been pushed to the server.");
  }
  client.stop();
}

void ethernetOpenConnection80() {
  client.connect(alix_server,80);
  long start=millis(); 
  while (! client.connected() && ((millis()-start)<500))
  {
    delay(1); //Could we do NilThreadSleep here ?
  }
  if (DEBUG) Serial.print("Connection achieved in (ms): ");
  if (DEBUG) Serial.println(millis()-start);
}
*/
void ethernetSendLog(){
  //TODO
  client.println("Test 1 <p> test 2 ");
  for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
  }
}
/*
//JSON PARSING
void ethernetReadCommand() {
 String jsonCommand = String(""); // reinitialize alocated string
  //char jsonCommandBuffer[MAX_HTTP_STRING_LENGTH]; // reserve space
  //  if(DEBUG)Serial.print("String initialized has length of: ");
  //  if(DEBUG)Serial.println(sizeof(jsonCommand));
  //open a connection and try to read the JSON command 
  if(DEBUG)Serial.println("Connecting to get newcommand...");

  ethernetOpenConnection80();

  // read the JSON object from the server and save it in a local String
  // for more informations look up www.json.org
  int jsonController = 0;
  client.println("GET /bioReacTor/command HTTP/1.0\n"); //connect to the command page of the server

  int STATUS=0; // the JSON didn't start yet
  
  // 1 : the JSON started
  // 2 : we get the field name
  // 3 : we get the value
   

  long start=millis();
  char fieldName[20];
  char fieldValue[10];
  while (client.connected() && ((millis()-start)<1000)) //TODO: change condition or allow nilThreadSleep()
  {
    if (client.available()) {

      char readChar = client.read();
      //    if(DEBUG) Serial.print(readChar);

      switch(STATUS)
      {
      case 0:
        if(readChar == '{') {
          STATUS=1;
          fieldName[0]='\0';
          fieldValue[0]='\0';
        }
        break;
      case 1: // we are receiving the fieldName
        if(readChar == ':') {
          STATUS=2;
        } 
        else {
          int len=strlen(fieldName);
          fieldName[len] = readChar;
          fieldName[len + 1] = '\0';
        }
        break;

      case 2:
        if(readChar == ',') {
          ethernetParseCommandValue(fieldName, atof(fieldValue));
          fieldName[0]='\0';
          fieldValue[0]='\0';
          STATUS=1;
        } 
        else if (readChar == '}') {
          ethernetParseCommandValue(fieldName, atof(fieldValue));
          fieldName[0]='\0';
          fieldValue[0]='\0';
          STATUS=3;
        } 
        else {
          int len=strlen(fieldValue);
          fieldValue[len] = readChar;
          fieldValue[len + 1] = '\0';
        }
        break;
      case 3:
        break;
      default:
        Serial.println("ERROR in fetching command");
      }
    }
  } // end while
  client.stop();
  if (DEBUG) Serial.print("Received command file in (ms): ");
  if (DEBUG) Serial.println(millis()-start);
}

void ethernetParseRequest(char *fieldName, double extractedValueFloat){
 //---------------------------------------
   The module has to be able to respond to 
   several HTML request.
   
   1. Help menu
   2. State vector
   3. logs
   4. i2c devices
   5. 1-wire devices
 //---------------------------------------
 int extractedValueInt=(int)extractedValueFloat;
  if (strcmp(fieldName,"StateVector")==0) {
     ethernetSendStatus();
  } else if (strcmp(fieldName,"helpMenu")==0) {
    ethernetSendPlugStatus();
  } else {
    //Error, request non existant
  }
}

----------------------------
  Parameter & requests related functions
----------------------------

void ethernetSendStatus(){
  //return the parameters corresponding to the vector state
}

void ethernetSendPlugStatus(){
 //return the state of all the module plug on the device 
}

void ethernetSendSensorLog(uint8_t device){
  //return the logs of a specific sensor
}

void ethernetSendGeneralLog(){
  //return the last X entry in the command log
}

void ethernetPrintHelp() {
  //return the menu
  
//  Serial.println(F("(d)ebug"));
//  Serial.println(F("(e)eprom"));
//  Serial.println(F("(f)ree mem"));
//  Serial.println(F("(h)elp"));
//  Serial.println(F("(i)2c"));
//  Serial.println(F("(l)og"));
//  #ifdef ONE_WIRE_BUS1 //TBC
//  Serial.println(F("(o)ne wire"));
//  #endif
//  Serial.println(F("(s)ettings"));

}

void ethernetPrintI2C(){
  //return the I2C devices plugged on the device
}

void ethernetPrintOneWire(){
  //return the One-Wire devices pluggedf on the device
}

void ethernetParseCommandValue(char *fieldName, double extractedValueFloat)
{
  int extractedValueInt=(int)extractedValueFloat;
  if (strcmp(fieldName,"liquidTemp")==0) {
    // first check if there is a difference between the read value and the stored one
    // (because floats are used, an error value of 0.05 has been set!)
    if(HEATING_TEMPERATURE_LIMIT >= extractedValueFloat + 0.05 || HEATING_TEMPERATURE_LIMIT <= extractedValueFloat-0.05)
    {
      // check if the input value is valid, then safe it
      if(extractedValueFloat > HEATING_MAX_ALLOWED_LIMIT)
      {
        if(DEBUG)Serial.println("WARNING: The input temperature is to high! New temperature value has not been set.");
      }
      else if (extractedValueFloat < HEATING_MIN_ALLOWED_LIMIT)
      {
        if(DEBUG)Serial.println("WARNING: The input temperature is to low! New temperature value has not been set.");
      }
      else
      {
        HEATING_TEMPERATURE_LIMIT = extractedValueFloat;
        if(DEBUG)Serial.print("The new temperature has been successfully set to: [C]");
        if(DEBUG)Serial.println(HEATING_TEMPERATURE_LIMIT);
      }
    }
    else if(DEBUG)Serial.println("The set temperature is the same as the saved one (deviation <0.05).");
  } 
  else if (strcmp(fieldName,"liquidLevelMax")==0) {
    if(extractedValueInt != LIQUID_LEVEL_WEB_MAX)
    {
      // check if the input value is valid, then safe it
      if(extractedValueInt > LIQUID_LEVEL_PHYSICAL_MAX_INTERVAL)
      {
        if(DEBUG)Serial.println("WARNING: The input liquid level is to high! New liquid value has not been set.");
      }
      else if (extractedValueInt < LIQUID_LEVEL_PHYSICAL_MIN_INTERVAL)
      {
        if(DEBUG)Serial.println("WARNING: The input liquid level negative! New liquid value has not been set.");
      }
      else
      {
        LIQUID_LEVEL_WEB_MAX = extractedValueInt;
        if(DEBUG)Serial.print("The new liquid level has been successfully set to [Arduino AD interval]: ");
        if(DEBUG)Serial.println(LIQUID_LEVEL_WEB_MAX);
      }
    }
    else if(DEBUG)Serial.println("The set liquid level is the same as the saved one.");
  } 
  else if (strcmp(fieldName,"liquidLevelMin")==0) {
    if(extractedValueInt != LIQUID_LEVEL_WEB_MAX)
    {
      // check if the input value is valid, then safe it
      if(extractedValueInt > LIQUID_LEVEL_PHYSICAL_MAX_INTERVAL)
      {
        if(DEBUG)Serial.println("WARNING: The input liquid level is to high! New liquid value has not been set.");
      }
      else if (extractedValueInt < LIQUID_LEVEL_PHYSICAL_MIN_INTERVAL)
      {
        if(DEBUG)Serial.println("WARNING: The input liquid level negative! New liquid value has not been set.");
      }
      else
      {
        LIQUID_LEVEL_WEB_MIN = extractedValueInt;
        if(DEBUG)Serial.print("The new liquid level has been successfully set to [Arduino AD interval]: ");
        if(DEBUG)Serial.println(LIQUID_LEVEL_WEB_MIN);
      }
    }
    else if(DEBUG)Serial.println("The set liquid level is the same as the saved one.");
  } 
  else if (strcmp(fieldName,"pH")==0) {
    // first check if there is a difference between the read value and the stored one
    // (because floats are used, an error value of 0.05 has been set!)
    if(pH_SET >= extractedValueFloat+0.05 || pH_SET <= extractedValueFloat-0.05)
    {
      // check if the input value is valid, then safe it
      if(extractedValueFloat > 14.0) // max pH level is 14
      {
        if(DEBUG)Serial.println("WARNING: The input pH level is to high! New pH value has not been set.");
      }
      else if (extractedValueFloat < 0.0) // min pH level is 0
      {
        if(DEBUG)Serial.println("WARNING: The input pH level negative! New pH value has not been set.");
      }
      else
      {
        pH_SET = extractedValueFloat;
        if(DEBUG)Serial.print("The new pH level has been successfully set to: ");
        if(DEBUG)Serial.println(pH_SET);
      }
    }
    else if(DEBUG)Serial.println("The set temperature is the same as the saved one (deviation <0.05).");

  } 
  else if (strcmp(fieldName,"waitTime")==0) {
    // Value read from WebUI is in min; the timer uses millisec!
    if(extractedValueInt != WAIT_TIME_BEFORE_PUMPING_OUT)
    {
      // check if the input value is valid, then safe it
      if(extractedValueInt * MIN_TO_SEC > WAIT_TIME_BEFORE_PUMPING_OUT_MAX) // min und max values in [sec]!
      {
        if(DEBUG)Serial.print("WARNING: The Wait Time is to high! Maximum allowed Wait Time is [sec]:");
        if(DEBUG)Serial.println(WAIT_TIME_BEFORE_PUMPING_OUT_MAX);
      }
      else if (extractedValueInt * MIN_TO_SEC < WAIT_TIME_BEFORE_PUMPING_OUT_MIN)
      {
        if(DEBUG)Serial.print("WARNING: The Wait Time is to low! Minimum allowed Wait Time is [sec]:");
        if(DEBUG)Serial.println(WAIT_TIME_BEFORE_PUMPING_OUT_MIN);
      }
      else
      {
        WAIT_TIME_BEFORE_PUMPING_OUT = extractedValueInt; //in [min]
        PUMPING_OUT_TIMER = WAIT_TIME_BEFORE_PUMPING_OUT * MIN_TO_SEC * SEC_TO_MILLISEC; // in [millisec]
        timerPumpingOut.setInterval(PUMPING_OUT_TIMER); 
        if(DEBUG)Serial.print("The new Wait Time has been successfully set to [millisec]: ");
        if(DEBUG)Serial.println(PUMPING_OUT_TIMER);
      }
    }
    else if(DEBUG)Serial.println("The Wait Time is the same as the saved one.");
  } 
  else if (strcmp(fieldName,"methaneIn")==0) {
    
    
  } 
  else if (strcmp(fieldName,"carbonDioxideIn")==0) {

  } 
  else if (strcmp(fieldName,"nitrogenIn")==0) {

  } 
  else if (strcmp(fieldName,"liquidIn")==0) {

  } 
  else if (strcmp(fieldName,"liquidOut")==0) {

  } 
  else if (strcmp(fieldName,"mode")==0) {
    // first check if there is a difference between the read value and the stored one
    // then check if in pumping mode; ONLY change from pumping mode if MANUAL mode is selected
    if(extractedValueInt != BIOREACTOR_MODE
      && (BIOREACTOR_MODE != BIOREACTOR_PUMPING_MODE 
      || (BIOREACTOR_MODE == BIOREACTOR_PUMPING_MODE && extractedValueInt == BIOREACTOR_MANUAL_MODE)))
    {
      // check if the input value is valid, then safe it
      if(extractedValueInt == BIOREACTOR_STANDBY_MODE
        || extractedValueInt == BIOREACTOR_RUNNING_MODE
        || extractedValueInt == BIOREACTOR_MANUAL_MODE)
      {

        //switch Bioreactor mode
        BIOREACTOR_MODE = extractedValueInt;
        if(DEBUG)Serial.print("The Bioreactor has been set to a new state: ");
        if(DEBUG)Serial.println(BIOREACTOR_MODE);
      }
      else
      {
        if(DEBUG)Serial.println("WARNING: The Bioreactor state is invalid!.");
      }
    }
    else if(DEBUG)Serial.println("The Bioreactor state is the same as the saved one.");

  } 
  else if (strcmp(fieldName,"pumpOut")==0) {
    //---------Set pumpOut state ----------
    // only get the pump's state if in MANUAL mode
    if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
    {


      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != relaySwitchPumpOutGetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) relaySwitchPumpOutTurnOn();
          else relaySwitchPumpOutTurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("PumpOut has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: PumpOut state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("PumpOut state is the same as the saved one.");
    }
    else if(DEBUG) Serial.println("Not in MANUAL mode: PumpOut's value not taken");
  } 
  else if (strcmp(fieldName,"pumpIn")==0) {

    //---------Set pumpIn state ----------
    // only get the pump's state if in MANUAL mode
    if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
    {
      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != relaySwitchPumpInGetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) relaySwitchPumpInTurnOn();
          else relaySwitchPumpInTurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("PumpIn has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: PumpIn state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("PumpIn state is the same as the saved one.");
    }
    else if(DEBUG) Serial.println("Not in MANUAL mode: PumpIn's value not taken");
  } 
  else if (strcmp(fieldName,"motor")==0) {

    //---------Set motor state ----------
    // only get the motor's state if in MANUAL mode
    if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
    {


      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != relaySwitchMotorGetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) relaySwitchMotorTurnOn();
          else relaySwitchMotorTurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The motor has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The motor state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("The motor state is the same as the saved one.");


    }
    else if(DEBUG) Serial.println("Not in MANUAL mode: Motor's value not taken");
  } 
  else if (strcmp(fieldName,"methane")==0) {
   //---------Set gas valve (methane) state ----------
    // only get the pump's state if in MANUAL mode
    if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
    {

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != gasValvesGetState(CH4))
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) gasValvesTurnOn(CH4);
          else gasValvesTurnOff(CH4); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The gas valve (methane) has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The gas valve (methane) state is invalid!.");
        }
      }
       else if(DEBUG)Serial.println("The gas valve (methane) state is the same as the saved one.");
    }
    else if(DEBUG) Serial.println("Not in MANUAL mode: Gas valve's value not taken");
    
  }   
  else if (strcmp(fieldName,"carbonDioxide")==0) {
    //---------Set gas valve (carbonDioxide) state ----------
    // only get the pump's state if in MANUAL mode
    if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
    {
      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != gasValvesGetState(CO2))
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) gasValvesTurnOn(CO2);
          else gasValvesTurnOff(CO2); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The gas valve (carbonDioxide) has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The gas valve (carbonDioxide) state is invalid!.");
        }
      }
     


    }
    else if(DEBUG) Serial.println("Not in MANUAL mode: Gas valve's value not taken");

  } 
  else if (strcmp(fieldName,"nitrogen")==0) {

    //---------Set gas valve (nitrogen) state ----------
    // only get the pump's state if in MANUAL mode
    if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
    {

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != gasValvesGetState(N2))
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) gasValvesTurnOn(N2);
          else gasValvesTurnOff(N2); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The gas valve (nitrogen) has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The gas valve (nitrogen) state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("The gas valve (nitrogen) state is the same as the saved one.");
    }

    else if(DEBUG) Serial.println("Not in MANUAL mode: Gas valve's value not taken");
  } 
}

*/

#endif
