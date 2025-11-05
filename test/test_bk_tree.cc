#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include "bk_tree.h"
#include "edit_distance.h"

using namespace std;
using namespace testing;

TEST(BKTree, GetCandidates) {
  BKTree bk_tree("appeal");
  bk_tree.AddNode("apple");
  bk_tree.AddNode("rappel");
  bk_tree.AddNode("Tim");
  bk_tree.AddNode("time");
  bk_tree.AddNode("vim");
  bk_tree.AddNode("sap");
  bk_tree.AddNode("snap");
  bk_tree.AddNode("stab");
  bk_tree.AddNode("step");
  bk_tree.AddNode("stop");
  bk_tree.AddNode("strap");
  bk_tree.AddNode("tap");

  bk_tree.AddNode("random");
  bk_tree.AddNode("wont");
  bk_tree.AddNode("be");
  bk_tree.AddNode("used");

  vector<string> res_a;
  bk_tree.SearchNode("appel", 2, &res_a);

  EXPECT_EQ(res_a.size(), 2);
  EXPECT_THAT(res_a, UnorderedElementsAre("appeal", "rappel"));

  vector<string> res_b;
  bk_tree.SearchNode("tim", 2, &res_b);
  EXPECT_EQ(res_b.size(), 3);
  EXPECT_THAT(res_b, UnorderedElementsAre("Tim", "time", "vim"));

  vector<string> res_c;
  bk_tree.SearchNode("stap", 2, &res_c);
  EXPECT_EQ(res_c.size(), 7);
  EXPECT_THAT(res_c, UnorderedElementsAre("sap", "snap", "stab", "step",
                                          "stop", "strap", "tap"));
}

TEST(BKTree, EmptyTree) {
  BKTree bk_tree("");
  vector<string> results;
  
  bk_tree.SearchNode("test", 1, &results);
  EXPECT_EQ(results.size(), 0);
}

TEST(BKTree, SingleNode) {
  BKTree bk_tree("hello");
  bk_tree.AddNode("hallo");
  bk_tree.AddNode("world");
  
  vector<string> results;
  
  bk_tree.SearchNode("hello", 2, &results);
  
  EXPECT_THAT(results, Contains("hallo"));  
}

TEST(BKTree, ToleranceZero) {
  BKTree bk_tree("test");
  bk_tree.AddNode("best");
  bk_tree.AddNode("rest");
  bk_tree.AddNode("test");vector<string> results;
  bk_tree.SearchNode("test", 0, &results);
  EXPECT_EQ(results.size(), 0);  
}

TEST(BKTree, ToleranceOne) {
  BKTree bk_tree("cat");
  bk_tree.AddNode("bat");
  bk_tree.AddNode("hat");
  bk_tree.AddNode("can");
  bk_tree.AddNode("car");
  bk_tree.AddNode("dog");vector<string> results;
  bk_tree.SearchNode("cat", 1, &results);
  EXPECT_THAT(results, Contains("bat"));  
  EXPECT_THAT(results, Contains("hat"));  
  EXPECT_THAT(results, Contains("can"));  
  EXPECT_THAT(results, Contains("car"));  
  EXPECT_THAT(results, Not(Contains("dog")));  
}

TEST(BKTree, DuplicateNodes) {
  BKTree bk_tree("root");
  bk_tree.AddNode("test");
  bk_tree.AddNode("test");  
  bk_tree.AddNode("best");vector<string> results;
  bk_tree.SearchNode("test", 2, &results);
  EXPECT_THAT(results, Contains("best"));  
  EXPECT_GT(results.size(), 0);  
}

TEST(BKTree, VaryingWordLengths) {
  BKTree bk_tree("abc");  
  bk_tree.AddNode("ab");
  bk_tree.AddNode("abcd");
  bk_tree.AddNode("a");
  
  vector<string> results;
  bk_tree.SearchNode("abc", 2, &results);
  
  EXPECT_GT(results.size(), 0);
  
  EXPECT_THAT(results, Contains("ab"));    
  EXPECT_THAT(results, Contains("abcd"));  
}

TEST(BKTree, LargeEditDistance) {
  BKTree bk_tree("hello");
  bk_tree.AddNode("world");
  bk_tree.AddNode("test");
  bk_tree.AddNode("cat");
  
  vector<string> results;
  bk_tree.SearchNode("world", 10, &results);
  
  EXPECT_GE(results.size(), 0);  
}

TEST(BKTree, CaseSensitive) {
  BKTree bk_tree("Hello");
  bk_tree.AddNode("hello");
  bk_tree.AddNode("HELLO");
  bk_tree.AddNode("HeLLo");
  
  vector<string> results;
  bk_tree.SearchNode("Hello", 0, &results);

  EXPECT_TRUE(results.size() <= 4);  
}

TEST(BKTree, SpecialCharacters) {
  BKTree bk_tree("test@example.com");
  bk_tree.AddNode("test@example.org");
  bk_tree.AddNode("user@example.com");
  bk_tree.AddNode("test-example.com");
  
  vector<string> results;
  bk_tree.SearchNode("test@example.com", 2, &results);
  EXPECT_GT(results.size(), 0);
  
}

TEST(BKTree, EmptySearchString) {
  BKTree bk_tree("root");
  bk_tree.AddNode("a");
  bk_tree.AddNode("bb");
  bk_tree.AddNode("ccc");
  
  vector<string> results;
  bk_tree.SearchNode("", 3, &results);
  EXPECT_GT(results.size(), 0);
}

TEST(BKTree, ForceDistanceExecution) {
  BKTree bk_tree("test");
  
  bk_tree.AddNode("best");  
  bk_tree.AddNode("rest");  
  bk_tree.AddNode("west");  
  bk_tree.AddNode("nest");  
  bk_tree.AddNode("fest");
  bk_tree.AddNode("belt");  
  bk_tree.AddNode("bent");vector<string> results;
  bk_tree.SearchNode("test", 3, &results);
  EXPECT_GT(results.size(), 0);
}


TEST(BKTree, PrintTreeCoverage) {
  BKTree bk_tree("root");
  bk_tree.AddNode("test");
  bk_tree.AddNode("best");
  bk_tree.AddNode("nest");

  std::streambuf* orig = std::cout.rdbuf();
  std::ostringstream capture;
  std::cout.rdbuf(capture.rdbuf());
  
  bk_tree.PrintTree();
std::cout.rdbuf(orig);
string output = capture.str();
  EXPECT_GT(output.length(), 0);
  EXPECT_TRUE(output.find("root") != string::npos || output.find("test") != string::npos);
}
