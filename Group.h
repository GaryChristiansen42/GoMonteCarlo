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
  unsigned char numberLiberties;
  Player color;
  // Change to be above stones for 'hotness'

  explicit Group(Player newColor);

  ~Group();

  void addStone(Point& p);

  void removeStones();

  bool contains(Point* p);
  bool isAdjacent(Point* p);

  // void recalculateLiberties(Board* b);
};

void printGroup(Group* g);

#endif  // GROUP_H_
