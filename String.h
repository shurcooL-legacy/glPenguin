#pragma once

#ifndef __String_H__
#define __String_H__

#include <string>

#include <cctype>
#include <functional>

typedef std::string String;

bool StringIgnoreCaseCompare(String sString1, String sString2);

#endif // __String_H__