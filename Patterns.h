#ifndef PATTERNS_H_
#define PATTERNS_H_

#include <string>
#include <unordered_map>

#include "Pattern.h"

class Board;
class Point;

class Patterns {
 public:
  bool operator==(const Patterns &p);

  int numCalled;
  std::unordered_map<std::string, Pattern> hashTable;
  bool initialized;

  explicit Patterns();

  void init(std::string patternFile);
  void clear();
  void save(std::string fileName);

  void mutate();

  Point* getMove(Board* b);
};
#endif  // PATTERN_H_
