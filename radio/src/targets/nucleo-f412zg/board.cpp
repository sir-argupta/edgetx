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
#include "hal/i2c_driver.h"
#include "rtc.h"

extern "C" void SystemInit(void);

void boardInit()
{
  delaysInit();
  timersInit();
  __enable_irq();

  // Try sync time from DS3231 on I2C1 @ 0x68 (optional)
  do {
    const uint8_t DS3231_ADDR = 0x68 << 1; // 7-bit addr shifted for STM32 HAL
    if (i2c_init(I2C_Bus_1) >= 0) {
      uint8_t regs[7] = {0};
      // Read from register 0x00 (seconds) 7 bytes
      if (i2c_read(I2C_Bus_1, DS3231_ADDR, 0x00, 1, regs, 7) >= 0) {
        auto bcd2bin = [](uint8_t v) { return (uint8_t)(((v >> 4) * 10) + (v & 0x0F)); };
        struct gtm t = {};
        t.tm_sec  = bcd2bin(regs[0] & 0x7F);
        t.tm_min  = bcd2bin(regs[1] & 0x7F);
        uint8_t hr = regs[2];
        if (hr & 0x40) { // 12-hour
          t.tm_hour = bcd2bin(hr & 0x1F);
          if (hr & 0x20) t.tm_hour = (t.tm_hour % 12) + 12;
        } else {
          t.tm_hour = bcd2bin(hr & 0x3F);
        }
        t.tm_mday = bcd2bin(regs[4] & 0x3F);
        t.tm_mon  = bcd2bin(regs[5] & 0x1F) - 1;
        t.tm_year = bcd2bin(regs[6]) + 100; // years since 1900
        rtcSetTime(&t);
      }
      i2c_deinit(I2C_Bus_1);
    }
  } while (0);

  // Init module ports (external module over USART3 / TIM2)
  boardInitModulePorts();

  // Init switches (SA/SB/SC/SD)
  switchInit();

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
