
#include <SST.h>
#include <SPI.h>

//Ethernet libraries
#include <Ethernet.h>
#include <EthernetUdp.h>
// This is a more complex example showing many simultaneous action
//
#include <NilRTOS.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

// The normal serial takes 200 bytes more but is buffered
// And if we send a String for parameters it can not be understand ...
// #include <NilSerial.h>
// #define Serial NilSerial

// http://www.arduino.cc/playground/Code/Time
#include <Time.h>

#define THR_MONITORING 1000
/*define the IN/OUT ports of the card*/

#define PWM1    6//D6 OC4D
#define PWM2    8//D8 PCINT4
#define PWM3    9//D9 OC4B, OC1A, PCINT5
#define PWM4    5//D6 OC4A  
#define PWM5    11//D11 OC0A, OC1C, PCINT7
#define IO1     21//A3
#define IO2     20//A2
#define IO3     19//A1
#define IO4     22//A4
#define IO5     18//A0

#define I2C_RELAY  B00100100
#define I2C_FLUX   B00100101   //probably wrong (depends on how the address is set by hardware)

//select a Card definition

#define TEMP_CTRL     0
//#define PH_CTRL       0
//#define GAS_CTRL      0
//#define STEPPER_CTRL  0


// Device define

#ifdef     TEMP_CTRL
  #define  TEMP_LIQ       IO1
  #define  TEMP_PLATE     IO2
//  #define  RELAY_PID      I2C_RELAY 
#endif

#ifdef    PH_CTRL
  #define PH              IO1
  #define TAP_ACID_BASE   {I02,PWM2}
#endif

#ifdef     GAS_CTRL
  #define  FLUX           I2C_FLUX
  #define  TAP_GAS1_2     {IO1,PWM1}
  #define  TAP_GAS3_4     {IO2,PWM2}
#endif

#ifdef STEPPER_CTRL
  #define  WGHT           IO1
  #define  STEPPER        {IO4,PWM4}
  #define  TAP_FOOD       IO3
  #define  TEMP_STEPPER   IO5
  #define  RELAY_PUMP     I2C_RELAY
#endif


/*Hard coded parameters*/     

#ifdef TEMP_LIQ
#define PARAM_TEMP_LIQ      0
#endif

#ifdef TEMP_PLATE
#define PARAM_TEMP_PLATE    1
#endif

#ifdef TEMP_STEPPER
#define PARAM_TEMP_STEPPER  2
#endif

#ifdef   PH
#define  PARAM_PH           3
#endif  

#ifdef  WGHT         
#define  PARAM_WGHT         4
#endif

#ifdef FLUX  
  #define   PARAM_FLUX_GAS1      5
  #define   PARAM_FLUX_GAS2      6
  #define   PARAM_FLUX_GAS3      7
  #define   PARAM_FLUX_GAS4      8
  #define   PARAM_TAP_GAS1_2     9
  #define   PARAM_TAP_GAS3_4     10

#endif

#ifdef  TAP_ACID_BASE
#define  PARAM_TAP_ACID_BASE  13
#endif


#ifdef  TAP_FOOD
  #define  PARAM_TAP_GAS4       14
#endif

#ifdef  RELAY_PUMP
  #define  PARAM_RELAY_PUMP     15
#endif

#ifdef  RELAY_PID
  #define  PARAM_RELAY_PID      15
  //for the regulation of temperature values btw 10 and 45 [s] are commun
  #define HEATING_REGULATION_TIME_WINDOWS 5000 //in [ms] 
#endif

#ifdef  STEPPER
  #define  PARAM_STEPPER        16
#endif 



//to be CHECKED
/*control parameters*/

#define PARAM_GAS_MIX        17  //contains the indication on the 4 input gases (nothing, O2, Air, N2, ...), lookup table to be implemented by calibrating for each gas
#define PARAM_GAS_RATIO      18

#define PARAM_LVL_MAX        19
#define PARAM_LVL_MIN        20 
#define PRAM_WAIT_PUMP       21

#define PARAM_PH_EQ          22

#define PARAM_TEMP_EQ        23
#define PARAM_TEMP_MAX       24
#define PARAM_TEMP_MIN       25


/*State FlagVector*/

#define FLAG_VECTOR          26           //possible out of the table ??

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


/*Setup*/

byte IO[]={
  IO1, IO2, IO3, IO4};

void setup() {
  Serial.begin(9600);
  setupLogger();
  setupDebugger();
  setupParameters();
  nilSysBegin();
}

void loop() {
}

