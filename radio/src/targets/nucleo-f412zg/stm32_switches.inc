// NUCLEO-F412ZG: 4 physical switches: SA(3-pos), SB(3-pos), SC(2-pos), SD(2-pos)

#include "stm32_hal_ll.h"
#include "stm32_gpio_driver.h"

static inline void _cfg_input_pu(GPIO_TypeDef* port, uint32_t pinmask)
{
  stm32_gpio_enable_clock(port);
  LL_GPIO_SetPinMode(port, pinmask, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(port, pinmask, LL_GPIO_PULL_UP);
}

// Define switch descriptors
static const stm32_switch_t _switch_defs[] = {
  // name,   hi port, hi pin,      lo port, lo pin,      type,           inverted, defaultType
  { "SA",    GPIOB,   LL_GPIO_PIN_3, GPIOB,  LL_GPIO_PIN_4, SWITCH_HW_3POS,  false,    SWITCH_3POS },
  { "SB",    GPIOB,   LL_GPIO_PIN_5, GPIOB,  LL_GPIO_PIN_6, SWITCH_HW_3POS,  false,    SWITCH_3POS },
  { "SC",    GPIOA,   LL_GPIO_PIN_8, nullptr,            0, SWITCH_HW_2POS,  false,    SWITCH_2POS },
  { "SD",    GPIOA,   LL_GPIO_PIN_15, nullptr,           0, SWITCH_HW_2POS,  false,    SWITCH_2POS },
};

static const uint8_t n_switches = sizeof(_switch_defs) / sizeof(_switch_defs[0]);

static void _init_switches()
{
  // Configure GPIOs as input with pull-ups
  _cfg_input_pu(GPIOB, LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6);
  _cfg_input_pu(GPIOA, LL_GPIO_PIN_8 | LL_GPIO_PIN_15);
}

