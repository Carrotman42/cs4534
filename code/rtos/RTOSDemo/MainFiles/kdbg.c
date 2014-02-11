
#include "armcommon.h"
#include "lpc17xx_gpio.h"
#include "kdbg.h"

#define b(x) (1 << x)
void InitDBG() {
	GPIO_SetDir(0, b(0) | b(1), 1);
}

void DBGbit(unsigned value, int on) {
	if (value >= 2) {
		FATALSTR("Too many bits required for debug function");
	}
	if (on) {
		GPIO_SetValue(0, 1<<value);
	} else {
		GPIO_ClearValue(0, 1<<value);
	}
}
