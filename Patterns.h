#ifndef PATTERNS_H_
#define PATTERNS_H_

#include <random>
#include <string>
#include <unordered_map>

#include "Pattern.h"
#include "Pattern3x3.h"
#include "Pattern5x5.h"

class Board;
class Point;

class Patterns {
 public:
  bool operator==(const Patterns &p);

  int numCalled;
  int numNotLegal;
  int numNull;
  int numNotCalled;
  int numCalled2;
  int numNotLegal2;
  int numNull2;
  int numNotCalled2;
  int total;
  std::unordered_map<std::string, Pattern3x3> hashTable3x3;
  std::unordered_map<std::string, Pattern5x5> hashTable5x5;
  bool initialized;

  explicit Patterns();

  void init(std::string patternsFile3x3, std::string patternsFile5x5);
  void clear();
  void save(std::string fileName3x3, std::string fileName5x5);

  void addPattern(Pattern3x3 pattern);
  void addPattern(Pattern5x5 pattern);

  void mutate(std::default_random_engine& engine);
  void mutatePattern(std::string hash, std::default_random_engine& engine);

  Point* getMove(Board& b, std::default_random_engine& engine);
};
#endif  // PATTERNS_H_
