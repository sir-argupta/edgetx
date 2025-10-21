option(UNEXPECTED_SHUTDOWN "Enable the Unexpected Shutdown screen" ON)
option(PXX1 "PXX1 protocol support" OFF)
option(PXX2 "PXX2 protocol support" OFF)
option(AFHDS3 "AFHDS3 TX Module" OFF)
option(MULTIMODULE "DIY Multiprotocol TX Module" OFF)
option(GHOST "Ghost TX Module" OFF)
option(MODULE_SIZE_STD "Standard size TX Module" ON)

set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
set(CPU_TYPE STM32F4)
set(HSE_VALUE 8000000)
set(STORAGE_MODELSLIST YES)
set(GUI_DIR 128x64)
set(BITMAPS_DIR 128x64)
set(TARGET_DIR nucleo-f412zg)
set(USB_SERIAL ON CACHE BOOL "Enable USB serial (CDC)")
set(SDCARD YES)

set(HARDWARE_EXTERNAL_MODULE YES)
set(CROSSFIRE ON)

# Keep things simple for a dev board
set(HAPTIC OFF)
set(SOFTWARE_KEYBOARD OFF)

set(FLAVOUR nucleo_f412zg)

# Reuse F407 vector/clock setup (closest supported F4 family)
set(CPU_TYPE_FULL STM32F407xE)
set(TARGET_LINKER_DIR stm32f407)

add_definitions(
  -DSTM32_SUPPORT_32BIT_TIMERS
)

add_definitions(-DPWR_BUTTON_${PWR_BUTTON})

if(NOT UNEXPECTED_SHUTDOWN)
  add_definitions(-DNO_UNEXPECTED_SHUTDOWN)
endif()

# Minimal board sources (headless dev target)
set(TARGET_SRC_DIR targets/${TARGET_DIR})

set(BOARD_COMMON_SRC
  ${TARGET_SRC_DIR}/board.cpp
  ${TARGET_SRC_DIR}/backlight_driver.cpp
  ${TARGET_SRC_DIR}/haptic_driver.cpp
  ${TARGET_SRC_DIR}/usb_descriptor.h
  ${TARGET_SRC_DIR}/lcd_driver_spi.cpp
  targets/common/arm/stm32/abnormal_reboot.cpp
  targets/common/arm/stm32/delays_driver.cpp
  targets/common/arm/stm32/flash_driver.cpp
  targets/common/arm/stm32/watchdog_driver.cpp
)

# Bootloader board library (minimal)
add_library(board_bl OBJECT EXCLUDE_FROM_ALL
  ${BOARD_COMMON_SRC}
  targets/common/arm/stm32/stm32_pulse_driver.cpp
)
add_dependencies(board_bl minimal_board_lib)
set(BOOTLOADER_SRC ${BOOTLOADER_SRC} $<TARGET_OBJECTS:board_bl>)

set(CMSIS_SRC targets/common/arm/stm32/f4/system_clock.c)

# Firmware board library
add_library(board OBJECT EXCLUDE_FROM_ALL
  ${BOARD_COMMON_SRC}
  targets/common/arm/stm32/stm32_pulse_driver.cpp
)
add_dependencies(board board_lib)
set(FIRMWARE_SRC ${FIRMWARE_SRC} $<TARGET_OBJECTS:board>)

include_directories(${RADIO_SRC_DIR} ${TARGET_SRC_DIR})
