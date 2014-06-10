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
  bool legal;

  Point();
  Point(int newRow, int newColumn);
  Point(int newRow, int newColumn, Player color, Group* group,
    Point* north, Point* east, Point* south, Point* west, bool legal);

  bool operator==(const Point &p);
  bool operator!=(const Point &p);

  bool isAdjacent(const Point &p);
};
#endif  // POINT_H_
