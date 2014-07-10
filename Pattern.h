#ifndef PATTERN_H_
#define PATTERN_H_

#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Point.h"

class Board;

class Pattern {
 public:

  std::string hash;
  std::vector<std::pair<char, char>> goodMoves;

  Pattern();
  // Pattern(Point* p);
  virtual ~Pattern();
  friend bool operator== ( const Pattern &p1, const Pattern &p2);

  char pointToChar(Point* p);

  // void rotate90();
  void invertColor();

  static char getRandomColor(std::default_random_engine& engine);
  // bool isLegalPattern();
  // void determineRandomGoodMoves(std::default_random_engine& engine);
  // void mutate(std::default_random_engine& engine);
  // Pattern getMutated(std::default_random_engine& engine);
  
  std::vector<Point*> getGoodMoves(Board* b, Point move);

  // static Pattern getRandomPattern(std::default_random_engine& engine);

};

#endif  // PATTERN_H_
