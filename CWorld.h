#pragma once

#ifndef __CWorld_H__
#define __CWorld_H__

#include "Globals.h"

#define ICE_CUBE_SPAWN_HEIGHT	10

class CWorld
{
public:
	CWorld(int nWorldX, int nWorldY, int nWorldZ, int nWinHeight,float fWorldTickTime, float fIceCubeRespawnRate);
	~CWorld();

	void Tick();
	float GetTickProcess();
	void Render();
	int GetWorldX();
	int GetWorldY();
	int GetWorldZ();
	int GetWinHeight();
	sint GetIceCube(int nPosX, int nPosY, int nPosZ);
	void SetIceCube(int nPosX, int nPosY, int nPosZ, sint nValue);
	int TopIceCube(int nPosX, int nPosY);
	int TopFallingIceCube(int nPosX, int nPosY, int nPosZ);

	static void Create(int nWorldX, int nWorldY, int nWinHeight, float fWorldTickTime, float fIceCubeRespawnRate);
	static CWorld& Get();
	static void Destroy();

private:
	int		m_nWorldX;
	int		m_nWorldY;
	int		m_nWorldZ;
	int		m_nWinHeight;
	float	m_fWorldTickTime;
	float	m_fIceCubeRespawnRate;

	sint*	m_pWorld;

	float	m_fNextTick;
	float	m_fNextIceCube;

	static CWorld*	m_pSingleton;

	void RenderBase();
	void RenderIceCube(int nPosX, int nPosY, int nPosZ, int nState);
};

#endif // __CWorld_H__