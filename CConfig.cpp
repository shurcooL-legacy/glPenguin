#include "CConfig.h"

CConfig::CConfig(String sFilename)
{
	m_oFile.open(sFilename.c_str(), std::fstream::in);

	m_bBad = !m_oFile;
}

CConfig::~CConfig()
{
	m_oFile.close();
}

bool CConfig::IsBad() { return m_bBad; }

String CConfig::GetParam(String sDefault, String sParameter, String sSection)
{
	if (IsBad()) return sDefault;

	char		szBuffer[256];
	String		sLine;
	String		sCurrentSection = "";

	m_oFile.clear();
	m_oFile.seekg(std::ifstream::beg);
	while (!m_oFile.eof()) {
		m_oFile.getline(szBuffer, 256);
		sLine = szBuffer;

		if (sLine.find_first_not_of(" \t") == String::npos)				// Blank string
			continue;
		else {
			if (sLine.find_first_of('#') != String::npos) {				// Contains a '#'
				if ((sLine.find_first_of('#') - sLine.find_first_not_of(" \t")) == 0)
					// Nothing but a user comment
					continue;
				else
					// Cut off the user comment
					sLine = sLine.substr(sLine.find_first_not_of(" \t"), sLine.find_first_of('#') - sLine.find_first_not_of(" \t"));
			}

			// Cut off any preceding or succeeding whitespaces
			sLine = sLine.substr(sLine.find_first_not_of(" \t"), sLine.find_last_not_of(" \t") - sLine.find_first_not_of(" \t") + 1);

			if (sLine.find_first_not_of(" \t") == String::npos)			// Nothing but a user comment with whitespaces
				continue;
		}

																		// [Section] statement
		if (sSection != "" && sLine.find_first_of('[') != String::npos && sLine.find_first_of(']') != String::npos)
			sCurrentSection = sLine.substr(sLine.find_last_of('[') + 1, sLine.find_first_of(']') - sLine.find_last_of('[') - 1);
		else if (sLine.find_first_of('=') != String::npos) {			// Parameter=Value statement
			if (sSection == sCurrentSection && sParameter == sLine.substr(0, sLine.find_first_of('=')))
				return sLine.substr(sLine.find_last_of('=') + 1);
		}
	}

	return sDefault;
}

int CConfig::GetIntParam(int nDefault, String sParameter, String sSection)
{
	std::stringstream oSStream;
	oSStream << nDefault;

	return atoi(GetParam(oSStream.str(), sParameter, sSection).c_str());
}

float CConfig::GetFloatParam(float fDefault, String sParameter, String sSection)
{
	std::stringstream oSStream;
	oSStream << fDefault;

	return atof(GetParam(oSStream.str(), sParameter, sSection).c_str());
}

bool CConfig::GetBoolParam(bool bDefault, String sParameter, String sSection)
{
	String sTemporary = GetParam((bDefault ? "Yes" : "No"), sParameter, sSection);

	return (StringIgnoreCaseCompare(sTemporary, "Yes")
			|| StringIgnoreCaseCompare(sTemporary, "True")
			|| StringIgnoreCaseCompare(sTemporary, "1"));
}