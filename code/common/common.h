
#ifndef COMM_COMMON_H_INC
#define COMM_COMMON_H_INC

#define sensorADid 0
// TODO: Move this after syncking with Dave so that I don't break his build.
#define PIC
/*
// Private define
#define __sM(id) (1 << id)

#define sensorADmask __sM(sensorAD)
*/
// Remove privates
#undef __sM
#endif