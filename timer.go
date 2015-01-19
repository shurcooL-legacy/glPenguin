package main

import "time"

type CTimer struct {
	m_fCurrentTime time.Time
	m_fLastTime    time.Time
	m_fTimePassed  float32
}

func (t *CTimer) Update() {
	t.m_fCurrentTime = time.Now()
	t.m_fTimePassed = float32(t.m_fCurrentTime.Sub(t.m_fLastTime).Seconds())
	t.m_fLastTime = t.m_fCurrentTime
}

func (t CTimer) GetTimePassed() float32 { return t.m_fTimePassed }
