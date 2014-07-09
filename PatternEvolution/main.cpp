#include <assert.h>
#include <math.h>
#include <time.h>

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>

#include "../UCT.h"

const std::string patternsFile = "patterns.pat";

UCTNode *root;

// http://www.ai-junkie.com/ga/intro/gat2.html
const float mutationChance = 0.7f;
const float percentSurvivors = 0.1f;
const unsigned int populationSize = 9;
const unsigned int numGenerations = 200000;

const unsigned int numThreads = 2;

std::default_random_engine engine;

// int is fitness
std::vector<std::pair<Patterns, int>> patternPopulation;
Patterns originalPatterns;

std::vector<std::string> patternsEncountered;

std::mutex patternsEncounteredLock;
std::mutex patternPopulationLock;

void selectSurvivors() {
  unsigned int numSurvivors = (int)(populationSize * percentSurvivors);
  if (numSurvivors < 1)
    ++numSurvivors;

  int newLineAmount = sqrt(populationSize);
  int count = 0;
  float totalScore = 0;
  for (auto x : patternPopulation) {
    count++;
    printf("%d ", x.second);
    if (count == newLineAmount) {
      printf("\n");
      count = 0;
    }
    totalScore += (float)x.second;
  }
  printf("\nAverageScore: %f\n", totalScore / (float)patternPopulation.size());

  std::vector<std::pair<Patterns, int>> survivors;
  for (unsigned int i = 0; i < numSurvivors; ++i) {
    auto best = patternPopulation[0];
    for (auto member : patternPopulation) {
      if (member.second > best.second) {
        bool found = false;
        for (auto survivor : survivors) {
          if (survivor.first == member.first) {
            found = true;
            break;
          }
        }
        if (!found)
          best = member;
      }
    }
    survivors.push_back(best);
  }

  // repopulate
  patternPopulation.clear();
  std::uniform_int_distribution<> distSurvivorSize(0, (int)survivors.size()-1);
  std::uniform_int_distribution<> dist0To100(0, 100);
  std::uniform_int_distribution<> distPatternsEncounteredSize(0, (int)patternsEncountered.size()-1);
  for (unsigned int i = 0; i < populationSize; ++i) {
    unsigned long int choice = distSurvivorSize(engine);

    Patterns chosen = survivors[choice].first;
    if (dist0To100(engine) > mutationChance * 100) {
      if (patternsEncountered.size() > 0) {
        choice = distPatternsEncounteredSize(engine);
        chosen.mutatePattern(patternsEncountered[choice], engine);

        /*std::cout << choice << std::endl;
        std::cout << std::count(patternsEncountered.begin(), patternsEncountered.end(), patternsEncountered[choice])
          << " " << patternsEncountered.size() << std::endl;
        std::cout << (float)std::count(patternsEncountered.begin(), patternsEncountered.end(), patternsEncountered[choice])
          / (float)patternsEncountered.size() << std::endl;
        std::cout << patternsEncountered[choice] << std::endl;
        */
      }
    }

    patternPopulation.push_back(std::make_pair(chosen, 0));
  }
  // std::cout << patternsEncountered.size() << std::endl;
  patternsEncountered.clear();
}

void determineFitnessThread() {
  std::default_random_engine threadEngine(time(NULL));

  for (auto& member : patternPopulation) {
    for (unsigned int i = 0; i < 10000 / numThreads; ++i) {
      Board b;
      b.init();

      GameResult r;
      auto turn = &member.first;
      Player turnColor = Black;
      while (!b.isGameOver(&r)) {
        b.makeMove(*turn->getMove(&b, threadEngine));
        turn = turnColor == Black ? &originalPatterns : &member.first;
        turnColor = turnColor == Black ? White : Black;

        Pattern lastMove(b.lastMove);
        if (lastMove.isLegalPattern()) {
          patternsEncounteredLock.lock();
          patternsEncountered.push_back(lastMove.hash);
          patternsEncounteredLock.unlock();
        }
      }
      patternPopulationLock.lock();
      if ((Player)r == Black)
        ++member.second;
      else
        --member.second;
      patternPopulationLock.unlock();
    }
  }

}

void determineFitness() {
  std::thread* threads = new std::thread[numThreads];
  for (unsigned int threadNum = 0; threadNum < numThreads; threadNum++) {
    threads[threadNum] = std::thread(determineFitnessThread);
  }

  for (unsigned int threadNum = 0; threadNum < numThreads; threadNum++)
    threads[threadNum].join();
  delete[] threads;

}


int main(void) {
  printf("PatternEvolution\n");

  engine.seed(time(NULL));
  std::uniform_int_distribution<> dist(1, 100000);
  int randNumber = dist(engine);
  char fileName[256];
  snprintf(fileName, sizeof(fileName), "savedPatterns/patterns%d.pat", randNumber);

  originalPatterns.init("patterns.pat");

  for (unsigned int i = 0; i < populationSize; ++i)
    patternPopulation.push_back(std::make_pair(originalPatterns, 0));

  selectSurvivors();
  auto best = patternPopulation[0];

  for (unsigned int i = 0; i < numGenerations; ++i) {
    std::cout << "Generation " << i << std::endl;
    determineFitness();
    if (i % 1 == 0) {
      best = patternPopulation[0];
      for (auto member : patternPopulation)
        if (member.second > best.second)
          best = member;
      best.first.save(fileName);
    }
    selectSurvivors();
  }

  determineFitness();
  best = patternPopulation[0];
  for (auto member : patternPopulation)
    if (member.second > best.second)
      best = member;
  best.first.save(fileName);

  return 0;
}
