#include "kmeans.h"
#include "utils.h"

int main() {
  log_level = DEBUG;
  cluster::Kmeans<float> kmeans(2);
  std::vector<std::vector<float>> centers{{1., 2., 3.}, {4., 5., 6.}};
  kmeans.set_centers(centers);

  std::vector<float> p1{0.5, 1.5, 2.5}, p2{4.5, 5.5, 6.5}, p3{5, 6};
  int label;

  // test prediction
  float dist;
  auto ret = kmeans.predict(p1, dist, label);
  assert(ret == cluster::Status::OK);
  assert(label == 0);

  ret = kmeans.predict(p2, dist, label);
  assert(ret == cluster::Status::OK);
  assert(label == 1);

  // test prediction with inconsistent dimension
  ret = kmeans.predict(p3, dist, label);
  assert(ret == cluster::Status::DIM_ERROR);

  // test parallel prediction
  std::vector<int> labels;
  std::vector<std::vector<float>> points{p1, p2};
  kmeans.set_num_threads(2);
  ret = kmeans.predict(points, labels);
  assert(ret == cluster::Status::OK);
  assert(labels[0] == 0);
  assert(labels[1] == 1);

  // test parallel prediction with inconsistent dimension
  points = {p1, p2, p3};
  ret = kmeans.predict(points, labels);
  assert(ret == cluster::Status::DIM_ERROR);

  Test::test_passed("test predict");
  return 0;
}

// vim: ts=2 sts=2 sw=2
