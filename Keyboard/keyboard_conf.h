/*
 * keyboard_conf.h
 *
 *  Created on: Oct 29, 2023
 *      Author: xq123
 */

#ifndef KEYBOARD_CONF_H_
#define KEYBOARD_CONF_H_

// ADC_SAMPLETIME_1CYCLE_5       (0x00000000U)                              /*!< Sampling time 1.5 ADC clock cycle */
// ADC_SAMPLETIME_2CYCLES_5      ((uint32_t)ADC_SMPR2_SMP10_0)                       /*!< Sampling time 2.5 ADC clock cycles */
// ADC_SAMPLETIME_4CYCLES_5      ((uint32_t)ADC_SMPR2_SMP10_1)                       /*!< Sampling time 4.5 ADC clock cycles */
// ADC_SAMPLETIME_7CYCLES_5      ((uint32_t)(ADC_SMPR2_SMP10_1 | ADC_SMPR2_SMP10_0)) /*!< Sampling time 7.5 ADC clock cycles */
// ADC_SAMPLETIME_19CYCLES_5     ((uint32_t)ADC_SMPR2_SMP10_2)                       /*!< Sampling time 19.5 ADC clock cycles */
// ADC_SAMPLETIME_61CYCLES_5     ((uint32_t)(ADC_SMPR2_SMP10_2 | ADC_SMPR2_SMP10_0)) /*!< Sampling time 61.5 ADC clock cycles */
// ADC_SAMPLETIME_181CYCLES_5    ((uint32_t)(ADC_SMPR2_SMP10_2 | ADC_SMPR2_SMP10_1)) /*!< Sampling time 181.5 ADC clock cycles */
// ADC_SAMPLETIME_601CYCLES_5    ((uint32_t)ADC_SMPR2_SMP10)                         /*!< Sampling time 601.5 ADC clock cycles */
#define ADC_SAMPLETIME_601CYCLES_5 ADC_SAMPLETIME_19CYCLES_5

//#define EXTENDED_SAMPLING

//#define PAUSE_TIM_WHEN_CALCULATING

#define RING_BUFFER_LENGTH 5

#define MAX_SAMPLING_COUNT 15

#define THE_CHOSEN_ONE 8

#define TIM2_COUNTER_PERIOD 700-1

#define ADC12_BEGIN_TIME 10

#define ADC34_BEGIN_TIME 10

#define DEFAULT_ADVANCED_KEY_MODE LEFL_KEY_ANALOG_RAPID_MODE

#define DEFAULT_TRIGGER_DISTANCE 0.05

#define DEFAULT_RELEASE_DISTANCE 0.05

#define DEFAULT_UPPER_DEADZONE 0.02

#define DEFAULT_LOWER_DEADZONE 0.10

#endif /* KEYBOARD_CONF_H_ */
