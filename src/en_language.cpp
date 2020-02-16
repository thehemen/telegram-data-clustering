#include "en_language.h"
#include "utils.h"

EnLanguage::EnLanguage(string stopword_path, string keyword_path, int min_category_points) : Language(stopword_path, keyword_path, min_category_points)
{
    this->forbidden_first = {L"get", L"buy", L"bag", L"save", L"how"};
    this->pronouns = {L"you", L"you\'re", L"your", L"yours"};
    this->common_words = {L"an", L"and", L"in", L"of", L"on", L"the", L"that", L"to", L"is"};
    this->stems = {
        {
            L"lly", L"ly", L"al"
        }, 
        {
            L"ated", L"ed"
        }, 
        {
            L"ing", L"ies", L"s"
        }
    };
}

bool EnLanguage::is(wstring text)
{
    const float word_share = 0.01;
    
    int text_len = text.size();
    int word_count = 0;

    for(const auto & word : split(text))
    {
        for(const auto & common_word : this->common_words)
        {
            if(word == common_word)
            {
                word_count++;
                break;
            }
        }
    }

    if(word_count > int(word_share * text_len))
    {
        return true;
    }
    else
    {
        return false;
    }
}
