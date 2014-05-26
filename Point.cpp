#include "Point.h"

Point::Point() :
  row(0), column(0)
{ }

Point::Point(int newRow, int newColumn) :
  row(newRow), column(newColumn)
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
