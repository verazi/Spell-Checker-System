#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "candidates.h"
#include "dictionary.h"

using namespace std;
using ::testing::_;
using ::testing::Matcher;

TEST(Candidates, GetMisspelled) {
  string words_text = "stabX\nappel\ntim\nstap\nrandpm\nbananaa\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text =
      "appeal\napple\nrappel\nTim\ntime\nvim\nsap\nsnap\nstab"
      "\nstep\nstop\nstrap\ntap\nhello\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);
  for (auto& line : misspelled) {
    if (line.word == "appel") {
      EXPECT_EQ(line.candidates.size(), 3);
    } else if (line.word == "tim") {
      EXPECT_EQ(line.candidates.size(), 4);
    } else if (line.word == "stap") {
      EXPECT_EQ(line.candidates.size(), 7);
    } else if (line.word == "randpm") {
      EXPECT_EQ(line.candidates.size(), 0);
    } else if (line.word == "bananaa") {
      EXPECT_EQ(line.candidates.size(), 0);
    } else if (line.word == "stabX") {
      EXPECT_EQ(line.candidates.size(), 1);
    } else {
      cout << "Unexpected word: " << line.word << endl;
      EXPECT_EQ(0, 1);
    }
  }
}

class FileIOTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override { remove("test_word_list_misspelled.txt"); }
};

namespace {
using ::testing::_;
using ::testing::Return;

class FileMock {
 public:
  MOCK_METHOD(int, fputc, (int, FILE*));
  MOCK_METHOD(size_t, fwrite, (const void*, size_t, size_t, FILE*));

  static FileMock* instance;
  static FILE* test_file;

  static void SetTestFile(FILE* file) { test_file = file; }

  static void ClearTestFile() { test_file = nullptr; }
};

FileMock* FileMock::instance = nullptr;
FILE* FileMock::test_file = nullptr;

extern "C" int fputc(int character, FILE* stream) {
  if (FileMock::instance && stream == FileMock::test_file) {
    return FileMock::instance->fputc(character, stream);
  }
  return putc(character, stream);
}

extern "C" size_t fwrite(const void* ptr, size_t size, size_t count,
                         FILE* stream) {
  if (FileMock::instance && stream == FileMock::test_file) {
    return FileMock::instance->fwrite(ptr, size, count, stream);
  }
  size_t written = 0;
  const char* data = (const char*)ptr;
  for (size_t i = 0; i < count && written < count; i++) {
    for (size_t j = 0; j < size; j++) {
      if (putc(data[i * size + j], stream) == EOF) {
        return written;
      }
    }
    written++;
  }
  return written;
}
}  // namespace

MATCHER_P3(MemcmpMatcher, expected, size, count, "") {
  return std::memcmp(arg, expected, size * count) == 0;
}

TEST(FileIOTest, SaveMisspelled) {
  FileMock file_mock;
  FileMock::instance = &file_mock;

  FILE* file = fopen("test_word_list_misspelled.txt", "w");
  ASSERT_NE(file, nullptr);
  FileMock::SetTestFile(file);
  testing::Sequence s;

  EXPECT_CALL(file_mock, fwrite(MemcmpMatcher("bananaa:\n", sizeof(char), 9),
                                sizeof(char), 9, file))
      .Times(1);
  EXPECT_CALL(file_mock, fwrite(MemcmpMatcher("randpm:\n", sizeof(char), 8),
                                sizeof(char), 8, file))
      .Times(1);
  EXPECT_CALL(file_mock,
    fwrite(MemcmpMatcher("appel: Rappel appeal rappel\n", sizeof(char), 28),
      sizeof(char), 28, file))
      .Times(1);
  EXPECT_CALL(file_mock,
              fwrite(MemcmpMatcher("tim: Tim Vim time vim\n", sizeof(char), 22),
                     sizeof(char), 22, file))
      .Times(1);
  EXPECT_CALL(file_mock,
              fwrite(MemcmpMatcher("stap: sap snap stab step stop strap tap\n",
                                   sizeof(char), 40),
                     sizeof(char), 40, file))
      .Times(1);

  string words_text = "appel\ntim\nstap\nrandpm\nbananaa\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text =
      "appeal\napple\nrappel\nTim\ntime\nvim\nsap\nsnap\nstab"
      "\nstep\nstop\nstrap\ntap\nhello\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);
  Misspelled misspelled = GetMisspelled(word_list, dict);
  SaveMisspelled(misspelled, file);

  fclose(file);
  FileMock::instance = nullptr;
  FileMock::ClearTestFile();
}

TEST(Candidates, EmptyWordList) {
  string words_text = "";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  EXPECT_EQ(word_list.words.size(), 0);

  string dict_text = "apple\norange\nbanana\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());

  Misspelled misspelled = GetMisspelled(word_list, dict);
  EXPECT_EQ(misspelled.size(), 0);
}

TEST(Candidates, EmptyDictionary) {
  string words_text = "apple\norange\nbanana\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  EXPECT_THROW(dict.BuildBKTree(word_list.root_len), std::runtime_error);
}

TEST(Candidates, AllWordsCorrect) {
  string words_text = "apple\norange\nbanana\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "apple\norange\nbanana\ngrape\nkiwi\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);
  EXPECT_EQ(misspelled.size(), 0);
}

TEST(Candidates, AllWordsMisspelled) {
  string words_text = "appel\norang\nbanan\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "grape\nkiwi\nmango\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);
  EXPECT_EQ(misspelled.size(), 3);
  for (const auto& line : misspelled) {
    EXPECT_EQ(line.candidates.size(), 0);
  }
}

TEST(Candidates, SingleCharacterWords) {
  string words_text = "a\nb\nc\nx\ny\nz\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "a\nb\nc\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);
  EXPECT_EQ(misspelled.size(), 3);
  for (const auto& line : misspelled) {
    EXPECT_GT(line.candidates.size(), 0);
  }
}

TEST(Candidates, CaseSensitiveMatching) {
  string words_text = "Apple\nBANANA\nOrAnGe\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "apple\nbanana\norange\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);

  bool found_apple = false;
  bool found_banana = false;
  bool found_orange = false;

  for (const auto& line : misspelled) {
    if (line.word == "BANANA")
      found_banana = true;
    if (line.word == "OrAnGe")
      found_orange = true;
  }

  EXPECT_TRUE(found_banana);
  EXPECT_TRUE(found_orange);
}

TEST(Candidates, DuplicateWords) {
  string words_text = "apple\napple\norange\norange\nbanana\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "apple\norange\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);
  EXPECT_EQ(misspelled.size(), 1);
  EXPECT_EQ(misspelled[0].word, "banana");
}

TEST(Candidates, VeryLongWords) {
  string long_word = string(100, 'a') + "x";
  string words_text = long_word + "\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string correct_word = string(100, 'a') + "y";
  string dict_text = correct_word + "\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);
  EXPECT_EQ(misspelled.size(), 1);
  EXPECT_EQ(misspelled[0].candidates.size(), 1);
}

TEST(Candidates, SpecialCharacters) {
  string words_text = "test@example.com\nuser_name\nfile.txt\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "test@example.org\nuser-name\nfile.doc\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);
  EXPECT_EQ(misspelled.size(), 3);

  for (const auto& line : misspelled) {
    EXPECT_GE(line.candidates.size(), 0);
  }
}

TEST(Candidates, BuildWordListRootLength) {

  string words_text = "a\nab\nab\nabc\nabc\nabc\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  EXPECT_EQ(word_list.root_len, 3);
  EXPECT_EQ(word_list.words.size(), 6);
}

TEST(Candidates, BuildWordListEmptyLines) {
  string words_text = "word1\n\nword2\n\n\nword3\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  EXPECT_EQ(word_list.words.size(), 3);
  EXPECT_THAT(word_list.words, Contains("word1"));
  EXPECT_THAT(word_list.words, Contains("word2"));
  EXPECT_THAT(word_list.words, Contains("word3"));
}

TEST(Candidates, LowercaseDictionaryFirstLetter) {
  string words_text = "xApple\nday\nbanana\n";
  WordList word_list = BuildWordList(words_text.c_str(), words_text.size());

  string dict_text = "apple\nbanana\nApple\n";
  Dictionary dict = Dictionary(dict_text.c_str(), dict_text.size());
  dict.BuildBKTree(word_list.root_len);

  Misspelled misspelled = GetMisspelled(word_list, dict);

  bool found_banana = false;

  for (const auto& line : misspelled) {
    if (line.word == "xApple") {
      EXPECT_EQ(line.candidates.size(), 1);
    }
    if (line.word == "banana")
      found_banana = true;
  }

  EXPECT_FALSE(found_banana);
}
