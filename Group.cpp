#include "Group.h"

#include <boost/foreach.hpp>

#include <vector>
#include <cstdio>

#define foreach BOOST_FOREACH

Group::Group(Player newColor) :
  stones(std::vector<Point*>()), color(newColor)
{ }

Group::~Group() {
  foreach(Point* stone, stones)
    delete stone;
}

void Group::addStone(Point* p) {
  stones.push_back(p);
}

bool Group::isAdjacent(Point* p) {
  foreach(Point* stone, stones) {
    if (stone->row == p->row+1 && stone->column == p->column)
      return true;
    if (stone->row == p->row-1 && stone->column == p->column)
      return true;
    if (stone->row == p->row && stone->column == p->column+1)
      return true;
    if (stone->row == p->row && stone->column == p->column-1)
      return true;
  }
  return false;
}

bool Group::hasLiberties(Board* state) {
  foreach(Point* stone, stones) {
    // free above
    if (stone->column > 0 &&
      state->positions[stone->row][stone->column-1] == Empty)
      return true;
    // free below
    if (stone->column < state->boardSize-1
      && state->positions[stone->row][stone->column+1] == Empty)
      return true;
    // free to the left
    if (stone->row > 0
      && state->positions[stone->row-1][stone->column] == Empty)
      return true;
    if (stone->row < state->boardSize-1
      && state->positions[stone->row+1][stone->column] == Empty)
      return true;
  }
  return false;
}

void printGroup(Group* g) {
  foreach(Point* stone, g->stones) {
    printf("r=%d c=%d\n", stone->row, stone->column);
  }
}
