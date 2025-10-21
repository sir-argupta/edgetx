// Define ADC input names and defaults

static const etx_hal_adc_input_t _adc_main_names[] = {
  { "LH", "Left H",  "LH" },
  { "LV", "Left V",  "LV" },
  { "RV", "Right V", "RV" },
  { "RH", "Right H", "RH" },
};

// Order must match enum in hal/adc_driver.h
const etx_hal_adc_inputs_t _hal_inputs[] = {
  // ADC_INPUT_MAIN
  { (uint8_t)(sizeof(_adc_main_names)/sizeof(_adc_main_names[0])), 0, _adc_main_names },
  // ADC_INPUT_FLEX
  { 0, 4, nullptr },
  // ADC_INPUT_VBAT
  { 0, 4, nullptr },
  // ADC_INPUT_RTC_BAT
  { 0, 4, nullptr },
  // ADC_INPUT_ALL (unused)
  { 0, 0, nullptr },
};

// No special default pot configuration
const potconfig_t _pot_default_config = 0;

