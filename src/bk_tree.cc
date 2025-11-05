#include "bk_tree.h"
#include <stack>
#include <string>
#include "edit_distance.h"
#include "iostream"
#include "queue"

static int Abs(int x) {
  return x < 0 ? -x : x;
}

BKTree::~BKTree() {}

int BKTree::Distance(const std::string& s1, const std::string& s2) {
  return static_cast<int>(EditDistance(s1, s2));
}

void BKTree::AddNode(const std::string& word) {
  BKNode* node = &root;
  while (true) {
    int dist = Distance(*node->word, word);
    if (dist == 0) {
      return;
    }
    if (node->children.find(dist) == node->children.end()) {
      node->children[dist] = new BKNode(word);
      return;
    }
    node = node->children[dist];
  }
}

void BKTree::SearchNode(const std::string& word, const int tol,
                        std::vector<std::string>* result) {

  std::queue<BKNode*> nodes;
  nodes.push(&root);
  std::vector<std::string> tmp_res;
  while (!nodes.empty()) {
    BKNode* node = nodes.front();
    nodes.pop();
    int dist = Distance(*node->word, word);
    if (dist <= tol) {
      tmp_res.push_back(*node->word);
    }

    for (const auto& child : node->children) {
      if (Abs(child.first - dist) > tol) {
        continue;
      }
      nodes.push(child.second);
    }
  }

  for (const auto& res_word : tmp_res) {
    size_t dist = EditDistance(word, res_word);
    if (dist == 1) {
      result->push_back(res_word);
    }
  }
}

void BKTree::PrintTree() {
  std::stack<std::tuple<BKNode*, size_t, int>> nodes;
  nodes.push(std::make_tuple(&root, 0, 0));
  while (!nodes.empty()) {
    auto [current, current_depth, current_distance] = nodes.top();
    nodes.pop();

    for (size_t i = 0; i < current_depth; ++i) {
      std::cout << " ";
    }
    std::cout << current_distance << ": " << *current->word << std::endl;

    for (auto it = current->children.rbegin(); it != current->children.rend();
         ++it) {
      nodes.push(std::make_tuple(it->second, current_depth + 1, it->first));
    }
  }
}