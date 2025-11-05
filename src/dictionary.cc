#include "dictionary.h"
#include <cctype>
#include <stdexcept>
#include <string>
#include <unordered_set>

using namespace std;

void Dictionary::Build(const char* text, size_t text_size) {
  size_t i = 0;
  while (i < text_size) {
    string word;
    while (i < text_size && text[i] != '\n') {
      word.push_back(text[i]);
      ++i;
    }
    if (word.empty()) {
      ++i;
      continue;
    }
    orig.insert(word);
    // prepare for bk tree building
    len_to_words[word.size()].push_back(word);

    if (word[0] >= 'a' && word[0] <= 'z') {
      // e.ii
      string upper_word = word;
      upper_word[0] = toupper(upper_word[0]);
      orig.insert(upper_word);
      len_to_words[upper_word.size()].push_back(upper_word);
    }

    // Skip the newline character
    if (i < text_size && text[i] == '\n') {
      ++i;
    }
  }
  size = orig.size();
}

void Dictionary::BuildBKTree(size_t root_len) {
  std::string root_word;

  // Check if we have words of the requested length
  if (len_to_words.find(root_len) != len_to_words.end() &&
      !len_to_words[root_len].empty()) {
    root_word = len_to_words[root_len][0];
  } else {
    // If no words of root_len exist, use the first available word from any length
    if (!len_to_words.empty()) {
      root_word = len_to_words.begin()->second[0];
    } else {
      // This shouldn't happen if dictionary is not empty, but handle it gracefully
      if (!orig.empty()) {
        root_word = *orig.begin();
      } else {
        throw std::runtime_error("Dictionary is empty, cannot build BK tree");
      }
    }
  }

  bk_tree = BKTree(root_word);
  for (const auto& word : orig) {
    bk_tree.AddNode(word);
  }
}

bool Dictionary::Contains(const std::string& word) {
  if (orig.find(word) != orig.end()) {
    return true;
  }
  return false;
}

void Dictionary::Candidates(const std::string& word,
                            std::vector<std::string>* result) {
  bk_tree.SearchNode(word, 2, result);
}

WordList BuildWordList(const char* text, size_t text_size) {
  std::unordered_map<size_t, size_t> lens_count;
  WordList word_list = WordList();
  size_t i = 0;
  size_t max_count = 0;
  while (i < text_size) {
    string word;
    while (i < text_size && text[i] != '\n') {
      word.push_back(text[i]);
      ++i;
    }
    if (word.empty()) {
      ++i;
      continue;
    }
    word_list.words.push_back(word);
    size_t len = word.size();
    if (lens_count.find(len) == lens_count.end()) {
      lens_count[len] = 1;
    } else {
      lens_count[len] += 1;
    }
    if (lens_count[len] > max_count) {
      max_count = lens_count[len];
      word_list.root_len = len;
    }
  }
  return word_list;
}
