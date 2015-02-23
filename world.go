package main

import (
	"math/rand"

	"github.com/go-gl/gl/v2.1/gl"
)

const ICE_CUBE_SPAWN_HEIGHT = 10

type CWorld struct {
	m_nWorldX             int32
	m_nWorldY             int32
	m_nWorldZ             int32
	m_nWinHeight          int32
	m_fWorldTickTime      float32
	m_fIceCubeRespawnRate float32

	m_pWorld []int16

	m_fNextTick    float32
	m_fNextIceCube float32
}

func NewCWorld(nWorldX, nWorldY, nWorldZ, nWinHeight int32, fWorldTickTime, fIceCubeRespawnRate float32) CWorld {
	return CWorld{
		m_nWorldX:    nWorldX,
		m_nWorldY:    nWorldY,
		m_nWorldZ:    nWorldZ,
		m_nWinHeight: nWinHeight,

		m_pWorld: make([]int16, nWorldX*nWorldY*nWorldZ),

		m_fWorldTickTime:      fWorldTickTime,
		m_fIceCubeRespawnRate: fIceCubeRespawnRate,

		m_fNextTick:    fWorldTickTime,
		m_fNextIceCube: fIceCubeRespawnRate,
	}
}

func (CWorld) Close() error { return nil }

func (w *CWorld) Tick() {
	w.m_fNextTick -= globalTimer.GetTimePassed()
	for w.m_fNextTick <= 0 {
		for nLoop1 := int32(0); nLoop1 < w.m_nWorldZ; nLoop1++ {
			// Do player ticks to make sure he won't get crushed by a following ice cube
			TickAtAll(nLoop1)

			for nLoop2 := int32(0); nLoop2 < w.m_nWorldX; nLoop2++ {
				for nLoop3 := int32(0); nLoop3 < w.m_nWorldY; nLoop3++ {
					if w.GetIceCube(nLoop2, nLoop3, nLoop1) == 1 {
						if nLoop1-1 < 0 || w.GetIceCube(nLoop2, nLoop3, nLoop1-1) != 0 {
							w.SetIceCube(nLoop2, nLoop3, nLoop1, 2) // standing
						} else {
							w.SetIceCube(nLoop2, nLoop3, nLoop1, 0)
							w.SetIceCube(nLoop2, nLoop3, nLoop1-1, 1) // falling

							GetCrushedAtAll(nLoop2, nLoop3, nLoop1-1)
						}
					}
				}
			}
		}

		w.m_fNextIceCube--
		for w.m_fNextIceCube <= 0 {
			var oNextIceCubePos [2]int32 // next ice cube spawn position

			for {
				// think of a random ice cube spawn position
				oNextIceCubePos[0] = rand.Int31n(w.m_nWorldX)
				oNextIceCubePos[1] = rand.Int31n(w.m_nWorldY)

				// make sure that the next ice cube won't appear in the same place
				if !(w.GetIceCube(oNextIceCubePos[0], oNextIceCubePos[1], w.m_nWorldZ-1) != 0) {
					break
				}
			}

			// spawn the ice cube
			w.SetIceCube(oNextIceCubePos[0], oNextIceCubePos[1], w.m_nWorldZ-1, 1)

			w.m_fNextIceCube += w.m_fIceCubeRespawnRate
		}

		w.m_fNextTick += w.m_fWorldTickTime
	}
}

func (w *CWorld) GetTickProcess() float32 {
	return (1.0 - w.m_fNextTick/w.m_fWorldTickTime)
}

func (w *CWorld) Render() {
	gl.Translatef(float32(w.m_nWorldX)*-0.5, float32(w.m_nWorldY)*-0.5, 0.0)

	w.renderBase()

	for nLoop1 := int32(0); nLoop1 < w.m_nWorldZ; nLoop1++ {
		for nLoop2 := int32(0); nLoop2 < w.m_nWorldX; nLoop2++ {
			for nLoop3 := int32(0); nLoop3 < w.m_nWorldY; nLoop3++ {
				w.renderIceCube(nLoop2, nLoop3, nLoop1, w.GetIceCube(nLoop2, nLoop3, nLoop1))
			}
		}
	}

	// render players
	RenderAll()
}

func (w *CWorld) renderBase() {
	gl.DepthMask(false)
	gl.Color3f(1.0, 1.0, 1.0)
	gl.Begin(gl.QUADS)
	{
		gl.Vertex2f(0.0, 0.0)
		gl.Vertex2f(0.0, float32(w.m_nWorldY))
		gl.Vertex2f(float32(w.m_nWorldX), float32(w.m_nWorldY))
		gl.Vertex2f(float32(w.m_nWorldX), 0.0)
	}
	gl.End()
	gl.DepthMask(true)

	// 3D grid
	/*glColor3f(0.0, 0.0, 1.0);
	for (nLoop1 = int32(0); nLoop1 <= m_nWorldZ; nLoop1++) {
		for (nLoop2 = int32(1); nLoop2 < m_nWorldX; nLoop2++) {
			glBegin(gl.LINES);
				glVertex3f(0, nLoop2, nLoop1);
				glVertex3f(m_nWorldX, nLoop2, nLoop1);
				glVertex3f(nLoop2, 0, nLoop1);
				glVertex3f(nLoop2, m_nWorldY, nLoop1);
			glEnd();
		}
	}*/
}

func (w *CWorld) renderIceCube(nPosX, nPosY, nPosZ int32, nState int16) {
	if nState == 0 {
		return
	}

	gl.PushMatrix()

	switch nState {
	case 1: // falling
		gl.Translatef(float32(nPosX), float32(nPosY), (1.0-w.GetTickProcess())+float32(nPosZ))
	case 2: // lying still
		gl.Translatef(float32(nPosX), float32(nPosY), float32(nPosZ))
	default:
	}

	gl.Begin(gl.QUADS)
	{
		// bottom
		gl.Color3f(0.0, 1.0, 0.0)
		gl.Vertex3i(0, 1, 0)
		gl.Vertex3i(0, 0, 0)
		gl.Vertex3i(1, 0, 0)
		gl.Vertex3i(1, 1, 0)

		// front
		gl.Color3f(0.0, 1.0, 1.0)
		gl.Vertex3i(0, 0, 1)
		gl.Vertex3i(1, 0, 1)
		gl.Vertex3i(1, 0, 0)
		gl.Vertex3i(0, 0, 0)

		// left
		gl.Color3f(1.0, 1.0, 0.0)
		gl.Vertex3i(0, 1, 1)
		gl.Vertex3i(0, 0, 1)
		gl.Vertex3i(0, 0, 0)
		gl.Vertex3i(0, 1, 0)

		// back
		gl.Color3f(0.0, 1.0, 1.0)
		gl.Vertex3i(0, 1, 1)
		gl.Vertex3i(0, 1, 0)
		gl.Vertex3i(1, 1, 0)
		gl.Vertex3i(1, 1, 1)

		// right
		gl.Color3f(1.0, 1.0, 0.0)
		gl.Vertex3i(1, 1, 1)
		gl.Vertex3i(1, 1, 0)
		gl.Vertex3i(1, 0, 0)
		gl.Vertex3i(1, 0, 1)

		// top
		gl.Color3f(0.0, 0.0, 1.0)
		gl.Vertex3i(0, 1, 1)
		gl.Vertex3i(1, 1, 1)
		gl.Vertex3i(1, 0, 1)
		gl.Vertex3i(0, 0, 1)
	}
	gl.End()

	gl.PopMatrix()
}

func (w *CWorld) GetIceCube(nPosX, nPosY, nPosZ int32) int16 {
	if nPosZ < 0 {
		return 3 // out of boundaries
	} else {
		return w.m_pWorld[(nPosZ*(w.m_nWorldX*w.m_nWorldY))+(nPosY*(w.m_nWorldX))+nPosX]
	}
}

func (w *CWorld) SetIceCube(nPosX, nPosY, nPosZ int32, nValue int16) {
	w.m_pWorld[(nPosZ*(w.m_nWorldX*w.m_nWorldY))+(nPosY*(w.m_nWorldX))+nPosX] = nValue
}

func (w *CWorld) TopIceCube(nPosX, nPosY int32) int32 {
	if nPosX < 0 || nPosX >= w.m_nWorldX ||
		nPosY < 0 || nPosY >= w.m_nWorldY {
		return w.m_nWorldZ
	}

	var nPosZ int32

	for ; nPosZ < w.m_nWorldZ; nPosZ++ {
		if w.GetIceCube(nPosX, nPosY, nPosZ) != 2 {
			return nPosZ - 1
		}
	}

	return nPosZ
}

func (w *CWorld) TopFallingIceCube(nPosX, nPosY, nPosZ int32) int32 {
	for nPosZ--; nPosZ >= 0; nPosZ-- {
		if w.GetIceCube(nPosX, nPosY, nPosZ) == 1 {
			return nPosZ
		} else if w.GetIceCube(nPosX, nPosY, nPosZ) == 2 {
			return -1 // We found a 'standing' ice cube on the way down, meaning no falling cubes found
		}
	}

	return -1 // No falling ice cubes underneath
}
