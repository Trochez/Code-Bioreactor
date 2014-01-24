#if defined(THR_ETHERNET) || defined(THR_LINEAR_LOGS)

/****************************************************************
 *   THREAD ETHERNET & LOG EVENT + DATA
 *   This thread will take care of the Ethernet communications
 *   
 *   -Answer to request from the server (get logs, get parameters)
 *   -Get state updates from the server / other modules
 *   
 *   
 *  The module has to be able to respond to several HTML request.
 *   
 *  The first parameter defines the request type:
 *   -Upper case letters are for setting parameters
 *   Parameters range between A and Z
 *   -Lower case letter are for actions:
 *   
 *   p. Print Help Menu
 *   f. Settings Hardcoded  : IP, MAC,...
 *  i. i2c devices
 *   o. 1-wire devices
 *  g. Parameters in memory
 *   {A-Z}. print value of parameter given
 *   {A-Z}=X set parameter to the value given and save it to EEPROm
 *   {s,m,h,e}. last log of seconds,minutes,hours and events
 *   {s.m.h,e}=X. log X of seconds,minutes,hours and events
 *   l. return a vector of the last entry number:
 *   #events #seconds #minutes #hours
 * 
 *****************************************************************/


//#define MAX_HTTP_STRING_LENGTH 2048    // in bytes; max. http return string to read  
// (reserve space for the http header !AND! the JSON command string)
//#define MAX_COMMAND_STRING_LENGTH 400 // in bytes; max. JSON return string to read 

/***************************************************************
 * ASCII NUMBERS  * A-Z : 65-90  * a-z : 97-122  * 0-9 : 48-57
 ***************************************************************/
#define ASCII_A 65
#define ASCII_Z 90

#define ASCII_0 48
#define ASCII_9 57

#define ASCII_a 97
#define ASCII_z 122

//Define the bytes of the URL in the request from the client
#define GET_CHAR_1 5
#define GET_CHAR_2 6
#define GET_CHAR_3 7

//The longest request possible is "GET /s=4294967295"
#define REQUEST_LENGTH 17
#define TABLE_SIZE 32


//Prototypes
void parseRequest(Client* cl, uint8_t* req);
uint32_t getNumber(uint8_t start, uint8_t* tab);
void newLine(Print* output);
void printTab(Print* output, uint8_t* tab, char s);
void noSuchCommand(Print* output);
void noThread(Print* output);
void printHelp(Print* output);

#ifdef THR_LINEAR_LOGS
void printIndexes(Print* output);
#endif


uint8_t ip[] = IP;
uint8_t mac[] = MAC;
const uint8_t alix[] = ALIX;
unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress alix_server(alix[0],alix[1],alix[2],alix[3]); // local NTP server
EthernetServer server(80);


/****************
 * Ethernet Thread
 *****************/

NIL_WORKING_AREA(waThreadEthernet, 400); //change memoy allocation
NIL_THREAD(ThreadEthernet, arg) {

  SST sst = SST(4);
  setupMemory(sst);
  sst.flashTotalErase(); //  A Virer !!!
  free(&sst);

  Ethernet.begin(mac,ip);
  server.begin();
  /****************************
   * LOG & NTP Setup
   *****************************/
#ifdef THR_LINEAR_LOGS



  //const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 

  // Boolean variable to test a t the first place several times the actuall
  // time of the arduino. Count 5 times the arduino time before synchronization
  time_t time_now = 0;
  time_t previousNTP = 0;
  time_t previousLog = 0;
  boolean waitPacket = false;
  // A UDP instance to let us send and receive packets over UDP
  EthernetUDP Udp;

  /*********************************
   * SETUP ETHERNET SERVER & NTP
   ********************************/
  Udp.begin(localPort);

  sendNTPpacket(&Udp, alix_server, packetBuffer);
  delay(2000);
  updateNTP(Udp, packetBuffer);

  //Log of the reboot of the card
  
#endif

#ifdef DEBUG_ETHERNET
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
#endif

  while (TRUE) {


    /****************************
     * ERROR CHECKING % LOGGING
     ****************************/

    if(getParameter(FLAG_VECTOR) & EVENT_OCCURED){
#ifdef THR_LINEAR_LOGS
      writeLog(getParameter(PARAM_EVENT), getParameter(PARAM_EVENT_VALUE)); 
#endif
      setParameter(FLAG_VECTOR, getParameter(FLAG_VECTOR) & (~EVENT_OCCURED));
    }



    /****************************
     * THREAD LOG & TIME : STRUCTURE    
     * - Update NTP all days
     * Send packet
     * 2sec later check if answer
     * answer -> update
     * no answer -> log in event + try again in 3600 seconds
     * - Log parameter every 1 second
     *****************************/
#ifdef THR_LINEAR_LOGS
    time_now = now();
    if(!waitPacket && ((time_now - previousNTP ) >= NTP_UPDATE_TIME)) {
      sendNTPpacket(&Udp,alix_server, packetBuffer);
      waitPacket = true;
    } 
    // 2 seconds later we check if we have an answer from the server and update the time if possible
    else if(waitPacket && time_now - previousNTP >= NTP_UPDATE_TIME+2) {
      boolean success = updateNTP(Udp, packetBuffer);
      if(!success) {
        writeLog(NO_ANSWER_NTP_SERVER, 0); //TODO :update the function 
      }
      previousNTP = time_now;
      waitPacket = false;
    }

    // This function suppose that the thread is called regularly (at least 1 time every 10 second)
    // this is the linear logs
    // 
    if(time_now - previousLog >= LOG_INTERVAL) {
      writeLog( 0, 0);
      previousLog = time_now;
    }
#endif

    /****************************
     * THREAD ETHERNET 
     * - Receive request from clients
     *****************************/
#ifdef THR_ETHERNET
    EthernetClient client = server.available();
    if (client) {
#ifdef DEBUG_ETHERNET
      Serial.println("new client");
#endif
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      //Count the number of byte of the answer

      uint8_t request[TABLE_SIZE];
      int count = 0;
      while (client.connected()) {

        if (client.available()) {
          char c = client.read();
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
#ifdef DEBUG_ETHERNET
      Serial.println("client disconnected");
#endif
      /*while(client.status() != 0) {
       delay(5);
       }*/
    } 
#endif


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

  uint8_t c = req[GET_CHAR_1];







  // show settings
  if (c=='g') {
    printParameters(cl);
  } 

  else if (c=='l'){
#ifdef THR_LINEAR_LOGS
    printIndexes(cl); 
#else
    noThread(cl);
#endif
  }

  //return the log of the entry given
  else if ( (c=='s') || /*(c=='m') || (c=='h') ||*/ (c=='e')){

  }

  // The request is a parameter A-Z
  else if(c >= ASCII_A && c <= ASCII_Z){
    //Here we read the second byte of the URL to differentiate the requests
    uint8_t d = req[GET_CHAR_2];
    switch(d){

      //There is only one parameter in the GET
    case ' ':
      printParameter(cl, (byte) (c-ASCII_A));
      break;

    case '=':
      { // { } Allow to declare variables inside the switch
        uint32_t value = getNumber(GET_CHAR_3, req);
        byte p = (byte) (c-ASCII_A);
#ifdef THR_LINEAR_LOGS
        writeLog( (uint16_t) (PARAMETER_SET + p) , (uint16_t) value);
#endif
        setAndSaveParameter(p, value);
        printParameter(cl, p);
      }
      break;

    default:
      noSuchCommand(cl);
      break;
    }
  } 

  //This request does not exist
  else {
    noSuchCommand(cl); 
  }
}

uint32_t getNumber(uint8_t start, uint8_t* tab){
  uint32_t index = 0;
  uint8_t i = start;
  uint8_t c = tab[i];

  while(c >= ASCII_0  && c <= ASCII_9 && i<REQUEST_LENGTH){
    //Conversion in integers : The letters start at index 48 in ASCII
    index = (c-ASCII_0) + index*10;
    i++;
    c = tab[i];
  }
  return index; 
}

/****************************************
 * Parameter & requests related functions
 *****************************************/





void noSuchCommand(Print* output){
  output->println(F("No Such Command"));
}

void noThread(Print* output){
  output->println(F("No Thread"));
}



#endif



