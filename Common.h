#ifndef COMMON_H_
#define COMMON_H_

class UCTNode;
class Board;
class Patterns;

void Log(const char* message);

void playerMove(UCTNode** currentNode, Board* b);
void computerMove(UCTNode** currentNode, Board* b, int numSimulations,
  float millaSecondsToThink, Patterns* patterns);
void randomMove(UCTNode** currentNode, Board* b);

void printTabs(int numTabs);
void showTree(UCTNode* node, int numTabs, int maxDepth = 1);

#endif  // COMMON_H_
