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


//The longest request possible is "GET /s=4294967295"
#define REQUEST_LENGTH 20
#define REQUEST_START  4


#define TABLE_SIZE 32


uint8_t ip[] = IP;
uint8_t mac[] = MAC;


EthernetServer server(80);


/****************
 * Ethernet Thread
 *****************/

NIL_WORKING_AREA(waThreadEthernet, 600); //change memoy allocation
NIL_THREAD(ThreadEthernet, arg) {

  Ethernet.begin(mac,ip);
  server.begin();
  /****************************
   * LOG & NTP Setup
   *****************************/


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
      //   writeLog(getParameter(PARAM_EVENT), getParameter(PARAM_EVENT_VALUE)); 
#endif
      setParameter(FLAG_VECTOR, getParameter(FLAG_VECTOR) & (~EVENT_OCCURED));
    }



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
      boolean starting = true;
      //Count the number of byte of the answer

      char request[TABLE_SIZE];
      int count = 0;

      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          
          //store characters to string           
          if (starting && count<REQUEST_LENGTH && count>=REQUEST_START) {
            Serial.print(c);

            // += append a character to a string
            request[count-REQUEST_START] = c;

            if (c=='\n' || c=='\r' || c==' ') starting=false;
          }
          count++;
        }
      }


      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/html"));
      client.println(F("Connection: close"));  // the connection will be closed after completion of the response
      client.println();
      client.println(F("<!DOCTYPE HTML>"));
      client.println(F("<html><pre>"));
      printResult(request, &client);
      client.println(F("</pre></html>"));


      // give the web browser time to receive the data
      delay(1);
      // close the connection:
      client.stop();
#ifdef DEBUG_ETHERNET
      Serial.println("client disconnected");
#endif

    } 
#endif


    nilThdSleepMilliseconds(200);
  }
}

/*----------------------------
 Ethernet related functions
 ----------------------------*/

void parseRequest(Client* cl, uint8_t* req) {

  noSuchCommand(cl); 
}



void noSuchCommand(Print* output){
  output->println(F("No Such Command"));
}

void noThread(Print* output){
  output->println(F("No Thread"));
}



#endif







