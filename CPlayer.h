#pragma once

#ifndef __CPlayer_H__
#define __CPlayer_H__

#include "Globals.h"

class CCamera;

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	void Tick();
	void ProcessKey(int nKey);
	void Render();
	void Win();
	void Die();
	void GetCrushedAt(int nPosX, int nPosY, int nPosZ);
	int GetX();
	int GetY();
	int GetZ();
	sint GetRotation();
	void SetRotation(sint nRotation);

	static void TickAtAll(int nPosZ);
	static void ProcessKeyAll(int nKey);
	static void RenderAll();
	static void GetCrushedAtAll(int nPosX, int nPosY, int nPosZ);

	static void CreatePlayers(int nNumberPlayers);
	static CPlayer& Get(int nNumber);
	static void DestroyPlayers();

private:
	int		m_nPosX;
	int		m_nPosY;
	int		m_nPosZ;
	sint	m_nRotation;		// 0 - up, 1 - right, 2 - down, 3 - left
	int		m_nState;

	static int			m_nNumberPlayers;
	static CPlayer**	m_pPlayers;
};

#endif // __CPlayer_H__