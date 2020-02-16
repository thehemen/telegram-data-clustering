#include <cstring>
#include <vector>
#include <map>
#include "threads.h"

using namespace std;

#ifndef JSON_H
#define JSON_H

class JSONParser
{
public:
    JSONParser();
    void save_articles(vector<wstring>);
    void save_articles_categorized(wstring, map<wstring, vector<wstring>>);
    void save_articles_ranked(wstring, map<wstring, vector<Thread>>);
};

#endif