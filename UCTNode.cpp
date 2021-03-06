#include "UCTNode.h"

#include <assert.h>

#include <cstdint>
#include <stack>
#include <vector>

#include "Board.h"

Board* UCTNode::rootState = nullptr;

UCTNode::UCTNode(unsigned char newRow, unsigned char newColumn, UCTNode* newParent) :
  row(newRow), column(newColumn), child(nullptr), sibling(nullptr),
  parent(newParent), possibleChildren(std::vector<uint16_t>()),
  totalRewards(0.0), visits(0.0), mutex()
{ }

UCTNode::~UCTNode() {
  if (child != nullptr) {
    delete child;
  }
  if (sibling != nullptr) {
    delete sibling;
  }
}

std::unique_ptr<Board> UCTNode::getState() {
  std::unique_ptr<Board> state(UCTNode::rootState->clone());
  std::stack<Point> moves;

  auto node = this;
  while (node->parent != nullptr) {
    moves.push(Point(node->row, node->column));
    node = node->parent;
  }
  while (!moves.empty()) {
    state->makeMove(moves.top());
    moves.pop();
  }
  return state;
}

void UCTNode::addChild(UCTNode* newChild) {
  mutex.lock();
  if (child == nullptr) {
    child = newChild;
  } else {
    UCTNode* temp = child;
    while (temp->sibling != nullptr) {
      temp = temp->sibling;
    }
    temp->sibling = newChild;
  }
  mutex.unlock();
}

void UCTNode::removeChild(UCTNode* childToRemove) {
  if (child == childToRemove) {
    child = child->sibling;
    childToRemove->parent = nullptr;
    childToRemove->sibling = nullptr;
    return;
  }

  UCTNode* temp = child;
  while (temp != nullptr) {
    if (temp->sibling == childToRemove) {
      temp->sibling = childToRemove->sibling;
      childToRemove->parent = nullptr;
      childToRemove->sibling = nullptr;
      return;
    }
    temp = temp->sibling;
  }
  assert(false);
}

/*bool UCTNode::isChild(Point nodeMove) {
  mutex.lock();
  UCTNode* next = child;
  while (next != nullptr) {
    if (next->row == nodeMove.row && next->column == nodeMove.column) {
      mutex.unlock();
      return true;
    }
    next = next->sibling;
  }
  mutex.unlock();
  return false;
}*/
