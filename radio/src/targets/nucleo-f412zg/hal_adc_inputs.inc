// Define ADC input names and defaults

static const etx_hal_adc_input_t _adc_main_names[] = {
  { "LH", "Left H",  "LH" },
  { "LV", "Left V",  "LV" },
  { "RV", "Right V", "RV" },
  { "RH", "Right H", "RH" },
};

static const etx_hal_adc_input_t _adc_flex_names[] = {
  { "P1", "Pot1", "P1" },
  { "P2", "Pot2", "P2" },
};

// Order must match enum in hal/adc_driver.h
const etx_hal_adc_inputs_t _hal_inputs[] = {
  // ADC_INPUT_MAIN
  { (uint8_t)(sizeof(_adc_main_names)/sizeof(_adc_main_names[0])), 0, _adc_main_names },
  // ADC_INPUT_FLEX
  { (uint8_t)(sizeof(_adc_flex_names)/sizeof(_adc_flex_names[0])), 4, _adc_flex_names },
  // ADC_INPUT_VBAT
  { 0, 6, nullptr },
  // ADC_INPUT_RTC_BAT
  { 0, 6, nullptr },
  // ADC_INPUT_ALL (unused)
  { 0, 0, nullptr },
};

// No special default pot configuration
const potconfig_t _pot_default_config = 0;
