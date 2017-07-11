#include <random>
#include <iomanip>
#include "kmeans.h"
#include "utils.h"

int main(int argc, char **argv) {
  std::vector<std::vector<float>> data;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<> dis(0, 1);

  log_level = DEBUG;
  cluster::Kmeans<float> kmeans;

  // generate 10 clusters with 7000 2-d samples each
  for (int i = 0; i < 10; ++i) {
    float x = dis(gen) * 10;
    float y = dis(gen) * 10;
    for (int j = 0; j < 7000; ++j) {
      std::vector<float> sample;
      sample.push_back(dis(gen) + x);
      sample.push_back(dis(gen) + y);
      data.push_back(sample);
    }
  }

  kmeans.set_num_cluster(10);
  kmeans.set_num_threads(4);
  auto ret = kmeans.fit(data);
  assert(ret == cluster::Status::OK);

  Test::test_passed("test fit");
  return 0;
}
// vim: ts=2 sts=2 sw=2
