package common

import "math"

type RoverSnapshot struct {
	FrameData
	
	// In ArmUnits
	X, Y float64
	
	// In degrees
	Dir int
	
	// In ArmUnits/ArmTime
	Vel float64
	
	// In SendFrames mode
	SendFrames bool
}

func (m*Map) hLine(l, r, y int) {
	for i := l; i < r; i++ {
		if m.Course[i][y] {
			m.Course[i][y] = false
		} else {
			m.Course[i][y] = true
		}
	}
}
func (m*Map) vLine(t, b, x int) {
	for i := t; i < b; i++ {
		if m.Course[x][i] {
			m.Course[x][i] = false
		} else {
			m.Course[x][i] = true
		}
	}
}
func (m*Map) box(t, l, b, r int) {
	m.vLine(t, b, l)
	m.vLine(t, b, r)
	m.hLine(l, r, t)
	m.hLine(l, r, b)
}

func (m*Map) Init() {
	for i := 5; i < (CourseSize - 5); i++ {
		m.Course[i][5] = true
		m.Course[i][CourseSize - 5] = true
		m.Course[5][i] = true
		m.Course[CourseSize - 5][i] = true
	}
	
	//m.box(50, CourseSize-40, 70, CourseSize-10)
}

const (
	ArmUnitsPerTile = 10
	
	CourseSize = 48
)
type Map struct {
	Course [CourseSize][CourseSize]bool
}
// X and Y are in armunits
func (m*Map) DistToWall(x, y, dir int) (int, bool) {
	sin, cos := math.Sincos(float64(dir) / 180 * math.Pi)
	x /= ArmUnitsPerTile
	y /= ArmUnitsPerTile
	
	for i := 0; ; i++ {
		q,w := multAdd(x, i, cos), multAdd(y, i, sin)
		if q < 0 || q >= CourseSize || w < 0 || w >= CourseSize {
			return 0, false
		} else if m.Course[q][w] {
			return i*ArmUnitsPerTile + (ArmUnitsPerTile - (x%ArmUnitsPerTile)), true
		}
	}
}

func(m*Map) Outside(x, y, dir int) bool {
	_, ok := m.DistToWall(x, y, dir)
	return !ok
}


func multAdd(x, i int, v float64) int {
	return x + int(float64(i) * v)
}