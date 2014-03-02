package main

import (
	"fmt"
	//"math"
	//"github.com/tarm/goserial"
	//"io"
	"os"
	"strconv"
)


type Dest interface {
	Write(b uint8)
}

type Src interface {
	Read() uint8
}


func main() {
	fmt.Println("Started")
	tel := ListenTCP("1123")
	_ = NewRover(tel)
	select{}
}


//115200
func mainSerial() {
	os.Args = os.Args[1:]
	if len(os.Args) != 3 {
		fmt.Println("Kevin's Rover Emulator", os.Args)
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
		fmt.Println("\n    NOTE: default baud rate for our wiflies is 19200\n\n")
		fmt.Println("Must specify usb/com port and baud rate to use")
		os.Exit(1)
	}
	
	port := os.Args[0]
	baud, err := strconv.Atoi(os.Args[1])
	if err != nil { panic("Could not convert given baud rate to number!") }
	switch os.Args[2] {
		//case "1": Protocol = Protocol1
		//case "2": Protocol = Protocol2
		//case "3": Protocol = Protocol3
		//default: panic("Unknown protocol")
	}
	
	//m := CreateMap()
	//_ = CreateRover(&m, port, baud)
	_, _ = port, baud
	select{}
}
/*
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


const (
	ArmUnitsPerTile = 10
	
	CourseSize = 100
)
type Map struct {
	course [CourseSize][CourseSize]bool
}

func multAdd(x, i int, v float64) int {
	return x + int(float64(i) * v)
}

// Returns value in ticks
//func (r *Rover) GetFrontSensor(m *Map) int {
//	x, y := r.X/TicksPerTile, r.Y/TicksPerTile
//	return m.DistToWall(x, y, r.Dir)
//}

func (m*Map) DistToWall(x, y, dir int) int {
	sin, cos := math.Sincos(float64(dir) / 180 * math.Pi)
	
	for i := 0; ; i++ {
		if m.course[multAdd(x, i, cos)][multAdd(y, i, sin)] {
			return i*TicksPerTile
		}
	}
}*/













