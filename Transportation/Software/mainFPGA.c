#include "mainFPGA.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"
#include "adc_access.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

//=========================================================
//       Variable
//=========================================================
void *virtual_base;
int fd;

//=========================================================
//          RFS
//=========================================================
void *h2p_lw_gpio0_addr;
int gpio0_mask;

//=========================================================
//          RELAY
//=========================================================
void *h2p_lw_gpio1_output_addr;
int gpio1_mask;

//=========================================================
//          ADC
//=========================================================
void *h2p_base_addr_adc;
unsigned int SYSTEM_BUS_WIDTH = 32;		// SPI HDL uses 32-bit bus width
void * LTC_0X00_ADDR;
void * LTC_0X01_ADDR;
int test_read = 0;
int adc_value = 0;
float adc_voltage=0.0;

//==========================================================
//			DE10 NANO Board
//==========================================================
int boardInitialization() {
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( -1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( -1 );
	}
	
	//RELAY
	h2p_lw_gpio1_output_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + RELAY_PIO_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	
	//RELAY
	h2p_lw_gpio1_output_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + RELAY_PIO_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	
	//RFS
	h2p_lw_gpio0_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + RH_TEMP_DRDY_N_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	
	//ADC
	h2p_base_addr_adc = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ADC_0_BASE ) & ( unsigned long)( HW_REGS_MASK ) );

	LTC_0X00_ADDR = h2p_base_addr_adc + (0X00000000)*(SYSTEM_BUS_WIDTH/8);
	LTC_0X01_ADDR = h2p_base_addr_adc + (0X00000001)*(SYSTEM_BUS_WIDTH/8);

	return(1);
}

int boardMemoryCleanUp() {
	// clean up our memory mapping and exit
	
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( -1 );
	}

	close( fd );
	return( 1 );
}

int readFPGA_GPIO_1_input()	
{ 
  input_button = *(uint32_t *)h2p_lw_gpio1_input_addr; 
  return(input_button);
}

void writeFPGA_GPIO_1_output(int gpio1_mask_temp)	
{ *(uint32_t *)h2p_lw_gpio1_output_addr = ~gpio1_mask_temp; }
/*
void writeFPGA_GPIO_0_output(int gpio0_mask_temp)			
{ *(uint32_t *)h2p_lw_gpio0_output_addr = gpio0_mask_temp; }
*/
float readFPGA_ADC(int ch) {
	test_read = 0, adc_value = 0;
	adc_voltage=0.0;

	// set measure number for ADC convert
	*(uint32_t *)LTC_0X01_ADDR = 1;

	// start measure
	*(uint32_t *)LTC_0X00_ADDR = ((ch << 1) | 0x00);
	*(uint32_t *)LTC_0X00_ADDR = ((ch << 1) | 0x01);
	*(uint32_t *)LTC_0X00_ADDR = ((ch << 1) | 0x00);
	usleep(1);

	// wait measure done
	test_read = *(uint32_t *)LTC_0X00_ADDR;
	while ((test_read & 0x01) == 0x00)
	{
		test_read = *(uint32_t *)LTC_0X00_ADDR;
	}

	// Read raw ADC value
	adc_value = *(uint32_t *)LTC_0X01_ADDR;

	// Convert raw ADC value to voltage
	adc_voltage = (float)adc_value / 825.0;

	usleep(DELAY_BETWEEN_SAMPLES);

	printf("Channel %d : %.5f\n",ch, adc_voltage);
	return(adc_voltage);
}

//==========================================================
//			Main
//==========================================================
/*
int main() {
	if(boardInitialization() != 1)	{ return(0); }

	led_mask = 0x01;
	gpio1_mask = 0x01;
	
	printf("1. Image \n" );
	printf("2. Odour \n");
	printf("3. Capacitive \n");
	printf("4. Hybrid \n");

	while(1){
		
		readFPGA_GPIO_1_input();

		if(input_button & ImageButton_Mask) 		{ Image_Subroutine(); } 
		if(input_button & OdourButton_Mask) 		{ Odour_Subroutine(); }
		if(input_button & CapacitiveButton_Mask) 	{ Capacitive_Subroutine(); }
		if(input_button & HybridButton_Mask) 		{ Hybrid_Subroutine(); }
		
		// wait 100ms
		usleep( 100*1000 );
    }

	if(boardMemoryCleanUp() != 1) { return(0); }

	return( 1 );

}*/