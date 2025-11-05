#include "load_files.h"
#include <cstdio>
#include <cstdlib>
#include "codes.h"

char* LoadFile(const char* filename, size_t* size) {
  FILE* f = fopen(filename, "r");
  if (!f) {
    printf("File not found: %s\n", filename);
    exit(FILE_NOT_FOUND);
  }
  fseek(f, 0, SEEK_END);
  *size = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (*size == 0) {
    fclose(f);
    printf("File is empty: %s\n", filename);
    exit(FILE_EMPTY);
  }

  auto* data = (char*)malloc(*size * sizeof(char));
  if (!data) {
    fclose(f);
    printf("Memory error\n");
    exit(MEM_ERR);
  }
  fread(data, *size * sizeof(char), 1, f);
  fclose(f);
  return data;
}
