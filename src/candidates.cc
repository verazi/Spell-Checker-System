#include "candidates.h"
#include <omp.h>
#include <mpi.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <numeric>
#include <cstdint>
#include <string>
#include <unordered_set>
#include "dictionary.h"

using namespace std;

Misspelled GetMisspelled(const WordList& word_list, Dictionary& dict) {
  Misspelled misspelled;
  int num_threads = omp_get_max_threads();
  omp_set_num_threads(num_threads);

  cout << "[*] Number of threads: " << num_threads << endl;

#pragma omp parallel
  {
    Misspelled misspelled_local; // avoid push back race

#pragma omp for schedule(dynamic)
    for (size_t i = 0; i < word_list.words.size(); ++i) {
      const auto& word = word_list.words[i];

      if (dict.Contains(word)) { // correct word
        continue;
      }

      // get candidates
      Line line;
      line.word = word;
      dict.Candidates(word, &line.candidates);

      misspelled_local.push_back(line);
    }

    // merge results
#pragma omp critical
    {
      misspelled.insert(misspelled.end(), misspelled_local.begin(),
                        misspelled_local.end());
    }
  }

  return misspelled;
}

void SaveMisspelled(Misspelled& misspelled, FILE* file) {
  sort(misspelled.begin(), misspelled.end(), [](const Line& a, const Line& b) {
    if (a.candidates.size() == b.candidates.size()) {
      return strcmp(a.word.c_str(), b.word.c_str()) < 0;
    }
    return a.candidates.size() < b.candidates.size();
  });
  for (auto& line : misspelled) {
    string words_str;
    for (auto& word : line.candidates) {
      words_str += " " + word;
    }
    words_str += "\n";
    string line_str;
    line_str = line.word + ":" + words_str;
    fwrite(line_str.c_str(), sizeof(char), line_str.size(), file);
  }
}

// Local Misspelled: for each rank, get local misspelled words and candidates
LocalMisspelled GetMisspelledLocal(const WordList& word_list, Dictionary& dict) {
  LocalMisspelled out;
  const size_t n = word_list.words.size();
  out.present.assign(n, 0);
  out.local_candidates.assign(n, {});

  int num_threads = omp_get_max_threads();
  omp_set_num_threads(num_threads);

#pragma omp parallel for schedule(static)
  for (long long i = 0; i < (long long)n; ++i) {
    const string& w = word_list.words[i];

    if (dict.Contains(w)) { // correct word
      out.present[i] = 1;
      continue;
    }
  
    vector<string> cands;
    dict.Candidates(w, &cands);
    // sort and deduplicate here
    sort(cands.begin(), cands.end(), [](const string& a, const string& b) {
      return strcmp(a.c_str(), b.c_str()) < 0;
    });
    cands.erase(unique(cands.begin(), cands.end()), cands.end());
    out.local_candidates[i].swap(cands);
  }
  return out;
}

// Two phases:
// 1. Get present_local (1 if present, 0 if misspelled)
std::vector<unsigned char> GetPresentLocal(const WordList& word_list, Dictionary& dict) {
  const size_t n = word_list.words.size();
  std::vector<unsigned char> present(n, 0);

  int num_threads = omp_get_max_threads();
  omp_set_num_threads(num_threads);

#pragma omp parallel for schedule(static)
  for (long long i = 0; i < (long long)n; ++i) {
    const std::string& w = word_list.words[i];
    if (dict.Contains(w))
      present[i] = 1;
  }
  return present;
}

// 2. Get candidates only for misspelled words
LocalMisspelled GetCandidatesLocalMasked(const WordList& word_list, Dictionary& dict, const std::vector<unsigned char>& present_global) {
  LocalMisspelled out;
  const size_t n = word_list.words.size();
  out.present = present_global;
  out.local_candidates.assign(n, {});

  int num_threads = omp_get_max_threads();
  omp_set_num_threads(num_threads);

// #pragma omp parallel for schedule(dynamic, 128)
#pragma omp parallel for schedule(guided, 64)
  for (long long i = 0; i < (long long)n; ++i) {
    if (present_global[i])
      continue;
    const std::string& w = word_list.words[i];

    std::vector<std::string> cands;
    dict.Candidates(w, &cands);

    std::sort(cands.begin(), cands.end(), [](const std::string& a, const std::string& b) {
                return std::strcmp(a.c_str(), b.c_str()) < 0;
              });
    cands.erase(std::unique(cands.begin(), cands.end()), cands.end());
    out.local_candidates[i].swap(cands);
  }
  return out;
}

Packed PackAllCandidates(const WordList& wl, const LocalMisspelled& local) {
  Packed p;
  const size_t n = wl.words.size();
  p.off.resize(n); // offset in big
  p.len.resize(n); // length in big

  size_t reserve_bytes = 0;
  for (auto& v : local.local_candidates)
    for (auto& s : v)
      reserve_bytes += s.size() + 1;
  p.big.reserve(reserve_bytes);

  size_t cur = 0;
  for (size_t i = 0; i < n; ++i) {
    p.off[i] = (int)cur;
    for (auto& s : local.local_candidates[i]) {
      p.big.append(s);
      p.big.push_back('\n');
    }
    p.len[i] = (int)(p.big.size() - cur);
    cur = p.big.size();
  }
  return p;
}

Misspelled MergePackedAcrossRanks(
    const WordList& wl, const std::vector<unsigned char>& present_global,
    const Packed& pk_local, int rank, int size) {

  const size_t n = wl.words.size();
  Misspelled result;

  int my_big_bytes = (int)pk_local.big.size(); // local big size
  std::vector<int> big_bytes(size,0), big_displs(size,0);
  if (rank==0) {
    MPI_Gather(&my_big_bytes, 1, MPI_INT, big_bytes.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Gather(&my_big_bytes, 1, MPI_INT, nullptr, 0, MPI_INT, 0, MPI_COMM_WORLD);
  }

  std::vector<int> off_all, len_all;
  if (rank == 0) {
    off_all.resize(n * size);
    len_all.resize(n * size);
  }
  MPI_Gather(pk_local.off.data(), (int)n, MPI_INT, rank==0 ? off_all.data() : nullptr, (int)n, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Gather(pk_local.len.data(), (int)n, MPI_INT, rank==0 ? len_all.data() : nullptr, (int)n, MPI_INT, 0, MPI_COMM_WORLD);

  char dummy;
  char* recv_ptr = &dummy;
  std::vector<char> big_recv;
  int big_total = 0;
  if (rank==0) {
    for (int r = 0; r < size; ++r)
      big_total += big_bytes[r];
    if (big_total > 0) {
      big_recv.resize(big_total);
      recv_ptr = big_recv.data();
    }
    if (size > 1)
      std::partial_sum(big_bytes.begin(), big_bytes.end()-1, big_displs.begin()+1);
  }

  if (rank==0) {
    MPI_Gatherv(pk_local.big.data(), my_big_bytes, MPI_CHAR, recv_ptr,
                big_bytes.data(), big_displs.data(), MPI_CHAR, 0,
                MPI_COMM_WORLD);
  } else {
    MPI_Gatherv(pk_local.big.data(), my_big_bytes, MPI_CHAR, nullptr, nullptr,
                nullptr, MPI_CHAR, 0, MPI_COMM_WORLD);
  }

  if (rank==0) {
    std::vector<int> rank_base(size,0);
    for (int r = 1; r < size; ++r)
      rank_base[r] = rank_base[r - 1] + big_bytes[r - 1];

    result.reserve(n);
    for (size_t i=0; i<n; ++i) {
      if (present_global[i]) continue;

      set<string, bool (*)(const string&, const string&)> merged(
          [](const string& a, const string& b) {
            return strcmp(a.c_str(), b.c_str()) < 0;
          });

      for (int r=0; r<size; ++r) {
        int L = len_all[r*(int)n + (int)i];
        if (L <= 0) continue;
        int O = off_all[r*(int)n + (int)i] + rank_base[r];
        const char* base = big_recv.data() + O;
        int start = 0;
        for (int j=0; j<L; ++j) {
          if (base[j]=='\n') {
            if (j>start) merged.insert(string(base+start, base+j));
            start = j+1;
          }
        }
      }

      Line line;
      line.word = wl.words[i];
      line.candidates.reserve(merged.size());
      for (auto& s: merged) line.candidates.push_back(s);
      result.push_back(std::move(line));
    }
  }
  return result;
}