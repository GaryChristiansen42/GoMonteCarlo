#include "UCTNode.h"

#include <stddef.h>
#include <assert.h>
#include <vector>

UCTNode::UCTNode(Point newMove, UCTNode* newParent) :
  move(newMove), state(NULL), bestNode(NULL), child(NULL), sibling(NULL),
  parent(newParent), possibleChildren(std::vector<UCTNode*>()),
  totalRewards(0.0), visits(0.0)
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
  state = parent->state->clone();
  state->makeMove(move);
}

void UCTNode::addChild(UCTNode* newChild) {
  if (child == NULL) {
    child = newChild;
  } else {
    UCTNode* temp = child;
    while (temp->sibling != NULL)
      temp = temp->sibling;
    temp->sibling = newChild;
  }
}

void UCTNode::removeChild(UCTNode* childToRemove) {
  if (child == childToRemove) {
    child = child->sibling;
    childToRemove->parent = NULL;
    childToRemove->sibling = NULL;
    return;
  }

  UCTNode* temp = child;
  while (temp != NULL) {
    if (temp->sibling == childToRemove) {
      temp->sibling = childToRemove->sibling;
      childToRemove->parent = NULL;
      childToRemove->sibling = NULL;
      return;
    }
    temp = temp->sibling;
  }
  assert(false);
}

bool UCTNode::isChild(Point nodeMove) {
  UCTNode* next = child;
  while (next != NULL) {
    if (next->move.row == nodeMove.row && next->move.column == nodeMove.column)
      return true;
    next = next->sibling;
  }
  return false;
}
