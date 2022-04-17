// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
#include "iothub_module_client_ll.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "iothub_client_options.h"
#include "iothubtransportmqtt.h"
#include "iothub.h"

#include "terasic_includes.h"
#include "mainFPGA.h"
#include "rh_temp.h"
#include "hps_0.h"
#include "lcdi2c.h"

struct gps_tpv tpv;
char buf[256];
float fTemperature;
float fHumidity;
//static const char* connectionString = "HostName=de10-nano-iot-test-ZHPRO.azure-devices.net;DeviceId=de10-nano-device;SharedAccessKey=Wu1fjEmfjlX6azDPV/0FugQQwfiJg6JQm3Mms52oufk=";

static int callbackCounter;
static char msgText[1024];
static char propText[1024];
#define MESSAGE_COUNT 500
#define DOWORK_LOOP_NUM     60

typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    size_t messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;
    (void)printf("Confirmation[%d] received for message tracking id = %lu with result = %s\r\n", callbackCounter, (unsigned long)eventInstance->messageTrackingId, MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    /* Some device specific action code goes here... */
    callbackCounter++;
    IoTHubMessage_Destroy(eventInstance->messageHandle);
}

void lcd_display()
{
	char str[20];
	int i=0;

	sprintf(str, "%f", fTemperature);
	while (i<20) LCD_SEND_DATA (str[i]);

	sprintf(str, "%f", humidity);
	while (i<20) LCD_SEND_DATA (str[i]);

	sprintf(str, "%f", tpv.latitude);
	while (i<20) LCD_SEND_DATA (str[i]);

    sprintf(str, "%f", tpv.longitude);
	while (i<20) LCD_SEND_DATA (str[i]);
}

void Sensor_Report(void){
  	bool bPass;
    
  	////////////////////////////////
  	// report HDC1000 temperature & humidity sensor
  	bPass = RH_Temp_Sensor_Read(&fTemperature, &fHumidity);
  	if (bPass){
		printf("Temperature: %.3f*C\r\n", fTemperature);
    	printf("Humidity: %.3f%%\r\n",fHumidity);
  	}else{
  		printf("Failed to ready Temperature/Humidity sensor!\r\n");
  	}

  	printf("\r\n");
}


int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                error_message ("error %d setting term attributes", errno);
}

void read_gps_data()
{
    char *portname = "/dev/ttyACM0";

    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
            error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
            return;
    }

    set_interface_attribs (fd, B9600, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd, 0);                // set no blocking

    //write (fd, "hello!\n", );           // send 7 character greeting

    //usleep ((7 + 25) * 100);             // sleep enough to transmit the 7 plus
                                     // receive 25:  approx 100 uS per char transmit
    int n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read

    gps_data_analyse();
}

void print_tpv_value(const char *name, const char *format, const int32_t value, const int32_t scale_factor)
{
    printf("%s: ", name);
    if (GPS_INVALID_VALUE != value)
    {
        printf(format, (double)value / scale_factor);
    }
    else
    {
        puts("INVALID");
    }
}

int gps_data_analyse()
{
    
    int result;

    if (argc != 2)
    {
        fputs("Usage: " PROGNAME " NMEA\n", stderr);
        return EXIT_FAILURE;
    }

    /* Append CRLF to the user supplied string */
    snprintf(str, sizeof(buf), "%s\r\n", argv[1]);

    /* Sets the data to a known state */
    gps_init_tpv(&tpv);

    /* Attempt to decode the user supplied string */
    result = gps_decode(&tpv, buf);
    if (result != GPS_OK)
    {
        fprintf(stderr, "Error (%d): %s\n", result, gps_error_string(result));
        return EXIT_FAILURE;
    }

    /* Go through each TPV value and show what information was decoded */
    printf("Talker ID: %s\n", tpv.talker_id);
    printf("Time Stamp: %s\n", tpv.time);
    print_tpv_value("Latitude", "%.6f\n", tpv.latitude, GPS_LAT_LON_FACTOR);
    print_tpv_value("Longitude", "%.6f\n", tpv.longitude, GPS_LAT_LON_FACTOR);
    print_tpv_value("Altitude", "%.3f\n", tpv.altitude, GPS_VALUE_FACTOR);
    print_tpv_value("Track", "%.3f\n", tpv.track, GPS_VALUE_FACTOR);
    print_tpv_value("Speed", "%.3f\n", tpv.speed, GPS_VALUE_FACTOR);

    printf("Mode: ");
    switch (tpv.mode)
    {
    case GPS_MODE_UNKNOWN:
        puts("Unknown");
        break;
    case GPS_MODE_NO_FIX:
        puts("No fix");
        break;
    case GPS_MODE_2D_FIX:
        puts("2D");
        break;
    case GPS_MODE_3D_FIX:
        puts("3D");
        break;
    default:
        break;
    }

    printf("\n");

    return 0;
}

int main(void)
{
    if(boardInitialization() != 1)	{ return(0); }	
    
    LCD_Init();

    //RFS
    bool bPass = FALSE;
    printf("===== rfs test =====\r\n");

  	////////////////////////////////////
  	// init HDC1000: temperature and humidity sensor
  	RH_Temp_Init(RH_TEMP_I2C_OPENCORES_BASE);
  	bPass = RH_Temp_Sensor_Init();
  	if(bPass)
  		printf("Init HDC1000 successful!\r\n");
  	else
  		printf("Init HDC1000 failed!\r\n");
    
    // FOr IoT edge

    IOTHUB_MODULE_CLIENT_LL_HANDLE iotHubModuleClientHandle;
    EVENT_INSTANCE messages[MESSAGE_COUNT];

    srand((unsigned int)time(NULL));
    double avgWindSpeed = 10.0;
    double minTemperature = 20.0;
    double minHumidity = 60.0;

    callbackCounter = 0;

    if (IoTHub_Init() != 0)
    {
        (void)printf("Failed to initialize the platform.\r\n");
        return 1;
    }
    else if ((iotHubModuleClientHandle = IoTHubModuleClient_LL_CreateFromEnvironment(MQTT_Protocol)) == NULL)
    //else if ((iotHubModuleClientHandle = IoTHubModuleClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol)) == NULL)
    {
        (void)printf("ERROR: iotHubModuleClientHandle is NULL!\r\n");
    }
    else
    {
        // Uncomment the following lines to enable verbose logging (e.g., for debugging).
        // bool traceOn = true;
        // IoTHubModuleClient_LL_SetOption(iotHubModuleClientHandle, OPTION_LOG_TRACE, &traceOn);

        size_t iterator = 0;

        do
        {
            if (iterator < MESSAGE_COUNT)
            {
                Sensor_Report();

                sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":\"myFirstDevice\",\"Temp\":%f,\"Humi\":%d}", (int16_t)fTemperature, (int16_t)fHumidity);

                if ((messages[iterator].messageHandle = IoTHubMessage_CreateFromString(msgText)) == NULL)
                {
                    (void)printf("ERROR: iotHubMessageHandle is NULL!\r\n");
                }
                else
                {
                    (void)IoTHubMessage_SetMessageId(messages[iterator].messageHandle, "MSG_ID");
                    (void)IoTHubMessage_SetCorrelationId(messages[iterator].messageHandle, "CORE_ID");

                    messages[iterator].messageTrackingId = iterator;

                    if (IoTHubModuleClient_LL_SendEventToOutputAsync(iotHubModuleClientHandle, messages[iterator].messageHandle, "gsensorOutput", SendConfirmationCallback, &messages[iterator]) != IOTHUB_CLIENT_OK)
                    {
                        (void)printf("ERROR: IoTHubModuleClient_LL_SendEventAsync..........FAILED!\r\n");
                    }
                    else
                    {
                        (void)printf("IoTHubModuleClient_LL_SendEventAsync accepted message [%d] for transmission to IoT Hub.\r\n", (int)iterator);
                    }
                }

            }
            IoTHubModuleClient_LL_DoWork(iotHubModuleClientHandle);
            ThreadAPI_Sleep(1000);
            iterator++;

            read_gps_data();
            float gas=readFPGA_ADC(0);
	        float data=readFPGA_ADC(1);
            printf("\nGas = %.2f", gas);
            printf("\nData = %.2f", data);

            if(fTemperature>25 || Humidity<60)  
            {
                writeFPGA_GPIO_1_output(0x03);
            }else
            {
                writeFPGA_GPIO_1_output(0x00);
            }

            lcd_display();

        } while (1);


        // Loop while we wait for messages to drain off.
        size_t index = 0;
        for (index = 0; index < DOWORK_LOOP_NUM; index++)
        {
            IoTHubModuleClient_LL_DoWork(iotHubModuleClientHandle);
            ThreadAPI_Sleep(100);
        }

        IoTHubModuleClient_LL_Destroy(iotHubModuleClientHandle);
    }

    (void)printf("Finished executing\n");
    IoTHub_Deinit();

    if(boardMemoryCleanUp() != 1) { return(0); }

    return 0;
}
