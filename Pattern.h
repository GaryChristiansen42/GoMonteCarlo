#ifndef PATTERN_H_
#define PATTERN_H_

#include <iostream>
#include <string>

class Pattern {
 public:

  std::string hash;

  Pattern();

  void rotate90();
  void invertColor();

};

std::ostream& operator<<(std::ostream &os, const Pattern &pattern);
std::istream& operator>>(std::istream &is, Pattern& pattern);
#endif  // PATTERN_H_
