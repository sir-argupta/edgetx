/*
 * ILI9341 320x240 SPI LCD driver for NUCLEO-F412ZG
 */

#include "stm32_gpio.h"
#include "stm32_spi.h"

#include "delays_driver.h"
#include "hal/gpio.h"
#include "lcd.h"

#include "hal.h"
#include "board.h"

#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C

#define LCD_NRST_HIGH()               gpio_set(LCD_NRST)
#define LCD_NRST_LOW()                gpio_clear(LCD_NRST)
#define LCD_COMMAND_MODE()            gpio_clear(LCD_SPI_RS)
#define LCD_DATA_MODE()               gpio_set(LCD_SPI_RS)

static const stm32_spi_t lcdSpi = {
  .SPIx = LCD_SPI,
  .SCK = LCD_SPI_CLK,
  .MISO = LCD_SPI_MISO,
  .MOSI = LCD_SPI_MOSI,
  .CS = LCD_SPI_CS,
  .DMA = LCD_SPI_DMA,
#if defined(STM32H7) || defined(STM32H7RS)
  .txDMA_PeriphRequest = 0,
  .rxDMA_PeriphRequest = 0,
#else
  .DMA_Channel = LCD_SPI_DMA_CHANNEL,
#endif
  .txDMA_Stream = LCD_SPI_TX_DMA_STREAM,
  .rxDMA_Stream = LCD_SPI_RX_DMA_STREAM,
  .DMA_FIFOMode = LL_DMA_FIFOMODE_ENABLE,
  .DMA_FIFOThreshold = LL_DMA_FIFOTHRESHOLD_1_2,
  .DMA_MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD,
  .DMA_MemBurst = LL_DMA_MBURST_INC4,
};

static void* initialFrameBuffer = nullptr;

static void write_start_end(uint8_t cmd, uint16_t start, uint16_t end)
{
  uint8_t buf[4] = {
      (uint8_t)(start >> 8),
      (uint8_t)(start & 0xFF),
      (uint8_t)(end >> 8),
      (uint8_t)(end & 0xFF),
  };

  stm32_spi_select(&lcdSpi);
  LCD_COMMAND_MODE();
  stm32_spi_transfer_byte(&lcdSpi, cmd);
  LCD_DATA_MODE();
  stm32_spi_transfer_bytes(&lcdSpi, (uint8_t *)buf, nullptr, sizeof(buf));
  stm32_spi_unselect(&lcdSpi);
}

static inline void set_column_addr(uint16_t xs, uint16_t xe)
{
  write_start_end(CASET, xs, xe);
}

static inline void set_row_addr(uint16_t ys, uint16_t ye)
{
  write_start_end(RASET, ys, ye);
}

static void memory_write(const uint16_t* data, uint32_t length)
{
  stm32_spi_select(&lcdSpi);
  LCD_COMMAND_MODE();
  stm32_spi_transfer_byte(&lcdSpi, RAMWR);
  LCD_DATA_MODE();
  stm32_spi_set_data_width(&lcdSpi, LL_SPI_DATAWIDTH_16BIT);
  stm32_spi_dma_transmit_words(&lcdSpi, data, length);
  stm32_spi_unselect(&lcdSpi);
  stm32_spi_set_data_width(&lcdSpi, LL_SPI_DATAWIDTH_8BIT);
}

static void startLcdRefresh(lv_disp_drv_t *disp_drv, uint16_t *buffer,
                            const rect_t &copy_area)
{
  (void)disp_drv;
  coord_t x1 = copy_area.x;
  coord_t x2 = x1 + copy_area.w - 1;
  set_column_addr(x1, x2);
  coord_t y1 = copy_area.y;
  coord_t y2 = y1 + copy_area.h - 1;
  set_row_addr(y1, y2);
  memory_write(buffer, copy_area.w * copy_area.h);
}

extern "C" void lcdSetInitalFrameBuffer(void *fbAddress)
{
  initialFrameBuffer = fbAddress;
}

static void lcdSpiConfig(void)
{
  stm32_spi_init(&lcdSpi, LL_SPI_DATAWIDTH_8BIT);
  // ~20MHz is safe over jumpers; adjust if needed
  stm32_spi_set_max_baudrate(&lcdSpi, 20000000);

  gpio_init(LCD_NRST, GPIO_OUT, GPIO_PIN_SPEED_HIGH);
  gpio_init(LCD_SPI_RS, GPIO_OUT, GPIO_PIN_SPEED_HIGH);
}

static void lcdReset()
{
  LCD_NRST_HIGH();
  delay_ms(1);
  LCD_NRST_LOW();
  delay_ms(10);
  LCD_NRST_HIGH();
  delay_ms(120);
}

static void lcdWriteCommand(uint8_t cmd)
{
  LCD_COMMAND_MODE();
  stm32_spi_select(&lcdSpi);
  stm32_spi_transfer_byte(&lcdSpi, cmd);
  stm32_spi_unselect(&lcdSpi);
}

static void lcdWriteData(uint8_t data)
{
  LCD_DATA_MODE();
  stm32_spi_select(&lcdSpi);
  stm32_spi_transfer_byte(&lcdSpi, data);
  stm32_spi_unselect(&lcdSpi);
}

extern "C" void lcdInit()
{
  lcdSpiConfig();
  stm32_spi_unselect(&lcdSpi);
  lcdReset();

  // Minimal ILI9341 init
  lcdWriteCommand(0x3A); // Pixel format
  lcdWriteData(0x55);    // 16bpp

  lcdWriteCommand(0x36); // MADCTL (orientation)
  lcdWriteData(0x48);    // MX=0, MY=1, BGR

  lcdWriteCommand(0x11); // Sleep out
  delay_ms(120);

  // Init LCD RAM with initial buffer
  set_column_addr(0, LCD_W - 1);
  set_row_addr(0, LCD_H - 1);
  memory_write((const uint16_t*)initialFrameBuffer, LCD_W * LCD_H);

  lcdWriteCommand(0x29); // Display ON

  lcdSetFlushCb(startLcdRefresh);
}

