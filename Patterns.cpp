#include "Patterns.h"

#include <assert.h>
#include <fstream>
#include <iostream>

#include "Board.h"

Patterns::Patterns() :
  hashTable(std::unordered_map<std::string, Pattern>()),
  initialized(false) {

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

  for (auto p : hashTable)
    std::cout << p.second << std::endl; 
  in.close();
}

void Patterns::clear() {
  hashTable.clear();
  initialized = false;
}

Point* Patterns::getMove(Board* b) {

  Pattern lastMove(b->lastMove);
  auto it = hashTable.find(lastMove.hash);
  if (it != hashTable.end()) {
    std::vector<Point*> goodMoves = lastMove.getGoodMoves(b);
    long unsigned int choice = rand() % goodMoves.size();
    return goodMoves[choice];
  }

  return b->getRandomMove();
}
