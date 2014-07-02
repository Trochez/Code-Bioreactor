/**************
 * LIBRAIRIES
 **************/

//MultiThread
#include <NilRTOS.h>


#include <avr/wdt.h>


// The normal serial takes 200 bytes more but is buffered
// And if we send a String for parameters it can not be understand ...
// #include <NilSerial.h>
// #define Serial NilSerial

// http://www.arduino.cc/playground/Code/Time
#include <Time.h>



#define O12V_0    6
#define DATA0_0   2
#define DATA1_0   3
#define IO_0     21

#define O12V_1    8
#define DATA0_1   4
#define DATA1_1   7
#define IO_1     20

#define O12V_2    9
#define DATA0_2  10
#define DATA1_2  11
#define IO_2     19

#define O12V_3    5
#define DATA0_3  13
#define DATA1_3  14
#define IO_3     22

#define O12V_4   12
#define DATA0_4  15
#define DATA1_4  16
#define IO_4     18


byte O12V[] = { O12V_0, O12V_1, O12V_2, O12V_3, O12V_4 };
byte DATA0[] = { DATA0_0, DATA0_1, DATA0_2, DATA0_3, DATA0_4 };
byte DATA1[] = { DATA1_0, DATA1_1, DATA1_2, DATA1_3, DATA1_4 };
byte IO[] = { IO_0, IO_1, IO_2, IO_3, IO_4 };

// each port corresponds to a door identifier
byte DOORS[] = {10, 11, 12, 13, 14};


/*********
 * SETUP
 *********/

void setup() {
  Serial.begin(9600);
  
  initLogger();
  
  nilSysBegin();
}

void loop() {
}





