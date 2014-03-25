
#include "armcommon.h"

#if ETHER_EMU==1

xQueueHandle toEmu, fromEmu;


#include "klcd.h"

#define ETHER_TASKS
#include "tasks.h"

TASK_FUNC_NOARG(EtherEmu) {
	//static int cc = 0;
	LCDwriteLn(1, "Started");
//	int justTurnedRight = 0;
	for(;;) {
/*		emuMsgBuf d;
		RECV(fromEmu, d);
		
		aBuf(b, 100);
		int i;
		aByte(b, cc);
		cc++;
		for (i = 0; i < 15; i++) {
			aChar(b, ' ');
			aByte(b, d.data[i]);
		}
		aChar(b, 0);
		aPrint(b,14);
		
		if (d.data[0] == 4 && d.data[1] == 2) {
			if (d.data[3] > 0) {
				if (d.data[5] < 100) {
					SendTurnLeft();
				} else if (justTurnedRight == 0 && d.data[6] > 200 && d.data[7] > 200) {
					//SendTurnRight();
					justTurnedRight = 10;
				}
				//if (justTurnedRight) justTurnedRight--;
			}
			ReadFrames();
		} else {
			LCDwriteLn(15, "Unknown packet");
		}					 */
	}
} ENDTASK



#endif











