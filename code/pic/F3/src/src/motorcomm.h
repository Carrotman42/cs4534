#include "maindefs.h"

#ifndef MOTORCOMM_H
#define	MOTORCOMM_H

#include "brain_rover.h"
#include "comm.h"
#include "debug.h"
#include "frames.h"

#ifndef SENSOR_PIC
uint8 sendMotorAckResponse(uint8 parameters, uint8 messageid, uint8 wifly);
#endif
#ifdef MOTOR_PIC
void sendEncoderData(uint8 msgid);
#endif

#if defined(MASTER_PIC) || defined(ARM_EMU) || defined(PICMAN)
void turnStarted();
void turnCompleted();
uint8 isTurnComplete();
#endif

#endif	/* MOTORCOMM_H */
