/**
 * @file can_app.h
 *
 * @defgroup CANAPP    Application Module for canbus
 *
 * @brief The application layer module for canbus
 *
 */

#ifndef CAN_APP_H
#define CAN_APP_H

#include "conf.h"
#include "dbg_vrb.h"
#include "../lib/bit_utils.h"
#include "can.h"
#include "can_ids.h"
#include "machine.h"
#include "usart.h"

void can_app_print_msg(can_t *msg);
void can_app_task(void);
void can_app_send_state(void);

void can_app_msg_extractors_switch(can_t *msg);

/* MSC19 */
void can_app_extractor_msc19_1_state(can_t *msg);
void can_app_extractor_msc19_1_adc(can_t *msg);
void can_app_extractor_msc19_2_state(can_t *msg);
void can_app_extractor_msc19_2_adc(can_t *msg);
void can_app_extractor_msc19_3_state(can_t *msg);
void can_app_extractor_msc19_3_adc(can_t *msg);
void can_app_extractor_msc19_4_state(can_t *msg);
void can_app_extractor_msc19_4_adc(can_t *msg);
void can_app_extractor_msc19_5_state(can_t *msg);
void can_app_extractor_msc19_5_adc(can_t *msg);

/* MCS19 */
void can_app_extractor_mcs_state(can_t *msg);
void can_app_extractor_mcs_relay(can_t *msg);

/* MT19 */
void can_app_extractor_mt19_state(can_t *msg);
void can_app_extractor_mt19_rpm(can_t *msg);

/* MCC19 */
void can_app_extractor_mcc19_1_output_voltage(can_t *msg);
void can_app_extractor_mcc19_2_output_voltage(can_t *msg);
void can_app_extractor_mcc19_3_output_voltage(can_t *msg);
void can_app_extractor_mcc19_4_output_voltage(can_t *msg);

void check_can(void);

#ifdef CAN_ON
#define CAN_APP_SEND_STATE_CLK_DIV  CAN_APP_SEND_STATE_FREQ
#define CAN_APP_SEND_ADC_CLK_DIV    CAN_APP_SEND_ADC_FREQ
#else
#define CAN_APP_SEND_STATE_CLK_DIV  1
#define CAN_APP_SEND_ADC_CLK_DIV    1
#endif

#define CAN_APP_CHECKS_WITHOUT_MAM_MSG    500
#define CAN_APP_CHECKS_WITHOUT_MSC19_MSG  500
#define CAN_APP_CHECKS_WITHOUT_MIC19_MSG  500
#define CAN_APP_CHECKS_WITHOUT_MCS19_MSG  550
#define CAN_APP_CHECKS_WITHOUT_MT19_MSG   500
#define CAN_APP_CHECKS_WITHOUT_MCC19_MSG   500

// can_app_checks
extern uint16_t can_app_checks_without_mam_msg;
extern uint8_t can_app_checks_without_mic17_msg;
extern uint8_t can_app_checks_without_mcs19_msg;
extern uint8_t can_app_checks_without_msc19_1_msg;
extern uint8_t can_app_checks_without_msc19_2_msg;
extern uint8_t can_app_checks_without_msc19_3_msg;
extern uint8_t can_app_checks_without_msc19_4_msg;
extern uint8_t can_app_checks_without_msc19_5_msg;
extern uint8_t can_app_checks_without_mt19;

extern uint8_t can_app_send_state_clk_div;

#endif /* ifndef CAN_APP_H */
