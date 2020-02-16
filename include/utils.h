#include <vector>
#include <map>
#include <string>

using namespace std;

#ifndef UTILS_H
#define UTILS_H

enum CharType { CYRILLIC, LATIN, NONE };

wstring get_filename_only(string);
wstring read_unicode(string);
vector<wstring> get_lines(wstring);
vector<int> find_all(wstring, wstring);
vector<int> calc_token_occurences(wstring, vector<wstring>);
int accum(vector<int>);
vector<float> normalize(vector<int>);
pair<int, float> argmax(vector<float>);
void remove_by_tag(wstring&, wchar_t);
void remove_by_tags(wstring&, wstring, wstring);
void remove_punct_signs(wstring&);
void remove_digit_signs(wstring&);
void remove_extra_spaces(wstring&);
void transform_tolower(wstring&);
void replace_double_quotes(wstring&);
wstring extract_by_tags(wstring&, wstring, wstring);
wstring extract_header(wstring&);
time_t extract_datetime(wstring& text);
vector<wstring> split(wstring&);
bool is_token_latin(wchar_t);
bool is_token_cyrillic(wchar_t);
int get_sequence_type(wstring);
int get_text_alphabet_type(wstring, float);
int count_words(wstring, vector<wstring>);
map<int, vector<wstring>> split_by_length(vector<wstring>);

#endif