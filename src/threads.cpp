#include "threads.h"

using namespace std;

Record::Record(wstring filename, wstring header, int header_len, time_t datetime)
{
    this->filename = filename;
    this->header = header;
    this->header_len = header_len;
    this->datetime = datetime;   
}

bool operator<(Record const & a, Record const & b)
{
    return a.datetime > b.datetime;
}

Thread::Thread(wstring header, wstring category, time_t diff_time, vector<wstring> filenames)
{
    this->header = header;
    this->category = category;
    this->diff_time = diff_time;
    this->filenames = filenames;
    this->count = filenames.size();
}

bool operator<(Thread const & a, Thread const & b)
{
    return a.diff_time * a.count > b.diff_time * b.count;
}

Threads::Threads(float header_similar_share)
{
    this->header_similar_share = header_similar_share;
}

int Threads::add(wstring filename, vector<wstring> tokens, wstring raw_header, time_t datetime)
{
    int header_len = tokens.size();

    map<int, int> header_counter = get_count_dict(tokens);
    pair<int, bool> thread_data = get_thread_id(header_counter, header_len);
    int thread_id = thread_data.first;
    bool is_created_now = thread_data.second;
    
    this->records.push_back(Record(filename, raw_header, header_len, datetime));
    int header_index = this->records.size() - 1;

    this->thread_by_record[header_index] = thread_id;
    this->records_by_thread[thread_id].push_back(header_index);

    if(is_created_now)
    {
        for(const auto & token: tokens)
        {
            if(this->indexes_by_word.count(token) == 0)
            {
                this->indexes_by_word[token] = vector<int>();
            }

            this->indexes_by_word[token].push_back(header_index);
        }
    }

    return header_index;
}

map<wstring, vector<Record>> Threads::get()
{
    map<wstring, vector<Record>> threads;

    for(const auto & [thread_id, record_ids] : this->records_by_thread)
    {
        vector<Record> records;
        for(const auto & record_id : record_ids)
        {
            records.push_back(this->records[record_id]);
        }

        sort(records.begin(), records.end());
        wstring header = records[0].header;
        threads[header] = records;
    }

    return threads;
}

map<wstring, vector<Thread>> Threads::get_ranked(map<wstring, wstring> category_data)
{
    map<wstring, vector<Record>> thread_data = this->get();
    map<wstring, vector<Thread>> ranked_data = {
        {L"society", vector<Thread>()},
        {L"economy", vector<Thread>()},
        {L"technology", vector<Thread>()},
        {L"sports", vector<Thread>()},
        {L"entertainment", vector<Thread>()},
        {L"science", vector<Thread>()},
        {L"other", vector<Thread>()},
        {L"any", vector<Thread>()}
    };

    for(const auto & [header_name, records] : thread_data)
    {
        time_t begin_time = records[0].datetime;
        time_t end_time = records[records.size() - 1].datetime;
        time_t diff_time = begin_time - end_time;

        vector<wstring> filenames;
        map<wstring, int> category_counter;

        for(const auto & record : records)
        {
            wstring filename = record.filename;
            wstring category_name = category_data[record.filename];

            if(category_counter.count(category_name) == 0)
            {
                category_counter[category_name] = 0;
            }

            filenames.push_back(filename);
            category_counter[category_name]++;
        }

        wstring best_category_name = L"other";
        int best_category_count = 0;

        for(const auto & [category_name, category_count] : category_counter)
        {
            if(category_name != L"other")
            {
                if(category_count > best_category_count)
                {
                    best_category_count = category_count;
                    best_category_name = category_name;
                }
            }
        }

        Thread thread_data(header_name, best_category_name, diff_time, filenames);
        ranked_data[best_category_name].push_back(thread_data);
        ranked_data[L"any"].push_back(thread_data);
    }

    for(const auto & [category_name, threads] : ranked_data)
    {
        sort(ranked_data[category_name].begin(), ranked_data[category_name].end());
    }

    return ranked_data;
}

pair<int, bool> Threads::get_thread_id(map<int, int> header_counter, int header_len)
{
    int thresh_first = int(this->header_similar_share * header_len);

    int thread_id = -1;
    int max_count = 0;
    bool is_created_now = false;

    for(const auto & [header_index, count] : header_counter)
    {
        int thresh_second = int(this->header_similar_share * this->records[header_index].header_len);
        
        if(count >= thresh_first && count >= thresh_second)
        {
            if(count > max_count)
            {
                max_count = count;
                thread_id = this->thread_by_record[header_index];
            }
        }
    }

    if(thread_id == -1)
    {
        thread_id = records_by_thread.size();
        this->records_by_thread[thread_id] = vector<int>();
        is_created_now = true;
    }

    return pair<int, bool>(thread_id, is_created_now);
}

map<int, int> Threads::get_count_dict(vector<wstring> tokens)
{
    map<int, int> header_counter;
    for(const auto & token : tokens)
    {
        if(this->indexes_by_word.count(token) != 0)
        {
            for(const auto & header_index : this->indexes_by_word[token])
            {
                if(header_counter.count(header_index) == 0)
                {
                    header_counter[header_index] = 0;
                }

                header_counter[header_index]++;
            }
        }
    }

    return header_counter;
}