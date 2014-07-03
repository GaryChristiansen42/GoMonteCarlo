#ifndef PATTERN_H_
#define PATTERN_H_

#include <iostream>
#include <string>
#include <vector>

#include "Point.h"

class Board;

class Pattern {
 public:

  std::string hash;
  std::vector<std::pair<char, char>> goodMoves;

  Pattern();
  Pattern(Point* p);

  void rotate90();
  void invertColor();

  std::vector<Point*> getGoodMoves(Board* b, Point move);

  static Pattern getRandomPattern();
};

std::ostream& operator<<(std::ostream &os, const Pattern &pattern);
std::istream& operator>>(std::istream &is, Pattern& pattern);
#endif  // PATTERN_H_