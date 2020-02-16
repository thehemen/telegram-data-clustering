#include "language.h"
#include "utils.h"

Language::Language(string stopword_path, string keyword_path, int min_category_points)
{
    this->stop_words = split_by_length(get_lines(read_unicode(stopword_path)));

    vector<vector<wstring>> keywords;
    vector<wstring> lines = get_lines(read_unicode(keyword_path));
    for(int i = 0, len = lines.size(); i < len; ++i)
    {
        vector<wstring> words = split(lines[i]);
        keywords.push_back(words);
    }

    for(int i = 0; i < 6; ++i)
    {
        int category_index = i * 2;
        int keyword_index = i * 2 + 1;

        wstring w_cat_name = keywords[category_index][0].c_str();
        string category_name(w_cat_name.begin(), w_cat_name.end());
        this->category_names.push_back(category_name);

        for(const auto & keyword : keywords[keyword_index])
        {
            this->keyword_categories[keyword] = category_index / 2;
        }
    }

    this->min_category_points = min_category_points;
}

bool Language::is(wstring text) { return false; }

bool Language::is_news(wstring header, wstring text, int max_pronouns_count)
{
    bool is_news_flag = true;

    for(const auto & forbidden_first_word : forbidden_first)
    {
        int len = forbidden_first_word.size();
        if(header.substr(0, len) == forbidden_first_word)
        {
            is_news_flag = false;
            break;
        }
    }

    if(is_news_flag)
    {
        if(count_words(text, this->pronouns) >= max_pronouns_count)
        {
            is_news_flag = false;
        }
    }

    return is_news_flag;
}

vector<wstring> Language::tokenize(wstring text)
{
    vector<wstring> words;

    for(auto & word : split(text))
    {
        int len = word.size();

        // 0- and 1-length words are not informative and may be considered as stop-words
        if(len > 1)
        {
            if(find(this->stop_words[len].begin(), this->stop_words[len].end(), word) == this->stop_words[len].end())
            {
                for(int i = 0, stem_types = this->stems.size(); i < stem_types; ++i)
                {
                    for(const auto & stem : this->stems[i])
                    {
                        int rlen = stem.size();

                        if(len > rlen)
                        {
                            int begin_index = len - rlen;
                            if(word.substr(begin_index, rlen) == stem)
                            {
                                word.erase(begin_index, rlen);
                                len = word.size();  // update after modification
                                break;  // go to the next stemming type
                            }
                        }
                    }
                }

                words.push_back(word);
            }
        }
    }

    return words;
}

wstring Language::get_category_winner(vector<wstring> tokens)
{
    int category_num = this->category_names.size();
    vector<float> category_points(category_num);
    map<wstring, int> token_dict;

    for(const auto & token : tokens)
    {
        if(token_dict.count(token) == 0)
        {
            token_dict[token] = 0;
        }

        token_dict[token]++;
    }

    for(const auto & [token, count] : token_dict)
    {
        if(this->keyword_categories.count(token) != 0)
        {
            category_points[this->keyword_categories[token]] += count;
        }
    }

    pair<int, int> winner = argmax(category_points);

    wstring category_name;

    if(winner.second < this->min_category_points)
    {
        category_name = L"other";
    }
    else
    {
        string s_category_name = this->category_names[winner.first];
        category_name = wstring(s_category_name.begin(), s_category_name.end());
    }

    return category_name;
}