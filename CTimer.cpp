#include "CTimer.h"

CTimer*		CTimer::m_pSingleton = NULL;

CTimer::CTimer()
{
	m_fLastTime = glfwGetTime();
}

CTimer::~CTimer() {}

void CTimer::Update()
{
	m_fCurrentTime = glfwGetTime();
	m_fTimePassed = m_fCurrentTime - m_fLastTime;
	m_fLastTime = m_fCurrentTime;
}

float CTimer::GetCurrentTime()
{
	return m_fCurrentTime;
}

float CTimer::GetLastTime()
{
	return m_fLastTime;
}

float CTimer::GetTimePassed()
{
	return m_fTimePassed;
}

CTimer& CTimer::Get()
{
	if (m_pSingleton == NULL) {
		m_pSingleton = new CTimer();
	}

	return *m_pSingleton;
}

void CTimer::Destroy()
{
	if (m_pSingleton != NULL) {
		delete m_pSingleton;
		m_pSingleton = NULL;
	}
}