#include "CPlayer.h"

#include "CTimer.h"
#include "CCamera.h"
#include "CWorld.h"

int			CPlayer::m_nNumberPlayers = 0;
CPlayer**	CPlayer::m_pPlayers = NULL;

CPlayer::CPlayer()
{
	m_nPosX = rand() % CWorld::Get().GetWorldX();
	m_nPosY = rand() % CWorld::Get().GetWorldY();
	m_nPosZ = 0;
	m_nRotation = 0;
	m_nState = 2;		// standing
}

CPlayer::~CPlayer() {}

void CPlayer::Tick()
{
	// If we're falling, move us down a cell or stop falling
	if (m_nState == 1) {
		if (CWorld::Get().TopFallingIceCube(m_nPosX, m_nPosY, m_nPosZ) != -1) {
			m_nPosZ = CWorld::Get().TopFallingIceCube(m_nPosX, m_nPosY, m_nPosZ) + 1;
			m_nState = 1;		// falling
		} else {
			m_nPosZ = CWorld::Get().TopIceCube(m_nPosX, m_nPosY) + 1;
			m_nState = 2;		// standing
		}
	}
}

void CPlayer::ProcessKey(int nKey)
{
	int		nVelX = 0;
	int		nVelY = 0;

	switch (nKey) {
	case GLFW_KEY_UP:
		nVelX = (m_nRotation == 1) - (m_nRotation == 3);
		nVelY = (m_nRotation == 0) - (m_nRotation == 2);

		break;
	case GLFW_KEY_RIGHT:
		nVelX = (m_nRotation == 0) - (m_nRotation == 2);
		nVelY = (m_nRotation == 3) - (m_nRotation == 1);

		break;
	case GLFW_KEY_DOWN:
		nVelX = (m_nRotation == 3) - (m_nRotation == 1);
		nVelY = (m_nRotation == 2) - (m_nRotation == 0);

		break;
	case GLFW_KEY_LEFT:
		nVelX = (m_nRotation == 2) - (m_nRotation == 0);
		nVelY = (m_nRotation == 1) - (m_nRotation == 3);

		break;
	case GLFW_KEY_PAGEUP: //case GLFW_KEY_KP_9:
		nVelX = (m_nRotation == 0 || m_nRotation == 1) ? 1 : -1;
		nVelY = (m_nRotation == 0 || m_nRotation == 3) ? 1 : -1;

		break;
	case GLFW_KEY_PAGEDOWN: //case GLFW_KEY_KP_3:
		nVelX = (m_nRotation == 0 || m_nRotation == 3) ? 1 : -1;
		nVelY = (m_nRotation == 2 || m_nRotation == 3) ? 1 : -1;

		break;
	case GLFW_KEY_END: //case GLFW_KEY_KP_1:
		nVelX = (m_nRotation == 0 || m_nRotation == 1) ? -1 : 1;
		nVelY = (m_nRotation == 0 || m_nRotation == 3) ? -1 : 1;

		break;
	case GLFW_KEY_HOME: //case GLFW_KEY_KP_7:
		nVelX = (m_nRotation == 0 || m_nRotation == 3) ? -1 : 1;
		nVelY = (m_nRotation == 2 || m_nRotation == 3) ? -1 : 1;

		break;
	default:
		break;
	}

	if (nVelX || nVelY) {
		int		nTIC = CWorld::Get().TopIceCube(m_nPosX + nVelX, m_nPosY + nVelY);
		int		nTFIC = CWorld::Get().TopFallingIceCube(m_nPosX + nVelX, m_nPosY + nVelY, m_nPosZ + static_cast<int>(m_nState == 1));

		if (nTIC == m_nPosZ	&& CWorld::Get().GetIceCube(m_nPosX + nVelX, m_nPosY + nVelY, nTIC + 1) != 1
			&& CWorld::Get().GetIceCube(m_nPosX, m_nPosY, m_nPosZ + 1) == 0)
			// nothing blocking our way, so let's climb that ice cube
		{
			++m_nPosZ;
			m_nPosX += nVelX;
			m_nPosY += nVelY;
			m_nState = 2;		// standing

			// win?
			if (m_nPosZ >= CWorld::Get().GetWinHeight()) Win();
		} else if (nTIC < m_nPosZ && nTFIC == -1
				   && CWorld::Get().GetIceCube(m_nPosX + nVelX, m_nPosY + nVelY, m_nPosZ) == 0)
				   // no falling cubes in the way, just quickly jump down
		{
			m_nPosZ = nTIC + 1;
			m_nPosX += nVelX;
			m_nPosY += nVelY;
			m_nState = 2;		// standing
		} else if (nTIC < m_nPosZ && nTFIC != -1
				   && (CWorld::Get().GetIceCube(m_nPosX + nVelX, m_nPosY + nVelY, m_nPosZ) == 0
					   || (CWorld::Get().GetIceCube(m_nPosX + nVelX, m_nPosY + nVelY, m_nPosZ) == 1
						   && CWorld::Get().GetIceCube(m_nPosX, m_nPosY, m_nPosZ + 1) == 0)))
				   // ride an ice cube down
		{
			m_nPosZ = nTFIC + 1;
			m_nPosX += nVelX;
			m_nPosY += nVelY;
			m_nState = 1;		// falling

			// win?
			if (m_nPosZ >= CWorld::Get().GetWinHeight()) Win();
		}
	}
}

void CPlayer::Render()
{
	glPushMatrix();

	switch (m_nState) {
	case 1:		// falling
		glTranslatef(m_nPosX + 0.5, m_nPosY + 0.5, (1.0 - CWorld::Get().GetTickProcess()) + m_nPosZ);

		break;
	case 2:		// standing
		glTranslatef(m_nPosX + 0.5, m_nPosY + 0.5, m_nPosZ);

		break;
	default:
		break;
	}
	glRotatef(m_nRotation * 90.0, 0.0, 0.0, -1.0);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex3f( 0.0, 0.0, 0.5);
		glVertex3f( 0.0, 0.5, 0.5);
		glVertex3f(-0.5, 0.0, 0.5);
		glVertex3f( 0.5, 0.0, 0.5);
		glVertex3f( 0.0, 0.0, 0.0);
		glVertex3f( 0.0, 0.0, 1.0);
	glEnd();

	glPopMatrix();
}

void CPlayer::Win()
{
	g_bPaused = true;

	glfwSetWindowTitle("glPenguin v0.0 - you win!");
}

void CPlayer::Die()
{
	g_bPaused = true;

	glfwSetWindowTitle("glPenguin v0.0 - you got crushed by an ice cube...");
}

void CPlayer::GetCrushedAt(int nPosX, int nPosY, int nPosZ)
{
	if (nPosZ == m_nPosZ && nPosX == m_nPosX && nPosY == m_nPosY) {
		Die();
	}
}

int CPlayer::GetX() { return m_nPosX; }

int CPlayer::GetY() { return m_nPosY; }

int CPlayer::GetZ() { return m_nPosZ; }

sint CPlayer::GetRotation() { return m_nRotation; }

void CPlayer::SetRotation(sint nRotation) { m_nRotation = nRotation; }

void CPlayer::CreatePlayers(int nNumberPlayers)
{
	// delete old players
	DestroyPlayers();

	m_nNumberPlayers = nNumberPlayers;

	// create new players
	m_pPlayers = new CPlayer*[m_nNumberPlayers];
	for (int nLoop1 = 0; nLoop1 < m_nNumberPlayers; ++nLoop1) {
		m_pPlayers[nLoop1] = new CPlayer();
	}
}

CPlayer& CPlayer::Get(int nNumber)
{
	return *m_pPlayers[nNumber];
}

void CPlayer::TickAtAll(int nPosZ)
{
	for (int nLoop1 = 0; nLoop1 < m_nNumberPlayers; ++nLoop1) {
		if (m_pPlayers[nLoop1]->GetZ() == nPosZ)
			m_pPlayers[nLoop1]->Tick();
	}
}

void CPlayer::ProcessKeyAll(int nKey)
{
	for (int nLoop1 = 0; nLoop1 < m_nNumberPlayers; ++nLoop1) {
		m_pPlayers[nLoop1]->ProcessKey(nKey);
	}
}

void CPlayer::RenderAll()
{
	for (int nLoop1 = 0; nLoop1 < m_nNumberPlayers; ++nLoop1) {
		m_pPlayers[nLoop1]->Render();
	}
}

void CPlayer::GetCrushedAtAll(int nPosX, int nPosY, int nPosZ)
{
	for (int nLoop1 = 0; nLoop1 < m_nNumberPlayers; ++nLoop1) {
		m_pPlayers[nLoop1]->GetCrushedAt(nPosX, nPosY, nPosZ);
	}
}

void CPlayer::DestroyPlayers()
{
	for (int nLoop1 = 0; nLoop1 < m_nNumberPlayers; ++nLoop1) {
		delete m_pPlayers[nLoop1];
	}
	if (m_pPlayers != NULL) delete[] m_pPlayers;

	m_pPlayers = NULL;
}