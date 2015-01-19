package main

import (
	"math"

	"github.com/go-gl/glow/gl/2.1/gl"
)

const (
	MAX_CAMERA_ANGLE = 75
	//MAX_CAMERA_ANGLE = 120
	MAX_CAMERA_ZOOM     = 50
	MIN_CAMERA_ZOOM     = 1000
	DEFAULT_CAMERA_ZOOM = 200

	mouseSensitivity = 0.3
)

type eCameraStyle uint8

const (
	CS_TILTING eCameraStyle = iota
	CS_SPINNING
)

type CCamera struct {
	m_oCameraStyle eCameraStyle
	m_fCameraRotX  float32
	m_fCameraRotY  float32
	m_fCameraRotZ  float32
	m_fCameraZoom  float32
	m_bWireframe   bool
}

func NewCCamera() CCamera {
	return CCamera{
		// TODO: read from config file
		m_oCameraStyle: CS_TILTING,
		//m_oCameraStyle: CS_SPINNING,

		m_fCameraRotX: 0,
		m_fCameraRotY: 0,
		m_fCameraRotZ: 0,
		m_fCameraZoom: DEFAULT_CAMERA_ZOOM,

		m_bWireframe: false,
	}
}

func (CCamera) Close() error { return nil }

func (c *CCamera) ProcessMouse(oMouseMoved [2]int) {
	switch c.m_oCameraStyle {
	case CS_TILTING:
		c.m_fCameraRotX += float32(oMouseMoved[1]) * mouseSensitivity //CWindow::Get().GetMouseSensitivity();
		c.m_fCameraRotY += float32(oMouseMoved[0]) * mouseSensitivity //CWindow::Get().GetMouseSensitivity();

		if c.m_fCameraRotX > MAX_CAMERA_ANGLE {
			c.m_fCameraRotX = MAX_CAMERA_ANGLE
		} else if c.m_fCameraRotX < -MAX_CAMERA_ANGLE {
			c.m_fCameraRotX = -MAX_CAMERA_ANGLE
		}

		if c.m_fCameraRotY > MAX_CAMERA_ANGLE {
			c.m_fCameraRotY = MAX_CAMERA_ANGLE
		} else if c.m_fCameraRotY < -MAX_CAMERA_ANGLE {
			c.m_fCameraRotY = -MAX_CAMERA_ANGLE
		}
	case CS_SPINNING:
		c.m_fCameraRotZ += float32(oMouseMoved[0]) * mouseSensitivity //CWindow::Get().GetMouseSensitivity();
		c.m_fCameraRotX += float32(oMouseMoved[1]) * mouseSensitivity //CWindow::Get().GetMouseSensitivity();

		if c.m_fCameraRotX > 0 {
			c.m_fCameraRotX = 0
		} else if c.m_fCameraRotX < -MAX_CAMERA_ANGLE {
			c.m_fCameraRotX = -MAX_CAMERA_ANGLE
		}

		if c.m_fCameraRotZ >= 360 {
			c.m_fCameraRotZ -= 360
		} else if c.m_fCameraRotZ < 0 {
			c.m_fCameraRotZ += 360
		}

		// TODO: temporary hack
		if !g_bPaused {
			m_pPlayers[0].m_nRotation = int32(math.Floor((float64(c.m_fCameraRotZ)+45.0)/90.0)) % 4
		}
		//g_fFPS = CPlayer::Get(0).GetRotation();
	default:
	}
}

func (c *CCamera) Zoom(fAmount float32) {
	c.SetZoom(c.m_fCameraZoom - fAmount)
}

func (c *CCamera) SetZoom(fAmount float32) {
	c.m_fCameraZoom = fAmount

	if c.m_fCameraZoom < MAX_CAMERA_ZOOM {
		c.m_fCameraZoom = MAX_CAMERA_ZOOM
	} else if c.m_fCameraZoom > MIN_CAMERA_ZOOM {
		c.m_fCameraZoom = MIN_CAMERA_ZOOM
	}
}

func (c *CCamera) Translate() {
	gl.Translatef(0, 0, -c.m_fCameraZoom)

	switch c.m_oCameraStyle {
	case CS_TILTING:
		gl.Rotatef(c.m_fCameraRotX, 1, 0, 0)
		gl.Rotatef(c.m_fCameraRotY, 0, 1, 0)
	case CS_SPINNING:
		gl.Rotatef(c.m_fCameraRotX, 1, 0, 0)
		gl.Rotatef(c.m_fCameraRotZ, 0, 0, 1)
		//gl.Rotatef(-60.0, 1, 0, 0);
		//gl.Rotatef(45.0, 0, 0, 1);
	}
}

func (c *CCamera) ChangeStyle() {
	c.m_fCameraRotX = 0
	c.m_fCameraRotY = 0
	c.m_fCameraRotZ = 0

	// TODO: temporary hack
	if !g_bPaused {
		m_pPlayers[0].m_nRotation = 0
	}

	switch c.m_oCameraStyle {
	case CS_TILTING:
		c.m_oCameraStyle = CS_SPINNING
	case CS_SPINNING:
		c.m_oCameraStyle = CS_TILTING

		// TODO: figure this out
		//m_pOwner->SetRotation(0);
	default:
	}
}

func (c *CCamera) GetStyle() eCameraStyle { return c.m_oCameraStyle }

func (c *CCamera) ToggleWireframe() {
	c.m_bWireframe = !c.m_bWireframe

	if !c.m_bWireframe {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.FILL)
	} else {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)
	}
}
