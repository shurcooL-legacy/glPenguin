#pragma once

#ifndef __Globals_H__
#define __Globals_H__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <GL\glfw.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <sstream>
#include <fstream>

#include "MMGR\mmgr.h"

#include "Types.h"

//#pragma warning(disable : 4305)
#pragma warning(disable : 4244)
//#pragma warning(disable : 4183)

extern bool		g_bPaused;
//extern float	g_fFPS;

#endif // __Globals_H__