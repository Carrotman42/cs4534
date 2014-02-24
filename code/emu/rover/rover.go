package main

import (
	"fmt"
	"math"
	"github.com/tarm/goserial"
	"io"
	"os"
	"strconv"
)

type ProtocolImpl func(byte, byte, []byte) []byte
var Protocol ProtocolImpl

func Protocol1(flags, params byte, in []byte) []byte {
	l := len(in)
	ret := make([]byte, l+3)
	ret[0] = flags
	ret[1] = params
	ret[2] = byte(l)
	copy(ret[3:], in)
	return ret
}
func Protocol2(flags, params byte, in []byte) []byte {
	l := len(in)
	ret := make([]byte, l+4)
	ret[0] = flags
	ret[1] = params
	ret[2] = byte(l)
	ret[3] = messageID
	copy(ret[4:], in)
	
	messageID++
	return ret
}
func Protocol3(flags, params byte, in []byte) []byte {
	l := len(in)
	ret := make([]byte, l+4)
	ret[0] = flags
	ret[1] = params
	ret[2] = byte(l)
	ret[3] = messageID
	ret[4] = 0 // calculate checksum with 'checksum' value already 0
	copy(ret[5:], in)
	
	var sum byte
	for _,v := range ret {
		sum += v
	}
	ret[4] = sum
	
	messageID++
	return ret
}
// Will increase every time a packet is wrapped by protocol 2 or 3
var messageID = byte(0)

//115200
func main() {
	if len(os.Args) != 3 {
		fmt.Println("Kevin's Rover Emulator")
		fmt.Println("Command arguments:")
		fmt.Println("  [usb/com port] [baud rate] [Protocol version]")
		fmt.Println("    - Port: usually is a COM[0-9]+ port for windows. The port the WiFly is connected to")
		fmt.Println("    - Baud: the baud rate")
		fmt.Println("         Valid values for WiFly: 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, or 921600")
		fmt.Println("    - Protocol version: Allows us to test mutliple versions of the protocol without recompiling this program")
		fmt.Println("         1: [uint8] [uint8]  [uint8]       [bytes]")
		fmt.Println("             FLAGS | PARAMS | PAYLOAD LEN | ACTUAL PAYLOAD DATA")
		fmt.Println("         2: [uint8] [uint8]  [uint8]       [uint8]     [bytes]")
		fmt.Println("             FLAGS | PARAMS | PAYLOAD LEN | MESSAGEID | ACTUAL PAYLOAD DATA")
		fmt.Println("         3: [uint8] [uint8]  [uint8]       [uint8]     [uint8]    [bytes]")
		fmt.Println("             FLAGS | PARAMS | PAYLOAD LEN | MESSAGEID | CHECKSUM | ACTUAL PAYLOAD DATA")
		fmt.Println()
		fmt.Println("Example usage: `rover.exe COM4 19200 1`")
		fmt.Println("                Run rover emulator with WiFly module on COM4 at baud rate 19200")
		fmt.Println("                  with protocol description=[FLAGS][PARAMS][PAYLOADLEN][PAYLOAD]")
		
		fmt.Println("Must specify usb/com port and baud rate to use")
		os.Exit(1)
	}
	
	port := os.Args[0]
	baud, err := strconv.Atoi(os.Args[1])
	if err != nil { panic("Could not convert given baud rate to number!") }
	switch os.Args[2] {
		case "1": Protocol = Protocol1
		case "2": Protocol = Protocol2
		case "3": Protocol = Protocol3
		default: panic("Unknown protocol")
	}
	
	m := CreateMap()
	_ = CreateRover(&m, port, baud)
	select{}
}

func CreateRover(m *Map, com string, baud int) *Rover {
	r := &Rover{}
	r.X = TicksPerTile * 16
	r.Y = CourseSize * TicksPerTile / 2
	r.Dir = 0
	
	c := &serial.Config{Name: com, Baud: baud}
	if s, err := serial.OpenPort(c); err != nil {
		panic(err)
	} else {
		r.ReadWriteCloser = s
	}
	
	go r.commThread(m)
	
	return r
}

func CreateMap() (m Map) { 
	for i := 10; i < (CourseSize - 10); i++ {
		m.course[i][10] = true
		m.course[i][CourseSize - 10] = true
		m.course[10][i] = true
		m.course[CourseSize - 10][i] = true
	}
	
	return m
}

type Rover struct {
	// These are in ticks
	X, Y int
	// Angle in degrees
	Dir int
	
	// Communication classes
	io.ReadWriteCloser
}

const (
	TicksPerTile = 10
	
	CourseSize = 100
)
type Map struct {
	course [CourseSize][CourseSize]bool
}

func multAdd(x, i int, v float64) int {
	return x + int(float64(i) * v)
}

// Returns value in ticks
func (r *Rover) GetFrontSensor(m *Map) int {
	x, y := r.X/TicksPerTile, r.Y/TicksPerTile
	return distToWall(x, y, r.Dir, m)
}

func distToWall(x, y, dir int, m*Map) int {
	sin, cos := math.Sincos(float64(dir) / 180 * math.Pi)
	
	for i := 0; ; i++ {
		if m.course[multAdd(x, i, cos)][multAdd(y, i, sin)] {
			return i*TicksPerTile
		}
	}
}


func (r*Rover) commThread(m*Map) {
	r.milestone2(m)
}


// For milestone 2: just read a request and write back a dummy sensor value
func (r*Rover) milestone2(m*Map) {
	var curByte byte
	var buf [200]byte
	for {
		if n, err := r.Read(buf[:2]); err != nil {
			fmt.Println("Read error, ending goroutine:", err)
			return
		} else if n == 0 {
			continue
		} else if n == 1 {
			if n, err = r.Read(buf[1:2]); err != nil || n == 0 {
				if n == 0 { fmt.Println("##@#@$Data recv is annoying") }
				fmt.Println("Read error, ending goroutine:", err)
				return
			}
		}
		
		fmt.Printf("Got %#X %#X\n", buf[0], buf[1])
		
		curByte++
		buf[0] = curByte
		r.writeMessage(buf[:1])
	}
}

func (r*Rover) writeMessage(payload []byte) {
	// Sensor response flag == 1
	// Sensor ID == 0
	msg := Protocol(1, 0, payload)
	
	if _, err := r.Write(msg); err != nil {
		fmt.Println("ERROR during write:", err)
	}
}














