#ifndef MAINDECLARE_H
#define MAINDECLARE_H

//========================================================
//      Function Declaration from HPS-FPGA
//========================================================
int boardInitialization();
int boardMemoryCleanUp();
int readFPGA_GPIO_1_input();
int readFPGA_ARDUINO_DHT22();
void writeFPGA_GPIO_1_output(int);
void writeFPGA_GPIO_0_output(int);
void writeFPGA_ARDUINO_DHT(int);
void writeFPGA_GPIO_0_outputRGB(int);
float readFPGA_ADC(int);

#endif /* MAINDECLARE_H */
