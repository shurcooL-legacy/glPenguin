#pragma once

#ifndef __CConfig_H__
#define __CConfig_H__

#include "Globals.h"

class CConfig
{
private:
	std::fstream		m_oFile;
	bool				m_bBad;

public:
	CConfig(String sFilename);
	~CConfig();
    
	bool IsBad();
	String GetParam(String sDefault, String sParameter, String sSection = "");
	int GetIntParam(int nDefault, String sParameter, String sSection = "");
	float GetFloatParam(float fDefault, String sParameter, String sSection = "");
	bool GetBoolParam(bool bDefault, String sParameter, String sSection = "");
};

#endif // __CConfig_H__