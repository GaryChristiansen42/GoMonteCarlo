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

bool Group::contains(Point* p) {
  foreach(Point* stone, stones) {
    if (stone->row == p->row && stone->column == p->column)
      return true;
  }
  return false;
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

int Group::numLiberties(Board* state) {
  int num = 0;
  int** positions = new int*[state->boardSize];
  int i;
  for(i = 0; i < state->boardSize; i++) {
    positions[i] = new int[state->boardSize];
    memset(positions[i], 0, state->boardSize*sizeof(int));
  }
  foreach(Point* stone, stones) {
    // free above
    if (stone->column > 0 
      && state->positions[stone->row][stone->column-1] == Empty
      && positions[stone->row][stone->column-1] == 0) {
      num++;
      positions[stone->row][stone->column-1] = 1;
    }
    // free below
    if (stone->column < state->boardSize-1
      && state->positions[stone->row][stone->column+1] == Empty
      && positions[stone->row][stone->column+1] == 0) {
      num++;
      positions[stone->row][stone->column+1] = 1;
    }
    // free to the left
    if (stone->row > 0
      && state->positions[stone->row-1][stone->column] == Empty
      && positions[stone->row-1][stone->column] == 0) {
      num++;
      positions[stone->row-1][stone->column] = 1;
    }
    // free to the right
    if (stone->row < state->boardSize-1
      && state->positions[stone->row+1][stone->column] == Empty
      && positions[stone->row+1][stone->column] == 0) {
      num++;
      positions[stone->row+1][stone->column] = 1;
    }
  }
  
  //for(i = 0; i < state->boardSize; i++);
  //  delete positions[i];
  //delete positions;
  return num;
}

void printGroup(Group* g) {
  foreach(Point* stone, g->stones) {
    printf("r=%d c=%d\n", stone->row, stone->column);
  }
}
