package gui

import (
	"../common"
	"github.com/Carrotman42/kgui"
)

type Gui struct {
	window Main
}

type gSnap struct {
	common.RoverSnapshot
}

type gMap struct {
	common.Map
}


func (g*Gui) Update(sn common.RoverSnapshot) {
	g.window.SnapSet(gSnap{sn})
}
func (g*Gui) SetMap(m common.Map) {
	g.window.MapSet(gMap{m})
}

func MakeGui() *Gui {
	kgui.WaitForInit()
	ret := &Gui{}
	ret.window.Init()
	kgui.SetView(&ret.window)
	
	ret.window.SnapSet(gSnap{})
	return ret
}




