
// Definition of all events to be logged
#define ARDUINO_BOOT           1
#define ARDUINO_SET_SAFE       2
#define RESET_ETHERNET         3

#define PUMPING_FILLING_START    10
#define PUMPING_FILLING_STOP     11
#define PUMPING_FILLING_FAILURE  12

#define PUMPING_EMPTYING_START   13
#define PUMPING_EMPTYING_STOP    14
#define PUMPING_EMPTYING_FAILURE 15




#define MOTOR_START            20
#define MOTOR_STOP             21





#define TEMP_LIQ_FAILED        50
#define TEMP_LIQ_RECOVER       51
#define TEMP_PLATE_FAILED      52
#define TEMP_PLATE_RECOVER     53
#define TEMP_STEPPER_FAILED    54
#define TEMP_STEPPER_RECOVER   55


#define WEIGHT_FAILURE           129
#define WEIGHT_BACK_TO_NORMAL    130


#define ERROR_NOT_FOUND_ENTRY_N  150


//When parameters are set (and saved) an event is recorded (256-281 : A-Z)
#define SAVE_ALL_PARAMETER     255
#define PARAMETER_SET          256


