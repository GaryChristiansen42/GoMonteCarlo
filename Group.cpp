#include "Group.h"

#include <vector>
#include <cstdio>

Group::Group(Player newColor) :
  stones(std::list<Point*>()),
  numberLiberties(0), color(newColor)
{ }

Group::~Group() {
}

void Group::addStone(Point* p) {
  p->group = this;
  stones.push_back(p);
  // recalculateLiberties(b);

  if (!(p->north == NULL || p->north->color != Empty
    || (p->north->north != NULL && p->north->north->group == this)
    || (p->north->east != NULL && p->north->east->group == this)
    || (p->north->west != NULL && p->north->west->group == this)))
    numberLiberties++;
  if (!(p->east == NULL || p->east->color != Empty
    || (p->east->north != NULL && p->east->north->group == this)
    || (p->east->east != NULL && p->east->east->group == this)
    || (p->east->south != NULL && p->east->south->group == this)))
    numberLiberties++;
  if (!(p->south == NULL || p->south->color != Empty
    || (p->south->east != NULL && p->south->east->group == this)
    || (p->south->south != NULL && p->south->south->group == this)
    || (p->south->west != NULL && p->south->west->group == this)))
    numberLiberties++;
  if (!(p->west == NULL || p->west->color != Empty
    || (p->west->north != NULL && p->west->north->group == this)
    || (p->west->south != NULL && p->west->south->group == this)
    || (p->west->west != NULL && p->west->west->group == this)))
    numberLiberties++;
}

bool Group::contains(Point* p) {
  if (p->group == this)
    return true;
  return false;
}

bool Group::isAdjacent(Point* p) {
  for(Point* stone : stones) {
    if (stone->north == p)
      return true;
    if (stone->east == p)
      return true;
    if (stone->south == p)
      return true;
    if (stone->west == p)
      return true;
  }
  return false;
}

bool Group::hasLiberties() {
  return numberLiberties > 0;
}

int Group::numLiberties() {
  return numberLiberties;
}

void Group::recalculateLiberties(Board* b) {
  numberLiberties = 0;
  for(Point* stone : stones) {
    if (stone->north != NULL && stone->north->color == Empty
      && !stone->north->marked) {
      numberLiberties++;
      stone->north->marked = true;
    }
    if (stone->east != NULL && stone->east->color == Empty
      && !stone->east->marked) {
      numberLiberties++;
      stone->east->marked = true;
    }
    if (stone->south != NULL && stone->south->color == Empty
      && !stone->south->marked) {
      numberLiberties++;
      stone->south->marked = true;
    }
    if (stone->west != NULL && stone->west->color == Empty
      && !stone->west->marked) {
      numberLiberties++;
      stone->west->marked = true;
    }
  }

  for (int r = 0; r < BOARD_SIZE; ++r)
    for (int c = 0; c < BOARD_SIZE; ++c)
      b->positions[r][c].marked = false;
}

void printGroup(Group* g) {
  for(Point* stone : g->stones) {
    printf("r=%d c=%d\n", stone->row, stone->column);
  }
}
