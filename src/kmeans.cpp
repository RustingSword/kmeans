#include "kmeans.h"
#include <omp.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <limits>
#include <set>
#include <random>

LogLevel log_level = INFO;
namespace cluster {

const char* init_methods[2] = {"random", "k-means++"};

template <typename DType>
Kmeans<DType>::Kmeans(int n_cluster, int n_thread, int n_iter, float threshold,
    InitMethod init) :
  n_cluster_(n_cluster), n_thread_(n_thread), n_iter_(n_iter),
  threshold_(threshold), init_(init), num_reassigned_(0) {
}

template <typename DType>
std::vector<DType> Kmeans<DType>::parse_sample_from_string(std::string line) {
  std::vector<DType> sample;
  std::istringstream coordinates(line);
  std::copy(std::istream_iterator<DType>(coordinates),
      std::istream_iterator<DType>(),
      std::back_inserter(sample));
  return sample;
}

template <typename DType>
Status Kmeans<DType>::load_data(const char *filename,
                                std::vector<std::vector<DType>> &data) {
  std::ifstream fin(filename);
  if (!fin) {
    LOG(ERROR) << "unable to open file \"" << filename << "\" to read";
    return Status::IO_ERROR;
  }

  data.clear();

  std::string line;
  while (getline(fin, line)) {
    auto sample = parse_sample_from_string(line);
    data.push_back(sample);
  }

  return Status::OK;
}  // load_data

template <typename DType>
Status Kmeans<DType>::save_model(const char *model_path) {
  std::ofstream fout(model_path);
  if (!fout) {
    LOG(ERROR) << "unable to open file \"" << model_path << "\" to write";
    return Status::IO_ERROR;
  }

  for (auto const &center : centers_) {
    for (auto value : center) {
      fout << value << ' ';
    }
    fout << std::endl;
  }
  fout.close();
  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::load_model(const char *model_path) {
  return load_data(model_path, centers_);
}

template <typename DType>
Status Kmeans<DType>::save_labels(const char *label_path) {
  std::ofstream fout(label_path);
  if (!fout) {
    LOG(ERROR) << "unable to open file \"" << label_path << "\" to write";
    return Status::IO_ERROR;
  }

  for (auto label : labels_) {
    fout << label << std::endl;
  }
  fout.close();

  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::dist(std::vector<DType> &p, std::vector<DType> &q, DType &d) {
  d = 0;
  if (p.size() != q.size()) {
    return Status::DIM_ERROR;
  }
  for (size_t i = 0; i < p.size(); ++i) {
    d += (p[i] - q[i])*(p[i] - q[i]);
  }
  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::predict(std::vector<DType> &data_point,
    DType &min_dist, int &label) {
  if (data_point.size() != centers_[0].size()) {
    return Status::DIM_ERROR;
  }

  label = -1;
  DType cur_dist;
  min_dist = std::numeric_limits<DType>::max();
  for (size_t i = 0; i < centers_.size(); ++i) {
    auto ret = dist(data_point, centers_[i], cur_dist);
    if (ret != Status::OK) {
      return ret;
    }
    if (cur_dist < min_dist) {
      label = static_cast<int>(i);
      min_dist = cur_dist;
    }
  }
  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::predict(std::vector<std::vector<DType>> &data_points,
    std::vector<int> &labels) {
  labels.clear();
  if (n_thread_ > 1) {
    LOG(DEBUG) << "parallel predicting using " << n_thread_ << " threads";
    Status ret = Status::OK;
#pragma omp parallel for ordered num_threads(n_thread_)
    for (size_t i = 0; i < data_points.size(); ++i) {
      int label;
      DType min_dist;
      auto r = predict(data_points[i], min_dist, label);
      if (r != Status::OK) {
        ret = r;
        LOG(ERROR) << "data point " << i << " has inconsistent dimension";
      }
#pragma omp ordered
      labels.push_back(label);
    }
    if (ret != Status::OK) {
      return ret;
    }
  } else {
    for (auto &data_point : data_points) {
      int label;
      DType min_dist;
      auto ret = predict(data_point, min_dist, label);
      if (ret != Status::OK) {
        return ret;
      }
      labels.push_back(label);
    }
  }
  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::init(std::vector<std::vector<DType>> &data) {
  // allocate memory for variables
  thread_center_ids_.resize(n_thread_);
  thread_centers_.resize(n_thread_);

  centers_.resize(n_cluster_);
  center_ids_.resize(n_cluster_);

  // init labels to -1
  labels_.resize(data.size());
  std::fill(labels_.begin(), labels_.end(), -1);

  // init centers
  Status ret = Status::OK;
  switch (init_) {
    case InitMethod::RANDOM:
      ret = random_init(data);
      if (ret != Status::OK) {
        return ret;
      }
      break;
    case InitMethod::KMEANS_PLUSPLUS:
      ret = kmeans_plusplus_init(data);
      if (ret != Status::OK) {
        return ret;
      }
      break;
    default:
      break;
  }
  for (size_t i = 0; i < centers_.size(); ++i) {
    std::ostringstream ss;
    ss << "center[" << i << "]:";
    for (auto v : centers_[i]) {
      ss << ' '  << v;
    }
    LOG(VERBOSE) << ss.str();
  }
  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::random_init(std::vector<std::vector<DType>> &data) {
  std::set<int> indices;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, data.size()-1);

  centers_.clear();

  for (int i = 0; i < n_cluster_; ++i) {
    int index = dis(gen);
    while (indices.find(index) != indices.end()) {
      index = dis(gen);
    }
    indices.insert(index);
  }

  for (auto index : indices) {
    centers_.push_back(data[index]);
  }
  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::kmeans_plusplus_init(std::vector<std::vector<DType>> &data) {
  std::set<int> indices;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<DType> dis(0.0, 1.0);

  centers_.clear();

  // randomly sample first center
  indices.insert(static_cast<int>(dis(gen) * data.size()));

  std::vector<DType> dists(data.size());

  // sample rest n_cluster_ - 1 centers
  for (int i = 1; i < n_cluster_; ++i) {
    LOG(DEBUG) << "sampling center " << i;
    DType sum_dists = 0.0;
    auto ret = Status::OK;
#pragma omp parallel for num_threads(n_thread_) reduction(+:sum_dists)
    for (int j = 0; j < static_cast<int>(data.size()); ++j) {
      DType cur_dist = 0.0, min_dist = std::numeric_limits<DType>::max();
      for (auto id : indices) {
        auto status = dist(data[j], data[id], cur_dist);
        if (status != Status::OK) {
          ret = status;
        }
        if (cur_dist < min_dist) {
          min_dist = cur_dist;
        }
      }
      sum_dists += min_dist;
      dists[j] = min_dist;
    }
    if (ret != Status::OK) {
      return ret;
    }

    while (true) {
      DType cutoff_dist = dis(gen) * sum_dists, cur_dist_sum = 0.0;
      int j = 0;
      for (j = 0; j < static_cast<int>(data.size()); ++j) {
        cur_dist_sum += dists[j];
        if (cur_dist_sum >= cutoff_dist) {
          indices.insert(j);
          break;
        }
      }
      if (j < static_cast<int>(data.size())) {
        break;
      }
    }
  }

  for (auto index : indices) {
    centers_.push_back(data[index]);
  }

  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::fit(const char *input_file) {
  std::vector<std::vector<DType>> data;
  LOG(INFO) << "loading data from " << input_file;
  auto ret = load_data(input_file, data);
  if (ret != Status::OK) {
    return ret;
  }
  return fit(data);
}

template <typename DType>
Status Kmeans<DType>::fit(std::vector<std::vector<DType>> &data) {
  LOG(INFO) << "fitting data with n=" << data.size()
    << " d=" << data[0].size()
    << " k=" << n_cluster_;
  LOG(INFO) << "seeding centers...";
  auto ret = init(data);
  if (ret != Status::OK) {
    return ret;
  }

  LOG(INFO) << "start clustering...";
  int iter = 0;
  float reassign_ratio = 1.;
  DType total_cost = 0.0;
  while (iter < n_iter_ && reassign_ratio >= threshold_) {
    // clear membership
    for (size_t i = 0; i < center_ids_.size(); ++i) {
      center_ids_[i].clear();
    }
    num_reassigned_ = 0;
    auto ret = Status::OK;
    if (n_thread_ > 1) {
      ret = parallel_lloyd(data, total_cost);
    } else {
      ret = sequential_lloyd(data, total_cost);
    }
    if (ret != Status::OK)
      return ret;
    reassign_ratio = 1.0 * num_reassigned_ / data.size();
    ++iter;
    LOG(INFO) << "iter: " << iter << " reassign_ratio: " << reassign_ratio
      << " cost: " << total_cost;
  }
  LOG(INFO) << "finished";
  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::sequential_lloyd(std::vector<std::vector<DType>> &data,
    DType &total_cost) {
  total_cost = 0;
  // update membership
  for (size_t i = 0; i < data.size(); ++i) {
    int label = -1;
    DType min_dist = 0.0;
    predict(data[i], min_dist, label);
    total_cost += min_dist;
    center_ids_[label].push_back(static_cast<int>(i));
    if (label != labels_[i]) {
      num_reassigned_++;
      labels_[i] = label;
    }
  }

  // update centers
  for (int i = 0; i < n_cluster_; ++i) {
    std::vector<DType> center(data[0].size());
    for (auto id : center_ids_[i]) {  // iterate over members of cluster[i]
      for (size_t j = 0; j < data[id].size(); ++j) {
        center[j] += data[id][j];
      }
    }
    if (center_ids_[i].size() > 0) {  // skip empty cluster
      for (size_t j = 0; j < center.size(); ++j) {
        center[j] /= center_ids_[i].size();
      }
      centers_[i] = center;
    }
  }

  return Status::OK;
}

template <typename DType>
Status Kmeans<DType>::parallel_lloyd(std::vector<std::vector<DType>> &data,
    DType &total_cost) {
  total_cost = 0;
  // Older OpenMP cannot carry out reduction on class member (num_reassigned_
  // here), we need to use a temporary vector to accumulate this in each thread.
  std::vector<int> num_reassigned(n_thread_);
#pragma omp parallel num_threads(n_thread_)
  {
    int tid = omp_get_thread_num();
    thread_center_ids_[tid].clear();
    thread_center_ids_[tid].resize(n_cluster_);
    thread_centers_[tid].resize(n_cluster_);
    for (auto &tc : thread_centers_[tid])
      tc.resize(data[0].size());
#pragma omp for reduction(+:total_cost)
    for (size_t i = 0; i < data.size(); ++i) {
      int label = 0;
      DType min_dist;
      predict(data[i], min_dist, label);
      total_cost += min_dist;
      thread_center_ids_[tid][label].push_back(static_cast<int>(i));
      if (label != labels_[i]) {
        num_reassigned[tid]++;
        labels_[i] = label;
      }

      for (size_t j = 0; j < data[i].size(); ++j) {
        thread_centers_[tid][label][j] += data[i][j];
      }
    }
  }
  for (int i = 0; i < n_thread_; ++i) {
    num_reassigned_ += num_reassigned[i];
  }

  // main thread reduce centers of each thread
  for (int i = 0; i < n_cluster_; ++i) {
    int num_samples = 0;
    for (int j = 0; j < n_thread_; ++j) {
      for (size_t k = 0; k < data[0].size(); ++k) {
        centers_[i][k] += thread_centers_[j][i][k];
        thread_centers_[j][i][k] = 0.0;
      }
      num_samples += static_cast<int>(thread_center_ids_[j][i].size());
    }

    if (num_samples > 1) {
      for (size_t k = 0; k < centers_[i].size(); ++k) {
        centers_[i][k] /= num_samples;
      }
    }
  }

  return Status::OK;
}

template class Kmeans<float>;
template class Kmeans<double>;
}  // namespace cluster

// vim: ts=2 sts=2 sw=2
