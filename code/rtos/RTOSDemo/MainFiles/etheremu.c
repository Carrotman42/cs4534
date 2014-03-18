
#include "armcommon.h"

#if ETHER_EMU==1

xQueueHandle toEmu, fromEmu;

TASK_PROTOTYPE_NOARG(EtherEmu, 2000, tskIDLE_PRIORITY);
void initEtherEmu() {
	MAKE_Q(toEmu, emuMsgBuf, 4);
	MAKE_Q(fromEmu, emuMsgBuf, 4);
	
	StartEtherEmu();
	void StartFrames();
	void ReadFrames();
	StartFrames();
	ReadFrames();
}

void LCDwriteLn(int,char*);

void SendTurnLeft() {
	emuMsgBuf d;
	char* buf = &d.data[0];
	buf[0] = 2;
	buf[1] = 4;
	buf[2] = 0;
	buf[3] = 1;
	buf[4] = 2+4+1+90;
	buf[5] = 90;
	
	SEND(toEmu, d);
}	 
void ReadFrames() {
	emuMsgBuf d;
	char* buf = &d.data[0];
	buf[0] = 4;
	buf[1] = 2;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 4+2;
	
	SEND(toEmu, d);
}
void StartFrames() {
	emuMsgBuf d;
	char* buf = &d.data[0];
	buf[0] = 4;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 4;
	
	SEND(toEmu, d);
}

void ReportMsg(int len, char*in) {
	emuMsgBuf d;
	char *cur = &d.data[0];
	while (len-- > 0) {
		*cur++ = *in++;
	}
	*cur++ = 0x55;
	*cur++ = 0x56;
	*cur++ = 0x57;
	*cur++ = 0x58;
	SEND(fromEmu, d);
}


TASK_FUNC_NOARG(EtherEmu) {
	static int cc = 0;
	LCDwriteLn(1, "Started");
	for(;;) {
		emuMsgBuf d;
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
				}
			}
			ReadFrames();
		} else {
			LCDwriteLn(15, "Unknown packet");
		}
	}
} ENDTASK



#endif











