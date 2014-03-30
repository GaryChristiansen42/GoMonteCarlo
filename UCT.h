#ifndef UCT_H_
#define UCT_H_

#include <time.h>

#include "UCTNode.h"

// Implemented using http://www.cameronius.com/cv/mcts-survey-master.pdf


void printNode(UCTNode* n, char* spaces="");

UCTNode* bestChild(UCTNode* node);

UCTNode* getNewChild(UCTNode* node);

UCTNode* TreePolicy(UCTNode* node);

int DefaultPolicy(UCTNode* node);

void backup(UCTNode* v, int reward);

UCTNode* UCTSearch(UCTNode* root, Board* b, int numSimulations);

#endif  // UCT_H_
