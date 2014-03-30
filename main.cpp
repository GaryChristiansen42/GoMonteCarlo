#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "UCT.h"

UCTNode *root;

void playerMove(UCTNode** currentNode, Board* b) {
  bool validInput = false;
  Point choice(-1, -1);
  while (!validInput) {
    printf("Row: ");
    scanf("%d", &choice.row);

    printf("Column: ");
    scanf("%d", &choice.column);

    printf("\n");

    if (b->isValidMove(choice))
      validInput = true;
    else
      printf("That position is not valid\n");
  }

  printf("Row: %d\nColumn: %d\n", choice.row, choice.column);

  b->makeMove(choice);
  UCTNode* newCurrentNode = new UCTNode(choice, b, NULL);
  delete (*currentNode);
  (*currentNode) = newCurrentNode;
}

void printTabs(int numTabs) {
  for (int i = 0; i < numTabs; i++)
    printf("\t");
}

void showTree(UCTNode* node, int numTabs, int maxDepth = 1) {
  if (node == NULL || numTabs >= maxDepth || node->visits < 2)
    return;
  printTabs(numTabs);
  printf("Node\n");
  printTabs(numTabs);
  printf("\tMove: %d %d\n", node->move.row, node->move.column);
  printTabs(numTabs);
  printf("\tRewards/Visits: %f %d\n", node->totalRewards, node->visits);
  printTabs(numTabs);
  // (*file) << "Wins: " << root->wins
  // << " Losses: " << root->losses << " Visits: " << root->visits << endl;
  printTabs(numTabs);
  printf("Children: \n");
  showTree(node->child, numTabs+1, maxDepth);
  showTree(node->sibling, numTabs, maxDepth);
}

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

  printf("Row: %d\nColumn: %d\n",
    (*currentNode)->move.row, (*currentNode)->move.column);

  /*ofstream myfile;
  myfile.open ("tree.txt");
  showTree(&myfile, root, 0);
  myfile.close();*/
}

void randomMove(UCTNode** currentNode, Board* b) {
  static unsigned int seed = time(NULL);
  unsigned int choice = rand_r(&seed) % b->possibleMoves.size();
  Point chosenMove = b->possibleMoves[choice];

  printf("Row: %d\nColumn: %d\n", chosenMove.row, chosenMove.column);

  b->makeMove(chosenMove);
  UCTNode* newCurrentNode = new UCTNode(chosenMove, b, NULL);
  delete (*currentNode);
  (*currentNode) = newCurrentNode;
}

int main(void) {
  printf("Go MonteCarloAI\n");

  int boardSize = 5;
  int numSimulations = 10000;

  Board* b = new Board(boardSize);

  UCTNode *currentNode;
  root = new UCTNode(Point(-1, -1), b, NULL);
  currentNode = root;

  b->show();

  GameResult result = None;
  while (!b->isGameOver(&result)) {
    if (b->turn == Black)
      computerMove(&currentNode, b, numSimulations);
    else
      // computerMove(&currentNode, b);
      playerMove(&currentNode, b);
      // randomMove(&currentNode, b);
    b->show();
  }

  if (result == Draw)
    printf("Draw\n");
  else if (static_cast<Player>(result) == Black)
    printf("Black Won\n");
  else
    printf("White Won\n");

  float whiteScore = 0, blackScore = 0;
  b->getSimpleScore(&whiteScore, &blackScore);
  printf("\nWhite: %f\nBlack: %f\n", whiteScore, blackScore);

  delete currentNode;
  delete b;
  return 0;
}
