
#ifndef KDBG_H_INC
#define KDBG_H_INC

// TODO: Add in text debugging when we can get it.
void InitDBG();

// Will write a number in binary. Will fail if the number of bits required was
//    not requested in InitDGB(). Note that this value is hardcoded.
void DBGbit(unsigned bitnum, int on);


#endif