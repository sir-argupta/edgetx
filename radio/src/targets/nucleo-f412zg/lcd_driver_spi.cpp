/*
 * Monochrome 128x64 SPI LCD driver (adapted from Taranis)
 */

#include "hal/gpio.h"
#include "stm32_gpio.h"
#include "stm32_spi.h"
#include "stm32_dma.h"

#include "board.h"
#include "debug.h"
#include "lcd.h"

#include "hal/abnormal_reboot.h"
#include "timers_driver.h"

#if !defined(BOOT)
  #include "edgetx.h"
#endif

#define LCD_CONTRAST_OFFSET 160
#define RESET_WAIT_DELAY_MS 300
#define WAIT_FOR_DMA_END()  do { } while (lcd_busy)

#define LCD_NCS_HIGH()  gpio_set(LCD_NCS_GPIO)
#define LCD_NCS_LOW()   gpio_clear(LCD_NCS_GPIO)

#define LCD_A0_HIGH()   gpio_set(LCD_A0_GPIO)
#define LCD_A0_LOW()    gpio_clear(LCD_A0_GPIO)

#define LCD_RST_HIGH()  gpio_set(LCD_RST_GPIO)
#define LCD_RST_LOW()   gpio_clear(LCD_RST_GPIO)

bool lcdInitFinished = false;
void lcdInitFinish();

static inline SPI_TypeDef* LCD_SPIx() { return LCD_SPI; }

void lcdWriteCommand(uint8_t byte)
{
  LCD_A0_LOW();
  LCD_NCS_LOW();
  while ((LCD_SPIx()->SR & SPI_SR_TXE) == 0) {}
  (void)LCD_SPIx()->DR; // Clear receive
  LCD_SPIx()->DR = byte;
  while ((LCD_SPIx()->SR & SPI_SR_RXNE) == 0) {}
  LCD_NCS_HIGH();
}

void lcdHardwareInit()
{
  stm32_spi_enable_clock(LCD_SPI);
  gpio_init_af(LCD_MOSI_GPIO, LCD_GPIO_AF, GPIO_PIN_SPEED_HIGH);
  gpio_init_af(LCD_CLK_GPIO, LCD_GPIO_AF, GPIO_PIN_SPEED_HIGH);

  LCD_SPIx()->CR1 = 0;
  LCD_SPIx()->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPOL | SPI_CR1_CPHA | LCD_SPI_PRESCALER;
  LCD_SPIx()->CR2 = 0;
  LCD_SPIx()->CR1 |= SPI_CR1_MSTR;
  LCD_SPIx()->CR1 |= SPI_CR1_SPE;

  gpio_init(LCD_NCS_GPIO, GPIO_OUT, GPIO_PIN_SPEED_MEDIUM);
  LCD_NCS_HIGH();

  gpio_init(LCD_RST_GPIO, GPIO_OUT, GPIO_PIN_SPEED_MEDIUM);
  gpio_init(LCD_A0_GPIO, GPIO_OUT, GPIO_PIN_SPEED_HIGH);

  stm32_dma_enable_clock(LCD_DMA);
  LCD_DMA_Stream->CR &= ~DMA_SxCR_EN;
  LCD_DMA->HIFCR = LCD_DMA_FLAGS;
  LCD_DMA_Stream->CR =  DMA_SxCR_PL_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0;
  LCD_DMA_Stream->PAR = (uint32_t)&LCD_SPIx()->DR;
#if LCD_W == 128
  LCD_DMA_Stream->NDTR = LCD_W;
#else
  LCD_DMA_Stream->M0AR = (uint32_t)displayBuf;
  LCD_DMA_Stream->NDTR = LCD_W*LCD_H/8*4;
#endif
  LCD_DMA_Stream->FCR = 0x05;

  NVIC_SetPriority(LCD_DMA_Stream_IRQn, 7);
  NVIC_EnableIRQ(LCD_DMA_Stream_IRQn);
}

void lcdStart()
{
  // ST7565 init variant
  lcdWriteCommand(0xe2); // Soft reset
  lcdWriteCommand(0xa1); // Set seg
  lcdWriteCommand(0xc0); // Set com
  lcdWriteCommand(0xf8); // Set booster
  lcdWriteCommand(0x00); // 5x
  lcdWriteCommand(0xa3); // Set bias=1/6
  lcdWriteCommand(0x22); // Set rb/ra
  lcdWriteCommand(0x2f); // All power circuits on
  lcdWriteCommand(0x81); // Set contrast
#if defined(BOOT)
  lcdWriteCommand(LCD_CONTRAST_DEFAULT);
#else
  lcdWriteCommand(g_eeGeneral.contrast);
#endif
  lcdWriteCommand(0xa6); // Normal display
}

volatile bool lcd_busy;

void lcdRefreshWait()
{
  WAIT_FOR_DMA_END();
}

void lcdRefresh(bool wait)
{
  (void)wait;
  if (!lcdInitFinished) {
    lcdInitFinish();
  }

  uint8_t * p = displayBuf;
  for (uint8_t y=0; y < 8; y++, p+=LCD_W) {
    lcdWriteCommand(0x10);
    lcdWriteCommand(0xB0 | y);

    LCD_NCS_LOW();
    LCD_A0_HIGH();

    lcd_busy = true;
    LCD_DMA_Stream->CR &= ~DMA_SxCR_EN;
    LCD_DMA->HIFCR = LCD_DMA_FLAGS;
    LCD_DMA_Stream->M0AR = (uint32_t)p;
    LCD_DMA_Stream->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE;
    LCD_SPIx()->CR2 |= SPI_CR2_TXDMAEN;

    WAIT_FOR_DMA_END();

    LCD_NCS_HIGH();
    LCD_A0_HIGH();
  }
}

extern "C" void LCD_DMA_Stream_IRQHandler()
{
  DEBUG_INTERRUPT(INT_LCD);
  LCD_DMA_Stream->CR &= ~DMA_SxCR_TCIE;
  LCD_DMA->HIFCR |= LCD_DMA_FLAG_INT;
  LCD_SPIx()->CR2 &= ~SPI_CR2_TXDMAEN;
  LCD_DMA_Stream->CR &= ~DMA_SxCR_EN;
  while (LCD_SPIx()->SR & SPI_SR_BSY) {}
  LCD_NCS_HIGH();
  lcd_busy = false;
}

void lcdOff()
{
  WAIT_FOR_DMA_END();
  lcdWriteCommand(0xAE); // sleep
  delay_ms(3);
}

void lcdReset()
{
  LCD_NCS_HIGH();
  LCD_RST_LOW();
  delay_ms(1);
  LCD_RST_HIGH();
}

void lcdInit()
{
  lcdHardwareInit();
  if (IS_LCD_RESET_NEEDED()) {
    lcdReset();
  }
}

static inline bool LCD_DELAY_NEEDED() { return !WAS_RESET_BY_WATCHDOG_OR_SOFTWARE(); }

void lcdInitFinish()
{
  lcdInitFinished = true;
  if (LCD_DELAY_NEEDED()) {
    uint32_t end = timersGetMsTick() + RESET_WAIT_DELAY_MS;
    while (timersGetMsTick() < end);
  }
  lcdStart();
  lcdWriteCommand(0xAF);
  delay_ms(20);
}

void lcdSetRefVolt(uint8_t val)
{
  if (!lcdInitFinished) lcdInitFinish();
  WAIT_FOR_DMA_END();
  lcdWriteCommand(0x81);
  lcdWriteCommand(val + LCD_CONTRAST_OFFSET);
}

void lcdSetInvert(bool invert)
{
  lcdWriteCommand(invert ? 0xA7 : 0xA6);
}

