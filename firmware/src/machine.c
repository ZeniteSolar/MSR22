#include "machine.h"

volatile undervoltage_t undervoltage;
volatile uint16_t boat_rpm;
volatile mcc_measurements_t mcc_measurements;
volatile battery_voltage_t battery_voltage;
volatile battery_current_t battery_current;
volatile uint8_t machine_clk;
volatile uint8_t machine_clk_divider;
volatile uint8_t total_errors;           // Contagem de ERROS
volatile uint8_t led_clk_div;
volatile uint8_t ui_clk_div;
volatile state_machine_t state_machine;
volatile system_flags_t system_flags;
volatile error_flags_t error_flags;

/**
 * @brief
 */
void machine_init(void)
{
	//clr_bit(PRR0, PRTIM2);                          // Activates clock

    // MODE 2 -> CTC with TOP on OCR1
    TCCR2A  =    (1 << WGM21) | (0 << WGM20)        // mode 2
              | (0 << COM2B1) | (0 << COM2B0)       // do nothing
              | (0 << COM2A1) | (0 << COM2A0);      // do nothing

    TCCR2B =
#if MACHINE_TIMER_PRESCALER ==     1
                (0 << CS22) | (0 << CS21) | (1 << CS20) // Prescaler N=1
#elif MACHINE_TIMER_PRESCALER ==   8
                (0 << CS22) | (1 << CS21) | (0 << CS20) // Prescaler N=8
#elif MACHINE_TIMER_PRESCALER ==   32
                (0 << CS22) | (1 << CS21) | (1 << CS20) // Prescaler N=32
#elif MACHINE_TIMER_PRESCALER ==   64
                (1 << CS22) | (0 << CS21) | (0 << CS20) // Prescaler N=64
#elif MACHINE_TIMER_PRESCALER ==   128
                (1 << CS22) | (0 << CS21) | (1 << CS20) // Prescaler N=128
#elif MACHINE_TIMER_PRESCALER ==   256
                (1 << CS22) | (1 << CS21) | (0 << CS20) // Prescaler N=256
#elif MACHINE_TIMER_PRESCALER ==   1024
                (1 << CS22) | (1 << CS21) | (1 << CS20) // Prescaler N=1024
#else
                0
#endif
                | (0 << WGM22);      // mode 2

    OCR2A = MACHINE_TIMER_TOP;                       // OCR2A = TOP = fcpu/(N*2*f) -1

    TIMSK2 |=   (1 << OCIE2A);                      // Activates interruption

    set_machine_initial_state();
    set_state_initializing();
}

/**
 * @brief set machine initial state
 */
inline void set_machine_initial_state(void)
{
    error_flags.all = 0;
    machine_clk = machine_clk_divider = led_clk_div = 0;
}

/**
 * @brief set error state
 */
inline void set_state_error(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>STATE ERROR\n"));
    state_machine = STATE_ERROR;
}

/**
* @brief set initializing state
*/
inline void set_state_initializing(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>INITIALIZING STATE\n"));
    state_machine = STATE_INITIALIZING;
}

/**
* @brief set idle state
*/
inline void set_state_idle(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>IDLE STATE\n"));
    state_machine = STATE_IDLE;
}

/**
* @brief set running state
*/
inline void set_state_running(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>RUNNING STATE\n"));
    state_machine = STATE_RUNNING;
}

/**
 * @brief set reset state
 */
inline void set_state_reset(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>RESET STATE\n"));
    state_machine = STATE_RESET;
}

/**
 * @breif prints the configurations and definitions
 */
inline void print_configurations(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("CONFIGURATIONS:\n"));

#ifdef ADC_ON
    VERBOSE_MSG_MACHINE(usart_send_string("\nadc_f: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16( ADC_FREQUENCY ));
    VERBOSE_MSG_MACHINE(usart_send_char(','));
    VERBOSE_MSG_MACHINE(usart_send_uint16( ADC_AVG_SIZE_10 ));
#endif // ADC_ON

    VERBOSE_MSG_MACHINE(usart_send_string("\nmachine_f: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16( MACHINE_FREQUENCY ));
    VERBOSE_MSG_MACHINE(usart_send_char('\n'));
}

/**
* @brief prints the system flags
*/
inline void print_system_flags(void)
{
    //VERBOSE_MSG_MACHINE(usart_send_string(" EN "));
    //VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.enable));
}

inline void read_main_battery_voltage(void)
{

#ifdef MAIN_BATTERY_UNDERVOLTAGE_WARNING
	if(battery_voltage.main_bank < BATTERY_BANK_DISCHARGED_VOLTAGE)
		undervoltage.main_bank = 1;
	else
		undervoltage.main_bank = 0;
#endif

#ifdef MAIN_BATTERY_OVERVOLTAGE_WARNING

#endif

#ifdef UI_CHECK_MAIN_BATTERY_CELL_VOLTAGE
	if(battery_voltage.main_cell_1 < BATTERY_CELL_DISCHARGED_VOLTAGE)
		undervoltate.main_cell_1 = 1;
	else
		undervoltage.main_cell_1 = 0;

	if(battery_voltage.main_cell_2 < BATTERY_CELL_DISCHARGED_VOLTAGE)	
		undervoltage.main_cell_2 = 1;
	else
		undervoltate.main_cell_2 = 0;

	if(battery_voltage.main_cell_3 < BATTERY_CELL_DISCHARGED_VOLTAGE)
		undervoltage.main_cell_3 = 1;
	else
		undervoltage.main_cell_3 = 0;
#endif
}

/**
 * @brief Checks if the system is OK to run
 */
inline void task_initializing(void)
{
#ifdef LED_ON
    set_led(LED2);
#endif

    set_machine_initial_state();

#ifdef CAN_ON
  VERBOSE_MSG_INIT(usart_send_string("System initialized without errors.\n"));
#else
#ifdef UI_ON
  display_clear();
  display_send_string("CAN", 7, 2, font_big);
  display_send_string("OFF", 7, 5, font_big);
#endif  /* UI_ON */
  VERBOSE_MSG_ERROR(usart_send_string("CAN module disable.\n"));
#endif 

    set_state_idle();
}

/**
 * @brief waits for commands while checking the system
 */
inline void task_idle(void)
{
#ifdef LED_ON
    if(led_clk_div++ >= 20){
        cpl_led(LED2);
        led_clk_div = 0;
    }
#endif

#if defined CAN_ON && defined UI_ON
    
    #if CAN_SIGNATURE_SELF == CAN_SIGNATURE_MVC19_1
    ui_select_screen(VOLTAGE);
    #elif CAN_SIGNATURE_SELF == CAN_SIGNATURE_MVC19_2
    ui_select_screen(CURRENT);
    #endif

    set_led(LED2);

    ui_draw_layout();
    set_state_running();
#endif
}

/**
 * @brief
 */
inline void task_running(void)
{
#ifdef LED_ON
    if(led_clk_div++ >= 50){
        cpl_led(LED1);
        led_clk_div = 0;
    }
#endif

	if (I2C_ErrorCode){
		set_state_reset();
	}

#ifdef UI_ON
    if(++ui_clk_div == UI_UPDATE_CLK_DIV)
    {
        ui_clk_div = 0;
	    read_main_battery_voltage();
        ui_update();
    }
#endif
}

/**
 * @brief error task checks the system and tries to medicine it.
 */
inline void task_error(void)
{
#ifdef LED_ON
    if(led_clk_div++ >= 5){
        cpl_led(LED2);
        cpl_led(LED1);
        led_clk_div = 0;
    }
#endif

    total_errors++;

    if(total_errors > 20)
        set_state_reset();

    if(total_errors < 2)
        set_state_initializing();

}

/**
 * @brief reset error task just freezes the processor and waits for watchdog
 */
inline void task_reset(void)
{
#ifndef WATCHDOG_ON
    //wdt_init();
#endif

    cli();  // disable interrupts

    VERBOSE_MSG_ERROR(usart_send_string("WAITING FOR A RESET!\n"));
    for(;;)
    {
      cpl_led(LED2);
      cpl_led(LED1);
      _delay_ms(100);
    }
}

void print_infos(void)
{
    static uint8_t i = 0;

    switch(i++){
        case 0:
            //print_system_flags();
            break;
        case 1:
            //print_error_flags();
            break;
        case 2:
            //print_control_others();
        default:
            //VERBOSE_MSG_MACHINE(usart_send_char('\n'));
            i = 0;
            break;
    }
}

/**
 * @brief this is the machine state itself.
 */
inline void machine_run(void)
{
    //print_infos();

    if(machine_clk){
        machine_clk = 0;
            switch(state_machine){
                case STATE_INITIALIZING:
                    task_initializing();
                    break;

                case STATE_IDLE:
                    task_idle();
                    #ifdef CAN_ON
                    can_app_task();
                    #endif /* CAN_ON */
                    break;

                case STATE_RUNNING:
                    task_running();
                    #ifdef CAN_ON
                    can_app_task();
                    #endif /* CAN_ON */
                    break;

                case STATE_ERROR:
                    task_error();
                    break;

                case STATE_RESET:
                default:
                    task_reset();
                    break;
            }
        }
}

/**
* @brief ISR para ações de controle
*/
ISR(TIMER2_COMPA_vect)
{
    if(machine_clk_divider++ == MACHINE_CLK_DIVIDER_VALUE){
        machine_clk = 1;
        machine_clk_divider = 0;
    }
}
