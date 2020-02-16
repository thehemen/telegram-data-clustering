#include "ru_language.h"
#include "utils.h"

RuLanguage::RuLanguage(string stopword_path, string keyword_path, int min_category_points) : Language(stopword_path, keyword_path, min_category_points)
{  
    this->forbidden_first = {};
    this->pronouns = {
        L"вы", L"вас", L"вам", L"вами",
        L"ваш", L"вашего", L"вашему", L"вашим", L"вашем",
        L"ты", L"тебя", L"тебе", L"тобой", L"тобою",
        L"твой", L"твоего", L"твоему", L"твоим", L"твоём"
    };
    this->vowels = {L'я', L'ю', L'е', L'ё', L' '};
    this->hard_sign = L"ъ";
    this->stems = {
        {
            L"ся", L"сь"
        },
        {
            L"ии", L"ию", L"ие", L"ий", L"ия", L"ая",
            L"ат", L"ят", L"ем", L"ти", L"ам", L"ые", L"ым",
            L"ых", L"ый", L"ое", L"ом", L"ой", L"их", L"ую",
            L"ого", L"его", L"ло", L"ла", L"ал", L"ает", L"ите",
            L"ил", L"им", L"или", L"яют", L"йте", L"ать",
            L"ить", L"еть", L"ять", L"уть", L"ают", L"яет",
            L"но", L"те", L"ту", L"та", L"ов", L"ей", L"ах"
        },
        {
            L"ы", L"а", L"е", L"и", L"у", L"ю", L"о", L"ь"
        }
    };
}

bool RuLanguage::is(wstring text)
{
    // vowels with space than can be used after "hard sign" (ъ)
    // this rule is used to make difference from bulgarian
    vector<int> pos_list = find_all(text, this->hard_sign);

    bool is_russian = true;

    for(const auto & pos : pos_list)
    {
        if(pos == text.size() - 1)
        {
            continue;
        }

        wchar_t next_symb = text[pos + 1];
        bool is_vowel = false;

        for(const auto & vowel : this->vowels)
        {
            if(next_symb == vowel)
            {
                is_vowel = true;
                break;
            }
        }

        if(!is_vowel)
        {
            is_russian = false;
            break;
        }
    }

    return is_russian;
}
