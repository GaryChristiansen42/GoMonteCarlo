#ifndef UCT_H_
#define UCT_H_

#include <time.h>

#include <random>

#include "UCTNode.h"
#include "Patterns.h"

// Implemented using http://www.cameronius.com/cv/mcts-survey-master.pdf


void printNode(UCTNode* n, std::string spaces="");

UCTNode* bestChild(UCTNode* node);

UCTNode* getNewChild(UCTNode* node, std::default_random_engine&  engine);

UCTNode* TreePolicy(UCTNode* node, std::default_random_engine& engine);

int DefaultPolicy(UCTNode* node, Patterns* patterns,
    std::default_random_engine& engine);

void backup(UCTNode* v, int reward);

UCTNode* UCTSearch(UCTNode* root, int numSimulations, Patterns* patterns);
UCTNode* UCTSearch(UCTNode* root, float millaSecondsToThink, Patterns* patterns);

#endif  // UCT_H_
