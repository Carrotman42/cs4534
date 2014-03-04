package common

import "math"

type RoverSnapshot struct {
	FrameData
	
	// In ArmUnits
	X, Y int
	
	// In degrees
	Dir int
	
	// In ArmUnits/ArmTime
	Vel int
	
	// In SendFrames mode
	SendFrames bool
}


func (m*Map) Init() {
	for i := 10; i < (CourseSize - 10); i++ {
		m.Course[i][10] = true
		m.Course[i][CourseSize - 10] = true
		m.Course[10][i] = true
		m.Course[CourseSize - 10][i] = true
	}
	
	for i := 10; i < 30; i++ {
		m.Course[i][20] = true
	}
}

const (
	ArmUnitsPerTile = 10
	
	CourseSize = 100
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
			return i*ArmUnitsPerTile, true
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