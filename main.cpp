#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"
#include "Common.h"

const std::string patternsFile = "patterns.pat";

UCTNode *root;

int main(void) {
  printf("Go MonteCarloAI\n");

  int numSimulations = 0;
  float millaSecondsToThink = 1000;

  Board* b = new Board();

  UCTNode *currentNode;
  root = new UCTNode(Point(-1, -1), NULL);
  root->state = b;
  currentNode = root;

  b->show();

  Patterns patterns;
  patterns.init(patternsFile);

  GameResult result = None;
  while (!b->isGameOver(&result)) {
    if (b->turn == Black)
      computerMove(&currentNode, b, numSimulations, millaSecondsToThink, &patterns);
    else
      computerMove(&currentNode, b, numSimulations, millaSecondsToThink, &patterns);
      // playerMove(&currentNode, b);
      // randomMove(&currentNode, b);
    b->show();
  }

  if (result == Draw)
    printf("Draw\n");
  else if (static_cast<Player>(result) == Black)
    printf("Black Won\n");
  else
    printf("White Won\n");

  float score = 0;
  score = b->getTaylorScore(0.5);
  printf("\nScore: %f\n", score);

  delete currentNode;
  delete b;
  return 0;
}
