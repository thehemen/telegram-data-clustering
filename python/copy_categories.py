import os
import json
import shutil

in_dir = '../inputs/{}_news/'
out_dir = '../inputs/{}_{}/'

languages = ['en', 'ru']
categories = ['society', 'economy', 'technology', 'sports', 'entertainment', 'science', 'other']

for language in languages:
    with open("../outputs/{}_cat.json".format(language), "r") as f:
        json_data = json.load(f)

    for category in categories:
        in_dir_now = in_dir.format(language)
        out_dir_now = out_dir.format(language, category)

        if os.path.exists(out_dir_now):
            shutil.rmtree(out_dir_now)
        os.makedirs(out_dir_now)

        for i in range(len(categories)):
            category_now = json_data[i]['category']
            if category_now == category:
                for article_name in json_data[i]['articles']:
                    shutil.copyfile(in_dir_now + article_name, out_dir_now + article_name)
            else:
                continue
