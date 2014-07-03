#include <assert.h>
#include <math.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "../UCT.h"

const std::string patternsFile = "patterns.pat";

UCTNode *root;

// http://www.ai-junkie.com/ga/intro/gat2.html
const float mutationChance = 0.7f;
const float percentSurvivors = 0.05f;
const unsigned int populationSize = 100;
const unsigned int numGenerations = 200000;

// int is fitness
std::vector<std::pair<Patterns, int>> patternPopulation;
Patterns originalPatterns;


void selectSurvivors() {
  unsigned int numSurvivors = (int)(populationSize * percentSurvivors);
  if (numSurvivors < 1)
    ++numSurvivors;

  int newLineAmount = sqrt(populationSize);
  int count = 0;
  for (auto x : patternPopulation) {
    count++;
    printf("%d ", x.second);
    if (count == newLineAmount) {
      printf("\n");
      count = 0;
    }
  }
  printf("\n");

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
  for (unsigned int i = 0; i < populationSize; ++i) {
    unsigned long int choice = rand() % survivors.size();

    Patterns chosen = survivors[choice].first;
    if (rand() % 100 > mutationChance * 100)
      chosen.mutate();

    patternPopulation.push_back(std::make_pair(chosen, 0));
  }
}

void determineFitness() {
  auto count = 0;
  for (auto& member : patternPopulation) {
    for (auto i = 0; i < 100; ++i) {
      Board b;
      b.init();

      GameResult r;
      auto turn = &member.first;
      Player turnColor = Black;
      while (!b.isGameOver(&r)) {
        b.makeMove(*turn->getMove(&b));
        turn = turnColor == Black ? &originalPatterns : &member.first;
        turnColor = turnColor == Black ? White : Black;
      }
      if ((Player)r == Black)
        ++member.second;
      else
        --member.second;
    }
    printf("%d score: %d\n", count, member.second);
    ++count;
  }
}


int main(void) {
  printf("PatternEvolution\n");

  srand((unsigned int)time(NULL));
  int randNumber = rand();
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
