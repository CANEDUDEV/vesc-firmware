/*
        Copyright 2023 Benjamin Vedder	benjamin@vedder.se

        This file is part of the VESC firmware.

        The VESC firmware is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#include "hw.h"

#include "ch.h"
#include "commands.h"
#include "drv8301.h"
#include "hal.h"
#include "mc_interface.h"
#include "stm32f4xx_conf.h"
#include "terminal.h"
#include "utils_math.h"

void hw_init_gpio(void) {
  // GPIO clock enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  // LEDs
  // palSetPadMode(LED_GREEN_GPIO, LED_GREEN_PIN,
  //              PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
  // palSetPadMode(LED_RED_GPIO, LED_RED_PIN,
  //              PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

  // ENABLE_GATE
  palSetPadMode(GPIOB, 5, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

  ENABLE_GATE();

  // Current filter
  //  palSetPadMode(GPIOD, 2, PAL_MODE_OUTPUT_PUSHPULL |
  //  PAL_STM32_OSPEED_HIGHEST); CURRENT_FILTER_OFF();

  // Phase filters
  //  palSetPadMode(PHASE_FILTER_GPIO, PHASE_FILTER_PIN,
  //                PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
  //  PHASE_FILTER_OFF();

  // GPIOA Configuration: Channel 1 to 3 as alternate function push-pull
  palSetPadMode(GPIOA, 8,
                PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUDR_FLOATING);
  palSetPadMode(GPIOA, 9,
                PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUDR_FLOATING);
  palSetPadMode(GPIOA, 10,
                PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUDR_FLOATING);

  palSetPadMode(GPIOB, 13,
                PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUDR_FLOATING);
  palSetPadMode(GPIOB, 14,
                PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUDR_FLOATING);
  palSetPadMode(GPIOB, 15,
                PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUDR_FLOATING);

  // Hall sensors
  palSetPadMode(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3, PAL_MODE_INPUT_PULLUP);

  // Fault pin
  palSetPadMode(GPIOB, 7, PAL_MODE_INPUT_PULLUP);

  // ADC Pins
  palSetPadMode(GPIOA, 0, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOA, 1, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOA, 2, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOA, 3, PAL_MODE_INPUT_ANALOG);
  // palSetPadMode(GPIOA, 5, PAL_MODE_INPUT_ANALOG);
  // palSetPadMode(GPIOA, 6, PAL_MODE_INPUT_ANALOG);

  palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOC, 1, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOC, 2, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOC, 3, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOC, 4, PAL_MODE_INPUT_ANALOG);

  drv8301_init();
}

void hw_setup_adc_channels(void) {
  uint8_t t_samp = ADC_SampleTime_15Cycles;

  // ADC1 regular channels
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, t_samp);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 2, t_samp);
  // ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, t_samp);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, t_samp);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 5, t_samp);

  // ADC2 regular channels
  ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 1, t_samp);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 2, t_samp);
  // ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 3, t_samp);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_15, 4, t_samp);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 5, t_samp);

  // ADC3 regular channels
  ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, t_samp);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_2, 2, t_samp);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 3, t_samp);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 4, t_samp);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_1, 5, t_samp);

  // Injected channels
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_10, 1, t_samp);
  ADC_InjectedChannelConfig(ADC2, ADC_Channel_11, 1, t_samp);
  ADC_InjectedChannelConfig(ADC3, ADC_Channel_12, 1, t_samp);
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_10, 2, t_samp);
  ADC_InjectedChannelConfig(ADC2, ADC_Channel_11, 2, t_samp);
  ADC_InjectedChannelConfig(ADC3, ADC_Channel_12, 2, t_samp);
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_10, 3, t_samp);
  ADC_InjectedChannelConfig(ADC2, ADC_Channel_11, 3, t_samp);
  ADC_InjectedChannelConfig(ADC3, ADC_Channel_12, 3, t_samp);
}

void hw_start_i2c(void) {}

void hw_stop_i2c(void) {}

/**
 * Try to restore the i2c bus
 */
void hw_try_restore_i2c(void) {}