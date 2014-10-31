#include "Common.h"

#include <fstream>

#include <assert.h>

#include "Board.h"
#include "UCT.h"

void Log(const char* message) {
  std::ofstream file("log.txt", std::ios::app);
  file << message << '\n';
  file.close();
}

void playerMove(UCTNode** currentNode, Board* b) {
  bool validInput = false;
  Point choice(-1, -1);
  b->getPossibleMoves();
  while (!validInput) {
    printf("Row: ");
    int input;
    int tokensRead = scanf("%d", &input);
    choice.row = static_cast<unsigned char>(input);
    printf("%c", choice.row);
    if (tokensRead != 1) {
      printf("Problem %d\n", tokensRead);
    }

    printf("Column: ");
    tokensRead = scanf("%d", &input);
    choice.column = static_cast<unsigned char>(input);
    printf("%d", (int)choice.column);
    if (tokensRead != 1) {
      printf("Problem %d\n", tokensRead);
    }

    printf("\n");

    if (b->isValidMove(choice))
      validInput = true;
    else
      printf("That position is not valid\n");
  }

  printf("Row: %d\nColumn: %d\n", choice.row, choice.column);

  b->makeMove(choice);
  b->getPossibleMoves();
  UCTNode* newCurrentNode = new UCTNode(choice, (*currentNode));
  (*currentNode)->addChild(newCurrentNode);
  newCurrentNode->init();
  (*currentNode)->removeChild(newCurrentNode);
  delete (*currentNode);
  (*currentNode) = newCurrentNode;
}

void printTabs(int numTabs) {
  for (int i = 0; i < numTabs; i++)
    printf("\t");
}

void showTree(UCTNode* node, int numTabs, int maxDepth) {
  if (node == nullptr || numTabs >= maxDepth || node->visits < 2)
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

void computerMove(UCTNode** currentNode, Board* b, int numSimulations,
  float millaSecondsToThink, Patterns* patterns) {
  UCTNode* newCurrentNode = nullptr;
  if (numSimulations != 0) {
    newCurrentNode = UCTSearch((*currentNode), numSimulations, patterns);
  } else {
    newCurrentNode = UCTSearch((*currentNode), millaSecondsToThink, patterns);
  }

  (*currentNode)->removeChild(newCurrentNode);
  delete (*currentNode);

  (*currentNode) = newCurrentNode;
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
  std::default_random_engine engine(time(nullptr));

  Point* chosenMove = b->getRandomMove(engine);

  b->makeMove(*chosenMove);
  UCTNode* newCurrentNode = new UCTNode(*chosenMove, nullptr);
  delete (*currentNode);
  (*currentNode) = newCurrentNode;
  (*currentNode)->state = b->clone();

}


