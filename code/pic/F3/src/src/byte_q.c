
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

// The test file for this code. To test, all you have to do is
//   compile this single file and give the flag -DBYTE_Q_TEST to gcc
//   (and then run that executable)
#ifdef BYTE_Q_TEST
#include <stdio.h>

int fill(int start, int len, unsigned char*dest) {
	int i;
	for (i = 0; i < len; i++) {
		dest[i] = start;
		start++;
	}
	return start;
}

int main() {
	byte_queue q;
	
	vqInit(&q);
	
	unsigned char buf[100];
	
	unsigned vals[] = {10, 40, 20, 50, 100, 20, 100, 10};
	char skipPopp[] = { 1,  1,  1,  0,   1,  0,   1, 0};
	int valpos;
	
	char curNum = 0;
	char expNum = 0;
	char ok;
	for (valpos = 0; valpos < sizeof(vals) / sizeof(vals[0]); valpos++) {
		unsigned thisLen = vals[valpos];
		curNum = fill(curNum, thisLen, buf);
		if (vqSend(&q, thisLen, buf)) {
			printf("Q WAS FULL! at %d\n", valpos);
			return 1;
		}
		
		if (!skipPopp[valpos]) {
			while(1) {
				char ret = vqPop(&q, &ok);
				if (!ok) break;
				if (ret != expNum) {
					printf("Got %d, wanted %d!\n", ret, expNum);
				}
				expNum++;
			}
		}
	}
	
	return 0;
}

#endif




