/**
 * @file conf.h
 *
 * @defgroup CONF Configurations
 *
 * @brief General configuration of the system.
 *
 */

#ifndef CONF_H
//#define CONF_H

#ifndef F_CPU
#define F_CPU 16000000UL    //define a frequencia do microcontrolador - 16MHz
#endif /* ifndef F_CPU */


// CONFIGURACOES DE COMPILACAO
#define DEBUG_ON
// #define VERBOSE_ON
//#define VERBOSE_ON_CAN_APP
#define VERBOSE_ON_MACHINE
//#define VERBOSE_ON_ADC
#define VERBOSE_ON_INIT
#define VERBOSE_ON_ERROR
#define VERBOSE_ON_DISPLAY


// MODULES ACTIVATION
#define USART_ON
#define CAN_ON
// #define CAN_DEPENDENT
// #define ADC_ON
#define MACHINE_ON
#define LED_ON
#define WATCHDOG_ON
// #define SLEEP_ON
#define UI_ON

#define CAN_SIGNATURE_SELF                  CAN_SIGNATURE_MVC19_1

#ifdef UI_ON
// UI CONFIGURATION
#define UI_UPDATE_CLK_DIV                   50
// battery voltage check
//#define MAIN_BATTERY_UNDERVOLTAGE_WARNING
// #define MAIN_BATTERY_OVERVOLTAGE_WARNING
// #define UI_CHECK_AUXILIARY_BATTERY_VOLTAGE
#endif

#ifdef ADC_ON
// ADC CONFIGURATION
// note that changing ADC_FREQUENCY may cause problems with avg_sum_samples
#define ADC_FREQUENCY                       10000 // 20000
#define ADC_TIMER_PRESCALER                 8
#define ADC0_AVG                            adc.channel[ADC0].avg
#define ADC0_ANGULAR_COEF                   10000 //(40000/((4/5)*1024))
#define ADC0_LINEAR_COEF                    0
#define ADC_AVG_SIZE_2                      7                  // in base 2
#define ADC_AVG_SIZE_10                     128                // in base 10
//#define FAKE_ADC_ON
#ifdef FAKE_ADC_ON
#define FAKE_ADC                            1
#endif // FAKE_ADC_ON

#endif //ADC_ON

#define VSCALE				                100
#define VSCALE_FLOAT			            100.f
#define I_SCALE_FLOAT                       100.f


// BATTERY INFORMATION
#define BATTERY_CELL_DISCHARGED_VOLTAGE	    7.0 * VSCALE
#define BATTERY_CELL_OVERCHARGED_VOLTAGE    15.6 * VSCALE
#define BATTERY_SERIES_CELLS		        3
#define BATTERY_BANK_DISCHARGED_VOLTAGE	    BATTERY_CELL_DISCHARGED_VOLTAGE * BATTERY_SERIES_CELLS
#define BATTERY_BANK_OVERCHARGED_VOLTAGE    BATTERY_CELL_OVERCHARGED_VOLTAGE * BATTERY_SERIES_CELLS

#ifdef MACHINE_ON
// The machine frequency may not be superior of ADC_FREQUENCY/ADC_AVG_SIZE_10
#define MACHINE_TIMER_FREQUENCY             120           //<! machine timer frequency in Hz
#define MACHINE_TIMER_PRESCALER             1024          //<! machine timer prescaler
#ifdef ADC_ON
#define MACHINE_CLK_DIVIDER_VALUE           ((uint64_t)(uint32_t)MACHINE_TIMER_FREQUENCY*(uint32_t)ADC_AVG_SIZE_10)/(ADC_FREQUENCY)           //<! machine_run clock divider
#else
#define MACHINE_CLK_DIVIDER_VALUE           1
#endif // ADC_ON
#define MACHINE_FREQUENCY                   (MACHINE_TIMER_FREQUENCY)/(MACHINE_CLK_DIVIDER_VALUE)

// SCALE TO CONVERT ADC DEFINITIONS
//#define VSCALE                              (uint16_t)1000

#endif // MACHINE_ON

#ifdef LED_ON
#define     LED_PORT                PORTD
#define     LED_PIN                 PIND
#define     LED_DDR                 DDRD
#define     LED1                    PD5
#define     LED2                    PD6
#define     cpl_led(y)              cpl_bit(LED_PORT, y)
#define     set_led(y)              set_bit(LED_PORT, y)
#define     clr_led(y)              clr_bit(LED_PORT, y)
#else
#define     cpl_led(y)
#define     set_led(y)
#define     clr_led(y)
#endif // LED_ON


#ifdef CAN_ON
#define SPI_ON
#define CAN_APP_SEND_STATE_FREQ     40//36000     //<! state msg frequency in Hz
#define CAN_APP_SEND_ADC_FREQ       4//6000      //<! adc msg frequency in Hz


// CANBUS DEFINITONS
// ----------------------------------------------------------------------------
/* Global settings for building the can-lib and application program.
 *
 * The following two #defines must be set identically for the can-lib and
 * your application program. They control the underlying CAN struct. If the
 * settings disagree, the underlying CAN struct will be broken, with
 * unpredictable results.
 * If can.h detects that any of the #defines is not defined, it will set them
 * to the default values shown here, so it is in your own interest to have a
 * consistent setting. Ommiting the #defines in both can-lib and application
 * program will apply the defaults in a consistent way too.
 *
 * Select if you want to use 29 bit identifiers.
 */
#define	SUPPORT_EXTENDED_CANID	0

/* Select if you want to use timestamps.
 * Timestamps are sourced from a register internal to the AT90CAN.
 * Selecting them on any other controller will have no effect, they will
 * be 0 all the time.
 */
#define	SUPPORT_TIMESTAMPS		0
#endif // CAN_ON

#endif /* ifndef CONF_H */
