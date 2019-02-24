#include "Point.h"

#include <cstdlib>

#include "Group.h"

Point::Point() :
  color(Empty), row(0), column(0), marked(false), group(nullptr), north(nullptr),
  east(nullptr), south(nullptr), west(nullptr)
{ }

Point::Point(char newRow, char newColumn) :
  color(Empty), row(newRow), column(newColumn), marked(false), group(nullptr), 
  north(nullptr), east(nullptr), south(nullptr), west(nullptr)
{ }

Point::Point(char newRow, char newColumn, Player newColor, Group* newGroup,
  Point* newNorth, Point* newEast, Point* newSouth, Point* newWest) :
  color(newColor), row(newRow), column(newColumn), marked(false), group(newGroup),
  north(newNorth), east(newEast), south(newSouth), west(newWest)
{ }

bool Point::operator==(const Point &p) {
  return this->row == p.row && this->column == p.column;
}


bool Point::operator!=(const Point &p) {
  return !((*this) == p);
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

void Point::decrementNeighborGroups() {
  Group* decrementedGroups[4];
  unsigned char numDecrementedGroups = 0;
  if (north->color == Black || north->color == White) {
    north->group->numberLiberties--;
    decrementedGroups[0] = north->group;
    ++numDecrementedGroups;
  }
  if (east->color == Black || east->color == White) {
    bool found = false;
    for (unsigned char i = 0; i < numDecrementedGroups; ++i) {
      if (decrementedGroups[i] == east->group) {
        found = true;
      }
    }
    if (!found) {
      east->group->numberLiberties--;
      decrementedGroups[numDecrementedGroups] = east->group;
      ++numDecrementedGroups;
    }
  }
  if (south->color == Black || south->color == White) {
    bool found = false;
    for (unsigned char i = 0; i < numDecrementedGroups; ++i) {
      if (decrementedGroups[i] == south->group) {
        found = true;
      }
    }
    if (!found) {
      south->group->numberLiberties--;
      decrementedGroups[numDecrementedGroups] = south->group;
      ++numDecrementedGroups;
    }
  }
  if (west->color == Black || west->color == White) {
    bool found = false;
    for (unsigned char i = 0; i < numDecrementedGroups; ++i) {
      if (decrementedGroups[i] == west->group) {
        found = true;
      }
    }
    if (!found) {
      west->group->numberLiberties--;
    }
  }
}

void Point::incrementNeighborGroups() {
  Group* incrementedGroups[4];
  unsigned char numIncrementedGroups = 0;
  if (north->color == Black || north->color == White) {
    north->group->numberLiberties++;
    incrementedGroups[0] = north->group;
    ++numIncrementedGroups;
  }
  if (east->color == Black || east->color == White) {
    bool found = false;
    for (unsigned char i = 0; i < numIncrementedGroups; ++i) {
      if (incrementedGroups[i] == east->group) {
        found = true;
      }
    }
    if (!found) {
      east->group->numberLiberties++;
      incrementedGroups[numIncrementedGroups] = east->group;
      ++numIncrementedGroups;
    }
  }
  if (south->color == Black || south->color == White) {
    bool found = false;
    for (unsigned char i = 0; i < numIncrementedGroups; ++i) {
      if (incrementedGroups[i] == south->group) {
        found = true;
      }
    }
    if (!found) {
      south->group->numberLiberties++;
      incrementedGroups[numIncrementedGroups] = south->group;
      ++numIncrementedGroups;
    }
  }
  if (west->color == Black || west->color == White) {
    bool found = false;
    for (unsigned char i = 0; i < numIncrementedGroups; ++i) {
      if (incrementedGroups[i] == west->group) {
        found = true;
      }
    }
    if (!found) {
      west->group->numberLiberties++;
    }
  }
}
