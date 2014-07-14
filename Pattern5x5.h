#ifndef PATTERN_5X5_H_
#define PATTERN_5X5_H_

#include "Pattern.h"

class Pattern5x5 : public Pattern {
 public:
  Pattern5x5();
  Pattern5x5(Point* p);

  void rotate90();

  bool isLegalPattern();

  void changeOneGoodMove(std::default_random_engine& engine);
  void determineRandomGoodMoves(std::default_random_engine& engine);
  void mutate(std::default_random_engine& engine);
  Pattern5x5 getMutated(std::default_random_engine& engine);

  static Pattern5x5 getRandomPattern(std::default_random_engine& engine);
};

std::ostream& operator<<(std::ostream &os, const Pattern5x5 &pattern);
std::istream& operator>>(std::istream &is, Pattern5x5& pattern);
#endif  // PATTERN_5X5_H_
