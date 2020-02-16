import os
import json
import shutil

with open("../outputs/enru.json", "r") as en_ru:
    json_data = json.load(en_ru)

in_dir = '../inputs/initial/'
out_dir = '../inputs/{}/'

for language in ['en', 'ru']:
    if os.path.exists(out_dir.format(language)):
        shutil.rmtree(out_dir.format(language))
    os.makedirs(out_dir.format(language))

for i in range(2):
    language = json_data[i]['lang_code']
    for article_name in json_data[i]['articles']:
        shutil.copyfile((in_dir + '{}').format(article_name),
            (out_dir + '{}').format(language, article_name))
