/*
 * adc_access.h
 *
 *  Created on: Feb 4, 2018
 *  Modified on: Feb 4, 2018
 *      Author: Lloyd Emokpae
 *      Description:
 *      			 The module contains routines to access the LTC2308 ADC
 */

#ifndef ADC_ACCESS_H_
#define ADC_ACCESS_H_

#define NB_ENABLE 	1
#define NB_DISABLE	2

// ----------------------------------------------
// ADC VOLTAGE DIVIDER RESISTOR CONSTANTS
// ----------------------------------------------
#define R_DIV1				3230
#define R_DIV2				3230
#define R_DIV3				3230
#define R_DIV4				50000
#define R_DIV5				50000
#define R_DIV6				100000
#define R_DIV7				100000
#define R_DIV8				50000
#define STRAIGHT_RESISTANCE 37300.0				// resistance when straight
#define BEND_RESISTANCE		90000.0				// resistance at 90 deg

// ----------------------------------------------
// ADC Access State Machine
// (MAIN)
// ----------------------------------------------
#define MAIN_MENU_ADC    			0        	// Main Menu
#define CHANNEL_1					1			// Test Channel 1
#define CHANNEL_2    				2        	// Test Channel 2
#define CHANNEL_3    				3        	// Test Channel 3
#define CHANNEL_4    				4        	// Test Channel 3
#define CHANNEL_5    				5        	// Test Channel 3
#define CHANNEL_6    				6        	// Test Channel 3
#define CHANNEL_7    				7        	// Test Channel 3
#define CHANNEL_8    				8        	// Test Channel 3
#define CHANNEL_ALL    				9        	// Test Channel 3
#define EXIT_MAIN_MENU_ADC			10			// Exit Main Menu

// ----------------------------------------------
// OTHER CONSTANTS
// ----------------------------------------------
#define DELAY_BETWEEN_SAMPLES			30000
#define DELAY_BETWEEN_SAMPLES_ADC_TEST	10000
#define DELAY_BETWEEN_SAMPLES_STREAM	1
#define VCC								5.0
// ----------------------------------------------
// Function Prototypes
// ----------------------------------------------
float getAndConvChannel(int ch, void * LTC_0X00_ADDR, void * LTC_0X01_ADDR, float R_DIV, int DELAY_SAMPLES);
void adc_test_mode(void * base_addr_adc, void * led_addr, int readNum);
long map(long x, long in_min, long in_max, long out_min, long out_max);

int kbhit();
void nonblock(int state);

#endif /* IMU_ACCESS_H_ */
