///*
//  This code should replace the Linear Logs thread in the future
//*/
//
//#include <SST.h>
//
////ifndef SPI
////#include <SPI.h>
////endif
//
///*
//Memory area is a circle 
//Second : 0-100000*SIZE_LOG
//Minutes: 100001-20000
//Hour:    200001-30000
//*/
//#define SIZE_LOG  32
//
//#define  NB_SECONDS  100000
//#define  NB_MINUTES  200000
//#define  NB_HOURS    300000
//#define  ADD_SECONDS 0
//#define  ADD_MINUTES 3200000
//#define  ADD_HOURS   6400000
//
//void loggingSetup();
//void logging();
//int getMemoryAddress(int time, int type);
//void updateSeconds();
//void updateMinutes();
//void updateHours();
//
////operation on previous data : min max average, ...
//void average();
//
///*
//Update every second for 60 seconds
//Update every minute for 60 minutes
//Update every hour for 1 year
//*/
//
////SST flash = SSRT(pin);
//
//void loggingSetup(){
//    //SPI.begin()
//    //...
//    // sst.init();
//}
//
//void logging(int address, uint8_t* data, uint8_t dataSize){
// // 
//   for(int i=0; i<dataSize; i++){
//      //write 
//   }
//}
//
//// based on time_t the number of second elapsed since 1st january 1970
//// look at http://www.pjrc.com/teensy/td_libs_Time.html
//// setTime(t);
//int getMemoryAddress(int time, int type){
//    switch(type){
//       case 1:
//         return (time % NB_SECONDS)*SIZE_LOG + ADD_SECONDS;
//       break; 
//       
//       case 2:
//         return ((time/60) % NB_MINUTES)*SIZE_LOG + ADD_MINUTES;
//       break; 
//       
//       case 3:
//         return ((time/3600) % NB_HOURS)*SIZE_LOG + ADD_HOURS;
//       break;
//    }
//    return 0;
//}
