#pragma once

#ifndef __CCamera_H__
#define __CCamera_H__

#include "Globals.h"

#define MAX_CAMERA_ANGLE	75
//#define MAX_CAMERA_ANGLE	120
#define MAX_CAMERA_ZOOM		50
#define MIN_CAMERA_ZOOM		1000
#define DEFAULT_CAMERA_ZOOM		200

//class CPlayer;		// predefine it

class CCamera
{
public:
	enum eCameraStyle { CS_TILTING, CS_SPINNING };

	CCamera();
	~CCamera();

	void ProcessMouse(tPoint2 oMouseMoved);
	void Zoom(float fAmount);
	void SetZoom(float fAmount);
	void Translate();
	void ChangeStyle();
	eCameraStyle GetStyle();
	void ToggleWireframe();

	static CCamera& Get();
	static void Destroy();

private:
	eCameraStyle	m_oCameraStyle;
	float	m_fCameraRotX;
	float	m_fCameraRotY;
	float	m_fCameraRotZ;
	float	m_fCameraZoom;
	bool	m_bWireframe;

	static CCamera*		m_pSingleton;
};

#endif // __CCamera_H__