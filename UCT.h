#ifndef UCT_H_
#define UCT_H_

#include <time.h>

#include "UCTNode.h"
#include "Patterns.h"

// Implemented using http://www.cameronius.com/cv/mcts-survey-master.pdf


void printNode(UCTNode* n, const char* spaces="");

UCTNode* bestChild(UCTNode* node);

UCTNode* getNewChild(UCTNode* node);

UCTNode* TreePolicy(UCTNode* node);

int DefaultPolicy(UCTNode* node, Board* clone, Patterns* patterns);

void backup(UCTNode* v, int reward);

UCTNode* UCTSearch(UCTNode* root, int numSimulations, Patterns* patterns);
UCTNode* UCTSearch(UCTNode* root, float secondsToThink, Patterns* patterns);

#endif  // UCT_H_
