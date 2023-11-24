//
// Created by x on 11/23/2023.
//
#include <stdio.h>
#include "../include/filesystem.h"

void runTests() {
    printf("Running File System Tests...\n");

    printf("\nTest Case 1: Initialize File System\n");
    if (initializeFileSystem()) {
        printf("File System Initialized Successfully\n");
    } else {
        printf("Error Initializing File System\n");
        return;
    }

    printf("\nTest Case 2: Create File\n");
    if (createFile("testfile1")) {
        printf("File 'testfile1' created successfully\n");
    } else {
        printf("Error creating file 'testfile1'\n");
        return;
    }

    printf("\nTest Case 3: Write to File\n");
    const char* data = "This is a test file.";
    if (writeFile("testfile1", data)) {
        printf("Data written to 'testfile1' successfully\n");
    } else {
        printf("Error writing to file 'testfile1'\n");
        return;
    }

    printf("\nTest Case 4: Read from File\n");
    char buffer[1024];
    if (readFile("testfile1", buffer)) {
        printf("Data read from 'testfile1': %s\n", buffer);
    } else {
        printf("Error reading from file 'testfile1'\n");
        return;
    }

    printf("\nTest Case 5: List Immediate Directory\n");
    listDirectory("C:\\Users\\x\\Documents\\GitHub\\c-fs\\test");

    printf("\nTest Case 6: List All Directories\n");
    listDirectories("C:\\Users\\x\\Documents\\GitHub\\c-fs\\test", 0);

    printf("\nTest Case 6: File Tree\n");
    fileTree("C:\\Users\\x\\Documents\\GitHub\\c-fs\\test", 0);

    printf("\nTest Case 7: Clean up File System\n");
    cleanupFileSystem();
    printf("File System Cleaned Up\n");

    printf("\nAll File System Tests Completed\n");
}

int main() {
    runTests();
    return 0;
}