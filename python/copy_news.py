import os
import json
import shutil

for language in ['en', 'ru']:
	with open("../outputs/{}_news.json".format(language), "r") as en_ru:
	    json_data = json.load(en_ru)

	in_dir = '../inputs/{}/'.format(language)
	out_dir = '../inputs/{}_news/'.format(language)

	if os.path.exists(out_dir):
	    shutil.rmtree(out_dir)
	os.makedirs(out_dir)

	for article_name in json_data['articles']:
	    shutil.copyfile((in_dir + '{}').format(article_name),
	    	(out_dir + '{}').format(article_name))
