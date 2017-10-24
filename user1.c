#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#define MAX_BUF 1024

int send_data(char* path, char* data) {
  int fd;
  char *myfifo = path;

  /* creates the FIFO (named pipe) */
  mkfifo(myfifo, 0666);

  /* writes data to the FIFO */
  fd = open(myfifo, O_WRONLY);
  write(fd, data, strlen(data));
  close(fd);
  
  /* removes the FIFO */
  unlink(myfifo);

  return 0;
}

int receive_data(char* path, char* data_buffer) {
  int fd = -1;
  char * myfifo = path;

  /* open and read the message from the FIFO */
  while (fd = open(myfifo, O_RDONLY), fd == -1); /* checks to see if pipe has been written to */
  read(fd, data_buffer, MAX_BUF);
  close(fd);
  return 0;
}

void* thread_send_data(char* data) {
  /* repeatedly gets user input and sends it through the pipe */
  while (1) {
    char str[1024];
    printf("Me: ");
    gets(str);
    send_data("/tmp/user1_message", str);
  }
}

void* thread_receive_data(char* buf) {
  /* repeatedly receives pipe messages and prints them */
  while(1) {
    receive_data("/tmp/user2_message", buf);
    printf("\rFriend: %s\n", buf);
    printf("Me: ");
    fflush(stdout);
    memset(buf,0,1024); /* clear data stored in my_buf */
  }
}

int main() {
  char send_buffer[1024];
  char receive_buffer[1024];
  
  pthread_t send_thread;
  pthread_create(&send_thread, NULL, thread_send_data, send_buffer);

  pthread_t receive_thread;
  pthread_create(&receive_thread, NULL, thread_receive_data, receive_buffer);
  
  void * pv;
  pthread_join(send_thread, &pv); /* not sure why I only have to do this with one thread */
  return 0;
}