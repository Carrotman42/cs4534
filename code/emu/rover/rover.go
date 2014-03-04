package main

import (
	"sync"
	"time"
	"./common"
	"io"
)


import (
	"net"
	"fmt"
	serial "github.com/tarm/goserial"
)

type Rover struct {
	sync.Mutex
	common.Protocol
	Listener RoverListener
	
	// This value is automatically updated any time any of the more precise values have changed, eg X/Y
	common.RoverSnapshot
}

func NewRover(p common.Protocol, list RoverListener) *Rover {
	ret := &Rover{}
	ret.Listener = list
	ret.Protocol = p
	
	ret.StartLoops()
	return ret
}

type RoverListener interface {
	Update(common.RoverSnapshot)
}

func (r*Rover) StartLoops() {
	go r.FrameLoop()
	go r.Loop()
}

func (r*Rover) FrameLoop() {
	for {
		time.Sleep(time.Second)
		
		r.Lock()
		if r.SendFrames {
			r.WriteFrameData(r.FrameData)
		}
		if r.Listener != nil {
			fmt.Println("Writing to gui: ", r.RoverSnapshot, r.SendFrames)
			r.Listener.Update(r.RoverSnapshot)
		}
		
		r.Unlock()
	}
}

func (r*Rover) HandleFrameCmd(start bool) {
	r.Lock()
	r.SendFrames = start
	r.Unlock()
}

func (r*Rover) Loop() {
	for {
		cmd := r.ReadCmd()
		
		if start, stop := cmd.CheckFrameCmd(); start || stop {
			r.HandleFrameCmd(start)
		}
	}
}


func ListenTCP(port string) common.Protocol {
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

func ConnectSerial(port string, baud int) common.Protocol {
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
func (t TelnetProtocol) ReadCmd() common.InCmd {
	var buf [1]byte
	if _, err := t.d.Read(buf[:]); err != nil {
		panic(err)
	}
	return TelnetCmd{buf[0]}
}
func (t TelnetProtocol) WriteFrameData(f common.FrameData) {
	fmt.Fprint(t.d, f.String())
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
func (SerialProtocol) ReadCmd() common.InCmd {
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

func (t SerialProtocol) WriteFrameData(f common.FrameData) {
	t.writePacket(FrameDataCmd, 0, []byte{f.Ultrasonic, f.IR1, f.IR2, f.REnc, f.LEnc})
}