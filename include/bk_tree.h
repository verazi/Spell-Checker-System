// bk_tree.h
#ifndef BK_TREE_H
#define BK_TREE_H
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

struct BKNode {
  std::string* word = nullptr;
  std::map<int, BKNode*> children;

  BKNode() : word(nullptr) { children = std::map<int, BKNode*>(); }

  explicit BKNode(const std::string& word) : word(new std::string(word)) {
    children = std::map<int, BKNode*>();
  }

  ~BKNode() { children.clear(); }
};

class BKTree {
 public:
  BKTree() : root("") {}

  explicit BKTree(const std::string& word) : root(word) {}

  ~BKTree();

  void AddNode(const std::string& word);
  void SearchNode(const std::string& word, int tol,
                  std::vector<std::string>* result);
  void PrintTree();

 private:
  static int Distance(const std::string& s1, const std::string& s2);
  BKNode root;
};

#endif  // BK_TREE_H