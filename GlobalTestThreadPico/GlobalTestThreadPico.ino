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

/*Card configuration*/
/*Word structure : 4 bits allocated to each connection port + option for I2C additionnal device
  DEV_PORT1/DEV_PORT2/DEV_PORT3/DEV_PORT4  (word1)
  DEV_PORT5/DEV_I2C_1/DEV_I2C_2/DEV_I2C_3  (word2)
  DEV_I2C_4/-/-/-                          (word3)
*/

#define CONFIG_1   0
#define CONFIG_2   1
#define CONFIG_3   2

/* related MASKS */

#define DEF_PORT1     0b0000000000001111
#define DEF_PORT2     0b0000000011110000
#define DEF_PORT3     0b0000111100000000
#define DEF_PORT4     0b1111000000000000
#define DEF_PORT5     0b0000000000001111
#define DEF_I2C_1     0b0000000011110000
#define DEF_I2C_2     0b0000111100000000
#define DEF_I2C_3     0b1111000000000000
#define DEF_I2C_4     0b0000000000001111


/*related DEVICE definitions*/

#define NO_DEVICE     0b1111

#define DEV_TEMP      0b0001
#define DEV_WGHT      0b0010
#define DEV_PH        0b0011
#define DEV_FLUX      0b0100
#define DEV_RELAY     0b0101
#define DEV_GAS_TAP   0b0110
#define DEV_STEPPER   0b0111

//add a new definition here for a new type of sensor + code the additionnal related thread.


/* System State Vectors */
/*Word structure : 4 bits allocated to each connection port + option for I2C additionnal device
  DEV_PORT1/DEV_PORT2/DEV_PORT3/DEV_PORT4  (word1)
  DEV_PORT5/DEV_I2C_1/DEV_I2C_2/DEV_I2C_3  (word2)
  DEV_I2C_4/-/-/-                          (word3)
*/

#define STATE_VCTR1  3  
#define STATE_VCTR2  4  
#define STATE_VCTR3  5  


/*Port values*/     

#define PORT1    6
#define PORT2    7
#define PORT3    8
#define PORT4    9
#define PORT5    10
#define I2C_1    11
#define I2C_2    12
#define I2C_3    13
#define I2C_4    14

/*hard coded parameters*/

#define PARAM_GAS_MIX      15  //contains the indication on the 4 input gases (nothing, O2, Air, N2, ...), lookup table to be implemented by calibrating for each gas
#define PARAM_GAS_RATIO    16

#define PARAM_LVL_MAX      17 
#define PARAM_LVL_MIN      18  

#define PARAM_PH_EQUIL     19


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
