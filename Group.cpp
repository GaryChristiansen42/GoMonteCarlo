#include "Group.h"

#include <vector>
#include <cstdio>

Group::Group(Player newColor) :
  stones(std::list<Point*>()), color(newColor),
  liberties(std::list<Point*>()), numberLiberties(0)
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

bool Group::hasLiberties() {
  return numberLiberties > 0;
}

int Group::numLiberties() {
  return numberLiberties;
}

void Group::recalculateLiberties() {
  liberties.clear();
  std::vector<Point*> marked;
  for(Point* stone : stones) {
    if (stone->north != NULL && stone->north->color == Empty
      && !stone->north->marked) {
      liberties.push_back(stone->north);
      marked.push_back(stone->north);
      stone->north->marked = true;
    }
    if (stone->east != NULL && stone->east->color == Empty
      && !stone->east->marked) {
      liberties.push_back(stone->east);
      marked.push_back(stone->east);
      stone->east->marked = true;
    }
    if (stone->south != NULL && stone->south->color == Empty
      && !stone->south->marked) {
      liberties.push_back(stone->south);
      marked.push_back(stone->south);
      stone->south->marked = true;
    }
    if (stone->west != NULL && stone->west->color == Empty
      && !stone->west->marked) {
      liberties.push_back(stone->west);
      marked.push_back(stone->west);
      stone->west->marked = true;
    }
  }

  for (Point* p : marked)
    p->marked = false;

  numberLiberties = (int)liberties.size();
}

void printGroup(Group* g) {
  for(Point* stone : g->stones) {
    printf("r=%d c=%d\n", stone->row, stone->column);
  }
}
