#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"

enum PlayerType {
  MyMonteCarlo,
  RandomPlayer,
  GnuGo
};

const char * playerTypeStrings[3] = {
  "MyMonteCarlo",
  "RandomPlayer",
  "GnuGo"
};

void computerMove(UCTNode** currentNode, Board* b, int numSimulations) {
  UCTNode* newCurrentNode = UCTSearch((*currentNode), b, numSimulations);

  (*currentNode)->removeChild(newCurrentNode);
  delete (*currentNode);

  (*currentNode) = newCurrentNode;
  b->makeMove((*currentNode)->move);
  // check if b == currentNode->state
  for (int row = 0; row < b->boardSize; row++)
    for (int column = 0; column < b->boardSize; column++)
      if (b->positions[row][column]
        != (*currentNode)->state->positions[row][column]) {
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
  static unsigned int seed = time(NULL);
  unsigned int choice = rand_r(&seed) % b->possibleMoves.size();
  Point chosenMove = b->possibleMoves[choice];

  b->makeMove(chosenMove);
  UCTNode* newCurrentNode = new UCTNode(chosenMove, b, NULL);
  delete (*currentNode);
  (*currentNode) = newCurrentNode;
}

int main(void) {
  printf("Running...\n");

  int numTrials = 100;
  int boardSize = 5;
  int numSimulations = 1000;
  float komi = 0;
  PlayerType player1 = RandomPlayer;
  PlayerType player2 = MyMonteCarlo;

  for (int x = 0; x < 2; x++) {
    int player1Wins = 0;
    int player2Wins = 0;

    for (int i = 0; i < numTrials; i++) {
      if (i % 10 == 0 && i != 0)
        printf("\nTrial Number: %d\n", i);
      Board* b = new Board(boardSize);
      UCTNode *node = new UCTNode(Point(-1, -1), b, NULL);

      GameResult r;
      while (!b->isGameOver(&r)) {
        PlayerType currentPlayer = b->turn == Black ? player1 : player2;
        switch (currentPlayer) {
         case RandomPlayer:
          randomMove(&node, b);
          break;
         case MyMonteCarlo:
          computerMove(&node, b, numSimulations);
          break;
         case GnuGo:
         default:
          assert(false);
          break;
        }
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
