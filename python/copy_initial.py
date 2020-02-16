import os
import tqdm
import shutil

in_dir = '../../DataClusteringSample/DataClusteringSample2225/201911{:02d}/{:02d}/'
out_dir = '../inputs/initial/'

if os.path.exists(out_dir):
    shutil.rmtree(out_dir)
os.makedirs(out_dir)

max_count = 100003

with tqdm.tqdm(total=max_count) as tqdm_bar:
    for i in range(22, 26):
        for j in range(24):
            in_dir_now = in_dir.format(i, j)

            for in_dir_file in os.listdir(in_dir_now):
                shutil.copyfile(in_dir_now + in_dir_file, out_dir + in_dir_file)
                tqdm_bar.update(1)

                if tqdm_bar.n == max_count:
                    break

            if tqdm_bar.n == max_count:
                break

        if tqdm_bar.n == max_count:
            break
