#include "kmeans.h"
#include "utils.h"
#include <iostream>
int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
            << " <data> <num_cluster> <num_threads>" << std::endl;
        return -1;
    }
    log_level = DEBUG;
    cluster::Kmeans<float> kmeans;
    int num_cluster = atoi(argv[2]);
    int num_threads = atoi(argv[3]);
    if (num_cluster <= 0 || num_threads <= 0) {
        LOG(ERROR) << "both num_cluster and num_threads should be positive "
            "integers";
        return 0;
    }
    if (num_cluster > 500) {
        kmeans.set_init_method(cluster::InitMethod::RANDOM);
    }
    kmeans.set_num_cluster(num_cluster);
    kmeans.set_num_threads(num_threads);
    auto ret = kmeans.fit(argv[1]);
    assert(ret == cluster::Status::OK);
    kmeans.save_labels("kmeans.labels");
    kmeans.save_model("kmeans.model");
    return 0;
}
