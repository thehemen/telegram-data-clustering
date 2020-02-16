import json

max_rows = 16
filenames = ['../outputs/ru_top.json', '../outputs/en_top.json']

for filename in filenames:
    with open(filename, 'r') as f:
        json_rows = json.load(f)

        for json_row in json_rows:
            category = json_row['category']
            threads = json_row['threads']

            print(category)
            for i, thread in enumerate(threads):
                title = thread['title']
                articles = thread['articles']

                print('\t{}'.format(title))
                for article in articles:
                    print('\t\t{}'.format(article))

                if i == max_rows:
                    break

