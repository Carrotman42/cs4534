
#include "byte_q.h"

void vqInit(byte_queue *q) {
	q->write = q->read = 0;
}

// NOTE: if write==read it is empty. This is achieved because it
//    is not possible to fully fill the queue's buffer (we'd have
//    to waste a byte somewhere if we ever want a buffer > 128 anyway)
//    doing it this way makes it easier to program
int vqSend(byte_queue *q, unsigned len, unsigned char *data) {
	// Make sure it can fit
	unsigned char w = q->write, r = q->read;
	unsigned char cap;
	// Deal with unsigned math carefully
	if (r > w) {
		cap = r - w;
	} else /*if (r <= w)*/ {
		cap = VAR_Q_LEN - (w - r);
	}
	if (cap <= len) return 1;
	
	unsigned char* dest = &q->data[0];
	unsigned char last = (w + len) & VAR_Q_MASK;
	while (w != last) {
		dest[w] = *data++;
		w = (w + 1) & VAR_Q_MASK;
	}
	
	q->write = w;
	return 0;
}

inline unsigned char vqPop(byte_queue *q, char *ok) {
	// Set whether there is any data to read
	register unsigned char r = q->read;
	if (*ok = (q->write != r)) {
		unsigned char ret = q->data[r];
		q->read = (r + 1) & VAR_Q_MASK;
		return ret;
	}
	return 0;
}


