#ifndef CONDUCTOR_H
#define CONDUCTOR_H

// This spawns off the tasks concerned with data coming into the given i2c port and distributing it
//   to the correct place depending on which sensor is being read.
void StartProcessingTasks(vtI2CStruct *i2c);

// Sends a msg to the rover.
//   Note that this may block while the communicator
//   task is busy. If it is an important message make sure it is being
//   sent from a high-priority task!
//
//   Please construct 'BrainMsg's by using helper functions in brain_rover.h
//void SendRoverMsg(BrainMsg msg);

#endif