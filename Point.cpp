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
