/**************
  LIBRAIRIES
**************/

//MultiThread
#include <NilRTOS.h>

//Memory
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

#define I2C_RELAY  B00100100
#define I2C_FLUX   B00100101   //probably wrong (depends on how the address is set by hardware)

//Define here if the LCD screen is used or not
//#define I2C_LCD B00100111
//WIRE_LCD_16_2 B00100111
//WIRE_LCD_20_4 B00100110

/*******************************
  THREADS AND PARAMETERS PRESENT IN EACH CARD 
*******************************/  

#define THR_LINEAR_LOGS       1
#define THR_ETHERNET          1

#define PARAM_ERROR_CODE          22  
#define PARAM_PUMP_STATUS         23



/******************
  DEFINE CARD TYPE
******************/

#define TEMP_CTRL     1
//#define PH_CTRL       1
//#define GAS_CTRL      1
//#define STEPPER_CTRL   1


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
  #define PH              IO1
  #define TAP_ACID_BASE   {I02,PWM2}
  
  // Parameters stored in memory
  #ifdef PH
    #define  PARAM_PH                  2
  #endif
  
  #define PARAM_DESIRED_PH             12
#endif

//*************************************

#ifdef     GAS_CTRL
  // Input/Output
  
  #define  FLUX           I2C_FLUX
  #define  TAP_GAS1_2     {IO1,PWM1}
  #define  TAP_GAS3_4     {IO2,PWM2}
  
  // Parameters stored in memory
  #ifdef FLUX  
    #define PARAM_FLUX_GAS1            3
    #define PARAM_FLUX_GAS2            4
    #define PARAM_FLUX_GAS3            5
    #define PARAM_FLUX_GAS4            6
    //#define PARAM_TAP_GAS1_2     
    //#define PARAM_TAP_GAS3_4   
    #define PARAM_DESIRED_FLUX_GAS1            13
    #define PARAM_DESIRED_FLUX_GAS2            14
    #define PARAM_DESIRED_FLUX_GAS3            15
    #define PARAM_DESIRED_FLUX_GAS4            16
    
  #endif
#endif

//*************************************

#ifdef STEPPER_CTRL
  // Input/Output
  
  //#define  WGHT           IO1
  #define  STEPPER          {IO5,PWM5}
  //#define  TAP_FOOD       IO3
  //#define  TEMP_STEPPER   IO4
  //#define  RELAY_PUMP     I2C_RELAY
  
  // Parameters stored in memory
  
  #ifdef WGHT         
    #define PARAM_ACTUAL_WGHT          7
    #define PARAM_LVL_MAX_WATER        17        
    #define PARAM_LVL_MIN_WATER        18  
  #endif
  
  #ifdef TEMP_STEPPER
    #define PARAM_TEMP_STEPPER         8
  #endif
  
  #define PARAM_WAIT_TIME_PUMP_MOTOR   21 
  
  #ifdef TAP_ACID_BASE
    #define PARAM_TAP_ACID_BASE  
  #endif
  
  #ifdef TAP_FOOD
    #define FOOD_SPEED_TAP            19              
  #endif
  
  #ifdef RELAY_PUMP
    #define PARAM_RELAY_PUMP     
  #endif
  
  #ifdef  STEPPER
    #define  PARAM_STEPPER_SPEED    20        
  #endif 
  
#endif


/**********************
  Control parameters
***********************/

//#define PARAM_GAS_MIX          //contains the indication on the 4 input gases (nothing, O2, Air, N2, ...), 
                                 //lookup table to be implemented by calibrating for each gas
//#define PARAM_GAS_RATIO   

/*State FlagVector*/

//#define FLAG_VECTOR          26           //possible out of the table ??

/*related masks*/

#define FLAG_STEPPER_OFF   (1<<0)   //motor turned off
#define FLAG_PUMPING       (1<<1)   //set the condition to disable targeted modules when pumping is performed

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

byte IO[]={
  IO1, IO2, IO3, IO4};

void setup() {
  delay(1000);
  Serial.begin(9600);
  setupLogger();
  setupDebugger();
  setupParameters();
  
  
  nilSysBegin();
}

void loop() {
}

