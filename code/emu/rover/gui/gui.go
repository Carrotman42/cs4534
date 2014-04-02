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

const ArmUnitsPerTile = float64(common.ArmUnitsPerTile)
const CourseSize = float64(common.CourseSize)

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
	
	return ret
}

func p(x,y float64) kgui.Point {
	return kgui.Point{x,y}
}
func ps(xy...float64) []kgui.Point {
	l := len(xy)/2
	ret := make([]kgui.Point, l)
	for i,p := 0, 0; i < l; i, p = i+1, p+2 {
		ret[i] = kgui.Point{xy[p], xy[p+1]}
	}
	return ret
}




