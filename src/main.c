//
// Created by x on 11/23/2023.
//
#include <stdio.h>
#include "../include/filesystem.h"

int main() {
    if (!initializeFileSystem()) {
        fprintf(stderr, "Error: Failed to initialize file system\n");
        return 1;
    } else {
        printf("File System Initialized\n-----------------------\n");
    }

    cleanupFileSystem();
    return 0;
}