#include "utils/files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t BUF_SIZE = 1024;

int bc_files_read_file(const char *filepath, FileBuffer *out) {
#ifdef __linux__
  out->length = 0;

  out->path = malloc(strlen(filepath) + 1);
  memcpy(out->path, filepath, strlen(filepath) + 1);

  out->data = NULL;
  char buf[BUF_SIZE];
  size_t bytes_read;
  FILE *file = fopen(filepath, "r");
  if (!file) {
    return -1;
  }

  while ((bytes_read = fread(buf, 1, BUF_SIZE, file)) > 0) {
    char *new_data = realloc(out->data, out->length + bytes_read);
    if (!new_data) {
      bc_files_free_owned_memory(out);
      return -1;
    }
    out->data = new_data;
    memcpy(out->data, buf, bytes_read);
    out->length += bytes_read;
  }
  char *new_data = realloc(out->data, out->length + 1);
  if (!new_data) {
    bc_files_free_owned_memory(out);
    return -1;
  }
  new_data[out->length] = 0;
  out->data = new_data;
  out->length += 1;

  return 0;
#endif // TODO: windows :(
}

void bc_files_free_owned_memory(FileBuffer *file) {
  free(file->data);
  free(file->path);
}