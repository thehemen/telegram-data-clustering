#include <iostream>
#include "json.h"

JSONParser::JSONParser() {}

void JSONParser::save_articles(vector<wstring> articles)
{
    wcout << "{" << "\n";
    wcout << "\t\"articles\": [\n";

    int article_num = articles.size();
    for(int i = 0; i < article_num; ++i)
    {
        wcout << "\t\t\"" << articles[i] << "\"";

        if(i < article_num - 1)
        {
            wcout << ",";
        }

        wcout << "\n";
    }

    wcout << "\t]\n";
    wcout << "}" << endl;
}

void JSONParser::save_articles_categorized(wstring category_name, map<wstring, vector<wstring>> lang_data)
{
    wcout << "[" << "\n";

    int lang_num = lang_data.size();
    int lang_index = 0;
    for(const auto & [key, articles] : lang_data)
    {
        wcout << "\t{" << "\n";
        wcout << "\t\t\"" << category_name << "\": \"" << key << "\",\n";
        wcout << "\t\t\"articles\": [\n";

        int article_num = articles.size();
        for(int i = 0; i < article_num; ++i)
        {
            wcout << "\t\t\t\"" << articles[i] << "\"";

            if(i < article_num - 1)
            {
                wcout << ",";
            }

            wcout << "\n";
        }

        wcout << "\t\t]\n";
        wcout << "\t}";

        if(lang_index < lang_num - 1)
        {
            wcout << ",";
        }

        wcout << "\n";
        lang_index++;
    }

    wcout << "]" << endl;
}

void JSONParser::save_articles_ranked(wstring category_name, map<wstring, vector<Thread>> top_data)
{
    wcout << "[" << "\n";

    int cat_num = top_data.size();
    int lang_index = 0;
    for(const auto & [category_now, threads] : top_data)
    {
        wcout << "\t{" << "\n";
        wcout << "\t\t\"" << category_name << "\": \"" << category_now << "\",\n";
        wcout << "\t\t\"threads\": [\n";

        int thread_num = threads.size();
        for(int i = 0; i < thread_num; ++i)
        {
            auto thread = threads[i];
            wstring category_name = thread.category;
            wstring main_header = thread.header;
            vector<wstring> articles = thread.filenames;

            wcout << "\t\t{\n";
            wcout << "\t\t\t\"title\": \"" << main_header << "\",\n";

            if(category_now == L"any")
            {
                wcout << "\t\t\t\"category\": \"" << category_name << "\",\n";
            }

            wcout << "\t\t\t\"articles\": [\n";

            int article_num = articles.size();
            for(int j = 0; j < article_num; ++j)
            {
                wcout << "\t\t\t\t\"" << articles[j] << "\"";

                if(j < article_num - 1)
                {
                    wcout << ",";
                }

                wcout << "\n";
            }

            wcout << "\t\t\t]\n";
            wcout << "\t\t}";

            if(i < thread_num - 1)
            {
                wcout << ",";
            }

            wcout << "\n";
        }

        wcout << "\t\t]\n";
        wcout << "\t}";

        if(lang_index < cat_num - 1)
        {
            wcout << ",";
        }

        wcout << "\n";
        lang_index++;
    }

    wcout << "]" << endl;
}