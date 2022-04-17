#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Communication.h"
#include "Timer.h"
#include "CN0398.h"
#include "UrtLib.h"

#include "mainFPGA.h"
#include "lcdi2c.h"

#define MAX_TIMINGS	85
#define DEBUG 0
#define WAIT_TIME 2000

char host_addr[30] = "127.0.0.1";
char usr_id[30] = "XXXX";
char usr_pass[30] = "XXX";
char database[30] = "XXX";
unsigned int header_display = 0;
unsigned int display_id_buf= 0;
char *A[1];
char *B[1];

float intensity;
float moisture;
float water_level;
float Co2_gas;

//DHT22
char mode = 'c';      // valid modes are c, f, h
int data[5] = { 0, 0, 0, 0, 0 };
float temperature = -1;
float temp_fahr = -1;
float humidity  = -1;
uint8_t pHvalue;

int pump1;
int pump2;
int pump3;
int valve;
string color;

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

void read_ADC_Data()
{
	intensity=readFPGA_ADC(0);
	moisture=readFPGA_ADC(1);
	water_level=readFPGA_ADC(2);
	Co2_gas=readFPGA_ADC(3);
}

void insert_data()
{
    MYSQL *con = mysql_init(NULL);

    char buf[1024] = {};
    char insert_query[] = "INSERT INTO farm_data(temp, humi, moisture, intensity, water_level, Co2_gas, pH, pump1, pump2, pump3, valve, color) VALUES(%.2f, %.2f,%.2f, %.2f,%.2f, %.2f,%.2f, %d, %d, %d, %d, %s)";

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, host_addr, usr_id, usr_pass,
        database, 0, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }

    sprintf(buf, insert_query,humidty,temperature, moisture, intensity, water_level, Co2_gas, pH, pump1, pump2, pump3, valve, color);
    if (mysql_query(con, buf)) 
    {
        finish_with_error(con);
    }

    mysql_close(con);
}


int select_lastrowFarm()
{
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }

    if (mysql_real_connect(con, host_addr, usr_id, usr_pass,
        database, 0, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }

    if (mysql_query(con, "SELECT id, temp, humi, moisture, intensity, water_level, Co2_gas, pH, pump1, pump2, pump3, valve, color FROM farm_data ORDER BY farm_datadate DESC LIMIT 1"))
    {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
    {
        finish_with_error(con);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
    MYSQL_FIELD *field;
    int row_count=0;

    while ((row = mysql_fetch_row(result)))
    {
        row_count++;
        B[0] = row[0] ? strdup(row[0]) : "NULL";
        printf("%s %s\n", A[0], B[0]);
        printf("%s %s\n", *A, *B);
        
        int K;
        sscanf(*A, "%d", K)
        printf("%d", K);

        for(int i = 0; i < num_fields; i++)
        {
            if (i == 0)
            {
                while(field = mysql_fetch_field(result))
                {
                    if(header_display==0)   printf("%s \t", field->name);
                }

                printf("\n");
            }
            
            printf("%s  \t", row[i] ? row[i] : "NULL");
        }
    }
    printf("\n");

    if(header_display==0) header_display=1;
    A[0] = B[0];

    mysql_free_result(result);
    mysql_close(con);
    return(1);
}

void pH_initialize()
{
	cn0398.setup();
	cn0398.init();

	timer.sleep(500);
	printf("Initialization complete!\r\n");

	cn0398.calibrate_ph();
}

void read_pH_Data()
{
	cn0398.use_nernst = true;
	cn0398.set_data();
	cn0398.display_data();
	usleep(5000000);
}

void lcd_display()
{
	char str[20];
	int i=0;

	sprintf(str, "%f", temperature);
	while (i<20) LCD_SEND_DATA (str[i]);

	sprintf(str, "%f", humidity);
	while (i<20) LCD_SEND_DATA (str[i]);

	sprintf(str, "%f", intensity);
	while (i<20) LCD_SEND_DATA (str[i]);

    sprintf(str, "%f", moisture);
	while (i<20) LCD_SEND_DATA (str[i]);

    sprintf(str, "%f", water_level);
	while (i<20) LCD_SEND_DATA (str[i]);

    sprintf(str, "%f", Co2_gas);
	while (i<20) LCD_SEND_DATA (str[i]);

    sprintf(str, "%f", pHValue);
	while (i<20) LCD_SEND_DATA (str[i]);
}

void activate_actuator()
{
    uint8_t maskPump = 0x00;
    uint8_t maskValve =0x00;

    if(water_level<400) maskPump = 0x05;
    if(moisture<200) maskValve = 0x08; maskPump = 0x02;
    if(pHValue<7)   maskPump = 0x01;
    if(pHValue>7)   maskPump = 0x04;

    writeFPGA_GPIO_0_outputRGB(0x06);  //cyan
    color = "Cyan";
    pump1=0; pump2=0; pump3=0; valve=0;
    
    uint8_t maskValue = maskValve | maskPump;
    if (maskValue & 0x01 == 1)  pump1 == 1;
    if (maskValue & 0x02 == 1)  pump2 == 1;
    if (maskValue & 0x04 == 1)  pump3 == 1;
    if (maskValue & 0x08 == 1)  valve == 1;

    writeFPGA_GPIO_0_output(maskValve | maskPump);
}

int read_DHT22_Data()
{
	uint8_t laststate = 0x01;
	uint8_t counter	= 0;
	uint8_t j = 0;
	uint8_t i;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	/* pull pin down for 18 milliseconds */
	writeFPGA_ARDUINO_DHT(0x00);
	usleep(18000000);

    writeFPGA_ARDUINO_DHT(0x01);

	/* detect change and read data */
	for ( i = 0; i < MAX_TIMINGS; i++ ) {
		counter = 0;
		while ( readFPGA_ARDUINO_DHT() == laststate ) {
			counter++;
			usleep(1000);
			if ( counter == 255 ) {
				break;
			}
		}
		laststate = readFPGA_ARDUINO_DHT();

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) ) {
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ( (j >= 40) && (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) ) {
		float h = (float)((data[0] << 8) + data[1]) / 10;
		if ( h > 100 ) {
			h = data[0];	// for DHT11
		}
		float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if ( c > 125 ) {
			c = data[2];	// for DHT11
		}
		if ( data[2] & 0x80 ) {
			c = -c;
		}
		float temperature = c;
		float temp_fahr = c * 1.8f + 32;
		float humidity = h;
		if (DEBUG) printf( "read_dht_data() Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", humidity, temperature, temp_fahr );
		return 0; // OK
		} else {
			if (DEBUG) printf( "read_dht_data() Data not good, skip\n" );
			temp_cels = temp_fahr = humidity = -1;
			return 1; // NOK
		}
}

int main(int argc, char **argv)
{
    if(boardInitialization() != 1)	{ return(0); }
    LCD_Init();
    pH_initialize();

    int count =0;
    while(count <3)
    {
        read_ADC_Data();
        read_DHT22_Data();
        read_pH_Data();
        activate_actuator();
        insert_data();

        lcd_display();
        select_lastrowFarm();
        usleep(1000000);
        count++;
    }

    if(boardMemoryCleanUp() != 1)	{ return(0); }
    exit(0);
}