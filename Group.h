#ifndef GROUP_H_
#define GROUP_H_

#include <list>

#include "Go.h"
#include "Point.h"
#include "Board.h"

class Board;

class Group {
 public:
  std::list<Point*> stones;
  unsigned char numberLiberties;
  Player color;

  explicit Group(Player newColor);

  ~Group();

  void addStone(Point* p);

  void removeStones();

  bool contains(Point* p);
  bool isAdjacent(Point* p);

  bool hasLiberties();
  int numLiberties();

  // void recalculateLiberties(Board* b);
};

void printGroup(Group* g);

#endif  // GROUP_H_
