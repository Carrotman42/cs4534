#include "maindefs.h"

#ifndef MOTORCOMM_H
#define	MOTORCOMM_H

#include "../../../../common/communication/brain_rover.h"
#include "comm.h"
#include "debug.h"
#include "../../../../common/communication/frames.h"

uint8 sendMotorAckResponse(uint8 parameters, uint8 messageid, uint8 wifly);
#ifdef MOTOR_PIC
void sendEncoderData();
#endif

#endif	/* MOTORCOMM_H */
