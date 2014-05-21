#include <iostream>
#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <fstream>

#include "../UCT.h"

//UCTNode *currentMove;
int boardSize = 9;
float millaSecondsToThink = 14000;
char name[] = "TestGoBot";
char version[] = "3.5.11";

Board* b = NULL;

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

void doPlay() {
  Player color = readColor();
  if (color == Empty)
    return;

  char moveStr[10];
  std::cin >> moveStr;
  int row = boardSize, column = boardSize;
  if (strcmp(moveStr, "pass")) {
    row = moveStr[0] - 'a';
    if (moveStr[0] >= 'i') {
      row--;
    }
    column = atoi(moveStr+1)-1;
  }

  switchTurnTo(color);
  b->makeMove(Point(row, column));
  printf("=\n\n");

  char buffer[100];
  sprintf(buffer, "Play %d at %d %d", color, row, column);
  log(buffer);
}

void doGenMove() {
  Player color = readColor();
  if (color == Empty)
    return;

  switchTurnTo(color);

  UCTNode* currentMove = new UCTNode(Point(-1, -1), b, NULL);
  Point bestMove = UCTSearch(currentMove, b, millaSecondsToThink)->move;
  delete currentMove;

  b->makeMove(bestMove);

  char rowChar = (char)(bestMove.row+'a');
  if (bestMove.row > 7)
    rowChar++;
  if (bestMove == Point(boardSize, boardSize))
    printf("= pass\n\n");
  else
    printf("= %c%d\n\n", rowChar, bestMove.column+1);

  char buffer[100];
  sprintf(buffer, "GenMove %d %d", bestMove.row, bestMove.column);
  log(buffer);
}

void doBoardSize() {
  std::cin >> boardSize;
  delete b;
  b = new Board(boardSize);

  printf("= \n\n");
}

int main() {
  b = new Board(boardSize);
  //currentMove = new UCTNode(Point(-1, -1), b, NULL);

  while (true) {
    char command[128];
    std::cin >> command;

    log(command);

    if (!strcmp(command, "boardsize")) {
      doBoardSize();
    } else if (!strcmp(command, "clear_board")) {
      delete b;
      b = new Board(boardSize);
      printf("= \n\n");
    } else if (!strcmp(command, "exit")) {
      exit(0);
    } else if (!strcmp(command, "genmove")) {
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
      printf("%d %d\n", b->koPoint.row, b->koPoint.column);
    } else if (!strcmp(command, "show_board")) {
      b->show();
      printf("\n\n");
    } else if (!strcmp(command, "show_moves")) {
      b->getPossibleMoves();
      for(Point p : b->possibleMoves)
        printf("%d%d ", p.row, p.column);
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
      log("? unknown command");
      log("command");
    }
  }

} 
