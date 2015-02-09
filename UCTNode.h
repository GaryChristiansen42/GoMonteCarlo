#ifndef UCTNODE_H_
#define UCTNODE_H_

#include <memory>
#include <mutex>
#include <vector>

#include "Point.h"
#include "Board.h"

struct UCTNode {
  static Board* rootState;

  unsigned char row;
  unsigned char column;


  UCTNode* child;
  UCTNode* sibling;
  UCTNode* parent;

  std::vector<unsigned short> possibleChildren;

  long totalRewards;
  long visits;
  std::mutex mutex;

  UCTNode(unsigned char newRow, unsigned char newColumn, UCTNode* newParent);
  UCTNode(const UCTNode& n);
  UCTNode operator=(const UCTNode& n);
  ~UCTNode();

  std::unique_ptr<Board> getState();

  void addChild(UCTNode* newChild);
  void removeChild(UCTNode* childToRemove);

  // bool isChild(Point nodeMove);
};
#endif  // UCTNODE_H_
