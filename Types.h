#pragma once

#ifndef __Types_H__
#define __Types_H__

#include "String.h"

typedef short int		sint;

struct tPoint2
{
	int nX;
	int nY;

	tPoint2() { nX = 0; nY = 0; }
	tPoint2(int nNewX, int nNewY) { nX = nNewX; nY = nNewY; }
	tPoint2 operator -(tPoint2 oParameter) { return tPoint2(nX - oParameter.nX, nY - oParameter.nY); }
	tPoint2 operator *(float fParameter) { return tPoint2(static_cast<int>(nX * fParameter), static_cast<int>(nY * fParameter)); }
};

#endif // __Types_H__