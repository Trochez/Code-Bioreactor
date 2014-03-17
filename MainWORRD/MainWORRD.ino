/**************
 * LIBRAIRIES
 **************/

//MultiThread
#include <NilRTOS.h>

//Memory Lib
#include <SST.h>
#include <SPI.h>

#include <avr/wdt.h>

//Ethernet libraries
#include <Ethernet.h>
#include <EthernetUdp.h>


// The normal serial takes 200 bytes more but is buffered
// And if we send a String for parameters it can not be understand ...
// #include <NilSerial.h>
// #define Serial NilSerial

// http://www.arduino.cc/playground/Code/Time
#include <Time.h>


/*define the IN/OUT ports of the card*/




#define AUTOREBOOT 36000 // we will reboot automatically every 1h ... bad trick to prevent some crash problems of ethernet ...
uint16_t autoreboot=0;
// the delay may be prolongated if we received request on the ethernet


/***********************
 * SERIAL, LOGGER AND DEBUGGER
 ************************/

#define SERIAL 1

/********************
 * PIN&ADRESS MAPPING
 *********************/

#define PWM1    6//D6 OC4D
#define PWM2    8//D8 PCINT4
#define PWM3    9//D9 OC4B, OC1A, PCINT5
#define PWM4    5//D5 OC4A  
#define PWM5    11//D11 OC0A, OC1C, PCINT7
#define IO1     21//A3
#define IO2     20//A2
#define IO3     19//A1
#define IO4     22//A4
#define IO5     18//A0

#define I2C_RELAY         32 //B00100000
#define I2C_RELAY_TAP     36 //B00100100
#define I2C_FLUX          104//B01101000
#define I2C_PH            104//B01101000

//Define here if the LCD screen is used or not
//#define I2C_LCD B00100111
//WIRE_LCD_16_2 B00100111
//WIRE_LCD_20_4 B00100110

/*******************************
 * THREADS AND PARAMETERS PRESENT IN EACH CARD 
 *******************************/



#define THR_LINEAR_LOGS       1

#ifdef THR_LINEAR_LOGS
#define LOG_INTERVAL          10  // define the interval in seconds between storing the log
//#define DEBUG_LOGS          1
//#define DEBUG_ETHERNET      0
#endif

#define THR_ETHERNET          1

/******************
 * DEFINE CARD TYPE
 ******************/

#define TEMP_CTRL      1
//#define GAS_CTRL       1
//#define PH_CTRL        1
//#define GAS_CTRL       1
#define STEPPER_CTRL   1


/**********************
 * NETWORK PARAMETERS
 * // Enter a MAC address and IP address for the Arduino controller below.
 * // The IP address is reserved on the routher for this Arduino controller.
 * // CAUTION
 * // Each different boards should have a different IP in the range 172.17.0.100 - 172.17.0.200
 * // and a different MAC address
 ***********************/
//#define IP {172, 17, 0 , 103}
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAB}
//#define IP {172, 17, 0 , 107}
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA}
//#define IP {172, 17, 0 , 105}
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}
//#define IP {172, 17, 0 ,101}                          //stepper
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}      //stepper
//#define IP {172, 17, 0 ,103}                             
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}        
#define IP {172, 17, 0 ,104}                          //bertha 104
#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAC}      //bertha 104
//#define IP {10, 0, 0 ,105}                          //pH
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}      //pH


/*******************************
 * CARD DEFINITION (HARD CODED)
 *******************************/

#ifdef     TEMP_CTRL
// Input/Output
#define  TEMP_LIQ       IO1  
#define  TEMP_PLATE     IO2    
#define  TRANS_PID      PWM4


#define PARAM_TEMP_LIQ             0  // temperature of the solution
#define PARAM_TEMP_PLATE           1  // temperature of the heating plate
#define PARAM_TARGET_LIQUID_TEMP   2  // target temperature of the liquid
#define PARAM_TEMP_MAX             3  // maximal temperature

#ifdef TRANS_PID
//for the regulation of temperature values btw 10 and 45 [s] are commun
#define HEATING_REGULATION_TIME_WINDOWS 5000 //in [ms] 
#define MIN_TEMPERATURE          1000
#define MAX_TEMPERATURE          4500
#endif
#endif


//*************************************

#ifdef STEPPER_CTRL
// Input/Output

#define  WEIGHT                       IO3 // SHOUD BE IO1 !!!!!!!!!!!
#define  STEPPER                      {IO5,PWM5}
#ifdef STEPPER
//#define  TEMP_STEPPER                 IO4
#endif

#define  RELAY_PUMP                   I2C_RELAY

#ifdef TEMP_STEPPER
#define PARAM_TEMP_STEPPER           4   // temperature of the stepper. Can be used for rotation error detection
#endif

#ifdef WEIGHT         
#define PARAM_WEIGHT                 5   // weight of the bioreactor
#define PARAM_WEIGHT_MIN             6   // minimal weight  
#define PARAM_WEIGHT_MAX             7   // maximal weight
//hard coded safety value, TO BE CHANGED ONCE THE SENSOR IS CALIBRATED and conversion performed automatically !!!!!!!!!
#define MIN_ABSOLUTE_WEIGHT          170
#define MAX_ABSOLUTE_WEIGHT          300
#endif

#ifdef RELAY_PUMP
#define PARAM_SEDIMENTATION_TIME     8   // number of MINUTES to wait without rotation before starting emptying
#define PARAM_MIN_FILLED_TIME        9   // minimal time in MINUTES to stay in the filled status
#define PARAM_FOOD_RATIO             10  // ratio between openings
#define PARAM_WEIGHT_STATUS          11  // current STATUS // BBBAAAAA AAAAAAAA : A = wait time in minutes, B = status

#endif


#endif  


//*************************************

#ifdef    PH_CTRL

// Input/Output  

#define PH                     I2C_PH
#define TAP_ACID               I2C_RELAY_TAP
#define TAP_BASE               I2C_RELAY_TAP


#if defined(TAP_ACID) || defined(TAP_BASE)
#define PARAM_RELAY_TAP     XXXX       // Should DISAPPEAR !!! CONTROLLED DIRECTLY (tap connected to output) or via PARAM_STATUS
#endif

#ifdef PH
#define PARAM_PH            12
#define PARAM_TARGET_PH     13
#define PARAM_PH_FACTOR_A   14
#define PARAM_PH_FACTOR_B   15

//not parameters, hard coded values, set the minimal delay between pH adjustements to 10 seconds
#define PH_ADJUST_DELAY      10    //delay between acid or base supplies
#define PH_OPENING_TIME      1     //1sec TAP opening when adjusting
#define PH_TOLERANCE         10    //correspond to a pH variation of 0.1
#endif


#endif


//*************************************

#ifdef     GAS_CTRL

// Input/Output
#define ANEMOMETER_WRITE            I2C_FLUX
#define ANEMOMETER_READ             I2C_FLUX
// #define  TAP_GAS1                   PWM1
// #define  TAP_GAS2                   PWM2
#define  TAP_GAS3                   PWM3
// #define  TAP_GAS4                   PWM4

// Parameters stored in memory
#ifdef TAP_GAS1  
#define PARAM_FLUX_GAS1            16
#define PARAM_DESIRED_FLUX_GAS1    17
#endif

#ifdef  TAP_GAS2
#define PARAM_FLUX_GAS2            18
#define PARAM_DESIRED_FLUX_GAS2    19
#endif

#ifdef  TAP_GAS3
#define PARAM_FLUX_GAS3            20
#define PARAM_DESIRED_FLUX_GAS3    21
#endif

#ifdef  TAP_GAS4
#define PARAM_FLUX_GAS4            22
#define PARAM_DESIRED_FLUX_GAS4    23
#endif

//few hard coded parameters for flux control
#define FLUX_TOLERANCE             10    //define a tolerance of 1 cc/min
#define FLUX_TIME_WINDOWS          10    //define a control windows of 10sec for the flux

//#define DEBUG_GAZ                    1  
#endif

/******************
 * FLAG DEFINITION
 ******************/

#define PARAM_STATUS       25


#define FLAG_STEPPER_CONTROL     0   // need to be set to 1 for control of engine
#define FLAG_PH_CONTROL          1   // set the condition to disable targeted modules when pumping is performed
#define FLAG_GAZ_CONTROL         2
#define FLAG_FOOD_CONTROL        3   // need to be set to 1 for control of food


#define FLAG_RELAY_FILLING       8
#define FLAG_RELAY_EMPTYING      9
#define FLAG_RELAY_NOTUSED1      10
#define FLAG_RELAY_NOTUSED2      11
#define RELAY_PUMP_SHIFT         8 // We need to shift of 4 bits to get the value to send to relay board

#define FLAG_RELAY_ACID          12
#define FLAG_RELAY_BASE          13
#define FLAG_RELAY_NOTUSED3      14
#define FLAG_RELAY_NOTUSED4      15
#define RELAY_TAP_SHIFT          12 // We need to shift of 4 bits to get the value to send to relay board


/*********
 * SETUP
 *********/

void setup() {
  delay(5000);
Serial.begin(9600);
  setupParameters();

#ifdef THR_LINEAR_LOGS
  setupMemory(); 
  recoverLastEntryN();
  loadLastEntryToParameters();
#endif

  setSafeConditions(false);
  nilSysBegin();

}

void loop() {
}








