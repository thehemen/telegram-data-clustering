import json

max_rows = 16
filenames = ['../outputs/ru_threads.json', '../outputs/en_threads.json']

for filename in filenames:
    with open(filename, 'r') as f:
        json_rows = json.load(f)

        for i, json_row in enumerate(json_rows):
            title = json_row['title']
            articles = json_row['articles']

            print(title)
            for article in articles:
                print('\t{}'.format(article))

            if i == max_rows:
                break

