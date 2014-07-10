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
const bool mutate3x3 = false;
const bool mutate5x5 = true;
const float mutationChance = 0.7f;
const float percentSurvivors = 0.1f;
const unsigned int populationSize = 9;
const unsigned int numGenerations = 20000;

const unsigned int numThreads = 4;

std::default_random_engine engine;

// int is fitness
std::vector<std::pair<Patterns, int>> patternPopulation;
Patterns originalPatterns;

std::vector<std::string> patterns3x3Encountered;
std::vector<std::string> patterns5x5Encountered;

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
  std::uniform_int_distribution<> dist3x3PatternsEncounteredSize(0, (int)patterns3x3Encountered.size()-1);
  std::uniform_int_distribution<> dist5x5PatternsEncounteredSize(0, (int)patterns5x5Encountered.size()-1);
  for (unsigned int i = 0; i < populationSize; ++i) {
    unsigned long int choice = distSurvivorSize(engine);

    Patterns chosen = survivors[choice].first;
    if (dist0To100(engine) > mutationChance * 100) {
      if (mutate3x3 && patterns3x3Encountered.size() > 0) {
        choice = dist3x3PatternsEncounteredSize(engine);
        chosen.mutatePattern(patterns3x3Encountered[choice], engine);
      }
      if (mutate5x5 && patterns5x5Encountered.size() > 0) {
        choice = dist5x5PatternsEncounteredSize(engine);
        chosen.mutatePattern(patterns5x5Encountered[choice], engine);

        std::cout << choice << std::endl;
        std::cout << std::count(patterns5x5Encountered.begin(), patterns5x5Encountered.end(), patterns5x5Encountered[choice])
          << " " << patterns5x5Encountered.size() << std::endl;
        std::cout << (float)std::count(patterns5x5Encountered.begin(), patterns5x5Encountered.end(), patterns5x5Encountered[choice])
          / (float)patterns5x5Encountered.size() << std::endl;
        std::cout << patterns5x5Encountered[choice] << std::endl;
      }
    }

    patternPopulation.push_back(std::make_pair(chosen, 0));
  }
  std::cout << patterns5x5Encountered.size() << std::endl;
  patterns3x3Encountered.clear();
  patterns5x5Encountered.clear();
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

        if (mutate3x3) {
          Pattern3x3 last3x3Move(b.lastMove);
          if (last3x3Move.isLegalPattern()) {
            patternsEncounteredLock.lock();
            // TODO(GaryChristiansen42) add all rotations and inverts
            patterns5x5Encountered.push_back(last3x3Move.hash);
            patternsEncounteredLock.unlock();
          }
        }
        if (mutate5x5) {
          Pattern5x5 last5x5Move(b.lastMove);
          if (last5x5Move.isLegalPattern()) {
            patternsEncounteredLock.lock();
            patterns5x5Encountered.push_back(last5x5Move.hash);
            patternsEncounteredLock.unlock();
          }
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
  char fileName3x3[256];
  char fileName5x5[256];
  snprintf(fileName3x3, sizeof(fileName3x3), "savedPatterns/patterns3x3%d.pat", randNumber);
  snprintf(fileName5x5, sizeof(fileName5x5), "savedPatterns/patterns5x5%d.pat", randNumber);
  originalPatterns.init("patterns3x3.pat", "patterns5x5.pat");

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
      best.first.save(fileName3x3, fileName5x5);
    }
    selectSurvivors();
  }

  determineFitness();
  best = patternPopulation[0];
  for (auto member : patternPopulation)
    if (member.second > best.second)
      best = member;
  best.first.save(fileName3x3, fileName5x5);

  return 0;
}
