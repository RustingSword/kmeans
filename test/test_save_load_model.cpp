#include "kmeans.h"
#include "utils.h"

using namespace std;

int main() {
  log_level = DEBUG;
  cluster::Kmeans<float> kmeans;
  vector<vector<float>> centers{{1., 2., 3.}, {4., 5., 6.}};
  auto centers_original = centers;

  // test set_centers
  auto ret = kmeans.set_centers(centers);
  assert(ret == cluster::Status::OK);

  // test write to file with no permission
  ret = kmeans.save_model("/unable_to_write");
  assert(ret == cluster::Status::IO_ERROR);

  // test write to normal file
  ret = kmeans.save_model("test_model");
  assert(ret == cluster::Status::OK);

  // test load from inexistent file
  ret = kmeans.load_model("/inexistent_file");
  assert(ret == cluster::Status::IO_ERROR);

  // test load from normal file
  ret = kmeans.load_model("test_model");
  assert(ret == cluster::Status::OK);

  // test get centers
  auto centers_loaded = kmeans.centers();
  assert(centers_original == centers_loaded);

  Test::test_passed("test save_load_model");
  return 0;
}

// vim: ts=2 sts=2 sw=2
