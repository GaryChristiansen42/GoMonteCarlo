#ifndef GROUP_H_
#define GROUP_H_

#include <vector>

#include "Go.h"
#include "Point.h"
#include "Board.h"

class Board;

class Group {
 public:
  std::vector<Point*> stones;
  Player color;

  explicit Group(Player newColor);

  ~Group();

  void addStone(Point* p);

  bool isAdjacent(Point* p);

  bool hasLiberties(Board* state);
};

void printGroup(Group* g);

#endif  // GROUP_H_
