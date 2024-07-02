#include "base.h"

typedef struct {
    char filename[MAXFILENAME];
    long filesize;
} Archive;

void CreateNode(const char* archivename, const char* filename) { 
    Archive node;
    
    FILE* archive = open_file(archivename, "ab");
    FILE* file = open_file(filename, "rb");

    snprintf(node.filename, sizeof(node.filename), "%s", filename);
    fseek(file, 0, SEEK_END);
    node.filesize = ftell(file);

    fwrite(&node, sizeof(node), 1, archive);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(node.filesize);
    fread(buffer, 1, node.filesize, file);
    fwrite(buffer, 1, node.filesize, archive);

    free(buffer);
    fclose(file);
    fclose(archive);
}

Archive LoadNode(FILE* archive) {
    Archive node;
    if (feof(archive)){
        perror("Reached end of file");
        exit(5);
    }
    if (fread(&node, sizeof(Archive), 1, archive) == 1) {
        return node;
    }
    return node;
}

char* ReadInPlace(FILE* archive, int size){
    char*buffer= (char*)calloc(size+1, sizeof(char));
    fread(buffer, 1, size, archive);
    return buffer;
}

char* ReadNode(const char* archivename, const char* filename) {
    FILE* archive = open_file(archivename, "rb");
    Archive node = LoadNode(archive);
    while (strcmp(node.filename, filename) != 0){
        fseek(archive, node.filesize, SEEK_CUR);
        node = LoadNode(archive);
    }
    char* buffer = (char*)calloc(node.filesize+1, sizeof(char));
    fread(buffer, 1, node.filesize, archive);
    fclose(archive);
    return buffer;
}

void WriteNode(const char* archivename, const char* filename){
    char* buffer=ReadNode(archivename, filename);
    FILE* file = open_file(filename, "wb");
    fwrite(buffer, sizeof(buffer), 1, file);
    fclose(file);
}

int distancetoend(FILE* archive){
    int current = ftell(archive);
    fseek(archive, 0, SEEK_END);
    int end = ftell(archive);
    fseek(archive, current, SEEK_SET); // Corrected this line
    return end - current;
}

void DeleteNode(const char* archivename, const char* filename){
    FILE* archive = open_file(archivename, "rb+");

    Archive node = LoadNode(archive);
    while (strcmp(node.filename, filename) != 0){
        fseek(archive, node.filesize, SEEK_CUR);
        node=LoadNode(archive);
    }
    fseek(archive, node.filesize, SEEK_CUR);
    int offset = sizeof(Archive)+node.filesize;
    while (distancetoend(archive) > offset) {
        node = LoadNode(archive);
        char* contents = ReadInPlace(archive, node.filesize);
        int extra = sizeof(Archive)+node.filesize;
        fseek(archive, -1*(offset+extra), SEEK_CUR);
        fwrite(&node, sizeof(node), 1, archive);
        fwrite(contents, 1, node.filesize, archive);
        fseek(archive, offset, SEEK_CUR);
    }
    fseek(archive, -offset, SEEK_CUR);
    truncate_file(archive, ftell(archive));
    fclose(archive);
}

void UpdateNode(const char* archivename, const char* filename) {
    DeleteNode(archivename, filename);
    CreateNode(archivename, filename);
}