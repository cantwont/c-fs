//
// Created by x on 11/23/2023.
//
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../include/filesystem.h"

struct Superblock superblock;
struct Inode inodes[MAX_FILE_SIZE / BLOCK_SIZE];
char storage[MAX_FILE_SIZE];

bool is_initialized = false;

bool initializeFileSystem() {
    superblock.block_size = BLOCK_SIZE;
    superblock.num_blocks = MAX_FILE_SIZE / BLOCK_SIZE;

    for (int i = 0; i < MAX_FILE_SIZE / BLOCK_SIZE; ++i) {
        inodes[i].inode_number = i + 1;
        inodes[i].file_name[0] = '\0';
        inodes[i].file_size = 0;
        memset(inodes[i].data_block_pointers, -1, sizeof(inodes[i].data_block_pointers));
    }

    memset(storage, 0, sizeof(storage));

    is_initialized = true;
    return true;
}

bool writeToDisk(const char* fileName, const char* data) {
    char filePath[MAX_FILE_NAME + 5];
    snprintf(filePath, sizeof(filePath), "%s.dat", fileName);

    FILE* file = fopen(filePath, "w");

    if (file == NULL) {
        perror("Error opening file for writing\n");
        return false;
    }

    fwrite(data, sizeof(char), strlen(data), file);
    fclose(file);

    return true;
}

bool readFromDisk(const char* fileName, char* buffer) {
    char filePath[MAX_FILE_NAME + 5];
    snprintf(filePath, sizeof(filePath), "%s.dat", fileName);

    FILE* file = fopen(filePath, "r");

    if (file == NULL) {
        perror("Error opening file for reading\n");
        return false;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fread(buffer, sizeof(char), fileSize, file);
    fclose(file);

    return true;
}

bool createFile(const char* fileName) {
    for (int i = 0; i< MAX_FILE_SIZE / BLOCK_SIZE; ++i) {
        if (strcmp(inodes[i].file_name, fileName) == 0) {
            printf("Error: File '%s' already exists\n", fileName);
            return false;
        }
    }

    int inode_index = -1;
    for (int i = 0; i < MAX_FILE_SIZE / BLOCK_SIZE; ++i) {
        if (inodes[i].file_name[0] == '\0') {
            inode_index = i;
            break;
        }
    }

    if (inode_index == -1) {
        printf("Error: Maximum number of files reached\n");
        return false;
    }

    strcpy(inodes[inode_index].file_name, fileName);
    inodes[inode_index].file_size = 0;

    return true;
}

bool writeFile(const char* fileName, const char* data) {
    int inode_index = -1;
    for (int i = 0; i< MAX_FILE_SIZE / BLOCK_SIZE; ++i) {
        if (strcmp(inodes[i].file_name, fileName) == 0) {
            inode_index = i;
            break;
        }
    }

    if (inode_index == -1) {
        printf("Error: File '%s' not found\n", fileName);
        return false;
    }

    int data_size = strlen(data);
    int num_blocks_needed = (data_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (num_blocks_needed > MAX_FILE_SIZE / BLOCK_SIZE) {
        printf("Error: File size exceeds maximum limit\n");
        return false;
    }

    for (int i = 0; i < num_blocks_needed; ++i) {
        int block_index = -1;
        for (int j = 0; j < superblock.num_blocks; ++j) {
            if (storage[j * BLOCK_SIZE] == '\0') {
                block_index = j;
                storage[j * BLOCK_SIZE] = 1;
                break;
            }
        }

        if (block_index == -1) {
            printf("Error: Insufficient storage space\n");
            return false;
        }

        inodes[inode_index].data_block_pointers[i] = block_index;
        int offset = 0;
        for (int i2 = 0; i2 < num_blocks_needed; ++i2) {
            int block_index2 = inodes[inode_index].data_block_pointers[i2];
            memcpy(storage + block_index2 * BLOCK_SIZE, data + offset, BLOCK_SIZE);
            offset += BLOCK_SIZE;
        }
    }

    inodes[inode_index].file_size = data_size;
    writeToDisk(fileName, data);
    return true;
}

bool readFile(const char* fileName, char* buffer) {
    int inode_index = -1;
    for (int i = 0; i < MAX_FILE_SIZE / BLOCK_SIZE; ++i) {
        if (strcmp(inodes[i].file_name, fileName) == 0) {
            inode_index = i;
            break;
        }
    }

    if (inode_index == -1) {
        printf("Error: File '%s' not found\n", fileName);
        return false;
    }

    int data_size = inodes[inode_index].file_size;
    int num_blocks_needed = (data_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    int offset = 0;
    for (int i = 0; i < num_blocks_needed; ++i) {
        int block_index = inodes[inode_index].data_block_pointers[i];
        memcpy(buffer + offset, storage + block_index * BLOCK_SIZE , BLOCK_SIZE);
        offset += BLOCK_SIZE;
    }

    readFromDisk(fileName, buffer);
    return true;
}

void listDirectory(const char* directoryName) {
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directoryName)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            printf("%s\n", ent->d_name);
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
    }
}

void fileTree(const char *basePath, int indent) {
    DIR *dir;
    struct dirent *ent;
    struct stat fileStat;

    if ((dir = opendir(basePath)) != NULL) {
        char *folderName = strrchr(basePath, '/');
        if (folderName != NULL) {
            folderName++;
        } else {
            folderName = basePath;
        }

        printf("%*s%s/\n", indent, "", folderName);

        while ((ent = readdir(dir)) != NULL) {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, ent->d_name);

            if (stat(fullPath, &fileStat) < 0) {
                perror("Error getting file status");
                closedir(dir);
                exit(EXIT_FAILURE);
            }

            if (S_ISDIR(fileStat.st_mode)) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                    continue;
                }

                fileTree(fullPath, indent + 4);
            } else {
                printf("%*s%s\n", indent + 4, "", ent->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
}

void listDirectories(const char *basePath, int indent) {
    DIR *dir;
    struct dirent *ent;
    struct stat fileStat;

    if ((dir = opendir(basePath)) != NULL) {
        char *folderName = strrchr(basePath, '/');
        if (folderName != NULL) {
            folderName++;
        } else {
            folderName = basePath;
        }

        printf("%*s%s/\n", indent, "", folderName);

        while ((ent = readdir(dir)) != NULL) {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, ent->d_name);

            if (stat(fullPath, &fileStat) < 0) {
                perror("Error getting file status");
                closedir(dir);
                exit(EXIT_FAILURE);
            }

            if (S_ISDIR(fileStat.st_mode)) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                    continue;
                }

                listDirectories(fullPath, indent + 4);
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
}

void cleanupFileSystem() {
    for (int i = 0; i < MAX_FILE_SIZE / BLOCK_SIZE; ++i) {
        inodes[i].inode_number = i + 1;
        inodes[i].file_name[0] = '\0';
        inodes[i].file_size = 0;
        memset(inodes[i].data_block_pointers, -1, sizeof(inodes[i].data_block_pointers));
    }

    memset(storage, 0, sizeof(storage));
    is_initialized = false;
}