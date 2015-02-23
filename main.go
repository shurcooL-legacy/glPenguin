package main

import (
	"fmt"
	"log"
	"math/rand"
	"runtime"
	"time"

	"github.com/BurntSushi/toml"
	"github.com/go-gl/gl/v2.1/gl"
	"github.com/go-gl/glfw/v3.1/glfw"
	"github.com/go-gl/mathgl/mgl64"
)

var g_oWindowSize [2]int
var g_bPaused bool = false

var globalWorld *CWorld
var globalCamera CCamera
var globalWindow *glfw.Window
var globalTimer CTimer

func InitOpenGL() {
	// set clear colour
	gl.ClearColor(0.0, 0.0, 0.0, 1.0)

	// perspective correction hint
	gl.Hint(gl.PERSPECTIVE_CORRECTION_HINT, gl.NICEST)

	// front face = CW
	gl.FrontFace(gl.CW)
	gl.CullFace(gl.BACK)
	gl.Enable(gl.CULL_FACE)

	// enable depth testing
	gl.Enable(gl.DEPTH_TEST)
}

func ProcessWindowResize(w *glfw.Window, nWidth, nHeight int) {
	if nHeight <= 0 {
		nHeight = 1
	}

	g_oWindowSize[0] = nWidth
	g_oWindowSize[1] = nHeight

	gl.Viewport(0, 0, int32(g_oWindowSize[0]), int32(g_oWindowSize[1]))

	gl.MatrixMode(gl.PROJECTION)
	gl.LoadIdentity()

	var projectionMatrix [16]float64
	perspMatrix := mgl64.Perspective(mgl64.DegToRad(45), float64(g_oWindowSize[0])/float64(g_oWindowSize[1]), 1.0, 1500.0)
	for i := 0; i < 16; i++ {
		projectionMatrix[i] = float64(perspMatrix[i])
	}
	gl.MultMatrixd(&projectionMatrix[0])

	gl.MatrixMode(gl.MODELVIEW)
	gl.LoadIdentity()
}

func ProcessKey(w *glfw.Window, key glfw.Key, scancode int, action glfw.Action, mods glfw.ModifierKey) {
	if action == glfw.Press || action == glfw.Repeat {
		switch key {
		case glfw.KeyC:
			globalCamera.ChangeStyle()
		case glfw.KeyF2:
			globalCamera.ToggleWireframe()
		case glfw.KeyP:
			g_bPaused = !g_bPaused

			if g_bPaused {
				globalWindow.SetTitle("glPenguin v0.0 - paused")
			} else {
				globalWindow.SetTitle("glPenguin v0.0")
			}
		default:
			if !g_bPaused {
				ProcessKeyAll(key)
			}
		}
	}
}

func ProcessScroll(w *glfw.Window, xoff float64, yoff float64) {
	// change camera zoom
	globalCamera.Zoom(float32(yoff) * 25)
}

func init() {
	runtime.LockOSThread()
	runtime.GOMAXPROCS(runtime.NumCPU())
}

func main() {
	var fFps, fFpsTime float32
	var nFpsFrames int32

	// Read config file
	if _, err := toml.DecodeFile("Config.cfg", &config); err != nil {
		log.Fatalln(err)

		fmt.Println("Configuration file 'Config.cfg' was not found in the root directory.\n\nUsing default values instead.")
	}

	if config.General.ShowIntro {
		fmt.Println("Welcome to glPenguin v0.0!\n\nYour objective is to get to higher ground.\nIf you'd like to change the world size or game rules, please edit the 'Config.cfg' file in Notepad.\n\nTo move, use the arrow keys.\n\n'P' pauses/unpauses the game. 'F2' toggles wireframe mode. 'C' cycles between two camera modes.\nUse the mouse to move the camera. If the game stops for some reason, read what it says in the title bar (only in windowed mode),\nas it's most likely you've either won or died.\n\nGood luck, and hope you enjoy this pre-release tech demo, while I work on some bug fixes, and soon enough, on actually *finishing* the game! :)")
	}

	if err := glfw.Init(); err != nil {
		log.Panicln("glfw.Init():", err)
	}
	defer glfw.Terminate()

	window, err := glfw.CreateWindow(int(config.Video.ResolutionX), int(config.Video.ResolutionY), "glPenguin v0.0", nil, nil)
	if err != nil {
		panic(err)
	}
	globalWindow = window
	window.MakeContextCurrent()

	//glfw.Enable(glfw.StickyKeys) // When GLFW_STICKY_KEYS is enabled, keys which are pressed will not be released until they are physically released and checked with glfwGetKey

	if err := gl.Init(); nil != err {
		log.Print(err)
	}

	glfw.SwapInterval(1) // Vsync

	/*framebufferSizeCallback := func(w *glfw.Window, framebufferSize0, framebufferSize1 int) {
		gl.Viewport(0, 0, int32(framebufferSize0), int32(framebufferSize1))

		g_oWindowSize[0], g_oWindowSize[1], _ = w.GetSize()
	}*/
	{
		var framebufferSize [2]int
		framebufferSize[0], framebufferSize[1] = window.GetFramebufferSize()
		ProcessWindowResize(window, framebufferSize[0], framebufferSize[1])
	}
	window.SetFramebufferSizeCallback(ProcessWindowResize)

	window.SetKeyCallback(ProcessKey)
	window.SetScrollCallback(ProcessScroll)
	window.SetInputMode(glfw.CursorMode, glfw.CursorDisabled)

	InitOpenGL()

	rand.Seed(time.Now().UnixNano())

	// create the world
	var world CWorld = NewCWorld(config.World.WorldX,
		config.World.WorldY,
		config.World.WinHeight+ICE_CUBE_SPAWN_HEIGHT,
		config.World.WinHeight,
		config.World.WorldTickTime,
		config.World.IceCubeRespawnRate)
	globalWorld = &world // HACK

	// TODO: debug pre-set falling ice cubes, to test movement code
	if config.General.TestFallingStairs {
		world.SetIceCube(0, 0, 13, 1)
		world.SetIceCube(1, 0, 12, 1)
		world.SetIceCube(1, 1, 12, 1)
		world.SetIceCube(0, 1, 13, 1)
		world.SetIceCube(1, 0, 14, 1)
		world.SetIceCube(2, 0, 11, 1)
		world.SetIceCube(3, 0, 10, 1)
		world.SetIceCube(4, 0, 9, 1)

		world.SetIceCube(5, 0, 1, 2)
		world.SetIceCube(5, 0, 0, 2)
		world.SetIceCube(6, 0, 0, 2)
	}

	CreatePlayers(1)
	globalCamera = NewCCamera()

	//glfwSetMousePos((g_oWindowSize * 0.5).nX, (g_oWindowSize * 0.5).nY);

	// Init the timer
	// HACK.
	globalTimer.Update()
	globalTimer.Update()
	globalTimer.Update()

	var lastMousePos mgl64.Vec2
	lastMousePos[0], lastMousePos[1] = window.GetCursorPos()

	for !window.ShouldClose() {
		glfw.PollEvents()

		// clear opengl buffers, reset the world matrix
		gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
		gl.LoadIdentity()

		// time calcs
		globalTimer.Update()

		// fps calcs
		nFpsFrames++
		fFpsTime += globalTimer.GetTimePassed()
		if fFpsTime >= 0.5 {
			fFps = float32(nFpsFrames) / fFpsTime

			if !g_bPaused {
				window.SetTitle(fmt.Sprintf("glPenguin v0.0 - %v fps", fFps))
			}

			nFpsFrames = 0
			fFpsTime = 0
		}

		// mouse calcs
		var mousePos mgl64.Vec2
		mousePos[0], mousePos[1] = window.GetCursorPos()
		var oMouseMoved [2]int
		oMouseMoved[0], oMouseMoved[1] = int(mousePos[0]-lastMousePos[0]), int(mousePos[1]-lastMousePos[1])
		lastMousePos = mousePos

		// camera position & angle calcs
		globalCamera.ProcessMouse(oMouseMoved)

		// opengl world matrix translation
		globalCamera.Translate()

		// Scale up the scene
		gl.Scalef(10.0, 10.0, 10.0)

		// physics
		if !g_bPaused {
			world.Tick()
		}

		// render the world
		world.Render()

		gl.Flush()
		window.SwapBuffers()

		//glfwSleep(0.25); glfwPollEvents();

		runtime.Gosched()
	}

	world.Close()
	globalCamera.Close()
}
