package main

import (
	"sync"
	"time"
	"./common"
	"io"
	"math"
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
	
	Map common.Map
}

func NewRover(p common.Protocol, list RoverListener) *Rover {
	ret := &Rover{}
	ret.Listener = list
	ret.Protocol = p
	
	ret.StartLoops()
	ret.Map.Init()
	return ret
}

type RoverListener interface {
	Update(common.RoverSnapshot)
}

func (r*Rover) StartLoops() {
	go r.FrameLoop()
	go r.Loop()
}

const SPEED_SCALE = 1./15

func (r*Rover) Tick() {
	ang := float64(r.Dir)*math.Pi/180
	newx, newy := r.X + r.Vel * SPEED_SCALE * math.Cos(ang), r.Y + r.Vel * SPEED_SCALE * math.Sin(ang)

	if !r.Map.Outside(int(newx), int(newy), r.Dir) {
		// Didn't hit the wall
		r.X, r.Y = newx, newy
	}
	
	// Update FrameData
	r.FrameData.Ultrasonic = r.CalcSensorDist(5, 0, 0)
	r.FrameData.IR1 = r.CalcSensorDist(2, 5, 90)
	r.FrameData.IR2 = r.CalcSensorDist(-2, 5, 90)
	
	d := uint16(r.Vel)
	r.FrameData.REnc += d
	r.FrameData.LEnc += d
	//r.FrameData.Ultrasonic = r.CalcSensorDist()
}

func multAdd(x, i int, v float64) int {
	return x + int(float64(i) * v)
}
func mAdd(x float64, i int, v float64) int {
	return int(x + float64(i) * v)
}

// TODO: Calculate the real offsets of the sensors from the centroid
func (r*Rover) CalcSensorDist(offx, offy, offdir int) uint8 {
	sin, cos := math.Sincos(float64(r.Dir) / 180 * math.Pi)
	
	x,y := mAdd(r.X, offx, cos), mAdd(r.Y, offy, sin)
	v, ok := r.Map.DistToWall(x, y, offdir + r.Dir)
	v /= 10
	if v > 255 || !ok {
		v = 255
	}
	return uint8(v)
}

func (r*Rover) FrameLoop() {
	//r.Vel = 3
	for {
		time.Sleep(time.Second/30)
		
		r.Lock()
		
		r.Tick()
		
		if r.SendFrames {
			r.WriteFrameData(r.FrameData)
			// Reset the encoder values
			r.REnc = 0
			r.LEnc = 0
		}
		if r.Listener != nil {
			r.Listener.Update(r.RoverSnapshot)
		}
		
		r.Unlock()
	}
}

func (r*Rover) HandleFrameCmd(start bool) {
	fmt.Println("START:", start)
	r.Lock()
	r.SendFrames = start
	r.Unlock()
}

func (r*Rover) Loop() {
	for {
		cmd := r.ReadCmd()
		if err := cmd.Error(); err != nil {
			fmt.Println("Error:", err)
		} else if start, stop := cmd.CheckFrameCmd(); start || stop {
			r.HandleFrameCmd(start)
		} else if speed, ok := cmd.CheckStartCmd(); ok {
			r.Vel = float64(speed)
		} else if cmd.CheckStopCmd() {
			r.Vel = 0
		} else if val, ok := cmd.CheckTurnCmd(); ok {
			r.Dir += val
			r.TurnFinished()
		} else {
			fmt.Println("UNKNOWN CMD:", cmd)
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
		return &TelnetProtocol{c,0}
	}
}

// Connects to the ARM on the given serial port
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
	
	lastSpeed int
}
func (t*TelnetProtocol) ReadCmd() common.InCmd {
	var buf [1]byte
	if _, err := t.d.Read(buf[:]); err != nil {
		panic(err)
	}
	ret := TelnetCmd{buf[0], t.lastSpeed}
	if v,ok := ret.CheckStartCmd(); ok {
		t.lastSpeed = v
	} else if ret.CheckStopCmd() {
		t.lastSpeed = 0
	}
	return ret
}
func (t TelnetProtocol) Error(f common.ErrorKind) {
	fmt.Fprint(t.d, f.Error())
}
func (t TelnetProtocol) WriteFrameData(f common.FrameData) {
	fmt.Fprint(t.d, f.String())
}
func (t TelnetProtocol) TurnFinished() {
	fmt.Fprintln(t.d, "Turn Finished")
}

type TelnetCmd struct {
	letter byte
	lastSpeed int
}
func (t TelnetCmd) CheckFrameCmd() (start, stop bool) {
	switch t.letter {
		case 'p': return false, true
		case 'o': return true, false
		default: return false, false
	}
}
func (t TelnetCmd) CheckStartCmd() (speed int, ok bool) {
	if t.letter == 'w' {
		return t.lastSpeed + 2, true
	} else if t.letter == 's' {
		return t.lastSpeed - 2, true
	}
	return 0,false
}
func (t TelnetCmd) CheckStopCmd() (ok bool) {
	return t.letter == ' '
}
func (t TelnetCmd) CheckTurnCmd() (val int, ok bool) {
	if t.letter == 'a' {
		return -90, true
	} else if t.letter == 'd' {
		return 90, true
	}
	return 0, false
}

// Don't even treat mistypes as errors, just ignore them
func (TelnetCmd) Error() error {
	return nil
}
	
	
// The protocol for the actual project
type SerialProtocol struct {
	io.ReadWriteCloser
}
func (s SerialProtocol) ReadByte() byte {
	var buf [1]byte
	if _,err := s.Read(buf[:]); err != nil {
		panic(err) //TODO remove panic
	}
	return buf[0]
}


type SerialErrCmd struct {
	errText string
	cmd, param, mId, ll, check byte
	payload []byte
	common.NullCmd
}
func (s SerialErrCmd) Error() error {
	return fmt.Errorf("%s: %d %d %d %d %d\nPayload: %v", s.errText, s.cmd, s.param, s.mId, s.ll, s.check, s.payload)
}
func SerialErr(str string, cmd, param, mId, ll, check byte, payload []byte) SerialErrCmd {
	return SerialErrCmd{
		str,cmd,param,mId,ll,check,payload,common.NullCmd{},
	}
}

func (s SerialProtocol) ReadCmd() (ret common.InCmd) {
	cmd := s.ReadByte()
	param := s.ReadByte()
	mId := s.ReadByte()
	ll := s.ReadByte()
	check := s.ReadByte()

	buf := make([]byte, ll)
	if ll > 0 {
		if _,err := s.Read(buf); err != nil {
			return SerialErr("Could not read payload: " + err.Error(), cmd,param,mId,ll,check,buf)
		}
	}
	fmt.Println("Got packet:", cmd, param, mId, ll, check)
	defer func() {
		if ret.Error() == nil {
			// If there was no error make sure to ack the message (there are no synchronous commands expected)
			s.writePacket(uint(cmd) | 0x20, param, nil)
		}
	}()
	
	// Calc checksum:
	tot := cmd + param + mId + ll
	for _,v := range buf {
		tot += v
	}
	if check != tot {
		return SerialErr("failed checksum:",cmd,param,mId,ll,check,buf)
	}
	
	switch cmd {
		case 0x04: // High-level command
			switch param {
				case 0: //Start frames
					return common.FrameCmd{StartStop:true}
				case 3: //Stop frames
					return common.FrameCmd{StartStop:false}
				default:
					return SerialErr("Invalid command: ",cmd,param,mId,ll,check,buf)
			}
		case 0x02: // Motor commands
			switch param {
				case 0: //Start forward
					return common.SpeedCmd{Speed:int(buf[0]), Stop: false}
				case 1: //Start backward
					return common.SpeedCmd{Speed:-int(buf[0]), Stop: false}
				case 2: //Stop
					return common.SpeedCmd{Speed:0, Stop: true}
				case 3: //Turn right
					return common.TurnCmd{Val: int(buf[0])}
				case 4: //Turn right
					return common.TurnCmd{Val: -int(buf[0])}
				default:
					return SerialErr("Invalid command: ",cmd,param,mId,ll,check,buf)
			}
		default:
			return SerialErr("Invalid command: ",cmd,param,mId,ll,check,buf)
	}
}

func (s SerialProtocol) WriteFrameData(f common.FrameData) {
	fmt.Println("write frame data, Implement me please!")
	//TODO!
}
func (s SerialProtocol) TurnFinished() {
	fmt.Println("Turn finished, Implement me please!")
}
func (t SerialProtocol) Error(f common.ErrorKind) {
	var p, l byte
	switch f {
	case common.SensorPIC:
		p, l = 1,1
	case common.MotorPIC:
		p, l = 1,2
	case common.MasterPIC: 
		p, l = 1,3
	case common.Ultrasonic: 
		p, l = 2,1
	case common.IR1: 
		p, l = 2,2
	case common.IR2: 
		p, l = 2,3
	case common.Color: 
		p, l = 2,4
	case common.LeftEnc: 
		p, l = 2,5
	case common.RightEnc: 
		p, l = 2,6
	case common.LeftWheel: 
		p, l = 3,1
	case common.RightWheel: 
		p, l = 3,2
	case common.Checksum: 
		p, l = 4,1
	default: panic("Unknown errkind")
	}
	
	_,_ = p,l
	//s.writePacket(0x40, 
}

// TODO: FIX THE AWFUL 0xFF hack!
func (p SerialProtocol) writePacket(cmd uint, param uint8, payload []byte) {
	if len(payload) > 255 {
		panic("Payload was too long for the protocol!")
	}
	buf := make([]byte, 5 + len(payload) + 1)
	buf[0] = uint8(cmd)
	buf[1] = param
	buf[2] = 0
	buf[3] = byte(len(payload))
	
	check := buf[0] + buf[1] + buf[2] + buf[3]
	for i,v := range payload {
		if v == 0xFF {
			v = 0xFE
		}
		check += v
		buf[5+i] = v
	}
	if check == 0xFF {
		check = 0xFE
	}
	buf[4] = check
	buf[len(buf)-1] = 0xFF
	
	fmt.Println("Writing packet:", buf)
	if _, err := p.Write(buf); err != nil {
		panic(err)
	}
}


// Connects to the ARM on the given ethernet address
func ConnectEthernet(addr string, port string) common.Protocol {
	if conn, err := net.Dial("tcp", addr + ":" + port); err != nil {
		panic(err)
	} else {
		return &PicmanProtocol{Arm: SerialProtocol{conn}}
	}
}
// Pretends we're the picman
type PicmanProtocol struct {
	Arm SerialProtocol
	last common.FrameData
	lastOk bool
	doneTurning bool
}

func (s*PicmanProtocol) TurnFinished() {
	s.doneTurning = true
}
func (s*PicmanProtocol) ReadCmd() (ret common.InCmd) {
	for {
		ret = s.Arm.ReadCmd()
		if r, ok := ret.(SerialErrCmd); ok {
			if r.cmd == 4 && r.param == 2 {
				// Report the last frame data we got
				payload := s.last.ToBytes()
				cmd := uint(r.cmd)
				if s.lastOk {
					s.lastOk = false
				} else {
					cmd |= 0x08
				}
				s.Arm.writePacket(cmd, 2, payload)
				continue
			} else if r.cmd == 4 && r.param == 5 {
				var pay [1]byte
				if s.doneTurning {
					pay[0] = 1
					s.doneTurning = false
				}
				s.Arm.writePacket(4, 5, pay[:])
				continue
			}
		}
		return ret
	}
}
func (s *PicmanProtocol) WriteFrameData(f common.FrameData) {
	if s.lastOk {
		// The last one was still valid, so don't
		//   overwrite it
		f.LEnc += s.last.LEnc
		f.REnc += s.last.REnc
	}
	s.last = f
	s.lastOk = true
}
func (t PicmanProtocol) Error(f common.ErrorKind) {
	fmt.Println("ERROR:", f)
}



