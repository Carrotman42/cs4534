package main

import (
	"io"
	"net"
	"fmt"
	serial "github.com/tarm/goserial"
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

func ConnectSerial(port string, baud int) Protocol {
	c := &serial.Config{Name: port, Baud: baud}
	if s, err := serial.OpenPort(c); err != nil {
		panic(err)
	} else {
		return SerialProtocol{s}
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

// The protocol for the actual project
type SerialProtocol struct {
	d io.ReadWriteCloser
}
func (SerialProtocol) ReadCmd() InCmd {
	return nil
}

type SerialCommand uint8
const (
	FrameDataCmd SerialCommand = 0x01 // TODO: Loop this up
)
func (p SerialProtocol) writePacket(cmd SerialCommand, param uint8, payload []byte) {
	if len(payload) > 255 {
		panic("Payload was too long for the protocol!")
	}
	buf := make([]byte, 5 + len(payload))
	buf[0] = uint8(cmd)
	buf[1] = param
	buf[2] = 0
	buf[3] = 0
	buf[4] = byte(len(payload))
	copy(buf[5:], payload)
	
	if _, err := p.d.Write(buf); err != nil {
		panic(err)
	}
}

func (t SerialProtocol) WriteFrameData(f FrameData) {
	t.writePacket(FrameDataCmd, 0, []byte{f.Ultrasonic, f.IR1, f.IR2, f.REnc, f.LEnc})
}






