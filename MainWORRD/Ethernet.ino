#if defined(THR_ETHERNET) || defined(THR_LINEAR_LOGS)

/****************************************************************
 *   THREAD ETHERNET & LOG EVENT + DATA
 *   This thread will take care of the Ethernet communications
 *   
 *   -Answer to request from the server (get logs, get parameters)
 *   -Get state updates from the server / other modules
 *   
 *   
 *  The module has to be able to respond to same command as Serial.
 *   
 *****************************************************************/


//The longest request possible is "GET /s=4294967295"
#define REQUEST_LENGTH 20
#define REQUEST_START  5


#define TABLE_SIZE 32


uint8_t ip[] = IP;
uint8_t mac[] = MAC;


EthernetServer server(80);


/****************
 * Ethernet Thread
 *****************/

NIL_WORKING_AREA(waThreadEthernet, 350); //change memoy allocation
NIL_THREAD(ThreadEthernet, arg) {

  nilThdSleepMilliseconds(1000);

  Ethernet.begin(mac,ip);
  server.begin();


#ifdef DEBUG_ETHERNET
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
#endif

  while (TRUE) {
    /****************************
     * THREAD ETHERNET 
     * - Receive request from clients
     *****************************/
#ifdef THR_ETHERNET
    EthernetClient client = server.available();

    if (client) {
#ifdef DEBUG_ETHERNET
      Serial.println("New client");
#endif
      // an http request ends with a blank line
      boolean starting = true;
      //Count the number of byte of the answer

      char request[TABLE_SIZE];
      int count = 0;


      while (client.available() && client.connected() && count<1000) {

        char c = client.read();

        //store characters to string           
        if (starting && count<REQUEST_LENGTH && count>=REQUEST_START) {
#ifdef DEBUG_ETHERNET
          Serial.print(c);
#endif
          // += append a character to a string
          if (c=='\n' || c=='\r' || c==' ') {
            request[count-REQUEST_START] = '\0';
            starting=false;
          } 
          else {
            request[count-REQUEST_START] = c;
          }
        }
        count++;
      }

      if (client.connected()) {

        client.println(F("HTTP/1.1 200 OK"));
        client.println(F("Content-Type: text/html"));
        client.println(F("Connection: close"));  // the connection will be closed after completion of the response
        client.println();
        printResult(request, &client);

        // give the web browser time to receive the data
        nilThdSleepMilliseconds(100);
        // close the connection:
        client.flush();
         nilThdSleepMilliseconds(100);
        client.stop();
      }
#ifdef DEBUG_ETHERNET
      Serial.println("Closed");
#endif

    } 
#endif

    nilThdSleepMilliseconds(200);
  }
}

#endif









