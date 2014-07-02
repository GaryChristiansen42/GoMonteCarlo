#include <assert.h>
#include <time.h>

#include <cstdlib>
#include <cstdio>
#include <string>

#include "../UCT.h"

const std::string patternsFile = "patterns.pat";

UCTNode *root;

// http://www.ai-junkie.com/ga/intro/gat2.html
const float mutationChance = 0.7f;
const float percentSurvivors = 0.01f;
const unsigned int populationSize = 1000;
const unsigned int numGenerations = 200;

// int is fitness
std::vector<std::pair<Patterns, unsigned int>> patternPopulation;
Patterns originalPatterns;


void selectSurvivors() {
  unsigned int numSurvivors = (int)(populationSize * percentSurvivors);
  if (numSurvivors < 1)
    ++numSurvivors;

  std::vector<std::pair<Patterns, unsigned int>> survivors;
  for (unsigned int i = 0; i < numSurvivors; ++i) {
    std::pair<Patterns, unsigned int> best = patternPopulation[0];
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
  for (auto member : patternPopulation) {
    for (int i = 0; i < 10; ++i) {
      Board b;
      b.init();

      GameResult r;
      auto turn = member.first;
      Player turnColor = Black;
      while (!b.isGameOver(&r)) {
        b.makeMove(*turn.getMove(&b));
        turn = turnColor == Black ? originalPatterns : member.first;
        turnColor = turnColor == Black ? White : Black;
      }
      if ((Player)r == Black)
        ++member.second;
      else
        --member.second;
    }
  }
}


int main(void) {
  printf("PatternEvolution\n");

  originalPatterns.init("patterns.pat");

  for (unsigned int i = 0; i < populationSize; ++i)
    patternPopulation.push_back(std::make_pair(originalPatterns, 0));

  selectSurvivors();

  for (unsigned int i = 0; i < numGenerations; ++i) {
    std::cout << "Generation " << i << std::endl;
    determineFitness();
    selectSurvivors();
  }

  determineFitness();
  
  std::pair<Patterns, unsigned int> best = patternPopulation[0];
  for (auto member : patternPopulation)
    if (member.second > best.second)
      best = member;

  char fileName[256];
  snprintf(fileName, sizeof(fileName), "savedPatterns/patterns%d.pat", rand());
  best.first.save(fileName);

  return 0;
}
