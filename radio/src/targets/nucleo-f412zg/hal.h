/*
 * Minimal HAL definitions for NUCLEO-F412ZG dev target
 */

#pragma once

// F412 runs at up to 100MHz (default Nucleo clocks)
#define CPU_FREQ            100000000
#define PERI1_FREQUENCY     50000000
#define PERI2_FREQUENCY     100000000
#define TIMER_MULT_APB1     1
#define TIMER_MULT_APB2     1

#define TELEMETRY_EXTI_PRIO 0

// External module (ExpressLRS over CRSF) via USART3 on PB10/PB11
#define HARDWARE_EXTERNAL_MODULE

// External module power control (tie module power externally for dev)
#define EXTMODULE_PWR_GPIO   GPIO_PIN(GPIOB, 0)

// Timer-based soft-serial (PPM/INV) on PB10 using TIM2_CH3 (AF1)
#define EXTMODULE_TX_GPIO               GPIO_PIN(GPIOB, 10) // PB10
#define EXTMODULE_RX_GPIO               GPIO_PIN(GPIOB, 11) // PB11
#define EXTMODULE_TIMER_TX_GPIO_AF      GPIO_AF1            // TIM2_CH3
#define EXTMODULE_TIMER                 TIM2
#define EXTMODULE_TIMER_Channel         LL_TIM_CHANNEL_CH3
#define EXTMODULE_TIMER_FREQ            (PERI1_FREQUENCY * TIMER_MULT_APB1)
#define EXTMODULE_TIMER_IRQn            TIM2_IRQn
#define EXTMODULE_TIMER_IRQHandler      TIM2_IRQHandler

// DMA for timer (matches common F4 mapping used on Horus)
#define EXTMODULE_TIMER_DMA_CHANNEL        LL_DMA_CHANNEL_3
#define EXTMODULE_TIMER_DMA                DMA1
#define EXTMODULE_TIMER_DMA_STREAM         LL_DMA_STREAM_1
#define EXTMODULE_TIMER_DMA_STREAM_IRQn    DMA1_Stream1_IRQn
#define EXTMODULE_TIMER_DMA_IRQHandler     DMA1_Stream1_IRQHandler

// Full-duplex UART for CRSF over USART3 with DMA (common F4 mapping)
#define EXTMODULE_USART                  USART3
#define EXTMODULE_USART_IRQn             USART3_IRQn
#define EXTMODULE_USART_IRQHandler       USART3_IRQHandler
#define EXTMODULE_USART_TX_DMA           DMA1
#define EXTMODULE_USART_TX_DMA_CHANNEL   LL_DMA_CHANNEL_4
#define EXTMODULE_USART_TX_DMA_STREAM    LL_DMA_STREAM_3
#define EXTMODULE_USART_RX_DMA_CHANNEL   LL_DMA_CHANNEL_4
#define EXTMODULE_USART_RX_DMA_STREAM    LL_DMA_STREAM_1

// I2C Bus 1 on PB8/PB9 (DS3231 RTC and others)
#define I2C_B1                           I2C1
#define I2C_B1_SCL_GPIO                  GPIO_PIN(GPIOB, 8)
#define I2C_B1_SDA_GPIO                  GPIO_PIN(GPIOB, 9)
#define I2C_B1_GPIO_AF                   LL_GPIO_AF_4
#define I2C_B1_CLK_RATE                  400000

// SPI LCD (ILI9341) over SPI1
#define LCD_SPI                           SPI1
#define LCD_SPI_CLK                       GPIO_PIN(GPIOA, 5)
#define LCD_SPI_MISO                      GPIO_PIN(GPIOA, 6)
#define LCD_SPI_MOSI                      GPIO_PIN(GPIOA, 7)
#define LCD_SPI_CS                        GPIO_PIN(GPIOC, 1)
#define LCD_SPI_RS                        GPIO_PIN(GPIOC, 0)
#define LCD_NRST                          GPIO_PIN(GPIOC, 2)
#define LCD_SPI_DMA                       DMA2
#define LCD_SPI_DMA_CHANNEL               LL_DMA_CHANNEL_3
#define LCD_SPI_TX_DMA_STREAM             LL_DMA_STREAM_3
#define LCD_SPI_RX_DMA_STREAM             LL_DMA_STREAM_0

// USB FS (CDC)
#define USB_GPIO_AF                   GPIO_AF10
#define USB_GPIO_DM                   GPIO_PIN(GPIOA, 11)
#define USB_GPIO_DP                   GPIO_PIN(GPIOA, 12)
// Optional VBUS detect: choose a free pin if wired
// #define USB_GPIO_VBUS              GPIO_PIN(GPIOA, 9)

// microSD over SPI2 (SPI mode)
#define STORAGE_USE_SDCARD_SPI
#define SD_SPI                         SPI2
#define SD_GPIO_PIN_SCK                GPIO_PIN(GPIOB, 13)
#define SD_GPIO_PIN_MISO               GPIO_PIN(GPIOB, 14)
#define SD_GPIO_PIN_MOSI               GPIO_PIN(GPIOB, 15)
#define SD_GPIO_PIN_CS                 GPIO_PIN(GPIOB, 12)
#define SD_SPI_DMA                     DMA1
#define SD_SPI_DMA_CHANNEL             LL_DMA_CHANNEL_0
#define SD_SPI_DMA_TX_STREAM           LL_DMA_STREAM_4
#define SD_SPI_DMA_RX_STREAM           LL_DMA_STREAM_3
