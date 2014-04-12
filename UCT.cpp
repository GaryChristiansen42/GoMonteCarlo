#include "UCT.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <boost/foreach.hpp>

#include <cstdlib>

#define foreach BOOST_FOREACH

#if _MSC_VER
#define snprintf _snprintf
#endif

// Implemented using http://www.cameronius.com/cv/mcts-survey-master.pdf


double diffclock(clock_t clock1, clock_t clock2) {
  double diffticks = clock1 - clock2;
  double diffms = diffticks / (CLOCKS_PER_SEC / 1000);
  return diffms;
}

void printNode(UCTNode* n, char* spaces) {
  printf("%sn(%d %d) V %d R %f\n", spaces, n->move.row, n->move.column,
    n->visits, n->totalRewards);
  // std::cout << spaces << "n (" << n->move.row << " " << n->move.column
    // << ") V " << n->visits << " R " << n->totalRewards << std::endl;

  UCTNode* next = n->child;
  while (next != NULL) {
    char moreSpaces[100];
    snprintf(moreSpaces, sizeof(spaces), "%s ", spaces);
    printNode(next, moreSpaces);
    next = next->sibling;
  }
}

UCTNode* bestChild(UCTNode* node) {
  const double c = 1;  // / sqrt(static_cast<float>(2));
  // const double c = 1;
  UCTNode* bestChild = node->child;
  double bestScore = -100000;
  UCTNode* next = node->child;
  while (next != NULL) {
    double uctScore = next->totalRewards / next->visits
      + c*sqrt((2*log(static_cast<float>(node->visits))) / next->visits);
    if (bestScore < uctScore) {
      bestChild = next;
      bestScore = uctScore;
    }
    next = next->sibling;
  }
  return bestChild;
}

UCTNode* getNewChild(UCTNode* node) {
  if (node->possibleChildren.size() == 0 && node->child == NULL) {
    // vector<UCTNode*> possibleChildren;
    /*for (int r = 0; r < node->state->boardSize; r++) {
      for (int c = 0; c < node->state->boardSize; c++) {
        if (node->state->positions[r][c] == Empty) {
          Point move(r, c);
          UCTNode* child = new UCTNode(move, node->state, node);
          child->state->makeMove(move);
          // if(!node->isChild(move))
            node->possibleChildren.push_back(child);
          // else
          //  delete child;
        }
      }
    }*/
    foreach(Point p, node->state->possibleMoves) {
      UCTNode* child = new UCTNode(p, node->state, node);
      child->state->makeMove(p);
      node->possibleChildren.push_back(child);
    }

    Point passMove(node->state->boardSize, node->state->boardSize);
    if (!node->isChild(passMove)) {
      // Add pass move to possible moves
      UCTNode* passChild = new UCTNode(passMove, node->state, node);
      passChild->state->makeMove(passMove);
      node->possibleChildren.push_back(passChild);
    }
  }

  if (node->possibleChildren.size() == 0)
    return NULL;
  // else //Pick random child
  // {
    static unsigned int seed = static_cast<unsigned int>(time(NULL));
    unsigned int choice = rand_r(&seed) %
      static_cast<unsigned int>(node->possibleChildren.size());
    UCTNode* chosenChild = node->possibleChildren[choice];
    // for(unsigned int i = 0; i < possibleChildren.size(); i++)
    //  if(i != choice)
    //    delete possibleChildren[i];
    // node->possibleChildren.erase(&node->possibleChildren.begin()+choice);
    node->possibleChildren[choice] = node->possibleChildren.back();
    node->possibleChildren.pop_back();
    return chosenChild;
  //  }
}

UCTNode* TreePolicy(UCTNode* node) {
  GameResult r;
  while (!node->state->isGameOver(&r)) {  // Not terminal
    UCTNode* newChild = getNewChild(node);
    if (newChild != NULL) {
      node->addChild(newChild);
      return newChild;
    } else {
      node = bestChild(node);
    }
  }
  Point pass(node->state->boardSize, node->state->boardSize);
  if (node->move != pass
    && node->state->positions[node->move.row][node->move.column] != Empty) {
    assert(false);
  }
  return node;
}

int DefaultPolicy(UCTNode* node) {
  Board* clone = node->state->clone();
  GameResult r = clone->playRandomGame();
  delete clone;
  if ((node->state->turn == Black ? White : Black) == static_cast<int>(r))
    return 1;
  else if (r != Draw)
    return -1;
  else if (r == Draw)
    return 0;
  else
    assert(false);
}

void backup(UCTNode* v, int reward) {
  // If not me, then its updating opponent
  bool me = true;
  while (v != NULL) {
    v->visits++;
    if (me)
      v->totalRewards += reward;
    else
      v->totalRewards -= reward;
    v = v->parent;
    me = !me;
  }
}

UCTNode* UCTSearch(UCTNode* root, Board* state, int numSimulations) {
  for (int i = 0; i < numSimulations; i++) {
    UCTNode* v = TreePolicy(root);
    int reward = DefaultPolicy(v);
    backup(v, reward);

    if (i % 1000 == 0 && i != 0)
      printf("%d\n", i);
  }
  UCTNode* best = root->child;
  UCTNode* next = root->child;
  while (next != NULL) {
    if (next->visits > best->visits)
      best = next;
    next = next->sibling;
  }

  Point pass(root->state->boardSize, root->state->boardSize);
  if (!(best->move == pass)
    && root->state->positions[best->move.row][best->move.column] != Empty) {
    assert(false);
  }
  if (!(best->move == pass)
    && state->positions[best->move.row][best->move.column] != Empty) {
    assert(false);
  }
  return best;
}

UCTNode* UCTSearch(UCTNode* root, Board* state, float millaSecondsToThink) {
  clock_t start = clock();
  clock_t end = clock();
  int i = 0;
  while (diffclock(end, start) < millaSecondsToThink) {
    UCTNode* v = TreePolicy(root);
    int reward = DefaultPolicy(v);
    backup(v, reward);

    if (i % 1000 == 0 && i != 0)
      printf("%d\n", i);
    end = clock();
    i++;
  }
  UCTNode* best = root->child;
  UCTNode* next = root->child;
  while (next != NULL) {
    if (next->visits > best->visits)
      best = next;
    next = next->sibling;
  }

  Point pass(root->state->boardSize, root->state->boardSize);
  if (!(best->move == pass)
    && root->state->positions[best->move.row][best->move.column] != Empty) {
    assert(false);
  }
  if (!(best->move == pass)
    && state->positions[best->move.row][best->move.column] != Empty) {
    assert(false);
  }
  printf("Thought for %d simulations.\n", i);
  return best;
}
