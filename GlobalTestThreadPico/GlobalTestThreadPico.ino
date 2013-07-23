// This is a more complex example showing many simultaneous action
//

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


//#define ONE_WIRE_BUS1    IO1
//#define THR_DISTANCEPIN  IO3
#define THR_MONITORING   13  // INCOMPATIBLE WITH OUT3
//#define THR_IRPIN        12
#define THR_STEPPER     {OUT2, IO2}
//#define THR_SERVO       IO4
//#define THR_WGHT         IO4
//#define THR_FLUX         I04  //control of gas flux, same Berta as for gas valves

#define PARAM_FLAG      0
#define PARAM_SCAN      2
#define PARAM_SERVO     3
#define PARAM_WGHT      4
#define PARAM_LVL_MAX   5  //set the maximum level for the tank, to be determines + sanity check to be added
//#define PARAM_LVL_MIN   6  //useless at the moment
#define PARAM_GAS_MIX   7  //contains the indication on the 4 input gases (nothing, O2, Air, N2, ...), lookup table to be implemented by calibrating for each gas
#define PARAM_RGB1      8
#define PARAM_RGB2      9
#define PARAM_IRCODE    10
#define PARAM_RELAY_1   17 // = 17 = R (elay)
#define PARAM_RELAY_2   18
#define PARAM_TEMP1     20
#define PARAM_WIRE      24 // contains the active wire devices
#define PARAM_DISTANCE  25


/* we define here a series of mode, error flags and special event flags (to be detailled) */

#define FLAG_MOTOR_OFF    (1<<0)
#define FLAG_SERVER_DWN   (1<<1)


#define FLAG_MODE_STDBY   (1<<13)
#define FLAG_MODE_MANUAL  (1<<14)
#define FLAG_MODE_AUTO    (1<<15)

/*end of the flag defnitions */


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
