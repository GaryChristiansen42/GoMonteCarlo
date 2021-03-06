#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"
#include "Common.h"

const std::string patternsFile3x3 = "patterns3x3.pat";
const std::string patternsFile5x5 = "patterns5x5.pat";

UCTNode *root;

int main() {
  printf("Go MonteCarloAI\n");

  int numSimulations = 0;
  float millaSecondsToThink = 15000;

  auto b = new Board();
  b->init();

  UCTNode *currentNode;
  root = new UCTNode(-1, -1, nullptr);
  UCTNode::rootState = b;
  currentNode = root;

  b->show();

  Patterns patterns;
  // patterns.init(patternsFile3x3, patternsFile5x5);

  GameResult result = None;
  while (!b->isGameOver(&result)) {
    if (b->turn == White) {
      computerMove(&currentNode, b, numSimulations, millaSecondsToThink, nullptr); // &patterns);
    } else {
      // computerMove(&currentNode, b, numSimulations, millaSecondsToThink, nullptr); // &patterns);
      playerMove(&currentNode, b);
      // randomMove(&currentNode, b);
    }
    b->show();
  }

  if (result == Draw) {
    printf("Draw\n");
  } else if (static_cast<Player>(result) == Black) {
    printf("Black Won\n");
  } else {
    printf("White Won\n");
  }

  float score = 0;
  score = b->getTaylorScore(0.5);
  printf("\nScore: %f\n", score);

  delete currentNode;
  delete b;
  return 0;
}
