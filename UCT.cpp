#include "UCT.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <random>
#include <thread>

#include "Common.h"
#include "Patterns.h"

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
  printf("%sn(%d %d) V %ld R %ld\n", spaces, n->row, n->column,
    n->visits, n->totalRewards);
  // std::cout << spaces << "n (" << n->move.row << " " << n->move.column
    // << ") V " << n->visits << " R " << n->totalRewards << std::endl;

  UCTNode* next = n->child;
  while (next != nullptr) {
    char moreSpaces[100];
    snprintf(moreSpaces, sizeof(spaces), "%s ", spaces);
    printNode(next, moreSpaces);
    next = next->sibling;
  }
}

UCTNode* bestChild(UCTNode* node) {
  node->mutex.lock();
  const double c = 1;  // / sqrt(static_cast<float>(2));
  // const double c = 1;
  UCTNode* bestChild = node->child;
  double bestScore = -100000;
  UCTNode* next = node->child;
  while (next != nullptr) {
    double uctScore = (double)(next->totalRewards) / (double)(next->visits)
      + c*sqrt((2*log(static_cast<float>(node->visits))) / (double)(next->visits));
    if (bestScore < uctScore) {
      bestChild = next;
      bestScore = uctScore;
    }
    next = next->sibling;
  }
  node->mutex.unlock();
  return bestChild;
}

UCTNode* getNewChild(UCTNode* node, std::default_random_engine& engine) {
  node->mutex.lock();
  if (node->possibleChildren.empty() && node->child == nullptr) {
    auto state = node->getState();
    auto legalMoves = state->getPossibleMoves();
    for (unsigned short i = 0; i < legalMoves.size(); ++i) {
      UCTNode* child = new UCTNode(legalMoves[i]->row, legalMoves[i]->column, node);
      node->possibleChildren.push_back(child);
    }

    UCTNode* passChild = new UCTNode(BOARD_SIZE, BOARD_SIZE, node);
    node->possibleChildren.push_back(passChild);
  }

  if (node->possibleChildren.size() == 0) {
    node->mutex.unlock();
    return nullptr;
  }
  // else //Pick random child
  // {
    std::uniform_int_distribution<> dist(0, (int)node->possibleChildren.size()-1);
    unsigned int choice = dist(engine);
    UCTNode* chosenChild = node->possibleChildren[choice];
    // for(unsigned int i = 0; i < possibleChildren.size(); i++)
    //  if(i != choice)
    //    delete possibleChildren[i];
    // node->possibleChildren.erase(&node->possibleChildren.begin()+choice);
    node->possibleChildren[choice] = node->possibleChildren.back();
    node->possibleChildren.pop_back();
    node->mutex.unlock();
    return chosenChild;
  //  }
}

UCTNode* TreePolicy(UCTNode* node, std::default_random_engine& engine) {
  Point passPoint(BOARD_SIZE, BOARD_SIZE);
  while (!(node->row == passPoint.row && node->column == passPoint.column && node->parent != nullptr 
        && node->parent->row == passPoint.row && node->parent->column == passPoint.column)) {  // Not terminal
    UCTNode* newChild = getNewChild(node, engine);
    if (newChild != nullptr) {
      node->addChild(newChild);
      return newChild;
    } else {
      node = bestChild(node);
    }
  }
  /*Point pass(BOARD_SIZE, BOARD_SIZE);
  if (node->move != pass
    && node->move.color != Empty) {
    assert(false);
  }*/
  return node;
}

int DefaultPolicy(std::default_random_engine& engine, UCTNode* node, Patterns* patterns) {
  auto state = node->getState();
  auto startingTurn = state->turn;

  GameResult r;
  if (patterns != nullptr && patterns->initialized)
    r = state->playGame(patterns, engine);
  else
    r = state->playRandomGame(engine);

  if ((startingTurn == Black ? White : Black) == static_cast<int>(r))
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
  while (v != nullptr) {
    v->mutex.lock();
    v->visits++;
    if (me)
      v->totalRewards += reward;
    else
      v->totalRewards -= reward;
    v->mutex.unlock();
    v = v->parent;
    me = !me;
  }
}

UCTNode* UCTSearch(UCTNode* root, int numSimulations, Patterns* patterns) {
  std::default_random_engine engine(time(nullptr));
  for (int i = 0; i < numSimulations; i++) {
    UCTNode* v = TreePolicy(root, engine);
    int reward = DefaultPolicy(engine, v, patterns);
    backup(v, reward);

    if (i % 1000 == 0 && i != 0)
      printf("%d\n", i);
  }

  UCTNode* best = root->child;
  UCTNode* next = root->child;
  while (next != nullptr) {
    if (next->visits > best->visits)
      best = next;
    next = next->sibling;
  }

  /*Point pass(BOARD_SIZE, BOARD_SIZE);
  if (!(best->move == pass)
    && best->move.color != Empty) {
    assert(false);
  }*/
  return best;
}

void runSimulationThread(UCTNode* root, int millaSecondsToThink, Patterns* patterns) {
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  clock_gettime(CLOCK_MONOTONIC, &end);

  int i = 0;

  std::default_random_engine engine(time(nullptr));

  while (diffclock(end, start) < millaSecondsToThink) {
    i++;

    UCTNode* v = TreePolicy(root, engine);

    int reward = DefaultPolicy(engine, v, patterns);

    backup(v, reward);

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

  simulationCount.fetch_add(i);
  Log(std::to_string(i).c_str());
}

UCTNode* UCTSearch(UCTNode* root, float millaSecondsToThink, Patterns* patterns) {
  simulationCount = 0;

  int numThreads = 4;
  std::thread* threads = new std::thread[numThreads];
  for (int threadNum = 0; threadNum < numThreads; threadNum++) {
    threads[threadNum] = std::thread(runSimulationThread, root,
      millaSecondsToThink, patterns);
  }

  for (int threadNum = 0; threadNum < numThreads; threadNum++)
    threads[threadNum].join();
  delete[] threads;

  UCTNode* best = root->child;
  UCTNode* next = root->child;
  while (next != nullptr) {
    if (next->visits > best->visits)
      best = next;
    next = next->sibling;
  }

  // if (!(best->row == BOARD_SIZE && best->column == BOARD_SIZE)) {
    // && best->move.color != Empty) {
    // assert(false);
  // }
  /*if (!(best->move == pass)
    && best->move.color != Empty) {
    assert(false);
  }*/

  char buffer[100];
  snprintf(buffer, sizeof(buffer),
    "Thought for %d simulations.\nR: %ld V: %ld\nR/V: %f\t%d %d",
    static_cast<int>(simulationCount), best->totalRewards, best->visits,
    static_cast<double>((double)(best->totalRewards)/(double)(best->visits)), best->row, best->column);
  Log(buffer);
  if (patterns != nullptr) {
    snprintf(buffer, sizeof(buffer), "Called\t\t%d times", patterns->numCalled);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Not Legal\t%d times", patterns->numNotLegal);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Null\t\t%d times", patterns->numNull);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Not Called\t%d times", patterns->numNotCalled);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Called2\t\t%d times", patterns->numCalled2);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Not Legal2\t%d times", patterns->numNotLegal2);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Null2\t\t%d times", patterns->numNull2);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Not Called2\t%d times", patterns->numNotCalled2);
    Log(buffer);
    snprintf(buffer, sizeof(buffer), "Total\t\t%d times\n", patterns->total);
    Log(buffer);
  }
  return best;
}
