#ifndef readfile_H
#define readfile_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t* readfile(const char* const path) {
  uint8_t* result = 0;
  size_t size = 0;
  FILE* file = fopen(path, "rb");
  if (!file) {
    fprintf(stderr, "file: %s not found.\n", path);
  }
  else if (
    0 > fseek(file, 0, SEEK_END) || 0 > (size = ftell(file))
    || 0 > fseek(file, 0, SEEK_SET) || !(result = calloc(size + 1, sizeof(*result)))) {
    fprintf(stderr, "Unable to allocate memory for file: %s.\n", path);
    fclose(file);
  }
  else if (size != fread(result, sizeof(*result), size, file)) {
    fprintf(stderr, "Unable to read file: %s correctly.\n", path);
    fclose(file);
    free(result);
    result = 0;
  }
  else {
    fclose(file);
  }
  return result;
}
#endif // readfile_H
