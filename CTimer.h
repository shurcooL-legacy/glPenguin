#pragma once

#ifndef __CTimer_H__
#define __CTimer_H__

#include "Globals.h"

class CTimer
{
public:
	CTimer();
	~CTimer();

	void Update();
	float GetCurrentTime();
	float GetLastTime();
	float GetTimePassed();

	static CTimer& Get();
	static void Destroy();

private:
	float	m_fCurrentTime;
	float	m_fLastTime;
	float	m_fTimePassed;

	static CTimer*	m_pSingleton;
};

#endif // __CTimer_H__