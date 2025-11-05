#ifndef CANDIDATES_H
#define CANDIDATES_H
#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "bk_tree.h"
#include "dictionary.h"
using namespace std;

struct Line {
  string word;
  vector<string> candidates;
};

using Misspelled = vector<Line>;

Misspelled GetMisspelled(const WordList& word_list, Dictionary& dict);
// the last argument is the file handler
void SaveMisspelled(Misspelled& misspelled, FILE* file);

// Local Misspelled
struct LocalMisspelled {
  vector<unsigned char> present; // 1 if present, 0 if misspelled
  vector<vector<string>> local_candidates; // candidates for each word
};

LocalMisspelled GetMisspelledLocal(const WordList& word_list, Dictionary& dict);


std::vector<unsigned char> GetPresentLocal(const WordList& word_list, Dictionary& dict);

LocalMisspelled GetCandidatesLocalMasked(
    const WordList& word_list, Dictionary& dict,
    const std::vector<unsigned char>& present_global);


struct Packed {
  std::string big;
  std::vector<int> off;
  std::vector<int> len;
};

// Pack local candidates to big/off/len (for MPI_Gatherv)
Packed PackAllCandidates(const WordList& wl, const LocalMisspelled& local);

// Rank 0 gathers all candidates
Misspelled MergePackedAcrossRanks(
    const WordList& wl, const std::vector<unsigned char>& present_global,
    const Packed& pk_local, int rank, int size);

#endif  // CANDIDATES_H