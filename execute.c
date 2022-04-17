/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "stdbool.h"
#include <pthread.h>

void *printWhenDie(int cPid, void *vargp);

int execute(char *argv[])
{

  int	pid;
  int	child_info = -1;
  int lastindex = 0;
  bool bg_flag = false;
  

  if (argv[0] == NULL )		/* nothing succeeds	*/
    return 0;
  else if (strcmp(argv[0], "exit") == 0 ) //user types exit
    if (argv[1] == NULL)
      exit(0);
    else
      exit(atoi(argv[1])); //exit with a number
  else if (strcmp(argv[0], "cd") == 0 ) {//user types cd
    if (argv[1] == NULL)
      chdir(getenv("HOME"));//go to home directory
    else {
      if (chdir(argv[1]) == -1 ) { //if cd fails
        fprintf(stderr, "cd: %s: No such file or directory\n", argv[1]);
        return -1;
      }
    }
    return 0;
  }

  //get the length of argv using argc gives segfault :(
  for (;argv[lastindex+1] != NULL; lastindex++);
  
  // Looks for background flag, removes from argv if existing and set bg_flag to true
  if (strcmp(argv[lastindex], "&") == 0) {
    bg_flag = true;
    argv[lastindex] = NULL;
  }

  if ((pid = fork())  == -1 )
    fprintf(stderr, "fork: failed\n");
  else if ( pid == 0 ) {
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    execvp(argv[0], argv);
    fprintf(stderr, "cmd: %s: Command not found\n", argv[0]);
    exit(1);
  }
  else {
    //if flag is present run in background
    if (bg_flag) {
      pthread_t thread_id;
      printf("Process %d started in background.\n", pid);
      // Creates thread to print out when process dies
      pthread_create(&thread_id, NULL, printWhenDie, (void *)pid);

    } else { //else just wait for process to finish
      if (waitpid(pid, &child_info, 0) == -1){
        fprintf(stderr, "wait");
      }
    }
  }
  return child_info;
}

void *printWhenDie(int cPid, void *vargp) {
  int status;
  sleep(1);
  //wait for process with cPid to end
  while (waitpid(cPid, &status, WNOHANG) == 0) {
    //do nothing
  }
  //if process has ended, print out the process id
  printf("Process %d ended. \n", cPid);
}