#ifndef PATTERNS_H_
#define PATTERNS_H_

#include <string>
#include <unordered_map>

#include "Pattern.h"

class Board;
class Point;

class Patterns {
 public:

  std::unordered_map<std::string, Pattern> hashTable;
  bool initialized;

  explicit Patterns();

  void init(std::string patternFile);
  void clear();

  Point getMove(Board* b);
};
#endif  // BOARD_H_
