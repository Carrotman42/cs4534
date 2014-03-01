package main

import (
	"io"
	"net"
	"fmt"
)

type FrameData struct {
	Ultrasonic, IR1, IR2, REnc, LEnc uint8
}

type Protocol interface {
	ReadCmd() InCmd
	WriteFrameData(f FrameData)
}

// Abstract definition of an incoming message
type InCmd interface {
	CheckFrameCmd() (start, stop bool)
}

func ListenTCP(port string) Protocol {
	if l, err := net.Listen("tcp", ":" + port); err != nil {
		panic(err)
	} else if c, err := l.Accept(); err != nil {
		panic(err)
	} else {
		if err := l.Close(); err != nil {
			panic(err)
		}
		return TelnetProtocol{c}
	}
}
// Telnet interface: used to test the emulator itself
type TelnetProtocol struct {
	d io.ReadWriteCloser
}
func (t TelnetProtocol) ReadCmd() InCmd {
	var buf [1]byte
	if _, err := t.d.Read(buf[:]); err != nil {
		panic(err)
	}
	return TelnetCmd{buf[0]}
}
func (t TelnetProtocol) WriteFrameData(f FrameData) {
	fmt.Fprintf(t.d, "Ultrasonic: %v, IR1: %v, IR2: %v, REnc: %v, LEnc: %v\r\n", f.Ultrasonic, f.IR1, f.IR2, f.REnc, f.LEnc)
}

type TelnetCmd struct {
	letter byte
}
func (t TelnetCmd) CheckFrameCmd() (start, stop bool) {
	switch t.letter {
		case 'p': return false, true
		case 'o': return true, false
		default: return false, false
	}
}









