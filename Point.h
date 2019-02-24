#ifndef POINT_H_
#define POINT_H_

#include "Go.h"

class Group;

// (BOARD_SIZE, BOARD_SIZE) is pass
struct Point{
  Player color;

  char row;
  char column;

  bool marked;

  Group* group;

  Point* north;
  Point* east;
  Point* south;
  Point* west;

  Point();
  Point(char newRow, char newColumn);
  Point(char newRow, char newColumn, Player newColor, Group* newGroup,
    Point* newNorth, Point* newEast, Point* newSouth, Point* newWest);

  bool operator==(const Point &p);
  bool operator!=(const Point &p);

  bool isAdjacent(const Point &p);

  void decrementNeighborGroups();
  void incrementNeighborGroups();
};
#endif  // POINT_H_
