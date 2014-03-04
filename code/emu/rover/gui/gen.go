package gui

import (
	"fmt"
	"os"
	"github.com/Carrotman42/kgui"
	"sync"
	"runtime"
)
var _ = fmt.Println
var _ = kgui.Canvas{}
var _ = os.Open
var _ = sync.Mutex{}
var _ = runtime.NumGoroutine
 
 
 
type GMapSlotConn struct {
	D chan gMap
}

func (c*GMapSlotConn) reportStaleVal(data gMap) {
	fmt.Println("Found stale data in a slot queue: ", data)
}

func (c*GMapSlotConn) Send(data gMap) {
	ch := c.D

	retry:
	for {
		select {
			case ch<-data:
				break retry
			default:
		}
		select {
			case oldVal := <-ch:
				c.reportStaleVal(oldVal)
			default:
		}
	}
}

func (c*GMapSlotConn) Init() {
	// Capacity also defined in SlotConn
	c.D = make(chan gMap, 2)
}

type GMapSlot struct {
	D <-chan gMap
}
 
type GSnapSlotConn struct {
	D chan gSnap
}

func (c*GSnapSlotConn) reportStaleVal(data gSnap) {
	fmt.Println("Found stale data in a slot queue: ", data)
}

func (c*GSnapSlotConn) Send(data gSnap) {
	ch := c.D

	retry:
	for {
		select {
			case ch<-data:
				break retry
			default:
		}
		select {
			case oldVal := <-ch:
				c.reportStaleVal(oldVal)
			default:
		}
	}
}

func (c*GSnapSlotConn) Init() {
	// Capacity also defined in SlotConn
	c.D = make(chan gSnap, 2)
}

type GSnapSlot struct {
	D <-chan gSnap
}
 
type GMapProperty struct {
	GMapSignal
	everSet bool
}

func (p*GMapProperty) Send(v gMap) {
	panic("Should not be using this function!")
}

func (p*GMapProperty) Set(v gMap) {
	if !p.everSet || !kgui.SafeCompare(p.GMapSignal.lastVal, v) {
		p.everSet = true
		p.GMapSignal.Send(v)
	} else {
		//fmt.Println("Ignored prop set. Old=",p.GMapSignal.lastVal, ", new=", v)
	}
}

func (p*GMapProperty) Attach(c GMapSlotConn) {
	p.GMapSignal.Attach(c)
	if p.everSet {
		c.Send(p.lastVal)
	}
}
func (p*GMapProperty) Tick(nv *gMap) bool {
	// Overridding like this may cause a long time to copy large structs, but
	//    if you use a large struct in a copy situation you should expect that sort
	//    of thing. The Signal.Tick method could be copied and modified here though
	//    if the copying becomes cumbersome.
	
	// nv is set after clearing the old val
	//   this allows us to keep the old value
	wasNew := p.GMapSignal.Tick(&p.lastVal)
	if nv != nil {
		*nv = p.lastVal
	}
	return wasNew
}
 
type GSnapProperty struct {
	GSnapSignal
	everSet bool
}

func (p*GSnapProperty) Send(v gSnap) {
	panic("Should not be using this function!")
}

func (p*GSnapProperty) Set(v gSnap) {
	if !p.everSet || !kgui.SafeCompare(p.GSnapSignal.lastVal, v) {
		p.everSet = true
		p.GSnapSignal.Send(v)
	} else {
		//fmt.Println("Ignored prop set. Old=",p.GSnapSignal.lastVal, ", new=", v)
	}
}

func (p*GSnapProperty) Attach(c GSnapSlotConn) {
	p.GSnapSignal.Attach(c)
	if p.everSet {
		c.Send(p.lastVal)
	}
}
func (p*GSnapProperty) Tick(nv *gSnap) bool {
	// Overridding like this may cause a long time to copy large structs, but
	//    if you use a large struct in a copy situation you should expect that sort
	//    of thing. The Signal.Tick method could be copied and modified here though
	//    if the copying becomes cumbersome.
	
	// nv is set after clearing the old val
	//   this allows us to keep the old value
	wasNew := p.GSnapSignal.Tick(&p.lastVal)
	if nv != nil {
		*nv = p.lastVal
	}
	return wasNew
}
 
type GMapSignal struct {
	Attacher chan GMapSlotConn
	slots [] GMapSlotConn

	lastVal gMap
	newVal bool
}

func (s*GMapSignal) Init() {
	// Unbuffered for now
	s.Attacher = make(chan GMapSlotConn)
}

func (s*GMapSignal) MakeSlot() GMapSlot {
	// Capacity also defined in SlotConn
	ret := make(chan gMap, 2)
	
	s.Attacher <- GMapSlotConn {
		ret,
	}
	
	return GMapSlot{ ret }
}

func (s*GMapSignal) Tick(nv *gMap) (wasNew bool) {
	if wasNew = s.newVal; wasNew {
		lv := s.lastVal
		// Clear out the saved value so that we don't accidentally
		//   hold on to pointers we shouldn't
		var blank gMap
		s.lastVal = blank
		if nv != nil {
			*nv = lv
		}
		for _,sl := range s.slots {
			sl.Send(lv)
		}
		s.newVal = false
	}
	return
}

func (s*GMapSignal) Attach(c GMapSlotConn) {
	s.slots = append(s.slots, c)
}

func (s*GMapSignal) Send(val gMap) {
	if s.newVal {
		fmt.Println("Warning: value for slot written twice without being broadcasted. Old: ", s.lastVal, ", new: ", val) 
	}
	s.newVal = true
	s.lastVal = val
}
 
type GSnapSignal struct {
	Attacher chan GSnapSlotConn
	slots [] GSnapSlotConn

	lastVal gSnap
	newVal bool
}

func (s*GSnapSignal) Init() {
	// Unbuffered for now
	s.Attacher = make(chan GSnapSlotConn)
}

func (s*GSnapSignal) MakeSlot() GSnapSlot {
	// Capacity also defined in SlotConn
	ret := make(chan gSnap, 2)
	
	s.Attacher <- GSnapSlotConn {
		ret,
	}
	
	return GSnapSlot{ ret }
}

func (s*GSnapSignal) Tick(nv *gSnap) (wasNew bool) {
	if wasNew = s.newVal; wasNew {
		lv := s.lastVal
		// Clear out the saved value so that we don't accidentally
		//   hold on to pointers we shouldn't
		var blank gSnap
		s.lastVal = blank
		if nv != nil {
			*nv = lv
		}
		for _,sl := range s.slots {
			sl.Send(lv)
		}
		s.newVal = false
	}
	return
}

func (s*GSnapSignal) Attach(c GSnapSlotConn) {
	s.slots = append(s.slots, c)
}

func (s*GSnapSignal) Send(val gSnap) {
	if s.newVal {
		fmt.Println("Warning: value for slot written twice without being broadcasted. Old: ", s.lastVal, ", new: ", val) 
	}
	s.newVal = true
	s.lastVal = val
}
 type mainG struct {
quitter chan *sync.Mutex
Paint kgui.CanvasProperty
Mouse kgui.MouseEventSlotConn
Map GMapProperty
Snap GSnapProperty
}
type Main struct {
mainG
SnapSetter GSnapSlotConn
MapSetter GMapSlotConn
}
func (c*Main) Loop() {
var flagPaintInitted bool
var Paint kgui.Canvas
_, _ = Paint, flagPaintInitted
var flagSnapInitted bool
var Snap gSnap
_, _ = Snap, flagSnapInitted
var flagMapInitted bool
var Map gMap
_, _ = Map, flagMapInitted
var inittedAllcalcPaint bool
mainLoop: for {
flagPaintUpdated := c.Paint.Tick(&Paint)
flagPaintInitted = flagPaintInitted || flagPaintUpdated
flagSnapUpdated := c.Snap.Tick(&Snap)
flagSnapInitted = flagSnapInitted || flagSnapUpdated
var Mouse kgui.MouseEvent
_ = Mouse
var flagMouseUpdated bool
_ = flagMouseUpdated
flagMapUpdated := c.Map.Tick(&Map)
flagMapInitted = flagMapInitted || flagMapUpdated
if !(flagPaintUpdated||flagSnapUpdated||flagMouseUpdated||flagMapUpdated) {
for { select {
case nl := <-c.Paint.Attacher:
c.Paint.Attach(nl)
continue
case nl := <-c.Snap.Attacher:
c.Snap.Attach(nl)
continue
case Snap = <-c.SnapSetter.D:
c.Snap.Set(Snap)
c.Snap.Tick(nil)
flagSnapInitted = true
flagSnapUpdated = true
case Mouse = <-c.Mouse.D:
flagMouseUpdated = true
case nl := <-c.Map.Attacher:
c.Map.Attach(nl)
continue
case Map = <-c.MapSetter.D:
c.Map.Set(Map)
c.Map.Tick(nil)
flagMapInitted = true
flagMapUpdated = true
case nl := <-c.quitter: defer func() { nl.Unlock() }(); break mainLoop
};break}}
inittedAllcalcPaint = inittedAllcalcPaint || flagSnapInitted
if inittedAllcalcPaint && (flagSnapUpdated) {
c.Paint.Set(c.calcPaint(Snap))
}
}}
func (t*mainG) Init() {
t.quitter = make(chan *sync.Mutex)
t.Paint.Init()
t.Mouse.Init()
t.Map.Init()
t.Snap.Init()
}
func (t*Main) Init() {
t.mainG.Init()
t.SnapSetter.Init()
t.MapSetter.Init()
go t.Loop()
}
func (m*Main) calcPaint(sn gSnap) kgui.Canvas {
fmt . Println ( "In paint!" ) 
 r := kgui . NewCanvasMaker ( ) 
 r . ChangeColor ( kgui . Color { 0 , 0 , 0 } ) 
 r . Text ( kgui . AFont , kgui . Point { 0 , 0 } , sn . FrameData . String ( ) ) 
 if sn . SendFrames { fmt . Println ( "Wrote send frames!" ) 
 r . Text ( kgui . AFont , kgui . Point { 0 , 15 } , "Sending frame data!" ) 
 } 
 return r . Freeze ( ) 
 
}
func (t*mainG) Quit() {
  var wait sync.Mutex
  wait.Lock()
  t.quitter <- &wait
  wait.Lock()
}
func (b*mainG) PaintSlot() kgui.CanvasSlot {
return b.Paint.MakeSlot()
}
func (b*mainG) MouseSend(in kgui.MouseEvent) {
b.Mouse.Send(in)
}
func (b*mainG) MapSlot() GMapSlot {
return b.Map.MakeSlot()
}
func (b*mainG) SnapSlot() GSnapSlot {
return b.Snap.MakeSlot()
}
func (b*Main) SnapSet(in gSnap) {
b.SnapSetter.Send(in)
}
func (b*Main) MapSet(in gMap) {
b.MapSetter.Send(in)
}
