#include "CCamera.h"

#include "CPlayer.h"

CCamera*	CCamera::m_pSingleton = NULL;

CCamera::CCamera()
{
	// TODO: read from config file
	//m_oCameraStyle = CS_TILTING;
	m_oCameraStyle = CS_SPINNING;

	m_fCameraRotX = 0;
	m_fCameraRotY = 0;
	m_fCameraRotZ = 0;
	m_fCameraZoom = DEFAULT_CAMERA_ZOOM;

	m_bWireframe = false;
}

CCamera::~CCamera() {}

void CCamera::ProcessMouse(tPoint2 oMouseMoved)
{
	switch (m_oCameraStyle) {
	case CS_TILTING:
		m_fCameraRotX += oMouseMoved.nY * 0.3f; //CWindow::Get().GetMouseSensitivity();
		m_fCameraRotY += oMouseMoved.nX * 0.3f; //CWindow::Get().GetMouseSensitivity();

		if (m_fCameraRotX > MAX_CAMERA_ANGLE) m_fCameraRotX = MAX_CAMERA_ANGLE;
		else if (m_fCameraRotX < -MAX_CAMERA_ANGLE) m_fCameraRotX = -MAX_CAMERA_ANGLE;

		if (m_fCameraRotY > MAX_CAMERA_ANGLE) m_fCameraRotY = MAX_CAMERA_ANGLE;
		else if (m_fCameraRotY < -MAX_CAMERA_ANGLE) m_fCameraRotY = -MAX_CAMERA_ANGLE;

		break;
	case CS_SPINNING:
		m_fCameraRotZ += oMouseMoved.nX * 0.3f; //CWindow::Get().GetMouseSensitivity();
		m_fCameraRotX += oMouseMoved.nY * 0.3f; //CWindow::Get().GetMouseSensitivity();

		if (m_fCameraRotX > 0) m_fCameraRotX = 0;
		else if (m_fCameraRotX < -MAX_CAMERA_ANGLE) m_fCameraRotX = -MAX_CAMERA_ANGLE;

		if (m_fCameraRotZ >= 360) m_fCameraRotZ -= 360;
		else if (m_fCameraRotZ < 0) m_fCameraRotZ += 360;

		// TODO: temporary hack
		if (!g_bPaused) CPlayer::Get(0).SetRotation(static_cast<int>(std::floor((m_fCameraRotZ + 45.0) / 90.0)) % 4);
		//g_fFPS = CPlayer::Get(0).GetRotation();

		break;
	default:
		break;
	}
}

void CCamera::Zoom(float fAmount)
{
	SetZoom(m_fCameraZoom - fAmount);
}

void CCamera::SetZoom(float fAmount)
{
	m_fCameraZoom = fAmount;

	if (m_fCameraZoom < MAX_CAMERA_ZOOM) m_fCameraZoom = MAX_CAMERA_ZOOM;
	else if (m_fCameraZoom > MIN_CAMERA_ZOOM) m_fCameraZoom = MIN_CAMERA_ZOOM;
}

void CCamera::Translate()
{
	glTranslatef(0, 0, -m_fCameraZoom);

	switch (m_oCameraStyle) {
	case CS_TILTING:
		glRotatef(m_fCameraRotX, 1, 0, 0);
		glRotatef(m_fCameraRotY, 0, 1, 0);

		break;
	case CS_SPINNING:
		glRotatef(m_fCameraRotX, 1, 0, 0);
		glRotatef(m_fCameraRotZ, 0, 0, 1);
		//glRotatef(-60.0, 1, 0, 0);
		//glRotatef(45.0, 0, 0, 1);

		break;
	}
}

void CCamera::ChangeStyle()
{
	m_fCameraRotX = 0;
	m_fCameraRotY = 0;
	m_fCameraRotZ = 0;

	// TODO: temporary hack
	if (!g_bPaused) CPlayer::Get(0).SetRotation(0);

	switch (m_oCameraStyle) {
	case CS_TILTING:
		m_oCameraStyle = CS_SPINNING;

		break;
	case CS_SPINNING:
		m_oCameraStyle = CS_TILTING;

		// TODO: figure this out
		//m_pOwner->SetRotation(0);

		break;
	default:
		break;
	}
}

CCamera::eCameraStyle CCamera::GetStyle() { return m_oCameraStyle; }

void CCamera::ToggleWireframe()
{
	m_bWireframe = !m_bWireframe;

	if (!m_bWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}

CCamera& CCamera::Get()
{
	if (m_pSingleton == NULL) {
		m_pSingleton = new CCamera();
	}

	return *m_pSingleton;
}

void CCamera::Destroy()
{
	if (m_pSingleton != NULL) {
		delete m_pSingleton;
		m_pSingleton = NULL;
	}
}