#ifndef MAINDECLARE_H
#define MAINDECLARE_H

//========================================================
//      Function Declaration from HPS-FPGA
//========================================================
int boardInitialization();
int boardMemoryCleanUp();
int readFPGA_GPIO_1_input();
void writeFPGA_GPIO_1_output(int);
void writeFPGA_GPIO_0_output(int);
float readFPGA_ADC(int);

#endif /* MAINDECLARE_H */
