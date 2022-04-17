#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include "lcdi2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define DATA_READY_TIMEOUT  (alt_ticks_per_second()/3)

bool LCD_REG_WRITE(int file, uint8_t address, uint8_t value){
	bool bSuccess = false;
	uint8_t szValue[2];
	
	// write to define register
	szValue[0] = address;
	szValue[1] = value;
	if (write(file, &szValue, sizeof(szValue)) == sizeof(szValue)){
			bSuccess = true;
	}
		
	
	return bSuccess;		
}

int lcd_send_cmd (char cmd)
{
    bool bSuccess;
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0

    for (int i=0;i<4;i++)
	{
        if (bSuccess){
        bSuccess = LCD_REG_WRITE(file, SLAVE_ADDRESS_LCD, data_t);
        
        }
    return bSuccess; 
}

int LCD_SEND_DATA (char data)
{
    //lcd initialize
    int file;
	const char *filename = "/dev/i2c-2";
	uint8_t id;

    // open bus
	if ((file = open(filename, O_RDWR)) < 0) {
  	  /* ERROR HANDLING: you can check errno to see what went wrong */
	    perror("Failed to open the i2c bus of lcd");
  	  exit(1);
	}	

    int addr = SLAVE_ADDRESS_LCD; 
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
  	  printf("Failed to acquire bus access and/or talk to slave.\n");
	    /* ERROR HANDLING; you can check errno to see what went wrong */
  	  exit(1);
	}

    bool bSuccess;
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1

	for (int i=0;i<4;i++)
	{
        if (bSuccess){
        bSuccess = LCD_REG_WRITE(file, SLAVE_ADDRESS_LCD, data_t);
        
        }
    return bSuccess; 
}

bool LCD_Init(int file){
    
    
    usleep(40000);  // wait for >40ms
	lcd_send_cmd (0x30);
	usleep(5000);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	usleep(100);  // wait for >100us
	lcd_send_cmd (0x30);
	usleep(10000000);
	lcd_send_cmd (0x20);  // 4bit mode
	usleep(10000000);

    // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	usleep(1000000);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	usleep(1000000);
	lcd_send_cmd (0x01);  // clear display
	usleep(1000000);
	usleep(1000000);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	usleep(1000000);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
        
}


