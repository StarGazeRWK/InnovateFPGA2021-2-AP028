#ifndef LCDI2C_H_
#define LCDI2C_H_

#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup

bool LCD_REG_WRITE(int file, uint8_t address, uint8_t value);
int lcd_send_cmd (char cmd);
int LCD_SEND_DATA (char data);
bool LCD_Init(int file);


#endif /*LCDI2C_H_*/