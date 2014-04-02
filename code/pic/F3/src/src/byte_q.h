#ifndef BYTE_Q_H_INC
#define BYTE_Q_H_INC

#define varqBits 7
#define VAR_Q_LEN (1 << varqBits)
#define VAR_Q_MASK (VAR_Q_LEN - 1)
typedef struct {
	unsigned char write;
	unsigned char read;
	unsigned char data[VAR_Q_LEN];
} byte_queue;

void vqInit(byte_queue *q);
int vqSend(byte_queue *q, unsigned len, unsigned char *data);

// ok is an out param. If it is true then there was a value, else the queue
//    was empty.
inline unsigned char vqPop(byte_queue*q, char *ok);

#endif