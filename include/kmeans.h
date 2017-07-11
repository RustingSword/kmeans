#ifndef KMEANS_H
#define KMEANS_H

#include "utils.h"

#include <iostream>
#include <vector>

namespace cluster {

enum class InitMethod { RANDOM, KMEANS_PLUSPLUS, KMEANS_PARALLEL };
enum class Status { OK, IO_ERROR, DIM_ERROR };
extern const char* init_methods[3];

template <typename DType>
class Kmeans {
  public:
    Kmeans(int n_cluster = 8,
           int n_thread = 1,
           int n_iter = 100,
           float threshold = 0.0001,  /* ratio of reassigned samples */
           InitMethod init = InitMethod::KMEANS_PLUSPLUS);
    ~Kmeans(){}

    Status fit(const char *input_file);
    Status fit(std::vector<std::vector<DType>> &data, bool seeded = false);

    Status predict(std::vector<DType> &data_point, DType &min_dist, int &label);
    Status predict(std::vector<std::vector<DType>> &data_points,
                   std::vector<int> &labels);

    Status save_model(const char *model_path);
    Status load_model(const char *model_path);
    Status save_labels(const char *label_path);

    Status set_centers(std::vector<std::vector<DType>> &centers) {
      centers_ = std::move(centers);
      return Status::OK;
    }
    const std::vector<std::vector<DType>>& centers() const { return centers_; }
    const std::vector<int>& labels() const { return labels_; }

    Status set_num_threads(int n_thread) {
      LOG(INFO) << "set number of threads to " << n_thread;
      n_thread_ = n_thread;
      return Status::OK;
    }

    Status set_num_cluster(int n_cluster) {
      LOG(INFO) << "set number of clusters to " << n_cluster;
      n_cluster_ = n_cluster;
      return Status::OK;
    }

    Status set_init_method(InitMethod init) {
      LOG(INFO) << "set init method to " << init_methods[static_cast<int>(init)];
      init_ = init;
      return Status::OK;
    }

    Status set_init_method(InitMethod init, int l, int r) {
      assert(init == InitMethod::KMEANS_PARALLEL);
      LOG(INFO) << "set init method to " << init_methods[static_cast<int>(init)]
        << ", l = " << l << ", r = " << r;
      init_ = init;
      kmeans_parallel_l_ = l;
      kmeans_parallel_r_ = r;
      return Status::OK;
    }

  private:
    int n_cluster_;
    int n_thread_;
    int n_iter_;
    float threshold_;
    InitMethod init_;
    int kmeans_parallel_l_;
    int kmeans_parallel_r_;
    std::vector<std::vector<DType>> centers_;
    std::vector<std::vector<std::vector<DType>>> thread_centers_;
    std::vector<std::vector<int>> center_ids_;
    std::vector<std::vector<std::vector<int>>> thread_center_ids_;
    std::vector<int> labels_;
    int num_reassigned_;

    std::vector<DType> parse_sample_from_string(std::string line);
    Status init(std::vector<std::vector<DType>> &data);
    Status dist(std::vector<DType> &p, std::vector<DType> &q, DType &d /*out*/);
    Status load_data(const char *filename, std::vector<std::vector<DType>> &data);

    Status random_init(std::vector<std::vector<DType>> &data);
    Status kmeans_plusplus_init(std::vector<std::vector<DType>> &data);
    Status kmeans_parallel_init(std::vector<std::vector<DType>> &data);
    Status sequential_lloyd(std::vector<std::vector<DType>> &data, DType &cost);
    Status parallel_lloyd(std::vector<std::vector<DType>> &data, DType &cost);
};  // class Kmeans

}  // namespace cluster

#endif  // KMEANS_H

// vim: ts=2 sts=2 sw=2
