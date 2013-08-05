#include <DallasTemperature.h>

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

#define PWM1    6//D6 OC4D
#define PWM2    8//D8 PCINT4
#define PWM3    9//D9 OC4B, OC1A, PCINT5
#define PWM4    5//D6 OC4A  
#define PWM5    11//D11 OC0A, OC1C, PCINT7
#define IO1     20//A2
#define IO2     21//A3
#define IO3     22//A4
#define IO4     19//A1
#define IO5     23//A5


// the THR define have to be replaced by dynamical variables (set by the server, not hard coded)

//#define THR_DISTANCEPIN  IO3
//#define THR_MONITORING   13  // INCOMPATIBLE WITH OUT3
//#define THR_IRPIN        12
//#define THR_FLUX         IO4  //control of gas flux, same Berta as for gas valves

/*Card configuration*/
/*Word structure : 4 bits allocated to each connection port + option for I2C additionnal device
  DEV_PORT4/DEV_PORT3/DEV_PORT2/DEV_PORT1  (word1)
  DEV_I2C_3/DEV_I2C_2/DEV_I2C_1/DEV_PORT5  (word2)
  -/-/-/DEV_I2C_4/                         (word3)
*/

#define CONFIG_1   0
#define CONFIG_2   1
#define CONFIG_3   2

/* related MASK */

#define PORT_MASK     0b0000000000001111

/*related DEVICE definitions*/

#define NO_DEVICE 0b1111

#define TEMP      0b0001
#define WGHT      0b0010
#define PH        0b0011
#define FLUX      0b0100
#define RELAY     0b0101
#define GAS_TAP   0b0110
#define STEPPER   0b0111

//add a new definition here for a new type of sensor + code the additionnal related thread.

/*Port values*/     

#define PARAM_PORT1    6
#define PARAM_PORT2    7
#define PARAM_PORT3    8
#define PARAM_PORT4    9
#define PARAM_PORT5    10
#define PARAM_I2C_1    11
#define PARAM_I2C_2    12
#define PARAM_I2C_3    13
#define PARAM_I2C_4    14

/*hard coded parameters*/

#define PARAM_GAS_MIX      15  //contains the indication on the 4 input gases (nothing, O2, Air, N2, ...), lookup table to be implemented by calibrating for each gas
#define PARAM_GAS_RATIO    16

#define PARAM_LVL_MAX        17 
#define PARAM_LVL_MIN        18  

#define PARAM_PH_EQUIL       20

#define PARAM_TEMP_EQUIL     21

/*State FlagVector*/

#define FLAG_VECTOR     3

/*related masks*/

#define FLAG_STEPPER_OFF   (1<<0)   //motor turned off
#define FLAG_PUMPING       (1<<1)   //set the condition to disable targeted modules when pumping is performed

#define ERROR_SERVER_DWN   (1<<10)   //set the condition for individual data control (alert useless here)
#define ERROR_TEMP         (1<<11)   //set the condition to stop temperature control + alert message
#define ERROR_PH           (1<<12)   //set the condition to disable ph control       + alert message
#define ERROR_WEIGHT       (1<<13)   //set the condition to disable pumping control  + alert message
#define ERROR_MEMORY       (1<<14)   //set the condition to disable 
#define MODE_STDBY         (1<<13)   //motor and temperature PID On only
#define MODE_MANUAL        (1<<14)   //everything is set manually
#define MODE_AUTO          (1<<15)   //reactor working by itself, log can be performed

/*Local events*/

#define LOCAL_VECTOR     4

/*related masks*/

#define ENABLE_STEPPER      (1<<0)
#define ENABLE_TEMP_PID     (1<<1)
#define ENABLE_PH_CTRL      (1<<2)
#define CONFIG_MODIF        (1<<3)                                       



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
