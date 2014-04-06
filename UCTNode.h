#ifndef UCTNODE_H_
#define UCTNODE_H_

#include <vector>

#include "Point.h"
#include "Board.h"

struct UCTNode {
  int visits;
  double totalRewards;

  Point move;

  Board* state;

  UCTNode* bestNode;
  UCTNode* child;
  UCTNode* sibling;
  UCTNode* parent;

  std::vector<UCTNode*> possibleChildren;

  UCTNode(Point newMove, Board* newState, UCTNode* newParent);
  UCTNode(const UCTNode& n);
  UCTNode operator=(const UCTNode& n);
  ~UCTNode();

  void addChild(UCTNode* newChild);
  void removeChild(UCTNode* childToRemove);

  bool isChild(Point nodeMove);
};
#endif  // UCTNODE_H_
