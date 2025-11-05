#include <gtest/gtest.h>

#include "test_bk_tree.cc"
#include "test_candidates.cc"
#include "test_dictionary.cc"
#include "test_edit_distance.cc"
#include "test_load_files.cc"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}