#ifndef UCTNODE_H_
#define UCTNODE_H_

#include <memory>
#include <mutex>
#include <vector>

#include "Point.h"
#include "Board.h"

struct UCTNode {
  Point move;

  static Board* rootState;


  UCTNode* bestNode;
  UCTNode* child;
  UCTNode* sibling;
  UCTNode* parent;

  std::vector<UCTNode*> possibleChildren;

  double totalRewards;
  int visits;
  std::mutex mutex;

  UCTNode(Point newMove, UCTNode* newParent);
  UCTNode(const UCTNode& n);
  UCTNode operator=(const UCTNode& n);
  ~UCTNode();

  std::unique_ptr<Board> getState();

  void addChild(UCTNode* newChild);
  void removeChild(UCTNode* childToRemove);

  bool isChild(Point nodeMove);
};
#endif  // UCTNODE_H_
