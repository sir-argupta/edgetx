/* Minimal backlight stubs for NUCLEO-F412ZG */

#include <stdint.h>
#include "board.h"

static bool _bl_on = false;

void backlightInit() { _bl_on = false; }
void backlightEnable(uint8_t) { _bl_on = true; }
void backlightFullOn() { _bl_on = true; }
bool isBacklightEnabled() { return _bl_on; }

