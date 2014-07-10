#ifndef PATTERN_3X3_H_
#define PATTERN_3X3_H_

#include "Pattern.h"

class Pattern3x3 : public Pattern {
 public:
  Pattern3x3();
  Pattern3x3(Point* p);

  void rotate90();

  bool isLegalPattern();

  void determineRandomGoodMoves(std::default_random_engine& engine);
  void mutate(std::default_random_engine& engine);
  Pattern3x3 getMutated(std::default_random_engine& engine);
};

std::ostream& operator<<(std::ostream &os, const Pattern3x3 &pattern);
std::istream& operator>>(std::istream &is, Pattern3x3& pattern);
#endif  // PATTERN_3X3_H_
