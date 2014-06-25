#include "Patterns.h"

#include <assert.h>
#include <fstream>
#include <iostream>

#include "Board.h"

Patterns::Patterns() : hashTable(std::unordered_map<std::string, Pattern>()) {

}

void Patterns::init(std::string patternsFile) {
  std::ifstream in;
  in.open(patternsFile);
  if (!in.is_open())
    assert(false);

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

Point Patterns::getMove(Board* b) {

  Point temp = Point(-1, -1);
  return *b->getPoint(&temp);
}
