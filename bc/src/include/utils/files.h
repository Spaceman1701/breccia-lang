#include <stdlib.h>

//avoid leaking platform specific file I/O
//Breccia just reads the entire file into memory, so this is a very simple struct
typedef struct {
    char* path;
    char* data;
    size_t length;
} FileBuffer;


int bc_files_read_file(const char* filepath, FileBuffer* out);

void bc_files_free_owned_memory(FileBuffer* file);