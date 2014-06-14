#include <iostream>
#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <fstream>

#include "../UCT.h"
#include "../Common.h"
#include "CommonNetwork.h"

//UCTNode *currentMove;
int boardSize = 9;
float millaSecondsToThink = 100;
char name[] = "TestGoBot";
char version[] = "3.5.11";

Board* b = NULL;
std::list<unsigned long int> previousHashes;

void switchTurnTo(Player color) {
  if (b->turn == color) {
    return;
  } else {
    // Pass
    b->makeMove(Point(boardSize, boardSize));
  }
}

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

const char* handlePlayMove(Player color, Point move) {
  if (color == Empty) {
    printf("Color should not be empty");
    exit(0);
  }

  switchTurnTo(color);
  b->makeMove(move);
  previousHashes.push_back(b->getHash());

  char buffer[100];
  sprintf(buffer, "Play %d at %d %d", color, move.row, move.column);
  Log(buffer);

  return goodResponse;
}

char* handleGenMove(Player color) {
  if (color == Empty) {
    printf("Color should not be empty");
    exit(0);
  }

  switchTurnTo(color);

  b->eliminatePositionalSuperKo(previousHashes);

  UCTNode* currentMove = new UCTNode(Point(-1, -1), NULL);
  currentMove->state = b->clone();
  Point bestMove = UCTSearch(currentMove, millaSecondsToThink)->move;

  char rowChar = (char)(bestMove.row+'a');
  if (bestMove.row > 7)
    rowChar++;

  char* response = new char[BUFFSIZE];
  if (bestMove == *b->pass)
    snprintf(response, sizeof(response), "= pass\n\n");
  else
    snprintf(response, sizeof(response),
      "= %c%d\n\n", rowChar, bestMove.column+1);

  char buffer[100];
  sprintf(buffer, "GenMove %d %d", bestMove.row, bestMove.column);
  Log(buffer);
  
  // Takes a long time, print out message to send to server first
  delete currentMove;

  b->makeMove(bestMove);
  previousHashes.push_back(b->getHash());

  return response;
}

const char* handleBoardSize(char* request) {
  boardSize = atoi(request);

  delete b;
  previousHashes.clear();
  b = new Board(boardSize);
  b->init();

  previousHashes.push_back(b->getHash());
  
  return goodResponse;
}

Player getPlayer(char c) {
  if (c == 'b')
    return Black;
  if (c == 'B')
    return Black;
  if (c == 'w')
    return White;
  if (c == 'W')
    return White;
  printf("%c is not a valid player\n", c);
  exit(0);
}

Point getMove(char* strMove) {
  Point move(-1, -1);
  
  if (!strncmp(strMove, "pass", strlen("pass"))) {
    move.row = boardSize;
    move.column = boardSize;
  } else if (strMove[0] >= 'a') {
    move.row = strMove[0] - 'a';
    if (strMove[0] >= 'i') {
      move.row--;
    }
    move.column = atoi(strMove+1)-1;
  } else if (strMove[0] >= 'A') {
    move.row = strMove[0] - 'A';
    if (strMove[0] >= 'I') {
      move.row--;
    }
    move.column = atoi(strMove+1)-1;
  } else {
    printf("%s is not a valid move", strMove);
    exit(0);
  }

  return move;
}

void handleClient(int sock) {

  while (true) {
    char* request = receieveText(sock);
    const char* response = NULL;

    Log(request);

    if (!strncmp(request, "boardsize ", strlen("boardsize "))) {
      response = handleBoardSize(request + strlen("boardsize "));
    } else if (!strncmp(request, "genmove ", strlen("genmove "))) {
      Player turn = getPlayer(*(request+strlen("genmove ")));
      response = handleGenMove(turn);
    } else if (!strncmp(request, "play ", strlen("play "))) {
      Player turn = getPlayer(*(request+strlen("play ")));
      Point move = getMove(request+strlen("play b "));
      response = handlePlayMove(turn, move);
    } else if (!strncmp(request, "quit", strlen("quit"))) {
      break;
    }

    sendText(sock, response);
  }

  close(sock);
}

int main(int argc, char *argv[]) {
  int serverSock, clientSock;
  struct sockaddr_in echoServer, echoClient;

  if (argc != 2) {
    printf("%s <port>\n", argv[0]);
    exit(0);
  }
  if ((serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("Failed to create socket");
    exit(0);
  }

  memset(&echoServer, 0, sizeof(echoServer));
  echoServer.sin_family = AF_INET;
  echoServer.sin_addr.s_addr = htonl(INADDR_ANY);
  echoServer.sin_port = htons((short unsigned int)atoi(argv[1]));

  if (bind(serverSock, (struct sockaddr*) &echoServer,
    sizeof(echoServer)) < 0) {
    perror("Failed to bind the server socket");
    exit(0);
  }
  if (listen(serverSock, MAXPENDING) < 0) {
    perror("Failed to listen on the server socket");
    exit(0);
  }

  b = new Board(boardSize);
  b->init();

  previousHashes.push_back(b->getHash());
  //currentMove = new UCTNode(Point(-1, -1), b, NULL);

  while (true) {
    unsigned int clientLen = sizeof(echoClient);
    if ((clientSock =
        accept(serverSock, (struct sockaddr*) &echoClient,
        &clientLen)) < 0) {
      perror("Failed to listen on the server socket");
      exit(0);
    }
    printf("Client connected: %s\n", inet_ntoa(echoClient.sin_addr));
    handleClient(clientSock);
  }
/*
  while (true) {
    char command[128];
    std::cin >> command;

    // Log(command);

    if (!strcmp(command, "boardsize")) {
      doBoardSize();
    } else if (!strcmp(command, "clear_board")) {
      delete b;
      b = new Board(boardSize);
      b->init();
      printf("= \n\n");
    } else if (!strcmp(command, "exit")) {
      exit(0);
    } else if (!strcmp(command, "final_status_list")) {
      char throwaway[200];
      std::cin >> throwaway;
      printf("= \n\n");
      Log(throwaway);
    } else if (!strcmp(command, "genmove")) {
      doGenMove();
    } else if (!strcmp(command, "kgs-genmove_cleanup")) {
      doGenMove();
    } else if (!strcmp(command, "komi")) {
      float throwaway;
      std::cin >> throwaway;
      printf("= \n\n");
    } else if (!strcmp(command, "list_commands")) {
      printf("= boardsize\n");
      printf("clear_board\n");
      printf("genmove\n");
      printf("komi\n");
      printf("list_commands\n");
      printf("name\n");
      printf("play\n");
      printf("quit\n");
      printf("time_left\n");
      printf("version\n");
      printf("\n");
    } else if (!strcmp(command, "name")) {
      printf("= %s\n\n", name);
    } else if (!strcmp(command, "play")) {
      doPlay();
    } else if (!strcmp(command, "quit")) {
      printf("= \n\n");
    } else if (!strcmp(command, "special")) {
      printf("%d\n", b->isSuicide(Point(4,1)));
      printf("%d %d\n", b->koPoint->row, b->koPoint->column);
    } else if (!strcmp(command, "show_board")) {
      b->show();
      printf("\n\n");
    } else if (!strcmp(command, "show_moves")) {
      b->getPossibleMoves();
      for (int i = 0; i < b->numLegalMoves; i++)
        printf("%d%d ", b->legalMoves[i]->row, b->legalMoves[i]->column);
      printf("\n\n");
    } else if (!strcmp(command, "time_left")) {
      char throwawayColor[10];
      std::cin >> throwawayColor;

      int throwawayFirstNumber;
      std::cin >> throwawayFirstNumber;

      int throwawaySecondNumber;
      std::cin >> throwawaySecondNumber;

      printf("= \n\n");
    } else if (!strcmp(command, "version")) {
      printf("= %s\n\n", version);
    } else {
      printf("? unknown command \n\n");
      Log("? unknown command");
      Log("command");
    }
  }*/

} 
