#include "CWorld.h"

#include "CTimer.h"
#include "CPlayer.h"

CWorld*		CWorld::m_pSingleton = NULL;

CWorld::CWorld(int nWorldX, int nWorldY, int nWorldZ, int nWinHeight, float fWorldTickTime, float fIceCubeRespawnRate)
{
	m_nWorldX = nWorldX;
	m_nWorldY = nWorldY;
	m_nWorldZ = nWorldZ;
	m_nWinHeight = nWinHeight;

	m_pWorld = new sint[m_nWorldX * m_nWorldY * m_nWorldZ];

	// clear the world
	//memset(m_pWorld, 0, m_nWorldX * m_nWorldY * m_nWorldZ * sizeof(sint));
	for (int nLoop1 = 0; nLoop1 < m_nWorldZ; ++nLoop1) {
		for (int nLoop2 = 0; nLoop2 < m_nWorldX; ++nLoop2) {
			for (int nLoop3 = 0; nLoop3 < m_nWorldY; ++nLoop3) {
				SetIceCube(nLoop2, nLoop3, nLoop1, 0);
			}
		}
	}

	m_fWorldTickTime = fWorldTickTime;
	m_fIceCubeRespawnRate = fIceCubeRespawnRate;

	m_fNextTick = fWorldTickTime;
	m_fNextIceCube = fIceCubeRespawnRate;
}

CWorld::~CWorld()
{
	delete[] m_pWorld;
}

void CWorld::Tick()
{
	m_fNextTick -= CTimer::Get().GetTimePassed();
	while (m_fNextTick <= 0)
	{
		for (int nLoop1 = 0; nLoop1 < m_nWorldZ; ++nLoop1) {
			// Do player ticks to make sure he won't get crushed by a following ice cube
			CPlayer::TickAtAll(nLoop1);

			for (int nLoop2 = 0; nLoop2 < m_nWorldX; ++nLoop2) {
				for (int nLoop3 = 0; nLoop3 < m_nWorldY; ++nLoop3) {
					if (GetIceCube(nLoop2, nLoop3, nLoop1) == 1)
					{
						if (nLoop1 - 1 < 0 || GetIceCube(nLoop2, nLoop3, nLoop1 - 1) != 0) {
							SetIceCube(nLoop2, nLoop3, nLoop1, 2);		// standing
						} else {
							SetIceCube(nLoop2, nLoop3, nLoop1, 0);
							SetIceCube(nLoop2, nLoop3, nLoop1 - 1, 1);		// falling

							CPlayer::GetCrushedAtAll(nLoop2, nLoop3, nLoop1 - 1);
						}
					}
				}
			}
		}

		--m_fNextIceCube;
		while (m_fNextIceCube <= 0)
		{
			tPoint2		oNextIceCubePos;		// next ice cube spawn position

			do {
				// think of a random ice cube spawn position
				oNextIceCubePos.nX = rand() % m_nWorldX;
				oNextIceCubePos.nY = rand() % m_nWorldY;
			} // make sure that the next ice cube won't appear in the same place
			while (GetIceCube(oNextIceCubePos.nX, oNextIceCubePos.nY, m_nWorldZ - 1) != 0);

			// spawn the ice cube
			SetIceCube(oNextIceCubePos.nX, oNextIceCubePos.nY, m_nWorldZ - 1, 1);

			m_fNextIceCube += m_fIceCubeRespawnRate;
		}

		m_fNextTick += m_fWorldTickTime;
	}
}

float CWorld::GetTickProcess()
{
	return (1.0 - m_fNextTick / m_fWorldTickTime);
}

void CWorld::Render()
{
	glTranslatef(m_nWorldX * -0.5, m_nWorldY * -0.5, 0.0);

	RenderBase();

	for (int nLoop1 = 0; nLoop1 < m_nWorldZ; ++nLoop1) {
		for (int nLoop2 = 0; nLoop2 < m_nWorldX; ++nLoop2) {
			for (int nLoop3 = 0; nLoop3 < m_nWorldY; ++nLoop3) {
				RenderIceCube(nLoop2, nLoop3, nLoop1, GetIceCube(nLoop2, nLoop3, nLoop1));
			}
		}
	}

	// render players
	CPlayer::RenderAll();
}

void CWorld::RenderBase()
{
	glDepthMask(GL_FALSE);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(0.0, 0.0);
		glVertex2f(0.0, m_nWorldY);
		glVertex2f(m_nWorldX, m_nWorldY);
		glVertex2f(m_nWorldX, 0.0);
	glEnd();
	glDepthMask(GL_TRUE);

	// 3D grid
	/*glColor3f(0.0, 0.0, 1.0);
	for (int nLoop1 = 0; nLoop1 <= m_nWorldZ; ++nLoop1) {
		for (int nLoop2 = 1; nLoop2 < m_nWorldX; ++nLoop2) {
			glBegin(GL_LINES);
				glVertex3f(0, nLoop2, nLoop1);
				glVertex3f(m_nWorldX, nLoop2, nLoop1);
				glVertex3f(nLoop2, 0, nLoop1);
				glVertex3f(nLoop2, m_nWorldY, nLoop1);
			glEnd();
		}
	}*/
}

void CWorld::RenderIceCube(int nPosX, int nPosY, int nPosZ, int nState)
{
	if (!nState)
		return;

	glPushMatrix();

	switch (nState) {
	case 1:		// falling
		glTranslatef(nPosX, nPosY, (1.0 - GetTickProcess()) + nPosZ);

		break;
	case 2:		// lying still
		glTranslatef(nPosX, nPosY, nPosZ);

		break;
	default:
		break;
	}

	glBegin(GL_QUADS);
		glColor3f(0.0, 1.0, 0.0); glVertex3i(0, 1, 0); glVertex3i(0, 0, 0); glVertex3i(1, 0, 0); glVertex3i(1, 1, 0);	// bottom
		glColor3f(0.0, 1.0, 1.0); glVertex3i(0, 0, 1); glVertex3i(1, 0, 1); glVertex3i(1, 0, 0); glVertex3i(0, 0, 0);	// front
		glColor3f(1.0, 1.0, 0.0); glVertex3i(0, 1, 1); glVertex3i(0, 0, 1); glVertex3i(0, 0, 0); glVertex3i(0, 1, 0);	// left
		glColor3f(0.0, 1.0, 1.0); glVertex3i(0, 1, 1); glVertex3i(0, 1, 0); glVertex3i(1, 1, 0); glVertex3i(1, 1, 1);	// back
		glColor3f(1.0, 1.0, 0.0); glVertex3i(1, 1, 1); glVertex3i(1, 1, 0); glVertex3i(1, 0, 0); glVertex3i(1, 0, 1);	// right
		glColor3f(0.0, 0.0, 1.0); glVertex3i(0, 1, 1); glVertex3i(1, 1, 1); glVertex3i(1, 0, 1); glVertex3i(0, 0, 1);	// top
	glEnd();

	glPopMatrix();
}

int CWorld::GetWorldX() { return m_nWorldX; }

int CWorld::GetWorldY() { return m_nWorldY; }

int CWorld::GetWorldZ() { return m_nWorldZ; }

int CWorld::GetWinHeight() { return m_nWinHeight; }

sint CWorld::GetIceCube(int nPosX, int nPosY, int nPosZ)
{
	if (nPosZ < 0)
		return 3;		// out of boundaries
	else
		return *(m_pWorld + (nPosZ * (m_nWorldX * m_nWorldY)) + (nPosY * (m_nWorldX)) + nPosX);
}

void CWorld::SetIceCube(int nPosX, int nPosY, int nPosZ, sint nValue)
{
	*(m_pWorld + (nPosZ * (m_nWorldX * m_nWorldY)) + (nPosY * (m_nWorldX)) + nPosX) = nValue;
}

int CWorld::TopIceCube(int nPosX, int nPosY)
{
	if (nPosX < 0 || nPosX >= m_nWorldX
		|| nPosY < 0 || nPosY >= m_nWorldY)
		return m_nWorldZ;

	int nPosZ;

	for (nPosZ = 0; nPosZ < m_nWorldZ; ++nPosZ) {
		if (GetIceCube(nPosX, nPosY, nPosZ) != 2) {
			return nPosZ - 1;
		}
	}

	return nPosZ;
}

int CWorld::TopFallingIceCube(int nPosX, int nPosY, int nPosZ)
{
	for (--nPosZ; nPosZ >= 0; --nPosZ) {
		if (GetIceCube(nPosX, nPosY, nPosZ) == 1) {
			return nPosZ;
		} else if (GetIceCube(nPosX, nPosY, nPosZ) == 2) {
			return -1;		// We found a 'standing' ice cube on the way down, meaning no falling cubes found
		}
	}

	return -1;		// No falling ice cubes underneath
}

void CWorld::Create(int nWorldX, int nWorldY, int nWinHeight, float fWorldTickTime, float fIceCubeRespawnRate)
{
	if (m_pSingleton == NULL) {
		m_pSingleton = new CWorld(nWorldX, nWorldY, nWinHeight + ICE_CUBE_SPAWN_HEIGHT, nWinHeight, fWorldTickTime, fIceCubeRespawnRate);
	}
}

CWorld& CWorld::Get()
{
	return *m_pSingleton;
}

void CWorld::Destroy()
{
	delete m_pSingleton;

	m_pSingleton = NULL;
}