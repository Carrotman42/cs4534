#ifndef DEBUG_H
#define	DEBUG_H

#ifdef	__cplusplus
extern "C" {
#endif

    enum {DBG1, DBG2, DBG3, DBG4};

    void setDBG(unsigned char );
    void resetDBG(unsigned char );
    void flipDBG(unsigned char );
    void debugNum(int );


#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_H */

