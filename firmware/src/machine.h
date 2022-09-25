/**
 * @file machine.h
 *
 * @defgroup MACHINE State Machine Module
 *
 * @brief Implements the main state machine of the system.
 *
 */

#ifndef MACHINE_H
#define MACHINE_H

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "conf.h"

// Equations for mode 2 (CTC with TOP OCR2A)
// Note the resolution. For example.. at 150hz, ICR1 = PWM_TOP = 159, so it
#define MACHINE_TIMER_TOP ((F_CPU/(2*MACHINE_TIMER_PRESCALER))/(MACHINE_TIMER_FREQUENCY) -1)

#ifdef ADC_ON
#include "adc.h"
#endif // ADC_ON
#ifdef USART_ON
#include "usart.h"
#endif //USART_ON
#include "dbg_vrb.h"
#ifdef CAN_ON
#include "can.h"
#include "can_app.h"
extern const uint8_t can_filter[];
#endif // CAN_ON

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
    STATE_RESET,
} state_machine_t;

typedef union system_flags{
    struct{
        uint8_t none                      :1;
        uint8_t cap_charging              :1;
        uint8_t charge_failed             :1;
        uint8_t boat_on                   :1;
        uint8_t no_message_from_MSC19_1   :1;
        uint8_t no_message_from_MSC19_2   :1;
        uint8_t no_message_from_MSC19_3   :1;
        uint8_t no_message_from_MSC19_4   :1;
        uint8_t no_message_from_MSC19_5   :1;
        uint8_t no_message_from_MT19      :1;
        uint8_t no_message_from_MCC19_1   :1;
        uint8_t no_message_from_MCC19_2   :1;
        uint8_t no_message_from_MCC19_3   :1;
        uint8_t no_message_from_MCC19_4   :1;
    };
    uint16_t   all;
} system_flags_t;


typedef union error_flags{
    struct{
        uint8_t no_communication_with_mcs :1;
        uint8_t no_communication_with_mam :1;
        uint8_t no_communication_with_mic :1;
        uint8_t no_communication_with_mcc :1;
    };
    uint8_t   all;
}error_flags_t;

typedef struct mcc_measurements
{
    uint16_t voltage[4];
    uint16_t current[4];
    uint16_t power[4];
}mcc_measurements_t;

typedef struct battery_voltage
{
    uint16_t main_bank;
    uint16_t main_cell_1;
    uint16_t main_cell_2;
    uint16_t main_cell_3;
    uint16_t aux;
    uint16_t extra;
}battery_voltage_t;

typedef struct battery_current
{
    uint16_t in;
    uint16_t out;
}battery_current_t;

typedef union{
    struct{
	uint8_t main_bank	:1;
	uint8_t main_cell_1	:1;
	uint8_t main_cell_2	:1;
	uint8_t main_cell_3	:1;
    };
    uint8_t all;
}undervoltage_t;

// debug functions
void print_infos(void);
void print_configurations(void);
void print_system_flags(void);

// machine tasks
void task_initializing(void);
void task_idle(void);
void task_running(void);
void task_error(void);
void task_reset(void);

// the machine itself
void set_machine_initial_state(void);
void machine_init(void);
void machine_run(void);
void set_state_error(void);
void set_state_initializing(void);
void set_state_idle(void);
void set_state_running(void);
void set_state_reset(void);

void read_main_battery_voltage(void);

// machine variables
extern volatile state_machine_t state_machine;
extern volatile system_flags_t system_flags;
extern volatile error_flags_t error_flags;

extern volatile undervoltage_t undervoltage;
extern volatile uint16_t boat_rpm;

extern volatile mcc_measurements_t mcc_measurements;
extern volatile battery_voltage_t battery_voltage;
extern volatile battery_current_t battery_current;

extern volatile uint8_t machine_clk;
extern volatile uint8_t machine_clk_divider;
extern volatile uint8_t total_errors;           // Contagem de ERROS

// other variables
extern volatile uint8_t led_clk_div;

#endif /* ifndef MACHINE_H */
