

#ifdef DEBUG_ETHERNET
  void printDigits(int digits);
  void digitalClockDisplay();
#endif


#define NTP_PACKET_SIZE 48



#ifdef DEBUG_ETHERNET
  // function to print formated digit
  void printDigits(int digits){
    // utility for digital clock display: prints preceding colon and leading 0
    Serial.print(":");
    if(digits < 10)
      Serial.print('0');
    Serial.print(digits);
  }
  
  // print the actuall time
  void digitalClockDisplay(){
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(" ");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(month());
    Serial.print(" ");
    Serial.print(year()); 
    Serial.println(); 
  }
#endif

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(EthernetUDP * Udp, IPAddress address, byte * packetBuffer)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:         
  (*Udp).beginPacket(address, 123); //NTP requests are to port 123
  (*Udp).write(packetBuffer, NTP_PACKET_SIZE);
  (*Udp).endPacket();
  
  // TODO: implement error detection
  return 0;
}

/*-----------------------
  NTP related functions
-----------------------*/

boolean updateNTP(EthernetUDP Udp, byte* packetBuffer ){
   if ( Udp.parsePacket() ) {
     
     #ifdef DEBUG_ETHERNET
       Serial.println("receive");
     #endif
     
       // We've received a packet, read the data from it
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer
  
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word( packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word( packetBuffer[42],  packetBuffer[43]);  
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;  
      
      // now convert NTP time into everyday time:
      const unsigned long seventyYears = 2208988800UL;     
      // subtract seventy years, the hour in Colombia: -5 from GMT
      unsigned long epoch = secsSince1900 - seventyYears - 5 * 3600; 
      
      #ifdef DEBUG_ETHERNET 
        Serial.println(epoch);
      #endif
      
      setTime(epoch);
      return true;
   } else {
      #ifdef DEBUG_ETHERNET
        Serial.println("no packet received");
      #endif
      //TODO: What should we do here ? New update 
      return false;
   }
   
}

