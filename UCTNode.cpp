#include "UCTNode.h"

#include <stddef.h>
#include <assert.h>
#include <vector>

UCTNode::UCTNode(Point newMove, UCTNode* newParent) :
  move(newMove), state(nullptr), bestNode(nullptr), child(nullptr), sibling(nullptr),
  parent(newParent), possibleChildren(std::vector<UCTNode*>()),
  totalRewards(0.0), visits(0.0), mutex()
{ }

UCTNode::~UCTNode() {
  if (state)
    delete state;
  if (child)
    delete child;
  if (sibling)
    delete sibling;

  for (unsigned int i = 0; i < possibleChildren.size(); i++)
    delete possibleChildren[i];
}

void UCTNode::init() {
  mutex.lock();
  state = parent->state->clone();
  state->makeMove(*parent->state->getPoint(&move));
  mutex.unlock();
}

void UCTNode::addChild(UCTNode* newChild) {
  mutex.lock();
  if (child == nullptr) {
    child = newChild;
  } else {
    UCTNode* temp = child;
    while (temp->sibling != nullptr)
      temp = temp->sibling;
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

bool UCTNode::isChild(Point nodeMove) {
  mutex.lock();
  UCTNode* next = child;
  while (next != nullptr) {
    if (next->move.row == nodeMove.row && next->move.column == nodeMove.column) {
      mutex.unlock();
      return true;
    }
    next = next->sibling;
  }
  mutex.unlock();
  return false;
}
