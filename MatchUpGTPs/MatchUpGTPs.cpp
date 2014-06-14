#include <iostream>
#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <fstream>

#include "../Common.h"
#include "CommonNetwork.h"


/*int boardSize = 9;

Player readColor() {
  char colorStr[10];
  std::cin >> colorStr;
  if (!strcmp(colorStr, "w")) {
    return White;
  } else if (!strcmp(colorStr, "b")) {
    return Black;
  } else {
    printf("? Unknown Player color\n");
    return Empty;
  }
}

void doPlay() {
  Player color = readColor();
  if (color == Empty)
    return;

  char moveStr[10];
  std::cin >> moveStr;
  Log("MoveStr");
  Log(moveStr);
  int row = boardSize, column = boardSize;
  if (strcmp(moveStr, "pass")) {
    row = moveStr[0] - 'A';
    if (moveStr[0] >= 'I') {
      row--;
    }
    column = atoi(moveStr+1)-1;
  }

  switchTurnTo(color);
  b->makeMove(Point(row, column));
  previousHashes.push_back(b->getHash());
  printf("=\n\n");

  char buffer[100];
  sprintf(buffer, "Play %d at %d %d", color, row, column);
  Log(buffer);
}

void doGenMove() {
  Player color = readColor();
  if (color == Empty)
    return;

  switchTurnTo(color);

  b->eliminatePositionalSuperKo(previousHashes);

  UCTNode* currentMove = new UCTNode(Point(-1, -1), NULL);
  currentMove->state = b->clone();
  Point bestMove = UCTSearch(currentMove, millaSecondsToThink)->move;

  char rowChar = (char)(bestMove.row+'a');
  if (bestMove.row > 7)
    rowChar++;
  if (bestMove == *b->pass)
    printf("= pass\n\n");
  else
    printf("= %c%d\n\n", rowChar, bestMove.column+1);

  char buffer[100];
  sprintf(buffer, "GenMove %d %d", bestMove.row, bestMove.column);
  Log(buffer);
  
  // Takes a long time, print out message to send to server first
  delete currentMove;

  b->makeMove(bestMove);
  previousHashes.push_back(b->getHash());
}

void doBoardSize() {
  std::cin >> boardSize;
  delete b;
  previousHashes.clear();
  b = new Board(boardSize);
  b->init();

  previousHashes.push_back(b->getHash());

  printf("= \n\n");
}

void handleClient(int sock) {
  char buffer[BUFFSIZE];
  int received = -1;
  if ((received = (int)recv(sock, buffer, BUFFSIZE, 0)) < 0) {
    perror("Failed to receive bytes from client");
    exit(0);
  }

  while (received > 0) {
    if (send(sock, buffer, received, 0) != received) {
      perror("Failed to send bytes to client");
    }

    if ((received = (int)recv(sock, buffer, BUFFSIZE, 0)) < 0) {
      perror("Failed to receive additional bytes from client");
    }
  }
  close(sock);
}*/

void setupClient(int* sock, char* ip, int port) {
  struct sockaddr_in client;
  if ((*sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    char buffer[200];
    snprintf(buffer, sizeof(buffer), 
      "Failed to create socket for %s %d", ip, port);
    perror(buffer);
    exit(0);
  }

  memset(&client, 0, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = inet_addr(ip);
  client.sin_port = htons((short unsigned int)port);

  if (connect(*sock,
      (struct sockaddr*) &client,
      sizeof(client)) < 0) {
    char buffer[200];
    snprintf(buffer, sizeof(buffer), 
      "Failed to connect to %s %d", ip, port);
    perror(buffer);
    exit(0);
  }
}


void sendBoardSize(int sock, int boardSize) {
  char message[200];
  snprintf(message, sizeof(message), "boardsize %d", boardSize);
  sendText(sock, message);

  char* response = receieveText(sock);
  if (strcmp(response, goodResponse)) {
    printf("sendBoardSize: received bad response %s\n", response);
    exit(0);
  }
}

char* getMove(int sock, char turn) {
  char buffer[200];
  snprintf(buffer, sizeof(buffer), "genmove %c", turn);
  sendText(sock, buffer);
  char * response = receieveText(sock);
  if (strncmp(response, goodResponse, 2)) {
    printf("sendBoardSize: received bad response %s\n", response);
    exit(0);
  }
  return response+2;
}

void sendMove(int sock, char turn, char* move) {
  char buffer[200];
  snprintf(buffer, sizeof(buffer), "play %c %s", turn, move);
  sendText(sock, buffer);
  char * response = receieveText(sock);
  if (strcmp(response, goodResponse)) {
    printf("sendBoardSize: received bad response %s\n", response);
    exit(0);
  }
}

void playGame(int sock1, int sock2, int boardSize) {

  sendBoardSize(sock1, boardSize);
  sendBoardSize(sock2, boardSize);

  bool passLastTime = false;
  int sockTurn = sock1;
  char turn = 'b';
  
  printf("BoardSizes sent\n");

  while (true) {
    char* move = getMove(sockTurn, turn);
    printf("Got Move\n");
    printf("%s\n", move);
    if(!strncmp(move, "pass", strlen("pass"))) {
      if (passLastTime) {
        break;
      }
      passLastTime = true;
    } else if (!strcmp(move, "resign")) {
      if (sockTurn == sock1) {
        printf("%c won.\n", turn);
      } else {
        printf("%c won.\n", turn);
      }
      break;
    } else {
      passLastTime = false;
    }

    sockTurn = sockTurn == sock1 ? sock2 : sock1;
    sendMove(sockTurn, turn, move);
    turn = turn == 'b' ? 'w' : 'b';
    printf("Sent Move\n");
  }

  sendText(sock1, "quit");
  sendText(sock2, "quit");
}

int main(int argc, char *argv[]) {
  int client1Sock, client2Sock;

  if (argc != 6) {
    printf("%s <ip> <port> <ip> <port> <boardsize>\n", argv[0]);
    exit(0);
  }
  
  setupClient(&client1Sock, argv[1], atoi(argv[2]));
  setupClient(&client2Sock, argv[3], atoi(argv[4]));

  int boardSize = atoi(argv[5]);

  playGame(client1Sock, client2Sock, boardSize);
} 
