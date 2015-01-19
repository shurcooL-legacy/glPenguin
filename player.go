package main

import (
	"math/rand"

	"github.com/go-gl/glow/gl/2.1/gl"
	glfw "github.com/shurcooL/glfw3"
)

var m_pPlayers []CPlayer

type CPlayer struct {
	m_nPosX     int32
	m_nPosY     int32
	m_nPosZ     int32
	m_nRotation int32 // 0 - up, 1 - right, 2 - down, 3 - left
	m_nState    int32
}

func NewCPlayer(w *CWorld) CPlayer {
	return CPlayer{
		m_nPosX:     rand.Int31n(w.m_nWorldX),
		m_nPosY:     rand.Int31n(w.m_nWorldY),
		m_nPosZ:     0,
		m_nRotation: 0,
		m_nState:    2, // standing
	}
}

func (CPlayer) Close() error { return nil }

func (p *CPlayer) Tick() {
	// If we're falling, move us down a cell or stop falling
	if p.m_nState == 1 {
		if globalWorld.TopFallingIceCube(p.m_nPosX, p.m_nPosY, p.m_nPosZ) != -1 {
			p.m_nPosZ = globalWorld.TopFallingIceCube(p.m_nPosX, p.m_nPosY, p.m_nPosZ) + 1
			p.m_nState = 1 // falling
		} else {
			p.m_nPosZ = globalWorld.TopIceCube(p.m_nPosX, p.m_nPosY) + 1
			p.m_nState = 2 // standing
		}
	}
}

func (p *CPlayer) ProcessKey(key glfw.Key) {
	var nVelX, nVelY int32

	// HACK?
	plusMinusOne := func(b bool) int32 {
		if b {
			return +1
		} else {
			return -1
		}
	}
	boolToInt := func(b bool) int32 {
		if b {
			return +1
		} else {
			return 0
		}
	}

	switch key {
	case glfw.KeyUp, glfw.KeyW:
		nVelX = boolToInt(p.m_nRotation == 1) - boolToInt(p.m_nRotation == 3)
		nVelY = boolToInt(p.m_nRotation == 0) - boolToInt(p.m_nRotation == 2)
	case glfw.KeyRight, glfw.KeyD:
		nVelX = boolToInt(p.m_nRotation == 0) - boolToInt(p.m_nRotation == 2)
		nVelY = boolToInt(p.m_nRotation == 3) - boolToInt(p.m_nRotation == 1)
	case glfw.KeyDown, glfw.KeyS:
		nVelX = boolToInt(p.m_nRotation == 3) - boolToInt(p.m_nRotation == 1)
		nVelY = boolToInt(p.m_nRotation == 2) - boolToInt(p.m_nRotation == 0)
	case glfw.KeyLeft, glfw.KeyA:
		nVelX = boolToInt(p.m_nRotation == 2) - boolToInt(p.m_nRotation == 0)
		nVelY = boolToInt(p.m_nRotation == 1) - boolToInt(p.m_nRotation == 3)
	case glfw.KeyPageUp: //case glfw.KeyKP_9:
		nVelX = plusMinusOne(p.m_nRotation == 0 || p.m_nRotation == 1)
		nVelY = plusMinusOne(p.m_nRotation == 0 || p.m_nRotation == 3)
	case glfw.KeyPageDown: //case glfw.KeyKP_3:
		nVelX = plusMinusOne(p.m_nRotation == 0 || p.m_nRotation == 3)
		nVelY = plusMinusOne(p.m_nRotation == 2 || p.m_nRotation == 3)
	case glfw.KeyEnd: //case glfw.KeyKP_1:
		nVelX = plusMinusOne(p.m_nRotation == 0 || p.m_nRotation == 1) * -1
		nVelY = plusMinusOne(p.m_nRotation == 0 || p.m_nRotation == 3) * -1
	case glfw.KeyHome: //case glfw.KeyKP_7:
		nVelX = plusMinusOne(p.m_nRotation == 0 || p.m_nRotation == 3) * -1
		nVelY = plusMinusOne(p.m_nRotation == 2 || p.m_nRotation == 3) * -1
	default:
	}

	if nVelX != 0 || nVelY != 0 {
		nTIC := globalWorld.TopIceCube(p.m_nPosX+nVelX, p.m_nPosY+nVelY)
		nTFIC := globalWorld.TopFallingIceCube(p.m_nPosX+nVelX, p.m_nPosY+nVelY, p.m_nPosZ+boolToInt(p.m_nState == 1))

		if nTIC == p.m_nPosZ && globalWorld.GetIceCube(p.m_nPosX+nVelX, p.m_nPosY+nVelY, nTIC+1) != 1 &&
			globalWorld.GetIceCube(p.m_nPosX, p.m_nPosY, p.m_nPosZ+1) == 0 {
			// nothing blocking our way, so let's climb that ice cube

			p.m_nPosZ++
			p.m_nPosX += nVelX
			p.m_nPosY += nVelY
			p.m_nState = 2 // standing

			// win?
			if p.m_nPosZ >= globalWorld.m_nWinHeight {
				p.Win()
			}
		} else if nTIC < p.m_nPosZ && nTFIC == -1 &&
			globalWorld.GetIceCube(p.m_nPosX+nVelX, p.m_nPosY+nVelY, p.m_nPosZ) == 0 {
			// no falling cubes in the way, just quickly jump down

			p.m_nPosZ = nTIC + 1
			p.m_nPosX += nVelX
			p.m_nPosY += nVelY
			p.m_nState = 2 // standing
		} else if nTIC < p.m_nPosZ && nTFIC != -1 &&
			(globalWorld.GetIceCube(p.m_nPosX+nVelX, p.m_nPosY+nVelY, p.m_nPosZ) == 0 ||
				(globalWorld.GetIceCube(p.m_nPosX+nVelX, p.m_nPosY+nVelY, p.m_nPosZ) == 1 &&
					globalWorld.GetIceCube(p.m_nPosX, p.m_nPosY, p.m_nPosZ+1) == 0)) {
			// ride an ice cube down

			p.m_nPosZ = nTFIC + 1
			p.m_nPosX += nVelX
			p.m_nPosY += nVelY
			p.m_nState = 1 // falling

			// win?
			if p.m_nPosZ >= globalWorld.m_nWinHeight {
				p.Win()
			}
		}
	}
}

func (p *CPlayer) Render() {
	gl.PushMatrix()

	switch p.m_nState {
	case 1: // falling
		gl.Translatef(float32(p.m_nPosX)+0.5, float32(p.m_nPosY)+0.5, (1.0-globalWorld.GetTickProcess())+float32(p.m_nPosZ))
	case 2: // standing
		gl.Translatef(float32(p.m_nPosX)+0.5, float32(p.m_nPosY)+0.5, float32(p.m_nPosZ))
	default:
	}
	gl.Rotatef(float32(p.m_nRotation)*90.0, 0.0, 0.0, -1.0)

	gl.Color3f(1.0, 0.0, 0.0)
	gl.Begin(gl.LINES)
	{
		gl.Vertex3f(0.0, 0.0, 0.5)
		gl.Vertex3f(0.0, 0.5, 0.5)
		gl.Vertex3f(-0.5, 0.0, 0.5)
		gl.Vertex3f(0.5, 0.0, 0.5)
		gl.Vertex3f(0.0, 0.0, 0.0)
		gl.Vertex3f(0.0, 0.0, 1.0)
	}
	gl.End()

	gl.PopMatrix()
}

func (p *CPlayer) Win() {
	g_bPaused = true

	globalWindow.SetTitle("glPenguin v0.0 - you win!")
}

func (p *CPlayer) Die() {
	g_bPaused = true

	globalWindow.SetTitle("glPenguin v0.0 - you got crushed by an ice cube...")
}

func (p *CPlayer) GetCrushedAt(nPosX, nPosY, nPosZ int32) {
	if nPosZ == p.m_nPosZ && nPosX == p.m_nPosX && nPosY == p.m_nPosY {
		p.Die()
	}
}

func CreatePlayers(nNumberPlayers int) {
	// create new players
	m_pPlayers = make([]CPlayer, nNumberPlayers)
	for nLoop1 := range m_pPlayers {
		m_pPlayers[nLoop1] = NewCPlayer(globalWorld)
	}
}

func TickAtAll(nPosZ int32) {
	for nLoop1 := range m_pPlayers {
		if m_pPlayers[nLoop1].m_nPosZ == nPosZ {
			m_pPlayers[nLoop1].Tick()
		}
	}
}

func ProcessKeyAll(key glfw.Key) {
	for nLoop1 := range m_pPlayers {
		m_pPlayers[nLoop1].ProcessKey(key)
	}
}

func RenderAll() {
	for nLoop1 := range m_pPlayers {
		m_pPlayers[nLoop1].Render()
	}
}

func GetCrushedAtAll(nPosX, nPosY, nPosZ int32) {
	for nLoop1 := range m_pPlayers {
		m_pPlayers[nLoop1].GetCrushedAt(nPosX, nPosY, nPosZ)
	}
}
