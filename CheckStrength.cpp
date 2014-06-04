#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"

enum PlayerType {
  MyMonteCarlo1HalfSecond,
  MyMonteCarlo1Second,
  MyMonteCarlo5Seconds,
  MyMonteCarlo10Seconds,
  MyMonteCarlo14AndHalfSeconds,
  RandomPlayer,
  GnuGo
};

const char * playerTypeStrings[3] = {
  "MyMonteCarlo",
  "RandomPlayer",
  "GnuGo"
};

void computerMove(UCTNode** currentNode, Board* b, int numSimulations,
  float millaSecondsToThink) {
  UCTNode* newCurrentNode = NULL;
  if (numSimulations != 0)
    newCurrentNode = UCTSearch((*currentNode), numSimulations);
  else
    newCurrentNode = UCTSearch((*currentNode), millaSecondsToThink);

  (*currentNode)->removeChild(newCurrentNode);
  delete (*currentNode);

  (*currentNode) = newCurrentNode;
  newCurrentNode->parent = NULL;
  b->makeMove((*currentNode)->move);

  // check if b == currentNode->state
  for (int row = 0; row < b->boardSize; row++)
    for (int column = 0; column < b->boardSize; column++)
      if (*b->positions[row][column]
        != *(*currentNode)->state->positions[row][column]) {
        printf("Problem at %d %d\n", row, column);
      }
  if (!(*b == *(*currentNode)->state))
    assert(false);

  /*ofstream myfile;
  myfile.open ("tree.txt");
  showTree(&myfile, root, 0);
  myfile.close();*/
}

void randomMove(UCTNode** currentNode, Board* b) {
  static unsigned int seed = static_cast<unsigned int>(time(NULL));
  unsigned int choice = rand_r(&seed) %
    static_cast<unsigned int>(b->possibleMoves.size());
  Point *chosenMove = NULL;
  for (Point* p : b->possibleMoves) {
    if (choice == 0) {
      chosenMove = p;
      break;
    }
    choice--;
  }
  assert(chosenMove != NULL);

  UCTNode* newCurrentNode = new UCTNode(*chosenMove, *currentNode);
  if (newCurrentNode->parent != NULL)
    newCurrentNode->init();
  b->makeMove(*chosenMove);
  delete (*currentNode);
  (*currentNode) = newCurrentNode;
  newCurrentNode->parent = NULL;
}

int main(void) {
  printf("Running...\n");

  int numTrials = 1;
  int boardSize = 9;
  // int numSimulations = 1000;
  float komi = 0;
  PlayerType player1 = RandomPlayer;
  // PlayerType player2 = MyMonteCarlo14AndHalfSeconds;
  // PlayerType player2 = MyMonteCarlo1Second;
  PlayerType player2 = MyMonteCarlo1HalfSecond;

  for (int x = 0; x < 2; x++) {
    int player1Wins = 0;
    int player2Wins = 0;

    for (int i = 0; i < numTrials; i++) {
      if (i % 10 == 0 && i != 0)
        printf("\nTrial Number: %d\n", i);
      Board* b = new Board(boardSize);
      b->init();
      UCTNode *node = new UCTNode(Point(-1, -1), NULL);
      node->state = b->clone();

      GameResult r;
      int k = 0;
      while (!b->isGameOver(&r)) {
        PlayerType currentPlayer = b->turn == Black ? player1 : player2;
        switch (currentPlayer) {
         case RandomPlayer:
          randomMove(&node, b);
          break;
         case MyMonteCarlo1HalfSecond:
          computerMove(&node, b, 0, 500);
          break;
         case MyMonteCarlo1Second:
          computerMove(&node, b, 0, 1000);
          break;
         case MyMonteCarlo5Seconds:
          computerMove(&node, b, 0, 5000);
          break;
         case MyMonteCarlo10Seconds:
          computerMove(&node, b, 0, 10000);
          break;
         case MyMonteCarlo14AndHalfSeconds:
          computerMove(&node, b, 0, 14500);
          break;
         case GnuGo:
         default:
          assert(false);
          break;
        }
        b->show();
        k++;
        if (k > 2000)
          exit(0);
      }

      if (static_cast<Player>(r) == Black) {
        player1Wins++;
      } else if (static_cast<Player>(r) == White) {
        player2Wins++;
      } else {
        printf("Draw\n");
      }

      float score = 0;
      score = b->getTaylorScore(komi);
      score = score;
      // printf("\nWhite: %f\nBlack: %f\n", whiteScore, blackScore);

      delete b;
      delete node;
    }

    printf("\n%s: %d / %d\n%s: %d / %d\n",
      playerTypeStrings[static_cast<int>(player1)], player1Wins,
      numTrials, playerTypeStrings[static_cast<int>(player2)], player2Wins,
      numTrials);

    PlayerType temp = player1;
    player1 = player2;
    player2 = temp;
  }
}
