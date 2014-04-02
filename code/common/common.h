
#ifndef COMM_COMMON_H_INC
#define COMM_COMMON_H_INC

#define sensorADid 0
#define encoderID 0x05
#define sensorFrameID 0x01
/*
// Private define
#define __sM(id) (1 << id)

#define sensorADmask __sM(sensorAD)
*/
// Remove privates
#undef __sM
#endif