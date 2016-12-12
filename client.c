#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
			      (Linux-specific) */
};

int main(int argc, char* argv[]){
  printf(":('\n");
  int semid;
  int shmid, shc;
  int sem_key=ftok("control.c", 22);
  int shm_key=ftok("client.c", 22);
  int fd;

  printf("why so seg faulty?\n");
  
  semid=semget(sem_key, 1, 0);
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1;
  semop(semid, &sb, 1);

  //in semaphore
  shmid = shmget(shm_key, 100, 0);
  shmat(shmid, &shc, 0);
  
  fd = open("story.txt", O_RDWR);
  lseek(fd, -shc, SEEK_END);
  char line[shc];
  read(fd, line, shc);
  printf("previous line: %s", line);

  printf("enter the next line of the story:");
  char newline[1000];
  fgets(newline, 1000, stdin);
  shc=sizeof(newline);
  write(fd, newline, sizeof(newline));
  close(fd);
  shmdt(&shc);

  //up semaphore
  sb.sem_op = 1;
  semop(semid, &sb, 1);

  return 0;
}
