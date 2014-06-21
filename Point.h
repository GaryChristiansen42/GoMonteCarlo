#ifndef POINT_H_
#define POINT_H_

#include "Go.h"

class Group;

// (BOARD_SIZE, BOARD_SIZE) is pass
struct Point{
  int row;
  int column;

  Group* group;

  Point* north;
  Point* east;
  Point* south;
  Point* west;

  Player color;
  bool marked;

  Point();
  Point(int newRow, int newColumn);
  Point(int newRow, int newColumn, Player color, Group* group,
    Point* north, Point* east, Point* south, Point* west);

  bool operator==(const Point &p);
  bool operator!=(const Point &p);

  bool isAdjacent(const Point &p);

  void decrementNeighborGroups();
  void incrementNeighborGroups();
};
#endif  // POINT_H_
