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
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {
  10,0,0,3}; // reserved IP adress of the Arduino

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress server(10,0,0,2); // local NTP server 

/*---------------------------
  Ethernet Thread
---------------------------*/

NIL_WORKING_AREA(waThreadEthernet, 50); //change memoy allocation
NIL_THREAD(ThreadEthernet, arg) {
  
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
}  

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
  client.connect(server,80);
  long start=millis(); 
  while (! client.connected() && ((millis()-start)<500))
  {
    delay(1); //Could we do NilThreadSleep here ?
  }
  if (DEBUG) Serial.print("Connection achieved in (ms): ");
  if (DEBUG) Serial.println(millis()-start);
}

void ethernetSendLog(){
  //TODO
}

//JSON PARSING
void ethernetReadCommand(){
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
  /* STATUS:
   1 : the JSON started
   2 : we get the field name
   3 : we get the value
   */

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
 /*---------------------------------------
   The module has to be able to respond to 
   several HTML request.
   
   1. Help menu
   2. State vector
   3. logs
   4. i2c devices
   5. 1-wire devices
 ---------------------------------------*/
 int extractedValueInt=(int)extractedValueFloat;
  if (strcmp(fieldName,"StateVector")==0) {
     ethernetSendStatus();
  } else if (strcmp(fieldName,"helpMenu")==0) {
    ethernetSendPlugStatus();
  } else {
    //Error, request non existant
  }
}

/*-----------------------------
  Parameter & requests related functions
----------------------------*/

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


#endif
