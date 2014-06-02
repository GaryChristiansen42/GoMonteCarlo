#include "Point.h"

#include <cstdlib>

Point::Point() :
  row(0), column(0), color(Empty), north(NULL), east(NULL), south(NULL),
  west(NULL), marked(false)
{ }

Point::Point(int newRow, int newColumn) :
  row(newRow), column(newColumn), color(Empty), north(NULL), east(NULL),
  south(NULL), west(NULL), marked(false)
{ }

Point::Point(int newRow, int newColumn, Player newColor, Point* newNorth,
  Point* newEast, Point* newSouth, Point* newWest) :
  row(newRow), column(newColumn), color(newColor), north(newNorth),
  east(newEast), south(newSouth), west(newWest), marked(false)
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
