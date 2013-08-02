/*--------------------------------

This thread will take care of the Ethernet communications

-Answer to request from the server (get logs, get parameters)
-Get state updates from the server / other modules

--------------------------------*/

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
  
void ethernetSetup() {
  // the ethernet pin has been initialized in the main setup() from Bioreactor
  Ethernet.begin(mac,ip);
}  

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

//TODO : To be changed with the new data structure
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
  while (client.connected() && ((millis()-start)<1000)) 
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
