#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"
#include "Common.h"

const std::string patternsFile = "patterns.pat";

int main(void) {
  printf("Running...\n");

  Patterns patterns;
  patterns.init(patternsFile);

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
      computerMove(&node, b, 0, 500, &patterns);
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
