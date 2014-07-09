#include "Patterns.h"

#include <assert.h>
#include <fstream>
#include <iostream>

#include "Board.h"

Patterns::Patterns() :
  numCalled(0),
  hashTable(std::unordered_map<std::string, Pattern>()),
  initialized(false) {

}


bool Patterns::operator==(const Patterns &p) {
  if (p.hashTable.size() != hashTable.size())
    return false;
  for (auto i : hashTable) {
    bool found = false;
    for (auto j : p.hashTable) {
      if (!i.first.compare(j.first)) {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
  return true;
}

void Patterns::init(std::string patternsFile) {

  if (patternsFile == "")
    return;
  
  std::ifstream in;
  in.open(patternsFile);
  if (!in.is_open())
    assert(false);

  initialized = true;

  Pattern pattern;
  while (in >> pattern) {
    hashTable[pattern.hash] = pattern;

    pattern.rotate90();
    hashTable[pattern.hash] = pattern;

    pattern.rotate90();
    hashTable[pattern.hash] = pattern;

    pattern.rotate90();
    hashTable[pattern.hash] = pattern;

    pattern.invertColor();
    hashTable[pattern.hash] = pattern;

    pattern.rotate90();
    hashTable[pattern.hash] = pattern;

    pattern.rotate90();
    hashTable[pattern.hash] = pattern;

    pattern.rotate90();
    hashTable[pattern.hash] = pattern;
  }

  // for (auto p : hashTable)
    // std::cout << p.second << std::endl; 
  in.close();
}

void Patterns::clear() {
  hashTable.clear();
  initialized = false;
}

void Patterns::save(std::string fileName) {

  std::ofstream f;
  f.open(fileName);
  assert(f.is_open());

  for (auto i : hashTable) {
    f << i.second;
  }
  f.close();
}

void Patterns::addPattern(Pattern pattern) {
  hashTable[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable[pattern.hash] = pattern;

  pattern.invertColor();
  hashTable[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable[pattern.hash] = pattern;

  pattern.rotate90();
  hashTable[pattern.hash] = pattern;
}

void Patterns::mutate() {
  unsigned int choice = rand() % 1;
  if (choice == 1 && hashTable.size() > 0) { // delete pattern
    unsigned long int numToDelete = rand() % hashTable.size();
    unsigned long int i = 0;
    for (auto it = hashTable.begin(); it != hashTable.end(); ++i, ++it) {
      if (i == numToDelete) {
        hashTable.erase(it);
        break;
      }
    }
  } else { // add new random pattern
    Pattern randomPattern = Pattern::getRandomPattern();
    addPattern(randomPattern);
    // hashTable.insert(std::make_pair(randomPattern.hash, randomPattern));
  }
}

void Patterns::mutatePattern(std::string hash) {
  Pattern mutated = hashTable[hash].getMutated();
  addPattern(mutated);
}

Point* Patterns::getMove(Board* b) {

  if (b->lastMove != NULL) {
    Pattern lastMove(b->lastMove);
    auto it = hashTable.find(lastMove.hash);
    if (it != hashTable.end()) {
      numCalled++;
      std::vector<Point*> goodMoves = (*it).second.getGoodMoves(b, *b->lastMove);
      if (goodMoves.size() > 0) {
        long unsigned int choice = rand() % goodMoves.size();
        bool isLegal = false;
        for (unsigned int i = 0; i < b->numLegalMoves; ++i) {
          if (goodMoves[choice] == b->legalMoves[i]) {
            isLegal = true;
            break;
          }
        }
        if (isLegal)
          return goodMoves[choice];
      }
    }
  }

  return b->getRandomMove();
}
