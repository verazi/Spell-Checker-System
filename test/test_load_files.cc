#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "load_files.h"
#include "codes.h"

using namespace std;

class LoadFilesTest : public ::testing::Test {
 protected:
  void SetUp() override {
    
    test_file_ = "test_temp_file.txt";
    empty_file_ = "test_empty_file.txt";
    
    ofstream file(test_file_);
    file << "Hello\nWorld\nTest\n";
    file.close();
    
    ofstream empty_file(empty_file_);
    empty_file.close();
  }
  
  void TearDown() override {
    
    remove(test_file_.c_str());
    remove(empty_file_.c_str());
  }
  
  string test_file_;
  string empty_file_;
};

TEST_F(LoadFilesTest, LoadValidFile) {
  size_t size;
  char* data = LoadFile(test_file_.c_str(), &size);
  
  ASSERT_NE(data, nullptr);
  EXPECT_EQ(size, 17);string content(data, size);
  EXPECT_EQ(content, "Hello\nWorld\nTest\n");
  
  free(data);
}


TEST_F(LoadFilesTest, FileSize) {
  
  size_t size;
  char* data = LoadFile(test_file_.c_str(), &size);
  
  ifstream file(test_file_, ios::ate);
  size_t expected_size = file.tellg();
  
  EXPECT_EQ(size, expected_size);
  
  free(data);
}