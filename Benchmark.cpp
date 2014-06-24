#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"

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
  for (int row = 0; row < BOARD_SIZE; ++row)
    for (int column = 0; column < BOARD_SIZE; ++column)
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
  Point *chosenMove = b->getRandomMove();

  UCTNode* newCurrentNode = new UCTNode(*chosenMove, *currentNode);
  if (newCurrentNode->parent != NULL)
    newCurrentNode->init();
  b->makeMove(*chosenMove);
  // delete (*currentNode);
  (*currentNode) = newCurrentNode;
  newCurrentNode->parent = NULL;
}

int main(void) {
  printf("Running...\n");

  Board* b = new Board();
  b->init();
  UCTNode *node = new UCTNode(Point(-1, -1), NULL);
  node->state = b->clone();

  GameResult r;
  while (!b->isGameOver(&r)) {
    switch (b->turn) {
     case Black:
      randomMove(&node, b);
      break;
     case White:
      computerMove(&node, b, 0, 500);
      break;
     case Empty:
     case OutOfBounds:
     default:
      assert(false);
    }
  }

  delete node;
  delete b;
}
