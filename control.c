#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
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
  int semid, sc;
  int shmid, shc;
  int sem_key=ftok("control.c", 22);
  int shm_key=ftok("client.c", 22);
  int fd;
  union semun su;

  if (argc <= 1){
    printf("Must input a -c, -v, or, -r flag\n");
    return 0;
  }

  //printf("Hi\n");
  //create semaphore.
  //return id
  if (strncmp( argv[1], "-c", strlen(argv[1])) == 0){
    semid = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0644);
    su.val = 1;
    sc = semctl(semid, 0, SETVAL, su);
    printf("semaphore key: %#x\n", sem_key); //prints key

    shmid = shmget(shm_key, 100, IPC_CREAT | IPC_EXCL | 0644);
    shmat(shmid, &shc, 0);
    shc = 0;
    shmdt(&shc);
      
    printf("shared memory key: %#x\n", shm_key);

    fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
    close(fd);
  }

  //check value
  if (strncmp( argv[1], "-v", strlen(argv[1])) == 0){
    fd = open("story.txt", O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char story[size];
    read(fd, story, size);
    close(fd);
    printf("%s", story);
  }

  //remove
  if (strncmp( argv[1], "-r", strlen(argv[1])) == 0){
    fd = open("story.txt", O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char story[size];
    read(fd, story, size);
    close(fd);
    printf("%s", story);

    //clear story
    fd=open("story.txt", O_TRUNC);
    close(fd);
    
    shmid = shmget(shm_key, 100, 0);
    shmat(shmid, &shc, 0);
    shmctl(shc, IPC_RMID, 0);

    semid=semget(sem_key, 0, 0);
    semctl(semid, 0, IPC_RMID);
  }

  return 0;
}
