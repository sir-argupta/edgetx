// NUCLEO-F412ZG: 4 analog inputs on ADC1: PA0..PA3

static const uint8_t _adc1_channels[] = {
  (uint8_t)LL_ADC_CHANNEL_0,
  (uint8_t)LL_ADC_CHANNEL_1,
  (uint8_t)LL_ADC_CHANNEL_2,
  (uint8_t)LL_ADC_CHANNEL_3,
};

const stm32_adc_t _ADC_adc[] = {
  {
    .ADCx = ADC1,
    .DMAx = DMA2,
    .DMA_Channel = LL_DMA_CHANNEL_0,
    .DMA_Stream = LL_DMA_STREAM_0,
    .DMA_Stream_IRQn = DMA2_Stream0_IRQn,
    .channels = _adc1_channels,
    .offset = 0,
    .n_channels = (uint8_t)(sizeof(_adc1_channels) / sizeof(_adc1_channels[0])),
    .sample_time = LL_ADC_SAMPLINGTIME_84CYCLES,
  },
};

// GPIO ports and pins used by ADC (for setting analog mode)
static const uint32_t _adc_gpioa_pins[] = { 0, 1, 2, 3 };
const stm32_adc_gpio_t _ADC_GPIOs[] = {
  { GPIOA, _adc_gpioa_pins, (uint8_t)(sizeof(_adc_gpioa_pins) / sizeof(_adc_gpioa_pins[0])) },
};

// Per-input mapping
const stm32_adc_input_t _ADC_inputs[] = {
  { GPIOA, 0, (uint32_t)LL_ADC_CHANNEL_0, 0 },
  { GPIOA, 1, (uint32_t)LL_ADC_CHANNEL_1, 0 },
  { GPIOA, 2, (uint32_t)LL_ADC_CHANNEL_2, 0 },
  { GPIOA, 3, (uint32_t)LL_ADC_CHANNEL_3, 0 },
};

// No SPI-based ADCs
const stm32_spi_adc_t _ADC_spi[] = {};

extern "C" void DMA2_Stream0_IRQHandler()
{
  stm32_hal_adc_dma_isr(&_ADC_adc[0]);
}

