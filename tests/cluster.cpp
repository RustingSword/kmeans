#include "kmeans.h"
#include "utils.h"
#include <iostream>
int main(int argc, char **argv) {
    log_level = DEBUG;
    if (argc != 4) {
        LOG(ERROR) << "Usage: " << argv[0]
            << " <data> <num_cluster> <num_threads>";
        return 0;
    }
    cluster::Kmeans<float> kmeans;
    int num_cluster = atoi(argv[2]);
    int num_threads = atoi(argv[3]);
    if (num_cluster <= 0 || num_threads <= 0) {
        LOG(ERROR) << "both num_cluster and num_threads should be positive "
            "integers";
        return -1;
    }
    if (num_cluster > 10) {
        kmeans.set_init_method(cluster::InitMethod::KMEANS_PARALLEL, 2 * num_cluster, 5);
    }
    kmeans.set_num_cluster(num_cluster);
    kmeans.set_num_threads(num_threads);
    auto ret = kmeans.fit(argv[1]);
    assert(ret == cluster::Status::OK);
    kmeans.save_labels("kmeans.labels");
    kmeans.save_model("kmeans.model");
    return 0;
}
