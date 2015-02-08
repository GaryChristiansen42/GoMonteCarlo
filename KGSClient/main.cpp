#include <iostream>
#include <assert.h>
#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "../UCT.h"
#include "../Common.h"

#include "GnuGoInterface.h"

// #define USING_TOP_MOVES

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
  Log("readColor");
  Log(colorStr);
  if (!strcmp(colorStr, "w")) {
    return White;
  } else if (!strcmp(colorStr, "b")) {
    return Black;
  } else if (!strcmp(colorStr, "white")) {
    return White;
  } else if (!strcmp(colorStr, "black")) {
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
Point KGSPositionToPoint(const char* position) {
  Point p;
  if (strcmp(position, "pass")) {
    p.row = (unsigned char)(toupper(position[0]) - 'A');
    if (toupper(position[0]) >= 'I') {
      p.row--;
    }
    p.column = (unsigned char)(atoi(position+1)-1);
  }
  return p;
}

void doPlay() {
  Player color = readColor();
  if (color == Empty)
    return;

  char moveStr[10];
  std::cin >> moveStr;
  Log("moveStr");
  Log(moveStr);
  Point p = KGSPositionToPoint(moveStr);
  
  switchTurnTo(color);
  b->makeMove(p);
  previousHashes.push_back(b->getHash());
  printf("=\n\n");

#ifdef USING_TOP_MOVES
  std::string gnugoMessage = "play " + (color == Black ? std::string("b") : std::string("w")) + " " + moveStr;
  GnuGoSend(gnugoMessage);
  GnuGoReceive();
#endif

  char buffer[100];
  sprintf(buffer, "Play %d at %d %d", color, p.row, p.column);
  Log(buffer);
}

void doGenMove() {
  Player color = readColor();
  if (color == Empty)
    return;

  switchTurnTo(color);

  // b->eliminatePositionalSuperKo(previousHashes);

  UCTNode* currentMove = new UCTNode(-1, -1, nullptr);

#ifdef USING_TOP_MOVES
  GnuGoSend(std::string("top_moves_") + (color == Black ? std::string("black") : std::string("white")));
  auto response = GnuGoReceive(); // takes form of "= F3 16.77 E4 16.55 G4 16.55"

  std::string buf;
  std::stringstream ss(response);
  std::vector<std::string> tokens;

  while (ss >> buf)
    tokens.push_back(buf);

  for (unsigned int i = 1; i < tokens.size(); i += 2) {
    // printf("%s\n", tokens[i].c_str());
    Point p = KGSPositionToPoint(tokens[i].c_str());
    currentMove->possibleChildren.push_back((unsigned short)(p.row*BOARD_SIZE + p.column));
  }
#endif 



  UCTNode* bestMoveNode = UCTSearch(currentMove, millaSecondsToThink, nullptr);
  if ((1.0*((double)bestMoveNode->totalRewards))/((double)bestMoveNode->visits) < (-0.9)) {
    printf("= resign\n\n");
  } else {
    Point bestMove = Point(bestMoveNode->row, bestMoveNode->column);
    char* output = pointToKGSPosition(bestMove);
    printf("= %s\n\n", output);

#ifdef USING_TOP_MOVES
    std::string gnugoMessage = "play " + (color == Black ? std::string("b") : std::string("w")) + " " + output;
    GnuGoSend(gnugoMessage);
    GnuGoReceive();
#endif

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
  UCTNode::rootState = b;

  previousHashes.push_back(b->getHash());

#ifdef USING_TOP_MOVES
  GnuGoSend(std::string("boardsize ") + std::to_string(boardSize));
  GnuGoReceive();
#endif

  printf("= \n\n");
}

int main() {
  b = new Board();
  b->init();
  UCTNode::rootState = b;

  previousHashes.push_back(b->getHash());
  //currentMove = new UCTNode(Point(-1, -1), b, nullptr);

#ifdef USING_TOP_MOVES
  GnuGoSetup();
#endif

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
#ifdef USING_TOP_MOVES
      GnuGoCleanup();
#endif

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
    } else if (!strcmp(command, "genmove_black")) {
      b->turn = Black;
      doGenMove();
    } else if (!strcmp(command, "genmove_white")) {
      b->turn = White;
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
      printf("genmove_black\n");
      printf("genmove_white\n");
      printf("kgs-genmove_cleanup\n");
      printf("komi\n");
      printf("list_commands\n");
      printf("name\n");
      printf("play\n");
      printf("protocol_version\n");
      printf("quit\n");
      printf("time_left\n");
      printf("version\n");
      printf("\n");
    } else if (!strcmp(command, "name")) {
      printf("= %s\n\n", name);
    } else if (!strcmp(command, "play")) {
      doPlay();
#ifdef USING_TOP_MOVES
      GnuGoSend("showboard");
      Log(GnuGoReceiveBoard().c_str());
#endif
    } else if (!strcmp(command, "protocol_version")) {
      printf("= %s\n\n", version);
    } else if (!strcmp(command, "quit")) {
      printf("= \n\n");
#ifdef USING_TOP_MOVES
      GnuGoCleanup();
#endif

      exit(0);
    } else if (!strcmp(command, "special")) {
      // printf("%d\n", b->isSuicide(Point(4,1)));
      // printf("%d %d\n", b->koPoint->row, b->koPoint->column);
      GnuGoSend("top_moves_black");
      printf("%s", GnuGoReceive().c_str());
    } else if (!strcmp(command, "show_board") || !strcmp(command, "showboard")) {
      /*b->show();
#ifdef USING_TOP_MOVES
      GnuGoSend("showboard");
      printf("%s\n", GnuGoReceiveBoard().c_str());
#endif
      */
      printf("= \n");
      printf("\n");
    } else if (!strcmp(command, "show_moves")) {
      auto legalMoves = b->getPossibleMoves();
      for (unsigned int i = 0; i < legalMoves.size(); i++)
        printf("%d%d ", legalMoves[i]->row, legalMoves[i]->column);
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
