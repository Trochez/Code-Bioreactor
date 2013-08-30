/*--------------------------------
 
 This thread will take care of the Ethernet communications
 
 -Answer to request from the server (get logs, get parameters)
 -Get state updates from the server / other modules
 
 TODO:
 -Add parsing for the new commands
 !! Need to set the flag for CONFIG_MODIF in FLAG_VECTOR
 
 ASCII 
 A-Z : 65-90
 a-z : 97-122
 0-9 : 48-57
 
 --------------------------------*/
#ifdef THR_ETHERNET

#define DEBUG 0
#define MAX_HTTP_STRING_LENGTH 2048    // in bytes; max. http return string to read  
// (reserve space for the http header !AND! the JSON command string)
#define MAX_COMMAND_STRING_LENGTH 400 // in bytes; max. JSON return string to read 

//Define ASCII position Bytes
#define ASCII_A 65
#define ASCII_Z 90
#define ASCII_0 48

#define ASCII_a 48
#define ASCII_z 57

//Define the bytes of the URL in the request from the client
#define URL_1 5
#define URL_2 6
#define URL_3 7

// THIS SHOULD BE DEFINED GLOBALLY SO THREADS CAN ACCESS IT 
// Enter a MAC address and IP address for the Arduino controller below.
// The IP address is reserved on the routher for this Arduino controller.
// CAUTION
// Each different boards should have a different IP in the range 172.17.0.100 - 172.17.0.200
// and a different MAC address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {172,17,0,100}; // reserved IP adress of the Arduino


// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):

unsigned int localPort = 8888;      // local port to listen for UDP packets
EthernetServer server(80);

IPAddress alix_server(172,17,0,10); // local NTP server 

#define REQUEST_LENGTH 20

/*---------------------------
 Ethernet Thread
 ---------------------------*/

NIL_WORKING_AREA(waThreadEthernet, 250); //change memoy allocation
NIL_THREAD(ThreadEthernet, arg) {
  char request[REQUEST_LENGTH];
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
      while (client.connected()) {

        if (client.available()) {
          char c = client.read();
          //Serial.write(c);
          //store characters to string           
          if (count < 20) {
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

void parseRequest(Client* cl, char* req) {
  /*---------------------------------------
   The module has to be able to respond to 
   several HTML request.
   The first parameter defines the request type:
   -Upper case letters are for setting parameters
   -Lower case letter are for actions such as logging, info, ...
   
   1. Help menu (h)
   2. Settings Hardcoded  : IP, MAC,... (f)
   3. logs (to be defined)
   4. i2c devices (i)
   5. 1-wire devices (o)
   6. set Parameter (A - Z + value)
   ---------------------------------------*/
  //The request has the form:
  //"GET /X HTTP/1.1 
  // ..."
  // We are interested by the 5th character (X)
  char c = req[URL_1];
  // show settings hardCoded on the card
  if (c=='f') {
    //TODO
  } 
  //Show help menu
  else if (c=='h') {
    printHelp(cl);
  } 
  // show i2c (wire) information
  else if (c=='i') { 
    #ifdef GAS_CTRL || I2C_LCD
      wireInfo(cl);
    #elseif
      (*cl).println("I2C Thread not activated");
    #endif
  } 
  // show oneWire information
  else if (c=='o') {
    #ifdef ONE_WIRE_BUS1
      oneWireInfo(cl);
    #elseif
      (*cl).println("1-wire Thread not activated");
    #endif
  }
  // show settings
  else if (c=='s') {
    printParameters(cl);
  } 
  //return the log of all the sensors
  else if (c == 'l'){
    //TODO
  }
  //return the X last log of command
  else if (c == 'e'){
    //TODO
  }
  // The request is a parameter
  else if(c >= ASCII_A && c <= ASCII_Z){
    
    //Here we read the second byte of the URL to differentiate the requests
    char d = req[URL_2];
    switch(d){
       //There is only one parameter in the GET
      case ' ':
        printParameter(cl, (byte) (c-ASCII_A));
        break;
      case '=':
        { // { } Allow to declare variables inside the switch
          int value = 0;
          //We are interested by the 7th bit of the request
          char i=URL_3;
          //The request modifies a parameter
          //We need to check if the value is correct
          while(req[i] != ' '){
            //The letters start at index 48 in ASCII
            value = (req[i]-ASCII_0) + value*10;
            i++;
          }
          setParameter((byte) (c-ASCII_A), value);
          printParameter(cl, (byte) (c-ASCII_A));
        }
        break;
        //We should get the number following before the case switch
      case 's':
        //sendLog(parameter, SECONDS, getNumber());
        break;
  
      case 'm':
        //sendLog(parameter, MINUTES, getNumber());
        break;
  
      case 'h':
        //sendLog(parameter, HOURS, getNumber());
        break; 
      }
  } 
  //This request does not exist
  else {
    (*cl).println("No such command"); 
  }
}

/*void ethernetSendLog(){
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
 */

/****************************************
 * Parameter & requests related functions
 *****************************************/

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

void ethernetPrintI2C(){
  //return the I2C devices plugged on the device
}

void ethernetPrintOneWire(){
  //return the One-Wire devices pluggedf on the device
}
/*
void ethernetParseCommandValue(char *fieldName, double extractedValueFloat)
 {
 int extractedValueInt=(int)extractedValueFloat;
 if (strcmp(fieldName,"liquidTemp")==0) {
 else if (strcmp(fieldName,"liquidLevelMax")==0) {
 else if (strcmp(fieldName,"liquidLevelMin")==0) {
 else if (strcmp(fieldName,"pH")==0) {
 else if (strcmp(fieldName,"waitTime")==0) {
 else if (strcmp(fieldName,"methaneIn")==0) {
 else if (strcmp(fieldName,"carbonDioxideIn")==0) {
 else if (strcmp(fieldName,"nitrogenIn")==0) {
 else if (strcmp(fieldName,"liquidIn")==0) {
 else if (strcmp(fieldName,"liquidOut")==0) {
 else if (strcmp(fieldName,"mode")==0) {
 else if (strcmp(fieldName,"pumpOut")==0) {
 else if (strcmp(fieldName,"pumpIn")==0) {
 else if (strcmp(fieldName,"motor")==0) {
 else if (strcmp(fieldName,"methane")==0) {
 else if (strcmp(fieldName,"carbonDioxide")==0) {
 else if (strcmp(fieldName,"nitrogen")==0) {
 }
 */
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

