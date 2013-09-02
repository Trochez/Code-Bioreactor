/*-------------------------------------------------
 
 This thread will take care of the Ethernet communications
 
 -Answer to request from the server (get logs, get parameters)
 -Get state updates from the server / other modules
 
 
 The module has to be able to respond to several HTML request.
 
 The first parameter defines the request type:
 -Upper case letters are for setting parameters
  Parameters range between A and Z
 -Lower case letter are for actions:
   p. Print Help Menu
   f. Settings Hardcoded  : IP, MAC,...
   i. i2c devices
   o. 1-wire devices
   g. Parameters in memory
   {s,m,h,e}. last log of seconds,minutes,hours and events
   {s.m.h,e}=X. log X of seconds,minutes,hours and events
   l. return a vector of the last entry number:
      #seconds #minutes #hours #events
      
 -------------------------------------------------*/
 
 
#ifdef THR_ETHERNET

#define DEBUG 0
#define MAX_HTTP_STRING_LENGTH 2048    // in bytes; max. http return string to read  
// (reserve space for the http header !AND! the JSON command string)
#define MAX_COMMAND_STRING_LENGTH 400 // in bytes; max. JSON return string to read 

/*************
 ASCII 
 A-Z : 65-90
 a-z : 97-122
 0-9 : 48-57
************/
#define ASCII_A 65
#define ASCII_Z 90

#define ASCII_0 48
#define ASCII_9 57

#define ASCII_a 48
#define ASCII_z 57

//Define the bytes of the URL in the request from the client
#define URL_1 5
#define URL_2 6
#define URL_3 7

byte mac[] = MAC;
byte ip[] = IP; // reserved IP adress of the Arduino


// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):

EthernetServer server(80);
IPAddress alix_server(ALIX[0],ALIX[1],ALIX[2],ALIX[3]); // local NTP server 

//The longest request possible is "GET /s=4294967295"
#define REQUEST_LENGTH 17
#define TABLE_SIZE 32
/*---------------------------
 Ethernet Thread
 ---------------------------*/

NIL_WORKING_AREA(waThreadEthernet, 296); //change memoy allocation
NIL_THREAD(ThreadEthernet, arg) {
  
  // Initializate the connection with the server
  Ethernet.begin(mac,ip);
  server.begin();

  while (TRUE) {
    // listen for client request
    EthernetClient client = server.available();
    if (client) {
      Serial.println("new client");
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      //Count the number of byte of the answer
      int count = 0;
      uint8_t request[TABLE_SIZE];
      while (client.connected()) {

        if (client.available()) {
          char c = client.read();
          //Serial.write(c);
          //store characters to string           
          if (count < REQUEST_LENGTH) {
            // += append a character to a string
            request[count] = c;
            count++;
          }
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println(F("Connection: close"));  // the connection will be closed after completion of the response
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<html>"));
            client.write(request, HEX);
            parseRequest(&client, request);

            client.println(F("</html>"));
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
      delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
    }  
    nilThdSleepMilliseconds(200);
  }
}

/*----------------------------
 Ethernet related functions
 ----------------------------*/

void parseRequest(Client* cl, uint8_t* req) {
  
  //The request has the form:
  //"GET /XYZ HTTP/1.1 
  // ..."
  // We are interested by the 5th character (X)
  
  uint8_t c = req[URL_1];
  
  /****************************
    PARSE REQUEST FIRST BYTE
  *****************************/
  
  // show settings hardCoded on the card
  //Print help menu
  if (c=='p') {
    printHelp(cl);
  } 
  
  // show settings hardCoded on the card
  else if (c=='f') {
    printHardCodedParameters(cl);
  }
  
  // show i2c (wire) information
  else if (c=='i') { 
    #if defined(GAS_CTRL) || defined(I2C_LCD)
      wireInfo(cl);
    #else
      (*cl).println("I2C Thread not activated");
    #endif
  } 
  
  //show oneWire information
  else if (c=='o') {
    #ifdef ONE_WIRE_BUS1
      oneWireInfo(cl);
    #else
      (*cl).println("1-wire Thread not activated");
    #endif
  }
  
  // show settings
  else if (c=='g') {
    printParameters(cl);
  } 
  
  //return the log of the entry given
  else if ( (c=='s') || (c=='m') || (c=='h') || (c='e')){
    #ifdef THR_LINEAR_LOGS
    uint8_t d = req[URL_2];
    switch(d){
      //We return the last entry
      case ' ':
        //readLastEntry(c, req);
        (*cl).write(req, 32, HEX);
        break;
      case '=':
        {
          uint32_t index = 0;
          uint8_t i=URL_3;
          while(req[i] != ' ' && i<REQUEST_LENGTH){
            //The letters start at index 48 in ASCII
            index = (req[i]-ASCII_0) + index*10;
            i++;
          }
          //readEntryN(c, req, index);
          (*cl).write(req, 32, HEX);
        }
     }
     #else
     (*cl).println("Thread log not activated");
     #endif
  }
  // The request is a parameter A-Z
  else if(c >= ASCII_A && c <= ASCII_Z){
    
    //Here we read the second byte of the URL to differentiate the requests
    uint8_t d = req[URL_2];
    switch(d){
       //There is only one parameter in the GET
      case ' ':
        printParameter(cl, (byte) (c-ASCII_A));
        break;
      case '=':
        { // { } Allow to declare variables inside the switch
          int value = 0;
          //We are interested by the 7th bit of the request
          uint8_t i=URL_3;
          //The request modifies a parameter
          //We need to check if the value is correct
          while(req[i] != ' ' && i<REQUEST_LENGTH){
            //The letters start at index 48 in ASCII
            value = (req[i]-ASCII_0) + value*10;
            i++;
          }
          setParameter((byte) (c-ASCII_A), value);
          printParameter(cl, (byte) (c-ASCII_A));
        }
        break;
      }
  } 
  
  //This request does not exist
  else {
    (*cl).println("No such command"); 
  }
}

/****************************************
 * Parameter & requests related functions
 *****************************************/

void printHelp(Print* output) {
  //return the menu
  //output->println(F("(d)ebug<br/>"));
  //output->println(F("(e)eprom<br/>"));
  output->println(F("(f)ree mem<br/>"));
  output->println(F("(h)elp<br/>"));
  output->println(F("(i)2c<br/>"));
  output->println(F("(l)og<br/>"));
  output->println(F("(o)ne wire<br/>"));
  output->println(F("(s)ettings<br/>"));

}

void printHardCodedParameters(Print* output){
   output->println(F("Hardcoded Parameters :")); 
   output->print(F("IP : "));
   output->println(IP); 
   output->print(F("MAC : "));
   output->println(MAC); 
   output->print(F("ALIX : "));
   output->println(ALIX); 
   #ifdef RELAY_PUMP
     output->print(F("I2C relay : "));
     output->println(I2C_RELAY); 
   #endif
   #ifdef FLUX
     output->print(F("I2C Flux : "));
     output->println(I2C_FLUX); 
   #endif
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
 
 */

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

#endif

