#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"
#include "Common.h"

const std::string patternsFile3x3 = "patterns3x3.pat";
const std::string patternsFile5x5 = "patterns5x5.pat";

int main() {
  printf("Running...\n");

  Patterns patterns;
  // patterns.init(patternsFile3x3, patternsFile5x5);

  auto b = new Board();
  b->init();
  UCTNode::rootState = b;
  auto node = new UCTNode(1, -1, nullptr);

  GameResult r;
  while (!b->isGameOver(&r)) {
    switch (b->turn) {
     case Black:
      randomMove(&node, b);
      break;
     case White:
      computerMove(&node, b, 0, 500, nullptr); // &patterns);
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
