#include <NilRTOS.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>o
#include <LiquidCrystal.h>

// The normal serial takes 200 bytes more but is buffered
// And if we send a String for parameters it can not be understand ...
// #include <NilSerial.h>
// #define Serial NilSerial

// http://www.arduino.cc/playground/Code/Time
#include <Time.h>


/*define the IN/OUT ports of the card*/

#define OUT1    4
#define OUT2    11
#define OUT3    13
#define OUT4    6
#define IO1     8
#define IO2     18
#define IO3     20
#define IO4     19
#define RED1    5
#define GREEN1  9
#define BLUE1   10
#define RED2    21
#define GREEN2  22
#define BLUE2   23


// the THR define have to be replaced by dynamical variables (set by the server, not hard coded)

//#define ONE_WIRE_BUS1    IO1
//#define THR_DISTANCEPIN  IO3
//#define THR_MONITORING   13  // INCOMPATIBLE WITH OUT3
//#define THR_IRPIN        12
//#define THR_STEPPER      {OUT2, IO2}
//#define THR_SERVO        IO4
//#define THR_WGHT         IO4
//#define THR_FLUX         IO4  //control of gas flux, same Berta as for gas valves
//Global System Parameters

/*Device Addresses Parameters*/
/*
each position in the corresponds to one specific device type !
the bianry value at this position in the vector gives the IO position on the board
5 connections available on the board
*/
/*
word structure
0/ADD_TEMP1/ADD_TEMP2/ADD_TEMP3/ADD_TEMP4/ADD_RELAY1                (word1) A
0/ADD_RELAY2/ADD_WGHT1/ADD_WGHT2/ADD_GAS_TAP1/ADD_GAS_TAP2          (word2) B
0/ADD_GAS_TAP3/ADD_GAS_TAP4/ADD_FLUX1/ADD_FLUX2/ADD_FLUX3           (word3) C
0/ADD_FLUX4/ADD_LIQ_TAP1/ADD_LIQ_TAP2/ADD_LIQ_TAP3/ADD_LIQ_TAP4     (word4) D
0/ADD_MOTOR/ADD_PH/--/--/--                                         (word5) E
*/

#define DVCS_ADD1   0   //A
#define DVCS_ADD2   1   //B
#define DVCS_ADD3   2   //C
#define DVCS_ADD4   3   //D
#define DVCS_ADD5   4   //E


/*related PORT definitions*/
#define PORT1      0b000
#define PORT2      0b001
#define PORT3      0b010
#define PORT4      0b011
#define PORT5      0b100
#define PORT_I2C   0b101
#define UNPLUGED   0b111


/*related masks*/
#define ADD_TEMP1      0b0111000000000000
#define ADD_TEMP2      0b0000111000000000
#define ADD_TEMP3      0b0000000111000000
#define ADD_TEMP4      0b0000000000111000
#define ADD_RELAY1     0b0000000000000111
#define ADD_RELAY2     0b0111000000000000
#define ADD_WGHT1      0b0000111000000000
#define ADD_WGHT2      0b0000000111000000
#define ADD_GAS_TAP1   0b0000000000111000
#define ADD_GAS_TAP2   0b0000000000000111
#define ADD_GAS_TAP3   0b0111000000000000
#define ADD_GAS_TAP4   0b0000111000000000
#define ADD_FLUX1      0b0000000111000000
#define ADD_FLUX2      0b0000000000111000
#define ADD_FLUX3      0b0000000000000111
#define ADD_FLUX4      0b0111000000000000
#define ADD_LIQ_TAP1   0b0000111000000000
#define ADD_LIQ_TAP2   0b0000000111000000
#define ADD_LIQ_TAP3   0b0000000000111000
#define ADD_LIQ_TAP4   0b0000000000000111
#define ADD_MOTOR      0b0111000000000000
#define ADD_PH         0b0000111000000000


/* System State Vectors */
/*state vector written on three 16 bits words: 
  TEMP1/TEMP2/TEMP3/TEMP4/RELAY1/RELAY2/WGHT1/WGHT2           (word5) F
  GAS_TAP1/GAS_TAP2/GAS_TAP3/GAS_TAP4/FLUX1/FLUX2/FLUX3/FLUX4 (word6) G
  LIQ_TAP1/LIQ_TAP2/LIQ_TAP3/LIQ_TAP4/MOTOR/PH/--/ --         (word7) H
*/
#define STATE_VCTR1  5  //F
#define STATE_VCTR2  6  //G
#define STATE_VCTR3  7  //H

/* related masks */
#define TEMP1       0b1100000000000000
#define TEMP2       0b0011000000000000
#define TEMP3       0b0000110000000000
#define TEMP4       0b0000001100000000
#define RELAY1      0b0000000011000000
#define RELAY2      0b0000000000110000
#define WGHT1       0b0000000000001100
#define WGHT2       0b0000000000000011
#define GAS_TAP1    0b1100000000000000
#define GAS_TAP2    0b0011000000000000
#define GAS_TAP3    0b0000110000000000
#define GAS_TAP4    0b0000001100000000
#define FLUX1       0b0000000011000000
#define FLUX2       0b0000000000110000
#define FLUX3       0b0000000000001100
#define FLUX4       0b0000000000000011
#define LIQ_TAP1    0b1100000000000000
#define LIQ_TAP2    0b0011000000000000
#define LIQ_TAP3    0b0000110000000000
#define LIQ_TAP4    0b0000001100000000
#define MOTOR       0b0000000011000000
#define PH          0b0000000000110000


/* !!! Procedure for an additionnal sensor !!! */
/* 
1---> add an address mask in 'Device Address Parameters/related masks' and complete the word structure in the header of 'Device Address Parameters'
2---> add a  vector state mask in 'System State vectors/related masks' and complete the word structure in the header of 'System State Vectors'
3---> complete the dedicated Thread module by copying the developed code and adapting with the new masks or create a new dedicated Thread
4---> if a new Thread is created, don't forget to update the Z-vector module with the corresponding NilThread line at the right place in the priority list
*/
/* !!! ----------------------------------- !!! */

/*weight values and parameters*/
#define PARAM_WGHT         8
#define PARAM_LVL_MAX      9 
#define PARAM_LVL_MIN      10  



/*temperature values*/
#define PARAM_TEMP_LIQ     11
#define PARAM_TEMP_PLATE   12
#define PARAM_TEMP_MOTOR   13

/*gas flux values*/

#define PARAM_GAS_MIX      14  //contains the indication on the 4 input gases (nothing, O2, Air, N2, ...), lookup table to be implemented by calibrating for each gas
#define PARAM_FLUX1        15
#define PARAM_FLUX2        16
#define PARAM_FLUX3        17
#define PARAM_FLUX4        18

/* not properly defined yet*/
 
#define PARAM_WIRE         24 // contains the active wire devices
#define PARAM_SCAN         25  



/*Setup*/

byte IO[]={
  IO1, IO2, IO3, IO4};

void setup() {
  Serial.begin(115200);
  setupLogger();
  setupDebugger();
  setupParameters();
  nilSysBegin();
}

void loop() {
}
