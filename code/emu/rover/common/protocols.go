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
	Error() error
}

type NullCmd struct{}
func (NullCmd) CheckFrameCmd() (start, stop bool){return}
func (NullCmd) CheckStartCmd() (speed int, ok bool){return}
func (NullCmd) CheckStopCmd() (ok bool) {return}
func (NullCmd) CheckTurnCmd() (val int, ok bool){return}
func (NullCmd) Error() error {return nil}

// Convenience structs for protocols
type FrameCmd struct {
	NullCmd
	StartStop bool
}
func (f FrameCmd) CheckFrameCmd() (bool, bool) {
	return f.StartStop, !f.StartStop
}

type SpeedCmd struct {
	NullCmd
	Speed int
	Stop bool
}
func (s SpeedCmd) CheckStartCmd() (int, bool) {
	if s.Stop {
		return 0, false
	}
	return int(s.Speed), true
}
func (s SpeedCmd) CheckStopCmd() bool {
	return s.Stop
}

type TurnCmd struct {
	NullCmd
	Val int
}
func (s TurnCmd) CheckTurnCmd() (int, bool) {
	return s.Val, true
}



