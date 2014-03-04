package common

import "fmt"

type FrameData struct {
	Ultrasonic, IR1, IR2, REnc, LEnc uint8
}
func (f FrameData) String() string {
	return fmt.Sprintf("Ultrasonic: %v, IR1: %v, IR2: %v, REnc: %v, LEnc: %v\r\n", f.Ultrasonic, f.IR1, f.IR2, f.REnc, f.LEnc)
}

type Protocol interface {
	ReadCmd() InCmd
	WriteFrameData(f FrameData)
}

// Abstract definition of an incoming message
type InCmd interface {
	CheckFrameCmd() (start, stop bool)
	CheckStartCmd() (speed int, ok bool)
	CheckStopCmd() (ok bool)
	CheckTurnCmd() (val int, ok bool)
}

// Convenience structs for protocols
type FrameCmd struct {
	startStop bool
}
func (f FrameCmd) CheckFrameCmd() (bool, bool) {
	return f.startStop, !f.startStop
}






