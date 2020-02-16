#include <sstream>
#include <fstream>
#include <cmath>
#include <codecvt>
#include <algorithm>
#include <numeric>
#include <cwctype>
#include <regex>
#include <iomanip>
#include "utils.h"

using namespace std;

wstring get_filename_only(string filename)
{
    string filename_only = filename.substr(filename.find_last_of("/\\") + 1);
    return wstring(filename_only.begin(), filename_only.end());
}

wstring read_unicode(string path)
{
    wifstream ifile(path);
    ifile.imbue(locale(ifile.getloc(), new codecvt_utf8<wchar_t>));
    wstringstream istream;
    istream << ifile.rdbuf();
    ifile.close();
    return istream.str();
}

vector<wstring> get_lines(wstring text)
{
    vector<wstring> lines;

    wstringstream ss(text);
    wstring to;

    while(getline(ss, to, L'\n'))
    {
        lines.push_back(to);
    }

    return lines;
}

vector<int> find_all(wstring sequence, wstring subseq)
{
    vector<int> pos_list;
    int pos = sequence.find(subseq);

    while(pos != wstring::npos)
    {
        pos_list.push_back(pos);
        pos = sequence.find(subseq, pos + subseq.size());
    }

    return pos_list;
}

vector<int> calc_token_occurences(wstring sequence, vector<wstring> tokens)
{
    vector<int> occ;

    for(const auto & token : tokens)
    {
        int occurences = find_all(sequence, token).size();
        occ.push_back(occurences);
    }

    return occ;
}

int accum(vector<int> tokens)
{
    return accumulate(tokens.begin(), tokens.end(), 0);
}

vector<float> normalize(vector<int> points)
{
    int sum = accum(points);
    int count = points.size();

    vector<float> norm_data(count);
    for(int i = 0; i < count; ++i)
    {
        norm_data[i] = float(points[i]) / sum;
    }

    return norm_data;
}

pair<int, float> argmax(vector<float> share)
{
    int index = 0;
    float value = 0.0;

    for(int i = 0, len = share.size(); i < len; ++i)
    {
        if(share[i] > value)
        {
            value = share[i];
            index = i;
        }
    }

    return pair<int, float>(index, value);
}

void remove_by_tag(wstring &text, wchar_t tag)
{
    vector<int> left_pos;
    vector<int> right_pos;

    bool is_left = true;
    for(int i = 0, len = text.size(); i < len; ++i)
    {
        if(text[i] == tag)
        {
            if(is_left)
            {
                left_pos.push_back(i);
                is_left = false;
            }
            else
            {
                right_pos.push_back(i);
                is_left = true;
            }
        }
    }

    reverse(left_pos.begin(), left_pos.end());
    reverse(right_pos.begin(), right_pos.end());

    for(int i = 0, len = min(left_pos.size(), right_pos.size()); i < len; ++i)
    {
        text.erase(left_pos[i], right_pos[i] - left_pos[i] + 1);
    }
}

void remove_by_tags(wstring &text, wstring left_tag, wstring right_tag)
{
    vector<int> left_pos = find_all(text, left_tag);
    vector<int> right_pos = find_all(text, right_tag);

    reverse(left_pos.begin(), left_pos.end());
    reverse(right_pos.begin(), right_pos.end());

    for(int i = 0, len = min(left_pos.size(), right_pos.size()); i < len; ++i)
    {
        text.erase(left_pos[i], right_pos[i] - left_pos[i] + 1);
    }
}

void remove_punct_signs(wstring &text)
{
    replace_if(text.begin(), text.end(), ::iswpunct, ' ');
}

void remove_digit_signs(wstring &text)
{
    replace_if(text.begin(), text.end(), ::iswdigit, ' ');
}

void remove_extra_spaces(wstring &text)
{
    wstring output;
    unique_copy (text.begin(), text.end(), back_insert_iterator<wstring>(output), [](wchar_t a, wchar_t b)
    {
        return iswspace(a) && iswspace(b);
    });
    text = output;
}

void transform_tolower(wstring &text)
{
    transform(text.begin(), text.end(), text.begin(), ::towlower);
}

void replace_double_quotes(wstring& text)
{
    replace(text.begin(), text.end(), L'“', L'\'');
    replace(text.begin(), text.end(), L'”', L'\'');
    replace(text.begin(), text.end(), L'\"', L'\'');
}

wstring extract_by_tags(wstring& text, wstring left_tag, wstring right_tag)
{
    int left_tag_size = left_tag.size();
    int left_pos = text.find(left_tag) + left_tag_size;
    int right_pos = text.find(right_tag);
    return text.substr(left_pos, right_pos - left_pos);
}

wstring extract_header(wstring& text)
{
    return extract_by_tags(text, L"<h1>", L"</h1>");
}

time_t extract_datetime(wstring& text)
{
    const wstring published_time_tag = L"<meta property=\"article:published_time\" content=\"";
    const int time_length = 25;

    int index = text.find(published_time_tag) + published_time_tag.size();
    wstring time_clean = text.substr(index, time_length);
    string time_clean_s(time_clean.begin(), time_clean.end());

    istringstream istream(time_clean_s);
    struct std::tm tm;
    istream >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    time_t datetime = mktime(&tm);
    return datetime;
}

vector<wstring> split(wstring &text)
{
    vector<wstring> tokens;
    size_t pos = text.find(L' ');
    size_t initialPos = 0;

    while(pos != wstring::npos)
    {
        tokens.push_back(text.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;
        pos = text.find(L' ', initialPos);
    }

    tokens.push_back(text.substr(initialPos, min(pos, text.size()) - initialPos + 1));
    return tokens;
}

bool is_token_latin(wchar_t t)
{
    return (t >= L'a' && t <= L'z') || (t >= L'A' && t <= L'Z');
}

bool is_token_cyrillic(wchar_t t)
{
    return (t >= L'а' && t <= L'я') || (t >= L'А' && t <= L'Я');
}

int get_sequence_type(wstring seq)
{
    bool is_latin = true;
    bool is_cyrillic = true;

    for(const auto & t : seq)
    {
        if(!is_token_latin(t))
        {
            is_latin = false;
        }

        if(!is_token_cyrillic(t))
        {
            is_cyrillic = false;
        }

        if(!is_latin && !is_cyrillic)
        {
            break;
        }
    }

    if(is_latin)
    {
        return CharType::LATIN;
    }
    else if(is_cyrillic)
    {
        return CharType::CYRILLIC;
    }
    else
    {
        return CharType::NONE;
    }
}

#include <iostream>

int get_text_alphabet_type(wstring text, float share)
{
    int all_num = 0;
    int latin_num = 0;
    int cyrillic_num = 0;

    for(const auto & word : split(text))
    {
        auto alphabet_type = get_sequence_type(word);

        if(alphabet_type == CharType::LATIN)
        {
            latin_num++;
        }
        else if(alphabet_type == CharType::CYRILLIC)
        {
            cyrillic_num++;
        }

        all_num++;
    }

    int share_num = int(share * all_num);
    bool is_latin = false;
    bool is_cyrillic = false;

    if(latin_num > share_num)
    {
        is_latin = true;
    }

    if(cyrillic_num > share_num || ((cyrillic_num + latin_num) > share_num && cyrillic_num > latin_num))
    {
        // if cyrillic words number more than minimum share,
        // or both cyrillic and latin words number more than minimum share
        is_cyrillic = true;
    }

    if(is_latin && is_cyrillic)
    {
        if(latin_num > cyrillic_num)
        {
            is_cyrillic = false;
        }
        else
        {
            is_latin = false;
        }
    }

    if(is_latin)
    {
        return CharType::LATIN;
    }
    else if(is_cyrillic)
    {
        return CharType::CYRILLIC;
    }
    else
    {
        return CharType::NONE;
    }
}

int count_words(wstring text, vector<wstring> words)
{
    int word_count = 0;

    for(const auto & word : split(text))
    {
        for(const auto & eng_word : words)
        {
            if(word == eng_word)
            {
                word_count++;
                break;
            }
        }
    }

    return word_count;
}

map<int, vector<wstring>> split_by_length(vector<wstring> word_list)
{
    map<int, vector<wstring>> words_by_len;

    for(const auto & word : word_list)
    {
        int len = word.size();

        if(words_by_len.count(len) == 0)
        {
            words_by_len[len] = vector<wstring>();
        }

        words_by_len[len].push_back(word);
    }

    return words_by_len;
}