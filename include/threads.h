#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>

using namespace std;

#ifndef THREADS_H
#define THREADS_H

struct Record
{
	wstring filename;
	wstring header;
	int header_len;
	time_t datetime;
	Record(wstring, wstring, int, time_t);
};

bool operator<(Record const & a, Record const & b);

struct Thread
{
	wstring header;
	wstring category;
	time_t diff_time;
	vector<wstring> filenames; 
	int count;
	Thread(wstring, wstring, time_t, vector<wstring>);
};

bool operator<(Thread const & a, Thread const & b);

class Threads
{
	float header_similar_share;
	vector<Record> records;
    map<wstring, vector<int>> indexes_by_word;
    map<int, int> thread_by_record;
    map<int, vector<int>> records_by_thread;
public:
	Threads(float);
	int add(wstring, vector<wstring>, wstring, time_t);
	map<wstring, vector<Record>> get();
	map<wstring, vector<Thread>> get_ranked(map<wstring, wstring>);
private:
	pair<int, bool> get_thread_id(map<int, int>, int);
	map<int, int> get_count_dict(vector<wstring> tokens);
};

#endif