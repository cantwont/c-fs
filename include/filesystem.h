//
// Created by x on 11/23/2023.
//

#ifndef IO_FILESYSTEM_H
#define IO_FILESYSTEM_H

#include <stdbool.h>

#define BLOCK_SIZE 128
#define MAX_FILE_SIZE 1024
#define MAX_FILE_NAME 100

struct Superblock {
    int block_size;
    int num_blocks;
};

struct Inode {
    int inode_number;
    char file_name[MAX_FILE_NAME];
    int file_size;
    int data_block_pointers[MAX_FILE_SIZE / BLOCK_SIZE];
};

bool initializeFileSystem();
bool createFile(const char* fileName);
bool writeFile(const char* fileName, const char* data);
bool readFile(const char* fileName, char* buffer);
void listDirectory(const char* directoryName);
void fileTree(const char *basePath, int indent);
void listDirectories(const char *basePath, int indent);
void cleanupFileSystem();

#endif //IO_FILESYSTEM_H
