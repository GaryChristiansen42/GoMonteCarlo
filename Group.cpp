#include "Group.h"

#include <vector>
#include <cstdio>

Group::Group(Player newColor) :
  stones(std::list<Point*>()), liberties(std::list<Point*>()),
  numberLiberties(0), color(newColor)
{ }

Group::~Group() {
}

void Group::addStone(Point* p) {
  p->group = this;
  stones.push_back(p);
  recalculateLiberties();
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

void Group::recalculateLiberties() {
  numberLiberties = 0;
  std::vector<Point*> marked;
  for(Point* stone : stones) {
    if (stone->north != NULL && stone->north->color == Empty
      && !stone->north->marked) {
      marked.push_back(stone->north);
      numberLiberties++;
      stone->north->marked = true;
    }
    if (stone->east != NULL && stone->east->color == Empty
      && !stone->east->marked) {
      marked.push_back(stone->east);
      numberLiberties++;
      stone->east->marked = true;
    }
    if (stone->south != NULL && stone->south->color == Empty
      && !stone->south->marked) {
      marked.push_back(stone->south);
      numberLiberties++;
      stone->south->marked = true;
    }
    if (stone->west != NULL && stone->west->color == Empty
      && !stone->west->marked) {
      marked.push_back(stone->west);
      numberLiberties++;
      stone->west->marked = true;
    }
  }

  for (Point* p : marked)
    p->marked = false;
}

void printGroup(Group* g) {
  for(Point* stone : g->stones) {
    printf("r=%d c=%d\n", stone->row, stone->column);
  }
}
