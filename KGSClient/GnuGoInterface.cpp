#include "GnuGoInterface.h"

#include <unistd.h>

#include <string>

#include "../Common.h"
#include "../Go.h"

// This holds the fd for the input & output of the pipe
int commpipe[4] = { 1, 2, 3, 4};

void GnuGoSetup() {
  pid_t pid;

  if (pipe(commpipe)) {
    fprintf(stderr, "Pipe error!\n");
    exit(1);
  }
  if (pipe(commpipe+2)) {
    fprintf(stderr, "Pipe error!\n");
    exit(1);
  }

  if ((pid=fork()) == -1) {
    fprintf(stderr, "Fork error. Exiting.\n");  /* something went wrong */
    exit(1);
  }

  if (pid) {
    /* A positive (non-negative) PID indicates the parent process */
    // dup2(commpipe[2],0);    /* Replace stdin with in side of the pipe */
    // dup2(commpipe[1],1);    /* Replace stdout with out side of the pipe */
    // setvbuf(stdout,(char*)NULL,_IONBF,0);   /* Set non-buffered output on stdout */

  } else {
    /* A zero PID indicates that this is the child process */
    dup2(commpipe[0], 0);    /* Replace stdin with the in side of the pipe */
    dup2(commpipe[3], 1);    /* Replace stdout with the out side of the pipe */
    setvbuf(stdout, (char*)NULL, _IONBF, 0);   /* Set non-buffered output on stdout */
    // close(commpipe[1]);     /* Close unused side of pipe (out side) */
    /* Replace the child fork with a new process */

    /*printf("showboardchild\n");
    fflush(stdout);
    int x = 0;
    while(false && fgets(str, 120,stdin) != NULL) {

      std::ofstream myfile;
      myfile.open ("examplechild.txt", std::ofstream::out | std::ofstream::app);
      myfile << "child " << str << "\n";
      myfile.close();
      // fgets(str, 120, stdin);
      // printf("Recieved - %s",str);
      if (x++ > 1000)
        exit(0);
    }
    // printf("quit\n");


    // return 0;
    // write(commpipe[0], "showboard\n", 10);
    */
    if (execl("/usr/games/gnugo", "gnugo", "--mode", "gtp", NULL) == -1) {
    // if(execl("/bin/ls","ls", "-al", NULL) == -1){
    // if(execl("test","test", NULL) == -1){
      fprintf(stderr, "execl Error!");
      exit(1);
    }
  }
}

void GnuGoCleanup() {
  GnuGoSend("quit");
  GnuGoReceive();
}

void GnuGoSend(std::string message) {
  dprintf(commpipe[1], "%s\n", message.c_str());
  // fflush(stdout);
}

std::string GnuGoReceive() {
  FILE *fp = fdopen(commpipe[2], "r");

  char str[120];
  if (fgets(str, 120, fp) == nullptr)
    Log("GnuGoReceive: fgets returned nullptr");
  auto output = std::string(str);
  if (fgets(str, 120, fp) == nullptr)
    Log("GnuGoReceive: fgets returned nullptr");
  Log(output.c_str());
  return output;
}


std::string GnuGoReceiveBoard() {
  FILE *fp = fdopen(commpipe[2], "r");

  char str[120];
  if (fgets(str, 120, fp) == nullptr)
    Log("GnuGoReceive: fgets returned nullptr");
  auto output = std::string(str);

  for (int i = 0; i < BOARD_SIZE+2; i++) {
    if (fgets(str, 120, fp) == nullptr)
      Log("GnuGoReceive: fgets returned nullptr");
    output += std::string(str);
  }

  Log(output.c_str());
  return output;
}
