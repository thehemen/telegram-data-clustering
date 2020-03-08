#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <experimental/filesystem>
#include <locale>

#include "utils.h"
#include "json.h"
#include "en_language.h"
#include "ru_language.h"
#include "threads.h"

using namespace std;

wstring get_clean_text(wstring& text)
{
    wstring clean_text(text.begin(), text.end());
    remove_by_tags(clean_text, L"<", L">");
    remove_punct_signs(clean_text);
    remove_digit_signs(clean_text);
    remove_extra_spaces(clean_text);
    transform_tolower(clean_text);
    return clean_text;
}

wstring get_unquoted_text(wstring& text)
{
    wstring unquoted_text(text.begin(), text.end());
    remove_by_tags(unquoted_text, L"<i>", L"</i>");
    remove_by_tags(unquoted_text, L"<blockquote>", L"</blockquote>");
    remove_by_tags(unquoted_text, L"<a href=", L"</a>");
    remove_by_tags(unquoted_text, L"<", L">");
    remove_by_tag(unquoted_text, L'\"');  // ordinary quotation mark
    remove_by_tags(unquoted_text, L"“", L"”");  // left / right double quotation marks
    remove_by_tags(unquoted_text, L"«", L"»");  // left / right double quotation marks
    remove_extra_spaces(unquoted_text);
    transform_tolower(unquoted_text);
    return unquoted_text;
}

wstring get_raw_header(wstring& text)
{
    wstring raw_header = extract_header(text);
    replace_double_quotes(raw_header);
    return raw_header;
}

wstring get_clean_header(wstring& raw_header)
{
    wstring clean_header(raw_header.begin(), raw_header.end());
    remove_punct_signs(clean_header);
    remove_digit_signs(clean_header);
    remove_extra_spaces(clean_header);
    transform_tolower(clean_header);
    return clean_header;
}

class TGNews
{
    string path;
    float word_share;
    float header_similar_share;
    EnLanguage* en;
    RuLanguage* ru;
    JSONParser* jsonParser;
public:
    TGNews(string path, float word_share, float header_similar_share)
    {
        this->path = path;
        this->word_share = word_share;
        this->header_similar_share = header_similar_share;
        this->en = new EnLanguage("en_stop_words.txt", "en_key_words.txt", 3);
        this->ru = new RuLanguage("ru_stop_words.txt", "ru_key_words.txt", 3);
        this->jsonParser = new JSONParser();
    }

    void find_enru()
    {
        map<wstring, vector<wstring>> lang_data;
        lang_data[L"en"] = vector<wstring>();
        lang_data[L"ru"] = vector<wstring>();

        for (const auto & entry : experimental::filesystem::directory_iterator(path))
        {
            string filename = entry.path();
            wstring filename_only = get_filename_only(filename);
            wstring text = read_unicode(filename);
            wstring clean_text = get_clean_text(text);

            int alphabet_type = get_text_alphabet_type(clean_text, this->word_share);

            if(alphabet_type == CharType::LATIN)
            {
                if(this->en->is(clean_text))
                {
                    lang_data[L"en"].push_back(filename_only);
                }
            }
            else if(alphabet_type == CharType::CYRILLIC)
            {
                if(this->ru->is(clean_text))
                {
                    lang_data[L"ru"].push_back(filename_only);
                }
            }
        }

        this->jsonParser->save_articles_categorized(L"lang_code", lang_data);
    }

    void find_news()
    {
        const int max_pronouns_count = 2;
        const vector<wstring> html_headers = {
            L"<h1>", L"<h2>", L"<h3>", L"<h4>", L"<h5>", L"<h6>"
        };
        const int max_header_count = 3;

        vector<wstring> articles;

        for (const auto & entry : experimental::filesystem::directory_iterator(path))
        {
            string filename = entry.path();
            wstring filename_only = get_filename_only(filename);
            wstring text = read_unicode(filename);
            wstring clean_text = get_clean_text(text);

            // mark articles with many headers as no-news
            int header_count = accum(calc_token_occurences(text, html_headers));
            if(header_count > max_header_count)
            {
                continue;
            }

            int alphabet_type = get_text_alphabet_type(clean_text, this->word_share);

            if(alphabet_type != CharType::NONE)
            {
                wstring raw_header = get_raw_header(text);
                wstring clean_header = get_clean_header(raw_header);
                wstring unquoted_text = get_unquoted_text(text);

                if(alphabet_type == CharType::LATIN)
                {
                    if(this->en->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
                        articles.push_back(filename_only);
                    }
                }
                else
                {
                    if(this->ru->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
                        articles.push_back(filename_only);
                    }
                }
            }
        }

        this->jsonParser->save_articles(articles);
    }

    void group_by_category()
    {
    	const int max_pronouns_count = 2;
        const vector<wstring> html_headers = {
            L"<h1>", L"<h2>", L"<h3>", L"<h4>", L"<h5>", L"<h6>"
        };
        const int max_header_count = 3;

        map<wstring, vector<wstring>> category_data = {
            {L"society", vector<wstring>()},
            {L"economy", vector<wstring>()},
            {L"technology", vector<wstring>()},
            {L"sports", vector<wstring>()},
            {L"entertainment", vector<wstring>()},
            {L"science", vector<wstring>()},
            {L"other", vector<wstring>()}
        };

        for (const auto & entry : experimental::filesystem::directory_iterator(path))
        {
            string filename = entry.path();
            wstring filename_only = get_filename_only(filename);
            wstring text = read_unicode(filename);
            wstring clean_text = get_clean_text(text);

            // mark articles with many headers as no-news
            int header_count = accum(calc_token_occurences(text, html_headers));
            if(header_count > max_header_count)
            {
                continue;
            }

            int alphabet_type = get_text_alphabet_type(clean_text, this->word_share);

            if(alphabet_type != CharType::NONE)
            {
                wstring category_name;
                wstring raw_header = get_raw_header(text);
                wstring clean_header = get_clean_header(raw_header);
                wstring unquoted_text = get_unquoted_text(text);

                if(alphabet_type == CharType::LATIN)
                {
                	if(this->en->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
	                    vector<wstring> tokens = this->en->tokenize(text);
	                    category_name = this->en->get_category_winner(tokens);
	                }
                }
                else
                {
                	if(this->ru->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
	                    vector<wstring> tokens = this->ru->tokenize(text);
	                    category_name = this->ru->get_category_winner(tokens);
	                }
                }

                category_data[category_name].push_back(filename_only);
            }
        }

        this->jsonParser->save_articles_categorized(L"category", category_data);
    }

    void group_by_threads()
    {
    	const int max_pronouns_count = 2;
        const vector<wstring> html_headers = {
            L"<h1>", L"<h2>", L"<h3>", L"<h4>", L"<h5>", L"<h6>"
        };
        const int max_header_count = 3;

        Threads threads(header_similar_share);

        for (const auto & entry : experimental::filesystem::directory_iterator(path))
        {
            string filename = entry.path();
            wstring filename_only = get_filename_only(filename);
            wstring text = read_unicode(filename);
            wstring clean_text = get_clean_text(text);

            // mark articles with many headers as no-news
            int header_count = accum(calc_token_occurences(text, html_headers));
            if(header_count > max_header_count)
            {
                continue;
            }

            int alphabet_type = get_text_alphabet_type(clean_text, this->word_share);

            if(alphabet_type != CharType::NONE)
            {
                time_t datetime = extract_datetime(text);
                wstring raw_header = get_raw_header(text);
                wstring clean_header = get_clean_header(raw_header);
            	wstring unquoted_text = get_unquoted_text(text);

                if(alphabet_type == CharType::LATIN)
                {
                	if(this->en->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
	                    vector<wstring> tokens = this->en->tokenize(clean_header);
	                    threads.add(filename_only, tokens, raw_header, datetime);
	                }
                }
                else
                {
                	if(this->ru->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
	                    vector<wstring> tokens = this->ru->tokenize(clean_header);
	                    threads.add(filename_only, tokens, raw_header, datetime);
	                }
                }
            }
        }

        map<wstring, vector<Record>> thread_data_raw = threads.get();
        map<wstring, vector<wstring>> thread_data;

        for(const auto & [thread_key, records] : thread_data_raw)
        {
            thread_data[thread_key] = vector<wstring>();
            for(const auto & record : records)
            {
                thread_data[thread_key].push_back(record.filename);
            }
        }

        this->jsonParser->save_articles_categorized(L"title", thread_data);
    }

    void make_top()
    {
    	const int max_pronouns_count = 2;
        const vector<wstring> html_headers = {
            L"<h1>", L"<h2>", L"<h3>", L"<h4>", L"<h5>", L"<h6>"
        };
        const int max_header_count = 3;

        map<wstring, wstring> category_data;
        Threads threads(header_similar_share);

        for (const auto & entry : experimental::filesystem::directory_iterator(path))
        {
            string filename = entry.path();
            wstring filename_only = get_filename_only(filename);
            wstring text = read_unicode(filename);
            wstring clean_text = get_clean_text(text);

            // mark articles with many headers as no-news
            int header_count = accum(calc_token_occurences(text, html_headers));
            if(header_count > max_header_count)
            {
                continue;
            }

            int alphabet_type = get_text_alphabet_type(clean_text, this->word_share);

            if(alphabet_type != CharType::NONE)
            {
                time_t datetime = extract_datetime(text);
                wstring raw_header = get_raw_header(text);
                wstring clean_header = get_clean_header(raw_header);
                wstring unquoted_text = get_unquoted_text(text);
                wstring category_name;

                if(alphabet_type == CharType::LATIN)
                {
                	if(this->en->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
	                    vector<wstring> header_tokens = this->en->tokenize(clean_header);
	                    vector<wstring> text_tokens = this->en->tokenize(text);
	                    threads.add(filename_only, header_tokens, raw_header, datetime);
	                    category_name = this->en->get_category_winner(text_tokens);
	                }
                }
                else
                {
                	if(this->ru->is_news(clean_header, unquoted_text, max_pronouns_count))
                    {
	                    vector<wstring> header_tokens = this->ru->tokenize(clean_header);
	                    vector<wstring> text_tokens = this->ru->tokenize(text);
	                    threads.add(filename_only, header_tokens, raw_header, datetime);
	                    category_name = this->ru->get_category_winner(text_tokens);
	                }
                }

                category_data[filename_only] = category_name;
            }
        }

        map<wstring, vector<Thread>> ranked_data = threads.get_ranked(category_data);
        this->jsonParser->save_articles_ranked(L"category", ranked_data);
    }
};

int main(int argc, char** argv)
{
    const float word_share = 0.95;
    const float header_similar_share = 0.5;

    setlocale(LC_ALL, "");

    if(argc != 3)
    {
        cout <<
            "tgnews languages source_dir\n"
            "tgnews news source_dir\n"
            "tgnews categories source_dir\n"
            "tgnews threads source_dir\n"
            "tgnews top source_dir"
        << endl;
        return 1;
    }

    string option_name = argv[1];
    string source_dir = argv[2];
    TGNews tg(source_dir, word_share, header_similar_share);

    if(option_name == "languages")
    {
        tg.find_enru();
    }
    else if(option_name == "news")
    {
        tg.find_news();
    }
    else if(option_name == "categories")
    {
        tg.group_by_category();
    }
    else if(option_name == "threads")
    {
        tg.group_by_threads();
    }
    else if(option_name == "top")
    {
        tg.make_top();
    }
    
    return 0;
}