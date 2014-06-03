#include "Point.h"

#include <cstdlib>

Point::Point() :
  row(0), column(0), color(Empty), group(NULL),
  north(NULL), east(NULL), south(NULL), west(NULL), marked(false)
{ }

Point::Point(int newRow, int newColumn) :
  row(newRow), column(newColumn), color(Empty), group(NULL),
  north(NULL), east(NULL), south(NULL), west(NULL), marked(false)
{ }

Point::Point(int newRow, int newColumn, Player newColor, Group* newGroup,
  Point* newNorth, Point* newEast, Point* newSouth, Point* newWest) :
  row(newRow), column(newColumn), color(newColor), group(newGroup),
  north(newNorth), east(newEast), south(newSouth), west(newWest),
  marked(false)
{ }

bool Point::operator==(const Point &p) {
  if (this->row == p.row
      && this->column == p.column)
    return true;
  return false;
}


bool Point::operator!=(const Point &p) {
  if (this->row == p.row
      && this->column == p.column)
    return false;
  return true;
}

bool Point::isAdjacent(const Point &p) {
  if (p.row == row) {
    if (p.column == column + 1 || p.column == column - 1) {
      return true;
    }
  }

  if (p.column == column) {
    if (p.row == row + 1 || p.row == row - 1) {
      return true;
    }
  }

  return false;
}
