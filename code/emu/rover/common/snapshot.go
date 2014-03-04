package common

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
}

const (
	ArmUnitsPerTile = 10
	
	CourseSize = 100
)
type Map struct {
	Course [CourseSize][CourseSize]bool
}