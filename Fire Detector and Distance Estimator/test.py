import shutil
import os
import csv
if os.path.exists('results/') : 
    shutil.rmtree('/results', ignore_errors=True)
else : 
    os.mkdir('results/')
    os.mkdir('results/data/')
    os.mkdir('results/frames/')
    with open('results/data/data.csv', 'w', encoding='utf-8', newline='') as csvFile:
        writer = csv.writer(csvFile)
        writer.writerow(['frame', 'xmin', 'ymin', 'xmax', 'ymax', 'scaled_xmin',
                        'scaled_ymin', 'scaled_xmax', 'scaled_ymax'])
