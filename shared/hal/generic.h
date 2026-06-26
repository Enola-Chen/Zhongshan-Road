#ifndef _GENERIC_H
#define _GENERIC_H

#include <stdint.h>

// Basic settings
#define HAS_LORA 0        
//#define HAS_DISPLAY 1   

// Disable SD Card
#define HAS_SDCARD 0      

// Disable Sensors
//#define HAS_BME 1 

// Disable LoRa chip settings
#define CFG_sx1276_radio 0 
#define DISABLE_BROWNOUT 1 

// Disable peripherals
//#define BAT_MEASURE_ADC ADC1_GPIO35_CHANNEL 
//#define BAT_VOLTAGE_DIVIDER 2 
//#define HAS_BUTTON (39) 

// Onboard LED pin config for standard ESP32S
#define HAS_LED (2)       

#endif
