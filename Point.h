#ifndef POINT_H_
#define POINT_H_

#include "Go.h"

class Group;

// (BOARD_SIZE, BOARD_SIZE) is pass
struct Point{
  int row;
  int column;

  Player color;
  Group* group;

  Point* north;
  Point* east;
  Point* south;
  Point* west;

  bool marked;

  Point();
  Point(int newRow, int newColumn);
  Point(int newRow, int newColumn, Player color, Group* group,
    Point* north, Point* east, Point* south, Point* west);

  bool operator==(const Point &p) const;
  bool operator!=(const Point &p) const;

  bool isAdjacent(const Point &p) const;
};
#endif  // POINT_H_
