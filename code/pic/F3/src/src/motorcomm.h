#include "maindefs.h"

#ifndef MOTORCOMM_H
#define	MOTORCOMM_H

#include "../../../../common/communication/brain_rover.h"
#include "comm.h"
#include "debug.h"

uint8 sendMotorAckResponse(uint8 parameters, uint8 wifly);
void sendEncoderData();

#endif	/* MOTORCOMM_H */
