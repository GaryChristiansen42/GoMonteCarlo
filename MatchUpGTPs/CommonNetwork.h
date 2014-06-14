#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include "../Common.h"

#define MAXPENDING 5
#define BUFFSIZE 32

const char goodResponse[] = "= \n\n";

void sendText(int sock, const char* message) {
  int len = (int)strlen(message);
  if (send(sock, message, len, 0) != len) {
    Log("Failed to send");
    Log(message);
    char buffer[BUFFSIZE];
    snprintf(buffer, sizeof(buffer), 
      "Failed to send message %s\n", message);
    perror(buffer);
    exit(0);
  }
  Log("Send");
  Log(message);
}

char* receieveText(int sock) {
  char* buffer = new char[BUFFSIZE];
  int bytes = 0;
  if ((bytes = (int)recv(sock, buffer, BUFFSIZE-1, 0)) < 1) {
    char buf[200];
    snprintf(buf, sizeof(buf), 
      "Failed to receive text from server %d - %s\n", bytes, buffer);
    perror(buf);
    exit(0);
  }
  buffer[bytes] = '\0';
  Log("Receive");
  Log(buffer);
  return buffer;
}

