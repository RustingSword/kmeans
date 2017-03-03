# Parallel k-means/k-means++

Yet another wheel.

# Usage

## Cluster

    $ make
    g++-6 -fopenmp -std=c++11 -Wall -Wfatal-errors -O2 -I include -c src/kmeans.cpp -o src/kmeans.o
    mkdir -p bin
    g++-6 -fopenmp -std=c++11 -Wall -Wfatal-errors -O2 -I include src/kmeans.o test/kmeans.cpp -o bin/kmeans
    g++-6 -fopenmp -std=c++11 -Wall -Wfatal-errors -O2 -I include src/kmeans.o test/test_fit.cpp -o bin/test_fit
    g++-6 -fopenmp -std=c++11 -Wall -Wfatal-errors -O2 -I include src/kmeans.o test/test_predict.cpp -o bin/test_predict
    g++-6 -fopenmp -std=c++11 -Wall -Wfatal-errors -O2 -I include src/kmeans.o test/test_save_load_model.cpp -o bin/test_save_load_model
    $ ./bin/cluster data/test_data 10 4  # use 4 threads to cluster test_data into 10 clusters
    INFO    2017-02-08 17:28:18.828 set_num_cluster@kmeans.h:50] set number of clusters to 10
    INFO    2017-02-08 17:28:18.829 set_num_threads@kmeans.h:44] set number of threads to 4
    INFO    2017-02-08 17:28:18.829 fit@kmeans.cpp:296] loading data from data/test_data
    INFO    2017-02-08 17:28:19.061 fit@kmeans.cpp:303] fitting data with n=70000 d=2 k=10
    INFO    2017-02-08 17:28:19.061 fit@kmeans.cpp:306] seeding centers...
    DEBUG   2017-02-08 17:28:19.062 kmeans_plusplus_init@kmeans.cpp:248] sampling center 1
    DEBUG   2017-02-08 17:28:19.062 kmeans_plusplus_init@kmeans.cpp:248] sampling center 2
    DEBUG   2017-02-08 17:28:19.063 kmeans_plusplus_init@kmeans.cpp:248] sampling center 3
    DEBUG   2017-02-08 17:28:19.064 kmeans_plusplus_init@kmeans.cpp:248] sampling center 4
    DEBUG   2017-02-08 17:28:19.065 kmeans_plusplus_init@kmeans.cpp:248] sampling center 5
    DEBUG   2017-02-08 17:28:19.066 kmeans_plusplus_init@kmeans.cpp:248] sampling center 6
    DEBUG   2017-02-08 17:28:19.067 kmeans_plusplus_init@kmeans.cpp:248] sampling center 7
    DEBUG   2017-02-08 17:28:19.068 kmeans_plusplus_init@kmeans.cpp:248] sampling center 8
    DEBUG   2017-02-08 17:28:19.070 kmeans_plusplus_init@kmeans.cpp:248] sampling center 9
    INFO    2017-02-08 17:28:19.071 fit@kmeans.cpp:312] start clustering...
    INFO    2017-02-08 17:28:19.074 fit@kmeans.cpp:332] iter: 1 reassign_ratio: 1 cost: 540749
    INFO    2017-02-08 17:28:19.075 fit@kmeans.cpp:332] iter: 2 reassign_ratio: 0.0462429 cost: 177916
    INFO    2017-02-08 17:28:19.076 fit@kmeans.cpp:332] iter: 3 reassign_ratio: 0.000757143 cost: 139468
    INFO    2017-02-08 17:28:19.078 fit@kmeans.cpp:332] iter: 4 reassign_ratio: 0 cost: 139458
    INFO    2017-02-08 17:28:19.078 fit@kmeans.cpp:335] finished

## Plot cluster result
    ./tools/plot_cluster.py data/test_data kmeans.labels kmeans.model

sample result:
![cluster result](/clusters.png)
