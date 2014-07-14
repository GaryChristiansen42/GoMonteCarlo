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
const float percentSurvivors = 0.05f;
const unsigned int populationSize = 100;
const unsigned int numGenerations = 200000;
const int numTrials = 1000;

const unsigned int numThreads = 4;

std::default_random_engine engine;

// int is fitness
std::vector<std::pair<Patterns, int>> patternPopulation;
Patterns originalPatterns;

std::unordered_map<std::string, int> patterns3x3Encountered;
std::unordered_map<std::string, int> patterns5x5Encountered;
long unsigned int totalEncountered = 0;

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
  sort(patternPopulation.begin(), patternPopulation.end(), [] (const std::pair<Patterns, int>& first, const std::pair<Patterns, int>& second) {
    return first.second > second.second;
  });

  for (unsigned int i = 0; i < numSurvivors; ++i) {
    survivors.push_back(patternPopulation[i]);
    std::cout << survivors[i].second << " ";
    /*
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
    survivors.push_back(best);*/
  }
  std::cout << std::endl;

  // repopulate
  patternPopulation.clear();
  std::uniform_int_distribution<> distSurvivorSize(0, (int)survivors.size()-1);
  std::uniform_int_distribution<> dist0To100(0, 100);
  std::uniform_int_distribution<> dist3x3PatternsEncounteredSize(0, (int)patterns3x3Encountered.size()-1);
  std::uniform_int_distribution<> dist5x5PatternsEncounteredSize(0, (int)patterns5x5Encountered.size()-1);

  std::vector<std::pair<std::string, int>> patterns3x3, patterns5x5;
  if (mutate3x3) {
    for (auto i : patterns3x3Encountered) {
      patterns3x3.push_back(std::make_pair(i.first, i.second));
    }
    std::sort(patterns3x3.begin(), patterns3x3.end(), [] (const std::pair<std::string, int>& i, const std::pair<std::string, int>& j) { return i.second > j.second;});
  }
  if (mutate5x5) {
    for (auto i : patterns5x5Encountered) {
      patterns5x5.push_back(std::make_pair(i.first, i.second));
    }
    std::sort(patterns5x5.begin(), patterns5x5.end(), [] (const std::pair<std::string, int>& i, const std::pair<std::string, int>& j) { return i.second > j.second;});
  }


  // if (mutate5x5 && patterns5x5.size() > 0)
    // std::cout << (float)patterns5x5[0].second / (float)totalEncountered << std::endl;
  for (unsigned int i = 0; i < populationSize; ++i) {
    unsigned long int choice = distSurvivorSize(engine);

    Patterns chosen = survivors[choice].first;
    if (dist0To100(engine) > mutationChance * 100) {
      if (mutate3x3 && patterns3x3Encountered.size() > 0) {
        // choice = dist3x3PatternsEncounteredSize(engine);
        choice = dist0To100(engine);
        chosen.mutatePattern(patterns3x3[choice].first, engine);
      }
      if (mutate5x5 && patterns5x5Encountered.size() > 0) {
        // choice = dist5x5PatternsEncounteredSize(engine);
        choice = dist0To100(engine);
        chosen.mutatePattern(patterns5x5[choice].first, engine);

        // std::cout << choice << std::endl;
        // std::cout << patterns5x5[choice].second << " " << totalEncountered << std::endl;
        // std::cout << (float)patterns5x5[choice].second / (float)totalEncountered << std::endl;
        // std::cout << patterns5x5[choice].first << std::endl;
        
      }
    }

    patternPopulation.push_back(std::make_pair(chosen, 0));
  }
  std::cout << patterns5x5Encountered.size() << std::endl;
  patterns3x3Encountered.clear();
  patterns5x5Encountered.clear();
  patterns3x3.clear();
  patterns5x5.clear();
  totalEncountered = 0;
}

void determineFitnessThread() {
  std::default_random_engine threadEngine(time(NULL));

  for (auto& member : patternPopulation) {
    
    for (unsigned int i = 0; i < numTrials / numThreads; ++i) {
      Board b;
      b.init();

      GameResult r;
      Player memberColor = Black;
      if (i >= (numTrials / 2) / numThreads) {
        memberColor = White;
      }
      auto turn = b.turn == memberColor ? &member.first : &originalPatterns;
      while (!b.isGameOver(&r)) {
        turn = &member.first;
        b.makeMove(*turn->getMove(&b, threadEngine));
        turn = b.turn == memberColor ? &member.first : &originalPatterns;

        if (mutate3x3) {
          Pattern3x3 last3x3Move(b.lastMove);
          if (last3x3Move.isLegalPattern()) {
            patternsEncounteredLock.lock();
            // TODO(GaryChristiansen42) add all rotations and inverts
            ++patterns3x3Encountered[last3x3Move.hash];
            ++totalEncountered;
            patternsEncounteredLock.unlock();
          }
        }
        if (mutate5x5) {
          Pattern5x5 last5x5Move(b.lastMove);
          if (last5x5Move.isLegalPattern()) {
            patternsEncounteredLock.lock();
            ++patterns5x5Encountered[last5x5Move.hash];
            ++totalEncountered;
            patternsEncounteredLock.unlock();
          }
        }
      }
      patternPopulationLock.lock();
      if ((Player)r == memberColor)
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
