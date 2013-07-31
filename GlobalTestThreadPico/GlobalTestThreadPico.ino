#include <SST.h>


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



// the THR define have to be replaced by dynamical variables (set by the server, not hard coded)

//#define ONE_WIRE_BUS1    IO1
//#define THR_DISTANCEPIN  IO3
#define THR_MONITORING   13  // INCOMPATIBLE WITH OUT3
//#define THR_IRPIN        12
#define THR_STEPPER     {OUT2, IO2}
//#define THR_SERVO       IO4
//#define THR_WGHT         IO4
//#define THR_FLUX         I04  //control of gas flux, same Berta as for gas valves

#define PARAM_FLAG         0 
#define PARAM_DEVICES_ADDR 1  //store
#define PARAM_DEVICES_TYPE 2
#define PARAM_SCAN         3
#define PARAM_SERVO        4
#define PARAM_WGHT         5
#define PARAM_LVL_MAX      6  //set the maximum level for the tank, to be determines + sanity check to be added
#define PARAM_LVL_MIN      7  //define the pumping stop level
#define PARAM_PUMP_DELAY   8
#define PARAM_GAS_MIX      9  //contains the indication on the 4 input gases (nothing, O2, Air, N2, ...), lookup table to be implemented by calibrating for each gas
#define PARAM_RGB1         10
#define PARAM_RGB2         11
#define PARAM_IRCODE       12
#define PARAM_RELAY_1      17 // = 17 = R (elay)
#define PARAM_RELAY_2      18
#define PARAM_TEMP1        20
#define PARAM_WIRE         24 // contains the active wire devices
#define PARAM_DISTANCE     25


/* we define here a series of mode, error flags and special event flags (to be detailled) */

#define FLAG_MOTOR_OFF     (1<<0)   //motor turned off
#define FLAG_PUMPING       (1<<1)   //set the condition to disable targeted modules when pumping is performed
#define ERROR_SERVER_DWN   (1<<2)   //set the condition for individual data control (alert useless here)
#define ERROR_TEMP         (1<<3)   //set the condition to stop temperature control + alert message
#define ERROR_PH           (1<<4)   //set the condition to disable ph control       + alert message
#define ERROR_WEIGHT       (1<<5)   //set the condition to disable pumping control  + alert message
#define ERROR_MEMORY       (1<<6)   //set the condition to disable 

#define MODE_STDBY   (1<<13)   //motor and temperature PID On only
#define MODE_MANUAL  (1<<14)   //everything is set manually
#define MODE_AUTO    (1<<15)   //reactor working by itself, log can be performed

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
