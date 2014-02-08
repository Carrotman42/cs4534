
#include "armcommon.h"
#include "lpc17xx_gpio.h"
#include "kdbg.h"

#define b(x) (1 << x)
void InitDBG() {
	GPIO_SetDir(0, b(0) | b(1), 1);
}

void DBGval(unsigned value) {
	if (value >= 4) {
		FATALSTR("Too many bits required for debug function");
	}		
	GPIO_ClearValue(0, ~value);
	GPIO_SetValue(0, value);
}
