package common

import "fmt"

type FrameData struct {
	Ultrasonic, IR1, IR2 uint8
	REnc, LEnc uint16
}
func (f FrameData) String() string {
	return fmt.Sprintf("Ultrasonic: %v, IR1: %v, IR2: %v, REnc: %v, LEnc: %v\r\n", f.Ultrasonic, f.IR1, f.IR2, f.REnc, f.LEnc)
}
func (f FrameData) ToBytes() []byte {
	return []byte{f.Ultrasonic, f.IR1, f.IR2, byte(f.REnc>>8), byte(f.REnc&0xFF), byte(f.LEnc>>8), byte(f.LEnc&0xFF)}
}

type ErrorKind uint8
const (
	SensorPIC ErrorKind = iota
	MotorPIC
	MasterPIC
	Ultrasonic
	IR1
	IR2
	Color
	LeftEnc
	RightEnc
	LeftWheel
	RightWheel
	Checksum
)

func (e ErrorKind) String() string {
	switch e {
	case SensorPIC: return "SensorPIC"
	case MotorPIC: return "MotorPIC"
	case MasterPIC: return "MasterPIC"
	case Ultrasonic: return "Ultrasonic"
	case IR1: return "IR1"
	case IR2: return "IR2"
	case Color: return "Color"
	case LeftEnc: return "LeftEnc"
	case RightEnc: return "RightEnc"
	case LeftWheel: return "LeftWheel"
	case RightWheel: return "RightWheel"
	case Checksum: return "Checksum"
	default: panic("Unknown errkind")
	}
}	

func (e ErrorKind) Error() string {
	return e.String() + " error"
}

type Protocol interface {
	ReadCmd() InCmd
	WriteFrameData(f FrameData)
	TurnFinished()
	FinishLine()
	//WriteError(f ErrorKind)
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



