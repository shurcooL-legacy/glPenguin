#include "String.h"

template <class T>
struct equal_to_ignore_case : public std::binary_function<T, T, bool> {
	result_type operator ()(const T& first_argument_type, const T& second_argument_type) const
	{
		return std::toupper(first_argument_type) == std::toupper(second_argument_type);
	}
};

bool StringIgnoreCaseCompare(String sString1, String sString2)
{
	if (sString1.length() == sString2.length())
		return std::equal(sString1.begin(), sString1.end(), sString2.begin(), equal_to_ignore_case<String::value_type>());
	else return false;
}