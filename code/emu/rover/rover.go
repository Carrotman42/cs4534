package main

import (
	"sync"
	"time"
)

type Rover struct {
	sync.Mutex
	Protocol
	
	// This value is automatically updated any time any of the more precise values have changed, eg X/Y
	curFrame FrameData
	sendingFrames bool
}

func NewRover(p Protocol) *Rover {
	ret := &Rover{}
	ret.Protocol = p
	
	ret.StartLoops()
	return ret
}

func (r*Rover) StartLoops() {
	go r.FrameLoop()
	go r.Loop()
}

func (r*Rover) FrameLoop() {
	for {
		time.Sleep(time.Second)
		
		r.Lock()
		send := r.sendingFrames
		if send {
			r.WriteFrameData(r.curFrame)
		}
		r.Unlock()
	}
}

func (r*Rover) HandleFrameCmd(start bool) {
	r.Lock()
	r.sendingFrames = start
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