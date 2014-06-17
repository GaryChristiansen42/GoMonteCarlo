#include "UCT.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <cstdlib>
#include <thread>
#include <mutex>
#include <atomic>

#include "Common.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

// Implemented using http://www.cameronius.com/cv/mcts-survey-master.pdf

std::mutex treePolicyMutex;
std::mutex backUpMutex;
std::atomic<int> simulationCount;

double diffclock(timespec start, timespec finish) {
  double elapsed = static_cast<double>(finish.tv_sec - start.tv_sec);
  elapsed += static_cast<double>(finish.tv_nsec - start.tv_nsec)/1000000000.0;
  return elapsed * (-1000);
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
    for (int row = 0; row < BOARD_SIZE; ++row) {
      for (int column = 0; column < BOARD_SIZE; ++column) {
        if (node->state->positions[row][column].legal) {
          UCTNode* child = new UCTNode(node->state->positions[row][column], node);
          node->possibleChildren.push_back(child);
        }
      }
    }

    Point pass(BOARD_SIZE, BOARD_SIZE);
    if (!node->isChild(pass)) {
      // Add pass move to possible moves
      UCTNode* passChild = new UCTNode(pass, node);
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
    if (chosenChild->visits == 0)
      chosenChild->init();
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
  Point pass(BOARD_SIZE, BOARD_SIZE);
  if (node->move != pass
    && node->move.color != Empty) {
    assert(false);
  }
  return node;
}

int DefaultPolicy(UCTNode* node, Board* clone) {
  for (Group* g : clone->blackGroups)
    delete g;
  clone->blackGroups.clear();
  for (Group* g : clone->whiteGroups)
    delete g;
  clone->whiteGroups.clear();

  node->state->cloneInto(clone);
  GameResult r = clone->playRandomGame();
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

UCTNode* UCTSearch(UCTNode* root, int numSimulations) {
  Board* clone = new Board();
  clone->init();
  for (int i = 0; i < numSimulations; i++) {
    UCTNode* v = TreePolicy(root);
    int reward = DefaultPolicy(v, clone);
    backup(v, reward);

    if (i % 1000 == 0 && i != 0)
      printf("%d\n", i);
  }
  delete clone;

  UCTNode* best = root->child;
  UCTNode* next = root->child;
  while (next != NULL) {
    if (next->visits > best->visits)
      best = next;
    next = next->sibling;
  }

  Point pass(BOARD_SIZE, BOARD_SIZE);
  if (!(best->move == pass)
    && best->move.color != Empty) {
    assert(false);
  }
  if (!(best->move == pass)
    && best->move.color != Empty) {
    assert(false);
  }
  return best;
}

void runSimulationThread(UCTNode* root, int millaSecondsToThink) {
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  clock_gettime(CLOCK_MONOTONIC, &end);

  int i = 0;

  Board* clone = new Board();
  clone->init();

  while (diffclock(end, start) < millaSecondsToThink) {
    simulationCount++;
    i++;

    treePolicyMutex.lock();
    UCTNode* v = TreePolicy(root);
    treePolicyMutex.unlock();

    int reward = DefaultPolicy(v, clone);

    backUpMutex.lock();
    backup(v, reward);
    backUpMutex.unlock();

    /*if (i % 100 == 0 && i != 0) {
      char buffer[100];
      sprintf(buffer, "#%d - %d", getpid(), i);
      Log(buffer);
      sprintf(buffer, "  %f %d", diffclock(end, start), millaSecondsToThink);
      Log(buffer);
    }*/
    // end = clock();
    clock_gettime(CLOCK_MONOTONIC, &end);
  }
  delete clone;
  Log(std::to_string(i).c_str());
}

UCTNode* UCTSearch(UCTNode* root, float millaSecondsToThink) {
  simulationCount = 0;

  int numThreads = 4;
  std::thread* threads = new std::thread[numThreads];
  for (int threadNum = 0; threadNum < numThreads; threadNum++) {
    threads[threadNum] = std::thread(runSimulationThread, root,
      millaSecondsToThink);
  }

  for (int threadNum = 0; threadNum < numThreads; threadNum++)
    threads[threadNum].join();
  delete[] threads;

  UCTNode* best = root->child;
  UCTNode* next = root->child;
  while (next != NULL) {
    if (next->visits > best->visits)
      best = next;
    next = next->sibling;
  }

  Point pass(BOARD_SIZE, BOARD_SIZE);
  if (!(best->move == pass)
    && best->move.color != Empty) {
    assert(false);
  }
  if (!(best->move == pass)
    && best->move.color != Empty) {
    assert(false);
  }
  char buffer[100];
  snprintf(buffer, sizeof(buffer),
    "Thought for %d simulations.\nR: %f V: %d\nR/V: %f",
    static_cast<int>(simulationCount), best->totalRewards, best->visits,
    static_cast<double>(best->totalRewards/best->visits));
  Log(buffer);
  return best;
}
