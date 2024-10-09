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
  }
  else if (
    0 > fseek(file, 0, SEEK_END) || 0 > (size = ftell(file))
    || 0 > fseek(file, 0, SEEK_SET) || !(result = calloc(size, sizeof(*result)))) {
    fclose(file);
  }
  else if (size != fread(result, sizeof(*result), size, file)) {
    free(result);
    result = 0;
    fclose(file);
  }
  else {
    fclose(file);
  }
  return result;
}
#endif // readfile_H
