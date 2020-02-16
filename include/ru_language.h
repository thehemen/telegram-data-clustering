#include <string>
#include <vector>
#include <map>
#include "language.h"

using namespace std;

#ifndef RU_LANGUAGE_H
#define RU_LANGUAGE_H

class RuLanguage : public Language
{
	vector<wchar_t> vowels;
	wstring hard_sign;
public:
	RuLanguage(string, string, int);
	bool is(wstring);
};

#endif