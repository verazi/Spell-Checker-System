#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <cctype>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "bk_tree.h"


#define DELETED_OR_REPLACED_OR_ADDED 1
class Dictionary {
 public:
  size_t size;

  Dictionary(const char* text, size_t text_size) : size(0) {
    Build(text, text_size);
  }

  bool Contains(const std::string& word);
  void BuildBKTree(size_t root_len);
  void Candidates(const std::string& word, std::vector<std::string>* result);

 private:
  std::unordered_set<std::string> orig;
  std::unordered_map<size_t, std::vector<std::string>> len_to_words;
  BKTree bk_tree;

  void Build(const char* text, size_t text_size);
};

struct WordList {
  std::vector<std::string> words;
  size_t root_len;
};

WordList BuildWordList(const char* text, size_t text_size);
#endif  // DICTIONARY_H