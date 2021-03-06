#ifndef _ALTERA_HPS_0_H_
#define _ALTERA_HPS_0_H_

/*
 * This file was automatically generated by the swinfo2header utility.
 * 
 * Created from SOPC Builder system 'soc_system' in
 * file './soc_system.sopcinfo'.
 */

/*
 * This file contains macros for module 'hps_0' and devices
 * connected to the following masters:
 *   h2f_axi_master
 *   h2f_lw_axi_master
 * 
 * Do not include this header file and another header file created for a
 * different module or master group at the same time.
 * Doing so may result in duplicate macro names.
 * Instead, use the system header file which has macros with unique names.
 */

/*
 * Macros for device 'adc_0', class 'altera_up_avalon_adc'
 * The macros are prefixed with 'ADC_0_'.
 * The prefix is the slave descriptor.
 */
#define ADC_0_COMPONENT_TYPE altera_up_avalon_adc
#define ADC_0_COMPONENT_NAME adc_0
#define ADC_0_BASE 0x7000
#define ADC_0_SPAN 32
#define ADC_0_END 0x701f


/*
 * Macros for device 'lcd_i2c', class 'altera_avalon_i2c'
 * The macros are prefixed with 'LCD_I2C_'.
 * The prefix is the slave descriptor.
 */
#define LCD_I2C_COMPONENT_TYPE altera_avalon_i2c
#define LCD_I2C_COMPONENT_NAME lcd_i2c
#define LCD_I2C_BASE 0x0100
#define LCD_I2C_SPAN 64
#define LCD_I2C_END 0x013f


/*
 * Macros for device 'rh_temp_i2c_opencores', class 'i2c_opencores'
 * The macros are prefixed with 'RH_TEMP_I2C_OPENCORES_'.
 * The prefix is the slave descriptor.
 */
#define RH_TEMP_I2C_OPENCORES_COMPONENT_TYPE i2c_opencores
#define RH_TEMP_I2C_OPENCORES_COMPONENT_NAME rh_temp_i2c_opencores
#define RH_TEMP_I2C_OPENCORES_BASE 0x0200
#define RH_TEMP_I2C_OPENCORES_SPAN 32
#define RH_TEMP_I2C_OPENCORES_END 0x021f

/*
 * Macros for device 'gps_uart', class 'altera_up_avalon_rs232'
 * The macros are prefixed with 'GPS_UART_'.
 * The prefix is the slave descriptor.
 */
#define GPS_UART_COMPONENT_TYPE altera_up_avalon_rs232
#define GPS_UART_COMPONENT_NAME gps_uart
#define GPS_UART_BASE 0x0000
#define GPS_UART_SPAN 8
#define GPS_UART_END 0x0007

/*
 * Macros for device 'sysid_qsys', class 'altera_avalon_sysid_qsys'
 * The macros are prefixed with 'SYSID_QSYS_'.
 * The prefix is the slave descriptor.
 */
#define SYSID_QSYS_COMPONENT_TYPE altera_avalon_sysid_qsys
#define SYSID_QSYS_COMPONENT_NAME sysid_qsys
#define SYSID_QSYS_BASE 0x1000
#define SYSID_QSYS_SPAN 8
#define SYSID_QSYS_END 0x1007
#define SYSID_QSYS_ID 2899645186
#define SYSID_QSYS_TIMESTAMP 1648609350

/*
 * Macros for device 'jtag_uart', class 'altera_avalon_jtag_uart'
 * The macros are prefixed with 'JTAG_UART_'.
 * The prefix is the slave descriptor.
 */
#define JTAG_UART_COMPONENT_TYPE altera_avalon_jtag_uart
#define JTAG_UART_COMPONENT_NAME jtag_uart
#define JTAG_UART_BASE 0x2000
#define JTAG_UART_SPAN 8
#define JTAG_UART_END 0x2007
#define JTAG_UART_IRQ 2
#define JTAG_UART_READ_DEPTH 64
#define JTAG_UART_READ_THRESHOLD 8
#define JTAG_UART_WRITE_DEPTH 64
#define JTAG_UART_WRITE_THRESHOLD 8

/*
 * Macros for device 'led_pio', class 'altera_avalon_pio'
 * The macros are prefixed with 'LED_PIO_'.
 * The prefix is the slave descriptor.
 */
#define LED_PIO_COMPONENT_TYPE altera_avalon_pio
#define LED_PIO_COMPONENT_NAME led_pio
#define LED_PIO_BASE 0x3000
#define LED_PIO_SPAN 16
#define LED_PIO_END 0x300f
#define LED_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define LED_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LED_PIO_CAPTURE 0
#define LED_PIO_DATA_WIDTH 7
#define LED_PIO_DO_TEST_BENCH_WIRING 0
#define LED_PIO_DRIVEN_SIM_VALUE 0
#define LED_PIO_EDGE_TYPE NONE
#define LED_PIO_FREQ 50000000
#define LED_PIO_HAS_IN 0
#define LED_PIO_HAS_OUT 1
#define LED_PIO_HAS_TRI 0
#define LED_PIO_IRQ_TYPE NONE
#define LED_PIO_RESET_VALUE 127

/*
 * Macros for device 'dipsw_pio', class 'altera_avalon_pio'
 * The macros are prefixed with 'DIPSW_PIO_'.
 * The prefix is the slave descriptor.
 */
#define DIPSW_PIO_COMPONENT_TYPE altera_avalon_pio
#define DIPSW_PIO_COMPONENT_NAME dipsw_pio
#define DIPSW_PIO_BASE 0x4000
#define DIPSW_PIO_SPAN 16
#define DIPSW_PIO_END 0x400f
#define DIPSW_PIO_IRQ 0
#define DIPSW_PIO_BIT_CLEARING_EDGE_REGISTER 1
#define DIPSW_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define DIPSW_PIO_CAPTURE 1
#define DIPSW_PIO_DATA_WIDTH 4
#define DIPSW_PIO_DO_TEST_BENCH_WIRING 0
#define DIPSW_PIO_DRIVEN_SIM_VALUE 0
#define DIPSW_PIO_EDGE_TYPE ANY
#define DIPSW_PIO_FREQ 50000000
#define DIPSW_PIO_HAS_IN 1
#define DIPSW_PIO_HAS_OUT 0
#define DIPSW_PIO_HAS_TRI 0
#define DIPSW_PIO_IRQ_TYPE EDGE
#define DIPSW_PIO_RESET_VALUE 0

/*
 * Macros for device 'button_pio', class 'altera_avalon_pio'
 * The macros are prefixed with 'BUTTON_PIO_'.
 * The prefix is the slave descriptor.
 */
#define BUTTON_PIO_COMPONENT_TYPE altera_avalon_pio
#define BUTTON_PIO_COMPONENT_NAME button_pio
#define BUTTON_PIO_BASE 0x5000
#define BUTTON_PIO_SPAN 16
#define BUTTON_PIO_END 0x500f
#define BUTTON_PIO_IRQ 1
#define BUTTON_PIO_BIT_CLEARING_EDGE_REGISTER 1
#define BUTTON_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define BUTTON_PIO_CAPTURE 1
#define BUTTON_PIO_DATA_WIDTH 2
#define BUTTON_PIO_DO_TEST_BENCH_WIRING 0
#define BUTTON_PIO_DRIVEN_SIM_VALUE 0
#define BUTTON_PIO_EDGE_TYPE FALLING
#define BUTTON_PIO_FREQ 50000000
#define BUTTON_PIO_HAS_IN 1
#define BUTTON_PIO_HAS_OUT 0
#define BUTTON_PIO_HAS_TRI 0
#define BUTTON_PIO_IRQ_TYPE EDGE
#define BUTTON_PIO_RESET_VALUE 0

/*
 * Macros for device 'relay_pio', class 'altera_avalon_pio'
 * The macros are prefixed with 'RELAY_PIO_'.
 * The prefix is the slave descriptor.
 */
#define RELAY_PIO_COMPONENT_TYPE altera_avalon_pio
#define RELAY_PIO_COMPONENT_NAME relay_pio
#define RELAY_PIO_BASE 0x6000
#define RELAY_PIO_SPAN 16
#define RELAY_PIO_END 0x600f
#define RELAY_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define RELAY_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define RELAY_PIO_CAPTURE 0
#define RELAY_PIO_DATA_WIDTH 8
#define RELAY_PIO_DO_TEST_BENCH_WIRING 0
#define RELAY_PIO_DRIVEN_SIM_VALUE 0
#define RELAY_PIO_EDGE_TYPE NONE
#define RELAY_PIO_FREQ 50000000
#define RELAY_PIO_HAS_IN 0
#define RELAY_PIO_HAS_OUT 1
#define RELAY_PIO_HAS_TRI 0
#define RELAY_PIO_IRQ_TYPE NONE
#define RELAY_PIO_RESET_VALUE 0

/*
 * Macros for device 'rh_temp_drdy_n', class 'altera_avalon_pio'
 * The macros are prefixed with 'RH_TEMP_DRDY_N_'.
 * The prefix is the slave descriptor.
 */
#define RH_TEMP_DRDY_N_COMPONENT_TYPE altera_avalon_pio
#define RH_TEMP_DRDY_N_COMPONENT_NAME relay_pio
#define RH_TEMP_DRDY_N_BASE 0x0300
#define RH_TEMP_DRDY_N_SPAN 16
#define RH_TEMP_DRDY_N_END 0x030f
#define RH_TEMP_DRDY_N_BIT_CLEARING_EDGE_REGISTER 0
#define RH_TEMP_DRDY_N_BIT_MODIFYING_OUTPUT_REGISTER 0
#define RH_TEMP_DRDY_N_CAPTURE 0
#define RH_TEMP_DRDY_N_DATA_WIDTH 1
#define RH_TEMP_DRDY_N_DO_TEST_BENCH_WIRING 0
#define RH_TEMP_DRDY_N_DRIVEN_SIM_VALUE 0
#define RH_TEMP_DRDY_N_EDGE_TYPE NONE
#define RH_TEMP_DRDY_N_FREQ 50000000
#define RH_TEMP_DRDY_N_HAS_IN 1
#define RH_TEMP_DRDY_N_HAS_OUT 0
#define RH_TEMP_DRDY_N_HAS_TRI 0
#define RH_TEMP_DRDY_N_IRQ_TYPE NONE
#define RH_TEMP_DRDY_N_RESET_VALUE 0

#endif /* _ALTERA_HPS_0_H_ */
