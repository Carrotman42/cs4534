package common

type RoverSnapshot struct {
	FrameData
	
	// In ArmUnits
	X, Y int
	
	// In SendFrames mode
	SendFrames bool
}


func CreateMap() (m Map) { 
	for i := 10; i < (CourseSize - 10); i++ {
		m.Course[i][10] = true
		m.Course[i][CourseSize - 10] = true
		m.Course[10][i] = true
		m.Course[CourseSize - 10][i] = true
	}
	
	return m
}


const (
	ArmUnitsPerTile = 10
	
	CourseSize = 100
)
type Map struct {
	Course [CourseSize][CourseSize]bool
}