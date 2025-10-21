/*
 * Minimal board header for NUCLEO-F412ZG dev target
 */

#pragma once

#include "definitions.h"
#include "edgetx_constants.h"

#include "board_common.h"
#include "hal.h"

// Power driver (minimal stubs)
#define SOFT_PWR_CTRL
void pwrInit();
uint32_t pwrCheck();
void pwrOn();
void pwrOff();
bool pwrPressed();
bool pwrOffPressed();
uint32_t pwrPressedDuration();

// External module power control (no-op by default on dev board)
#define EXTERNAL_MODULE_ON()   ((void)0)
#define EXTERNAL_MODULE_OFF()  ((void)0)

// Board driver
void boardInit();
void boardOff();

// Backlight (stubs)
void backlightInit();
void backlightEnable(uint8_t dutyCycle);
void backlightFullOn();
bool isBacklightEnabled();

// LCD stubs/macros
#define lcdRefreshWait(...)

// Haptic (stubs)
void hapticInit();
void hapticDone();
void hapticOff();
void hapticOn(uint32_t pwmPercent = 0);
