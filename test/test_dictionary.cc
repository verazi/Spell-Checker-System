#include <gtest/gtest.h>

#include <string>
#include <unordered_map>
#include "dictionary.h"

using namespace std;

TEST(Dictionary, ContainsExactWord) {
  string text =
      "apple\nMelbourne\nmRNA\nmicroMole\neducation\nhello\nworld\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());

  ASSERT_EQ(13, dict.size);

  ASSERT_TRUE(dict.Contains("apple"));
  ASSERT_TRUE(dict.Contains("Apple"));
  ASSERT_TRUE(dict.Contains("Melbourne"));
  ASSERT_TRUE(dict.Contains("mRNA"));
  ASSERT_TRUE(dict.Contains("MRNA"));
  ASSERT_TRUE(dict.Contains("microMole"));
  ASSERT_TRUE(dict.Contains("MicroMole"));
  ASSERT_TRUE(dict.Contains("education"));
  ASSERT_TRUE(dict.Contains("Education"));
  ASSERT_TRUE(dict.Contains("hello"));
  ASSERT_TRUE(dict.Contains("Hello"));
  ASSERT_TRUE(dict.Contains("world"));
  ASSERT_TRUE(dict.Contains("World"));
}

TEST(Dictionary, NotContainExactWord) {
  string text =
      "apple\nMelbourne\nmRNA\nmicroMole\neducation\nhello\nworld\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  ASSERT_FALSE(dict.Contains("banana"));
  ASSERT_FALSE(dict.Contains("Sydney"));
  ASSERT_FALSE(dict.Contains("DNA"));
  ASSERT_FALSE(dict.Contains("Mole"));
}

TEST(Dictionary, ContainsFirstLetterLowercase) {
  string text = "melbourne\nab\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  ASSERT_TRUE(dict.Contains("Melbourne"));
  ASSERT_TRUE(dict.Contains("Ab"));
}

TEST(Dictionary, NotContainsFirstLetterLowercase) {
  string text = "melbourne\nab\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  ASSERT_FALSE(dict.Contains("Melbuorne"));
  ASSERT_FALSE(dict.Contains("Abc"));
}


TEST(Dictionary, EmptyDictionary) {
  string text = "";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(0, dict.size);
  EXPECT_FALSE(dict.Contains("any"));
  EXPECT_FALSE(dict.Contains(""));
}

TEST(Dictionary, SingleWord) {
  string text = "word";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(2, dict.size);
  EXPECT_TRUE(dict.Contains("word"));
  EXPECT_TRUE(dict.Contains("Word"));
  EXPECT_FALSE(dict.Contains("other"));
}

TEST(Dictionary, SingleCharacterWords) {
  string text = "a\nb\nc\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(6, dict.size);
  EXPECT_TRUE(dict.Contains("a"));
  EXPECT_TRUE(dict.Contains("A"));
  EXPECT_TRUE(dict.Contains("b"));
  EXPECT_TRUE(dict.Contains("B"));
  EXPECT_TRUE(dict.Contains("c"));
  EXPECT_TRUE(dict.Contains("C"));
  EXPECT_FALSE(dict.Contains("d"));
}

TEST(Dictionary, DuplicateWords) {
  string text = "word\nword\nother\nword\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  EXPECT_EQ(4, dict.size);  
  EXPECT_TRUE(dict.Contains("word"));
  EXPECT_TRUE(dict.Contains("Word"));
  EXPECT_TRUE(dict.Contains("other"));
  EXPECT_TRUE(dict.Contains("Other"));
}

TEST(Dictionary, WordsWithNumbers) {
  string text = "word1\n2word\nword3word\n123\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(6, dict.size);
  EXPECT_TRUE(dict.Contains("word1"));
  EXPECT_TRUE(dict.Contains("Word1"));
  EXPECT_TRUE(dict.Contains("2word"));
  EXPECT_TRUE(dict.Contains("word3word"));
  EXPECT_TRUE(dict.Contains("Word3word"));
  EXPECT_TRUE(dict.Contains("123"));
}

TEST(Dictionary, SpecialCharacters) {
  string text = "hello-world\ntest_case\nfile.txt\na@b\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(8, dict.size);
  EXPECT_TRUE(dict.Contains("hello-world"));
  EXPECT_TRUE(dict.Contains("Hello-world"));
  EXPECT_TRUE(dict.Contains("test_case"));
  EXPECT_TRUE(dict.Contains("Test_case"));
  EXPECT_TRUE(dict.Contains("file.txt"));
  EXPECT_TRUE(dict.Contains("File.txt"));
  EXPECT_TRUE(dict.Contains("a@b"));
  EXPECT_TRUE(dict.Contains("A@b"));
}

TEST(Dictionary, EmptyLines) {
  string text = "word1\n\nword2\n\n\nword3\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(6, dict.size);
  EXPECT_TRUE(dict.Contains("word1"));
  EXPECT_TRUE(dict.Contains("word2"));
  EXPECT_TRUE(dict.Contains("word3"));
  EXPECT_TRUE(dict.Contains("Word1"));
  EXPECT_TRUE(dict.Contains("Word2"));
  EXPECT_TRUE(dict.Contains("Word3"));
}

TEST(Dictionary, VeryLongWords) {
  string long_word = string(1000, 'a');  
  string text = long_word + "\nshort\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(4, dict.size);
  EXPECT_TRUE(dict.Contains(long_word));
  string capitalized_long_word = long_word;
  capitalized_long_word[0] = toupper(capitalized_long_word[0]);
  EXPECT_TRUE(dict.Contains(capitalized_long_word));
  EXPECT_TRUE(dict.Contains("short"));
  EXPECT_TRUE(dict.Contains("Short"));
}

TEST(Dictionary, BuildBKTreeEmptyDictionary) {
  string text = "";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  EXPECT_THROW(dict.BuildBKTree(5), std::runtime_error);
}

TEST(Dictionary, BuildBKTreeMissingLength) {
  string text = "abc\ndef\n";  
  Dictionary dict = Dictionary(text.c_str(), text.size());
  EXPECT_NO_THROW(dict.BuildBKTree(5));  
}

TEST(Dictionary, CandidatesSearchBasic) {
  string text = "apple\napple\nrappel\ntap\nstap\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  dict.BuildBKTree(dict.size);vector<string> candidates;
  dict.Candidates("appel", &candidates);
  
  EXPECT_GT(candidates.size(), 0);
  
}

TEST(Dictionary, CaseSensitivityEdgeCases) {
  string text = "Word\nWORD\nword\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_EQ(3, dict.size);  
  EXPECT_TRUE(dict.Contains("Word"));
  EXPECT_TRUE(dict.Contains("WORD"));
  EXPECT_TRUE(dict.Contains("word"));
}

TEST(Dictionary, UppercaseHandling) {
  string text = "apple\n";
  Dictionary dict = Dictionary(text.c_str(), text.size());
  EXPECT_TRUE(dict.Contains("Apple"));
  
  EXPECT_FALSE(dict.Contains("APPLE"));
  
  EXPECT_FALSE(dict.Contains("aPPLE"));
}


TEST(Dictionary, BuildBKTreeCornerCase) {
  
  string text = "a\n";  
  Dictionary dict = Dictionary(text.c_str(), text.size());
  
  EXPECT_NO_THROW(dict.BuildBKTree(10));EXPECT_TRUE(dict.Contains("a"));
  
  vector<string> candidates;
  dict.Candidates("b", &candidates);
  EXPECT_GE(candidates.size(), 0);  
}


TEST(Dictionary, BuildBKTreeMultipleScenarios) {

  string empty_text = "";
  Dictionary empty_dict = Dictionary(empty_text.c_str(), empty_text.size());
  EXPECT_THROW(empty_dict.BuildBKTree(5), std::runtime_error);
string text2 = "abc\ndef\nghi\n";  
  Dictionary dict2 = Dictionary(text2.c_str(), text2.size());
  EXPECT_NO_THROW(dict2.BuildBKTree(5));
  string text3 = "x";  
  Dictionary dict3 = Dictionary(text3.c_str(), text3.size());
  EXPECT_NO_THROW(dict3.BuildBKTree(1));
string text4 = "a\nbb\nccc\ndddd\n";
  Dictionary dict4 = Dictionary(text4.c_str(), text4.size());
  EXPECT_NO_THROW(dict4.BuildBKTree(2));  
  EXPECT_NO_THROW(dict4.BuildBKTree(10)); 
}


TEST(Dictionary, BuildBKTreeStressTest) {

  string text1 = "word";
  Dictionary dict1 = Dictionary(text1.c_str(), text1.size());
  EXPECT_NO_THROW(dict1.BuildBKTree(100));
string text2 = "\n\n\n";
  Dictionary dict2 = Dictionary(text2.c_str(), text2.size());
  EXPECT_THROW(dict2.BuildBKTree(1), std::runtime_error);
  string text3 = "!\n@\n#\n$\n%\n";
  Dictionary dict3 = Dictionary(text3.c_str(), text3.size());
  EXPECT_NO_THROW(dict3.BuildBKTree(5));
  string text4 = "test\n";
  Dictionary dict4 = Dictionary(text4.c_str(), text4.size());
  EXPECT_NO_THROW(dict4.BuildBKTree(SIZE_MAX));  
}