/**************
  LIBRAIRIES
**************/

<<<<<<< HEAD
//MultiThread  
=======
//MultiThread
>>>>>>> 6c8ffe67d0060a99780741634f96094e01ffee54
#include <NilRTOS.h>

//Memory Lib
#include <SST.h>
#include <SPI.h>

//Ethernet libraries
#include <Ethernet.h>
#include <EthernetUdp.h>

// The normal serial takes 200 bytes more but is buffered
// And if we send a String for parameters it can not be understand ...
// #include <NilSerial.h>
// #define Serial NilSerial

// http://www.arduino.cc/playground/Code/Time
#include <Time.h>

//#define THR_MONITORING 13
/*define the IN/OUT ports of the card*/

/***********************
SERIAL, LOGGER AND DEBUGGER
************************/

<<<<<<< HEAD
//#define SERIAL 1
=======
#define SERIAL 1
>>>>>>> 6c8ffe67d0060a99780741634f96094e01ffee54

/********************
  PIN&ADRESS MAPPING
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
  THREADS AND PARAMETERS PRESENT IN EACH CARD 
*******************************/  

#define THR_LINEAR_LOGS       1

#ifdef THR_LINEAR_LOGS
  #define LOG_INTERVAL        10
  #define RRD_OFF             1  
  //#define DEBUG_LOGS          1
  //#define DEBUG_ETHERNET      1
  //#define RRD_ON              1
#endif

#define THR_ETHERNET          1

/******************
  DEFINE CARD TYPE
******************/

//#define TEMP_CTRL      1
<<<<<<< HEAD
//#define PH_CTRL        1
#define GAS_CTRL       1
#define PH_CTRL        1
=======
#define PH_CTRL        1
//#define GAS_CTRL       1
//#define STEPPER_CTRL   1
>>>>>>> 6c8ffe67d0060a99780741634f96094e01ffee54


/**********************
  NETWORK PARAMETERS
// Enter a MAC address and IP address for the Arduino controller below.
// The IP address is reserved on the routher for this Arduino controller.
// CAUTION
// Each different boards should have a different IP in the range 172.17.0.100 - 172.17.0.200
// and a different MAC address
***********************/
<<<<<<< HEAD
#define IP {172, 17, 0 , 103}
#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAB}
//#define IP {172, 17, 0 , 104}
//define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA}
//#define IP {172, 17, 0 , 105}
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}
=======

>>>>>>> 6c8ffe67d0060a99780741634f96094e01ffee54
//#define IP {172, 17, 0 ,101}                          //stepper
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}      //stepper
//#define IP {172, 17, 0 ,103}                             
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}        
//#define IP {172, 17, 0 ,104}                          //gas
//#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDD}      //gas
#define IP {172, 17, 0 ,105}                          //pH
#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAD}      //pH

#define ALIX {172,17,0,10} 

#define NTP_UPDATE_TIME 7200

/*******************************
  CARD DEFINITION (HARD CODED)
*******************************/  

#ifdef     TEMP_CTRL

  // Input/Output
  #define  TEMP_LIQ       IO1
  #define  TEMP_PLATE     IO2
  #define  TRANS_PID      PWM5
  
  // Parameters stored in memory
  #ifdef TEMP_LIQ
    #define PARAM_TEMP_LIQ             0
  #endif
  
  #ifdef TEMP_PLATE
    #define PARAM_TEMP_PLATE           1
  #endif
  
  #define PARAM_DESIRED_LIQUID_TEMP    9  
  #define PARAM_TEMP_MIN               10
  #define PARAM_TEMP_MAX               11
  
  #ifdef TRANS_PID
    #define  RELAY_PID      200
    //for the regulation of temperature values btw 10 and 45 [s] are commun
    #define HEATING_REGULATION_TIME_WINDOWS 5000 //in [ms] 
  #endif
#endif

//*************************************

#ifdef    PH_CTRL
  
  // Input/Output  
  
  //#define PH                     IO1
  #define PH                     I2C_PH            // #define  RELAY_PUMP                   I2C_RELAY
  #define TAP_ACID               I2C_RELAY_TAP
  #define TAP_BASE               I2C_RELAY_TAP
  #define TAP_FOOD               I2C_RELAY_TAP
  
  
  #if defined(TAP_ACID) || defined(TAP_BASE) || defined (TAP_FOOD)
  
      //same value as for PARAM_RELAY_PUMP but used with a bit shift >>8
      #define PARAM_RELAY_TAP    25       
  #endif
  
  #ifdef PH                                       //#ifdef RELAY_PUMP
    #define PARAM_DESIRED_PH     12               //#define PARAM_WAIT_TIME_PUMP_MOTOR   21
    #define PARAM_PH             2                //#define PARAM_RELAY_PUMP             25
    
       
    //not parameters, hard coded values, set the minimal delay between pH adjustements to 10 seconds
    #define PH_ADJUST_DELAY      10    //delay between acid or base supplies
    #define PH_OPENING_TIME      1     //1sec TAP opening when adjusting
    #define PH_TOLERANCE         10    //correspond to a pH variation of 0.1
    
  #endif

  #ifdef TAP_FOOD
    #define PARAM_FOOD_PERIOD     19   //time between openings
    
    //not a parameter, hard coded 1 sec opening time
    #define FOOD_OPENING_TIME     1 

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
    #define PARAM_FLUX_GAS1            3
    #define PARAM_DESIRED_FLUX_GAS1    13
  #endif
  
  #ifdef  TAP_GAS2
    #define PARAM_FLUX_GAS2            4
    #define PARAM_DESIRED_FLUX_GAS2    14
  #endif
  
  #ifdef  TAP_GAS3
    #define PARAM_FLUX_GAS3            5
    #define PARAM_DESIRED_FLUX_GAS3    15
  #endif
  
  #ifdef  TAP_GAS4
    #define PARAM_FLUX_GAS4            6
    #define PARAM_DESIRED_FLUX_GAS4    16
  #endif
    
    //few hard coded parameters for flux control
    #define FLUX_TOLERANCE             10    //define a tolerance of 1 cc/min
    #define FLUX_TIME_WINDOWS          10    //define a control windows of 10sec for the flux
    
  //#define DEBUG_GAZ                    1  
#endif

//*************************************

#ifdef STEPPER_CTRL
  // Input/Output
  
<<<<<<< HEAD
  #define  WGHT                         IO5
  #define  STEPPER                      {IO2,PWM2}
=======
  #define  WGHT                         IO1
  #define  STEPPER                      {IO5,PWM5}
>>>>>>> 6c8ffe67d0060a99780741634f96094e01ffee54
  //#define  TEMP_STEPPER                 IO4
  #define  RELAY_PUMP                   I2C_RELAY
  
  // Parameters stored in memory
  #ifdef WGHT         
    #define PARAM_WGHT                   7
    #define PARAM_LVL_MAX_WATER          17        
    #define PARAM_LVL_MIN_WATER          18  
  #endif
  
  #ifdef TEMP_STEPPER
    #define PARAM_TEMP_STEPPER           8
  #endif
  
  #ifdef RELAY_PUMP
    #define PARAM_WAIT_TIME_PUMP_MOTOR   21
    #define PARAM_RELAY_PUMP             25
  #endif
  
  #ifdef  STEPPER
    #define  PARAM_STEPPER_SPEED         20        
  #endif 
  
#endif  


/******************
  ERROR DEFINITION
******************/

#define PARAM_EVENT        22
#define PARAM_EVENT_VALUE  24
#define FLAG_VECTOR        23

/*related masks*/

#define EVENT_OCCURED      (1<<0)
#define FLAG_STEPPER_OFF   (1<<1)   //motor turned off
#define FLAG_PUMPING       (1<<2)   //set the condition to disable targeted modules when pumping is performed

#define ERROR_SERVER_DWN   (1<<10)   //set the condition for individual data control (alert useless here)
#define ERROR_PID          (1<<11)   //set the condition to stop temperature control + alert message
#define ERROR_PH           (1<<12)   //set the condition to disable ph control       + alert message
#define ERROR_WEIGHT       (1<<13)   //set the condition to disable pumping control  + alert message
#define ERROR_MEMORY       (1<<14)   //set the condition to disable 
#define MODE_STDBY         (1<<13)   //motor and temperature PID On only
#define MODE_MANUAL        (1<<14)   //everything is set manually
#define MODE_AUTO          (1<<15)   //reactor working by itself, log can be performed                                    


/*********
  SETUP
*********/

void setup() {
  delay(1000);
  setupParameters();
  #ifdef SERIAL
    while(!Serial)
    { ; 
    }
  #endif
  
  nilSysBegin();
}

void loop() {
}
