/* use for simplifying the code e.g. complex functions (see ftruncate), 
common functions (see open_file), or proper error handling (see open_file)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

#define MAXFILENAME 256

FILE* open_file(const char* filename, const char* mode){
    FILE* file = fopen(filename, mode);
    if (!file) {
        perror("Error opening file");
        exit(5);
    }
    return file;
}

void truncate_file(FILE* archive, long offset){
    #ifdef _WIN32
        _chsize(fileno(archive), ftell(archive));
    #else
        ftruncate(fileno(archive), ftell(archive));
    #endif
}