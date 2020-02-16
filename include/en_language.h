#include <string>
#include <vector>
#include <map>
#include "language.h"

using namespace std;

#ifndef EN_LANGUAGE_H
#define EN_LANGUAGE_H

class EnLanguage : public Language
{
	vector<wstring> common_words;
public:
	EnLanguage(string, string, int);
	bool is(wstring);
};

#endif