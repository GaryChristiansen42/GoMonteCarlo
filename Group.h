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

  bool contains(const Point &p);
  bool isAdjacent(const Point &p);

  bool hasLiberties(const Board &state);
  int numLiberties(const Board &state);
};

void printGroup(Group* g);

#endif  // GROUP_H_
