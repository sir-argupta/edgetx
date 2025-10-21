/*
 * NUCLEO-F412ZG minimal bring-up for EdgeTX headless dev target
 */

#include "stm32_hal.h"
#include "stm32_hal_ll.h"
#include "stm32_gpio.h"

#include "hal/rotary_encoder.h"
#include "hal/switch_driver.h"
#include "hal/adc_driver.h"

#include "timers_driver.h"
#include "delays_driver.h"

#include "board.h"
#include "boards/generic_stm32/module_ports.h"

extern "C" void SystemInit(void);

void boardInit()
{
  delaysInit();
  timersInit();
  __enable_irq();

  // Init module ports (external module over USART3 / TIM2)
  boardInitModulePorts();

  // Optional: init inputs if wired
  keysInit();
}

void boardOff()
{
  // For a dev board just loop here
  while (1) {
  }
}

// Minimal power control stubs
void pwrInit() {}
uint32_t pwrCheck() { return 0; }
void pwrOn() {}
void pwrOff() { boardOff(); }
bool pwrPressed() { return true; }
bool pwrOffPressed() { return false; }
uint32_t pwrPressedDuration() { return 0; }

