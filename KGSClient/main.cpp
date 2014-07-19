#include <iostream>
#include <assert.h>
#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

#include "../UCT.h"
#include "../Common.h"

//UCTNode *currentMove;
float millaSecondsToThink = 14500;
char name[] = "TestGoBot";
char version[] = "3.5.11";

Board* b = nullptr;
std::list<unsigned long int> previousHashes;

void switchTurnTo(Player color) {
  if (b->turn == color) {
    return;
  } else {
    // Pass
    b->makeMove(Point(BOARD_SIZE, BOARD_SIZE));
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

char* pointToKGSPosition(Point p) {
  char rowChar = (char)(p.row+'a');
  if (p.row > 7)
    rowChar++;
  char* buffer = new char[32];
  if (p == *b->pass)
    snprintf(buffer, 32, "pass");
  else
    snprintf(buffer, 32, "%c%d", 
      rowChar, p.column+1);
  return buffer;
}

void doPlay() {
  Player color = readColor();
  if (color == Empty)
    return;

  char moveStr[10];
  std::cin >> moveStr;
  unsigned char row = BOARD_SIZE, column = BOARD_SIZE;
  if (strcmp(moveStr, "pass")) {
    row = (unsigned char)(moveStr[0] - 'a');
    if (moveStr[0] >= 'i') {
      row--;
    }
    column = (unsigned char)(atoi(moveStr+1)-1);
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

  UCTNode* currentMove = new UCTNode(Point(-1, -1), nullptr);
  currentMove->state = b->clone();
  UCTNode* bestMoveNode = UCTSearch(currentMove, millaSecondsToThink);
  if ((1.0*bestMoveNode->totalRewards)/bestMoveNode->visits < (-0.9)) {
    printf("= resign\n\n");
  } else {
    Point bestMove = bestMoveNode->move;
    char* output = pointToKGSPosition(bestMove);
    printf("= %s\n\n", output);
    delete output;

    char buffer[100];
    sprintf(buffer, "GenMove %d %d", bestMove.row, bestMove.column);
    Log(buffer);

    b->makeMove(bestMove);
    previousHashes.push_back(b->getHash());
  }   
  // Takes a long time, print out message to send to server first
  usleep(1000);
  delete currentMove;

}

void doBoardSize() {
  int boardSize;
  std::cin >> boardSize;
  assert(boardSize == BOARD_SIZE);
  delete b;
  previousHashes.clear();
  b = new Board();
  b->init();

  previousHashes.push_back(b->getHash());

  printf("= \n\n");
}

int main() {
  b = new Board();
  b->init();

  previousHashes.push_back(b->getHash());
  //currentMove = new UCTNode(Point(-1, -1), b, nullptr);

  while (true) {
    char command[128];
    std::cin >> command;

    Log(command);

    if (!strcmp(command, "boardsize")) {
      doBoardSize();
    } else if (!strcmp(command, "clear_board")) {
      delete b;
      b = new Board();
      b->init();
      printf("= \n\n");
    } else if (!strcmp(command, "exit")) {
      exit(0);
    } else if (!strcmp(command, "final_status_list")) {
      char throwaway[200];
      std::cin >> throwaway;
      std::string aliveStones = "";
      /*for (int r = 0; r < b->boardSize; r++) {
        for (int c = 0; c < b->boardSize; c++) {
          if (b->positions[r][c]->color != Empty) {
            aliveStones += " ";
            aliveStones += pointToKGSPosition(*b->positions[r][c]);
          }
        }
      }*/
      printf("= %s\n\n", aliveStones.c_str());
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
      printf("final_status_list\n");
      printf("genmove\n");
      printf("kgs-genmove_cleanup\n");
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
      // printf("%d\n", b->isSuicide(Point(4,1)));
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
  }

} 
