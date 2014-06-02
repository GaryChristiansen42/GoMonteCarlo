#ifndef POINT_H_
#define POINT_H_

#include "Go.h"

// (BOARD_SIZE, BOARD_SIZE) is pass
struct Point{
  int row;
  int column;

  Player color;

  Point* north;
  Point* east;
  Point* south;
  Point* west;

  bool marked;

  Point();
  Point(int newRow, int newColumn);
  Point(int newRow, int newColumn, Player color, Point* north, 
    Point* east, Point* south, Point* west);

  bool operator==(const Point &p);
  bool operator!=(const Point &p);
};
#endif  // POINT_H_
