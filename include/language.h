#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#ifndef LANGUAGE_H
#define LANGUAGE_H

class Language
{
protected:
	vector<wstring> forbidden_first;
	vector<wstring> pronouns;
	vector<vector<wstring>> stems;
	map<int, vector<wstring>> stop_words;
	vector<string> category_names;
	map<wstring, int> keyword_categories;
	int min_category_points;
public:
	Language(string, string, int);
	virtual bool is(wstring);
	bool is_news(wstring, wstring, int);
	vector<wstring> tokenize(wstring);
	wstring get_category_winner(vector<wstring>);
};

#endif