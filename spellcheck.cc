#include <chrono>
#include <iostream>
#include <string>
#include <unordered_set>
#include <mpi.h>
#include "include/candidates.h"
#include "include/dictionary.h"
#include "include/load_files.h"

using namespace std;
using namespace std::chrono;

// walltime helpers
static inline long long ms_since(high_resolution_clock::time_point t0) {
  return duration_cast<milliseconds>(high_resolution_clock::now() - t0).count();
}

static inline void print_hmsms(const char* label, long long ms) {
  auto minutes = ms / 60000;
  auto seconds = (ms % 60000) / 1000;
  auto milliseconds = ms % 1000;
  cout << "  - " << label << ": " << minutes << " mins " << seconds
      << " secs " << milliseconds << " ms (" << ms << " ms)" << endl;
}

// Load Dict helper
struct Slice {
  MPI_Offset off;
  MPI_Offset len;
};

Slice compute_slice(MPI_Offset n, int rank, int size) {
  MPI_Offset base = n / size;
  MPI_Offset extra = n % size;
  MPI_Offset start = rank * base + (rank < extra ? rank : extra);
  MPI_Offset length = base + (rank < extra ? 1 : 0);
  return {start, length};
}

std::string read_slice_aligned(const char* path, Slice s, MPI_Comm comm, MPI_Offset file_size_hint) {
  MPI_File f;
  MPI_File_open(comm, path, MPI_MODE_RDONLY, MPI_INFO_NULL, &f);

  const MPI_Offset PAD = 256;
  MPI_Offset read_off = (s.off > PAD) ? (s.off - PAD) : 0;
  MPI_Offset max_len  = s.len + 2 * PAD;
  if (read_off + max_len > file_size_hint)
    max_len = file_size_hint - read_off;

  std::string buf(static_cast<size_t>(max_len), '\0');
  MPI_Status st;
  MPI_File_read_at(f, read_off, buf.data(), buf.size(), MPI_CHAR, &st);
  MPI_File_close(&f);

  size_t begin = 0;
  if (s.off > 0) {
    size_t rel = static_cast<size_t>(s.off - read_off);
    size_t pos = buf.find('\n', (rel > 0) ? (rel - 1) : 0);
    begin = (pos == std::string::npos) ? 0 : (pos + 1);
  }

  size_t end = buf.size();
  if (s.off + s.len < file_size_hint) {
    size_t last = buf.rfind('\n');
    if (last != std::string::npos && last > begin)
      end = last;
  }

  std::string aligned = buf.substr(begin, end - begin);
  std::vector<std::string> lines;
  lines.reserve(aligned.size()/8+8);
  size_t start = 0;
  MPI_Offset aligned_file_off = read_off + begin;

  for (size_t j=0; j<aligned.size(); ++j) {
    if (aligned[j]=='\n') {
      MPI_Offset line_start_off = aligned_file_off + (MPI_Offset)start;
      if (line_start_off >= s.off && line_start_off < s.off + s.len) {
        lines.emplace_back(aligned.data()+start, j-start);
      }
      start = j+1;
    }
  }

  std::string shrunk;
  size_t tot = 0;
  for (auto& L : lines)
    tot += L.size() + 1;
  shrunk.reserve(tot);
  for (auto& L : lines) {
    shrunk.append(L);
    shrunk.push_back('\n');
  }
  return shrunk;
}

int main(int argc, char** argv) {
  std::cout.setf(std::ios::unitbuf);

  MPI_Init(&argc, &argv);
  int rank=0, size=1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc != 3) {
    cout << "Usage: " << argv[0] << " <dictionary> <word_list>" << endl;
    return 1;
  }
  
  auto t_total_start = high_resolution_clock::now();
  char* filename_dict = argv[1];
  char* filename_word_list = argv[2];

  if (rank == 0) {
    cout << "[*] Using dictionary: " << filename_dict << endl;
    cout << "[*] Using word list: " << filename_word_list << endl;
  }

  size_t dict_size, word_list_size;

  auto t0 = high_resolution_clock::now();
  MPI_Offset fsize = 0;
  MPI_File f;
  MPI_File_open(MPI_COMM_WORLD, filename_dict, MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
  MPI_File_get_size(f, &fsize);
  MPI_File_close(&f);
  MPI_Bcast(&fsize, 1, MPI_OFFSET, 0, MPI_COMM_WORLD);
  Slice s = compute_slice(fsize, rank, size);
  std::string local_dict_text = read_slice_aligned(filename_dict, s, MPI_COMM_WORLD, fsize);
  long long t_load_dict_ms = ms_since(t0);

  t0 = high_resolution_clock::now();
  std::string word_list_text_str;
  size_t word_list_size_local = 0;
  if (rank == 0) {
    char* word_list_text = LoadFile(filename_word_list, &word_list_size);
    word_list_text_str.assign(word_list_text, word_list_text + word_list_size);
    free(word_list_text);
    word_list_size_local = word_list_text_str.size();
  }
  MPI_Bcast(&word_list_size_local, sizeof(word_list_size_local), MPI_BYTE, 0, MPI_COMM_WORLD);

  if (rank != 0)
    word_list_text_str.resize(word_list_size_local);
  MPI_Bcast(word_list_text_str.data(), word_list_size_local, MPI_BYTE, 0, MPI_COMM_WORLD);
  WordList word_list = BuildWordList(word_list_text_str.data(), word_list_text_str.size());
  long long t_build_wordlist_ms = ms_since(t0);

  // build BK tree
  if (rank == 0) {
    cout << "[*] Building dictionary with bk trees" << endl;
  }
  t0 = high_resolution_clock::now();
  Dictionary dict = Dictionary(local_dict_text.data(), local_dict_text.size());
  long long t_build_dict_obj_ms = ms_since(t0);

  cout << "[r" << rank << "] Dictionary size: " << dict.size << endl;
  if (rank == 0) {
    cout << "\tWord list size: " << word_list.words.size() << endl;
    cout << "\tRoot len: " << word_list.root_len << endl;
  }

  t0 = high_resolution_clock::now();
  dict.BuildBKTree(word_list.root_len);
  long long t_build_bktree_ms = ms_since(t0);

  // Get misspelled words
  // if (rank == 0) {
  //   cout << "[*] Getting misspelled words" << endl;
  // }
  // t0 = high_resolution_clock::now();
  // auto local = GetMisspelledLocal(word_list, dict);
  // std::vector<unsigned char> present_global(word_list.words.size(), 0);
  // MPI_Allreduce(local.present.data(), present_global.data(),
  //               (int)word_list.words.size(),
  //               MPI_UNSIGNED_CHAR, MPI_BOR, MPI_COMM_WORLD);
  // Packed pk = PackAllCandidates(word_list, local);
  // Misspelled misspelled = MergePackedAcrossRanks(word_list, present_global, pk, rank, size);

  if (rank == 0) {
    cout << "[*] Getting misspelled words" << endl;
  }
  t0 = high_resolution_clock::now();

  // Pass 1
  auto present_local = GetPresentLocal(word_list, dict);
  std::vector<unsigned char> present_global(word_list.words.size(), 0);
  MPI_Allreduce(present_local.data(), present_global.data(), (int)word_list.words.size(), MPI_UNSIGNED_CHAR, MPI_BOR, MPI_COMM_WORLD);
  // Pass 2
  auto local_masked = GetCandidatesLocalMasked(word_list, dict, present_global);

  Packed pk = PackAllCandidates(word_list, local_masked);
  Misspelled misspelled =
      MergePackedAcrossRanks(word_list, present_global, pk, rank, size);

  long long t_get_misspelled_ms = ms_since(t0);
  // Format and save misspelled words

  const char* out_filename = "results/word_list_misspelled.txt";
  long long t_save_output_ms = 0;
  if (rank == 0) {
    t0 = high_resolution_clock::now();
    FILE* outfile = fopen(out_filename, "w");
    SaveMisspelled(misspelled, outfile);
    fclose(outfile);
    t_save_output_ms = ms_since(t0);
  }

  // Show wall time
  long long t_total_ms = ms_since(t_total_start);
  if (rank==0) {
    cout << "[*] Output: " << out_filename << endl;
    cout << "[*] Wall time breakdown (ms):" << endl;
    print_hmsms("Load dictionary file", t_load_dict_ms);
    print_hmsms("Build WordList", t_build_wordlist_ms);
    print_hmsms("Construct Dictionary object", t_build_dict_obj_ms);
    print_hmsms("Build BK-Tree", t_build_bktree_ms);
    print_hmsms("Get misspelled", t_get_misspelled_ms);
    print_hmsms("Save output", t_save_output_ms);
    print_hmsms("TOTAL", t_total_ms);
  }
  MPI_Finalize();
  return 0;
}
