#include "Board.h"

#include <assert.h>

#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>
#include <stack>

Board::Board(int newBoardSize) :
  boardSize(newBoardSize),
  positions(new Point**[newBoardSize]),
  blackGroups(std::vector<Group*>()),
  whiteGroups(std::vector<Group*>()),
  emptyGroups(std::vector<Group*>()),
  turn(Black),
  lastMove(NULL),
  secondLastMove(NULL),
  koPoint(NULL),
  pass(new Point(newBoardSize, newBoardSize)),
  capturedBlack(0),
  capturedWhite(0),
  possibleMoves(std::vector<Point*>()) {
  for (int row = 0; row < boardSize; row++) {
    positions[row] = new Point*[boardSize];
  }
}

/*Board::Board(const Board& b) :
  positions(new int*[BOARD_SIZE]),
  blackGroups(std::vector<Group*>()),
  whiteGroups(std::vector<Group*>()),
  turn(b.turn), computer(b.computer),
  lastMove(b.lastMove),
  secondLastMove(b.secondLastMove)
{
  for(int r = 0; r < BOARD_SIZE; r++)
  {
    positions[r] = new int[BOARD_SIZE];
    memcpy(positions[r], b.positions[r], BOARD_SIZE*sizeof(int));
  }

  for(Group* g : b.blackGroups)
  {
    Group* gClone = new Group(Black);
    for(Point* p : g->stones)
    {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    blackGroups.push_back(gClone);
  }
  for(Group* g : b.whiteGroups)
  {
    Group* gClone = new Group(White);
    for(Point* p : g->stones)
    {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    whiteGroups.push_back(gClone);
  }
}

Board& Board::operator=(const Board& b)
{
  positions = new int*[BOARD_SIZE];
  for(int r = 0; r < BOARD_SIZE; r++)
  {
    positions[r] = new int[BOARD_SIZE];
    memcpy(positions[r], b.positions[r], BOARD_SIZE*sizeof(int));
  }

  turn = b.turn;
  computer = b.computer;
  lastMove = b.lastMove;
  secondLastMove = b.secondLastMove;

  for(Group* g : b.blackGroups)
  {
    Group* gClone = new Group(Black);
    for(Point* p : g->stones)
    {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    blackGroups.push_back(gClone);
  }
  for(Group* g : b.whiteGroups)
  {
    Group* gClone = new Group(White);
    for(Point* p : g->stones)
    {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    whiteGroups.push_back(gClone);
  }
  return *this;
}*/

Board::~Board() {
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      delete positions[row][column];
    }
    delete[] positions[row];
  }
  delete[] positions;

  for(Group* g : blackGroups)
    delete g;
  for(Group* g : whiteGroups)
    delete g;
  for(Group* g : emptyGroups)
    delete g;

  if (pass != NULL)
    delete pass;
}

// Should only be called when creating a new board, not a clone
void Board::init() {
  for (int row = 0; row < boardSize; row++) {
    // memset(positions[row], 0, boardSize*sizeof(Point*));
    for (int column = 0; column < boardSize; column++) {
      positions[row][column] = new Point(row, column);
    }
  }
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      if (row + 1 < boardSize)
        positions[row][column]->north = positions[row+1][column];
      if (column + 1 < boardSize)
        positions[row][column]->east = positions[row][column+1];
      if (row - 1 >= 0)
        positions[row][column]->south = positions[row-1][column];
      if (column - 1 >= 0)
        positions[row][column]->west = positions[row][column-1];
    }
  }

  for (Group* g : blackGroups) {
    g->recalculateLiberties();
  }
  for (Group* g : whiteGroups) {
    g->recalculateLiberties();
  }

  // Fill emptyGroups with correct data
  Group* g = new Group(Empty);
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      g->addStone(positions[row][column]);
    }
  }
  emptyGroups.push_back(g);

  getPossibleMoves();
}

bool Board::operator==(const Board &b) {
  if (boardSize != b.boardSize)
    return false;
  for (int row = 0; row < boardSize; row++)
    for (int column = 0; column < boardSize; column++)
      if (*positions[row][column] != *b.positions[row][column])
        return false;
  if (secondLastMove == NULL && b.secondLastMove == NULL
    && !(*lastMove == *b.lastMove))
    return false;
  if (secondLastMove != NULL && b.secondLastMove != NULL
    && !(*secondLastMove == *b.secondLastMove))
    return false;
  return true;
}


/*void Board::initialize(Player computer)
{
  positions = new int*[BOARD_SIZE];
  for(int r = 0; r < BOARD_SIZE; r++)
  {
    positions[r] = new int[BOARD_SIZE];
    memset(positions[r], 0, BOARD_SIZE*sizeof(int));
  }

  turn = Black;
  this->computer = computer;
  
  lastMove = Point(-1, -1);
  secondLastMove = Point(-1, -1);
}*/

Board* Board::clone() {
  Board* b = new Board(boardSize);
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      Point * p = positions[row][column];
      b->positions[row][column] = new Point(p->row, p->column, p->color,
        NULL, NULL, NULL, NULL, NULL);
    }
  }
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      if (row + 1 < boardSize)
        b->positions[row][column]->north = b->positions[row+1][column];
      if (column + 1 < boardSize)
        b->positions[row][column]->east = b->positions[row][column+1];
      if (row - 1 >= 0)
        b->positions[row][column]->south = b->positions[row-1][column];
      if (column - 1 >= 0)
        b->positions[row][column]->west = b->positions[row][column-1];
    }
  }

  b->possibleMoves.clear();
  for (unsigned int i = 0; i < possibleMoves.size(); i++)
    b->possibleMoves.push_back(b->getPoint(possibleMoves[i]));

  b->turn = turn;

  b->lastMove = b->getPoint(lastMove);
  b->secondLastMove = b->getPoint(secondLastMove);

  b->koPoint = b->getPoint(koPoint);

  b->capturedBlack = capturedBlack;
  b->capturedWhite = capturedWhite;

  for(Group* g : blackGroups) {
    Group* gClone = new Group(Black);
    for(Point* p : g->stones) {
      Point temp(p->row, p->column);
      Point* pClone = b->getPoint(&temp);
      gClone->addStone(pClone);
    }
    b->blackGroups.push_back(gClone);
  }
  for(Group* g : whiteGroups) {
    Group* gClone = new Group(White);
    for(Point* p : g->stones) {
      Point temp(p->row, p->column);
      Point* pClone = b->getPoint(&temp);
      gClone->addStone(pClone);
    }
    b->whiteGroups.push_back(gClone);
  }
  for(Group* g : emptyGroups) {
    Group* gClone = new Group(White);
    for(Point* p : g->stones) {
      Point temp(p->row, p->column);
      Point* pClone = b->getPoint(&temp);
      gClone->addStone(pClone);
    }
    b->emptyGroups.push_back(gClone);
  }

  return b;
}

/*bool Board::isValidMove(Point move) {
  Point pass(boardSize, boardSize);
  if (move == pass)  // passes are valid
    return true;
  else if (move.column >= boardSize || move.row >= boardSize
    || move.row < 0 || move.column < 0)
    return false;
  else if (positions[move.row][move.column] != Empty)
    return false;
  return true;
}*/
bool Board::isValidMove(Point move) {
  for(Point *p : possibleMoves) {
    if (*p == move)
      return true;
  }
  return false;
}

std::vector<Point*> Board::getNeighbors(Point *p) {
  std::vector<Point*> neighbors;
  if (p->north != NULL)
    neighbors.push_back(p->north);
  if (p->east != NULL)
    neighbors.push_back(p->east);
  if (p->south != NULL)
    neighbors.push_back(p->south);
  if (p->west != NULL)
    neighbors.push_back(p->west);
  return neighbors;
}

bool Board::hasPathAStar(const Point &from, const Point &to) {
  bool x = false;
  if (from == to)
    x = !x;
  if (x)
    x = !x;
  return true;
}

bool Board::hasPathDFS(Point *from, Point *to) {
  bool found = false;
  std::vector<Point*> marked;
  std::stack<Point*> s;
  s.push(from);
  while (!s.empty()) {
    Point *p = s.top();
    s.pop();

    if (positions[p->row][p->column]->marked) {
      continue;
    } else if (positions[p->row][p->column]->color == Empty) {
      positions[p->row][p->column]->marked = true;
      marked.push_back(p);
      std::vector<Point*> neighbors = getNeighbors(p);
      for (Point *p2 : neighbors) {
        if (p2 == to) {
          found = true;
          goto out;
        }
        if (positions[p2->row][p2->column]->color == Empty)
          s.push(p2);
      }
    }
  }

  out:
  for (Point* p : marked)
    p->marked = false;
  return found;
}

// Fuego
void Board::getSimpleScore(float* whiteScore, float* blackScore) {
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      if (positions[row][column]->color == White) {
        *whiteScore = *whiteScore + 1;
      } else if (positions[row][column]->color == Black) {
        *blackScore = *blackScore + 1;
      }
    }
  }
  // printf("%f %f\n", *whiteScore, *blackScore);
}

float Board::getTaylorScore(float komi) {
  float score = -komi;  // score = blackScore - whiteScore
  score += static_cast<float>(capturedWhite) -
    static_cast<float>(capturedBlack);
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      if (positions[row][column]->color == Black) {
        score++;
        continue;
      } else if (positions[row][column]->color == White) {
        score--;
        continue;
      } else if (positions[row][column]->color == Mark) {
        // Already visited empty space territory
        continue;
      }

      // Unseen empty space
      std::stack<Point*> s;
      s.push(positions[row][column]);
      bool adjacentToBlack = false;
      bool adjacentToWhite = false;
      int sizeOfEmptySpace = 0;
      while (!s.empty()) {
        Point *p = s.top();
        s.pop();
        if (p->color == Black) {
          adjacentToBlack = true;
        } else if (p->color == White) {
          adjacentToWhite = true;
        } else if (p->color == Empty) {
          if (p->north != NULL)
            s.push(p->north);
          if (p->east != NULL)
            s.push(p->east);
          if (p->south != NULL)
            s.push(p->south);
          if (p->west != NULL)
            s.push(p->west);
          sizeOfEmptySpace++;

          p->color = Mark;
        }
      }

      if (adjacentToBlack && !adjacentToWhite) {
        score += static_cast<float>(sizeOfEmptySpace);
      } else if (!adjacentToBlack && adjacentToWhite) {
        score -= static_cast<float>(sizeOfEmptySpace);
      }
    }
  }
  return score;
}

bool Board::isGameOver(GameResult *result) {
  if (lastMove == pass && secondLastMove == pass) {
    /*float whiteScore = 0;
    float blackScore = 0;
    getSimpleScore(&whiteScore, &blackScore);
    if (whiteScore > blackScore)
      *result = WhiteWon;
    else if (blackScore > whiteScore)
      *result = BlackWon;
    else
      *result = Draw;*/
    float komi = 7.5;
    Board* temp = clone();
    float score = temp->getTaylorScore(komi);
    delete temp;
    if (score > 0)
      *result = BlackWon;
    else if (score < 0)
      *result = WhiteWon;
    else
      *result = Draw;
    return true;
  }
  return false;
}

void testPossibleMoves(Board* b) {
    std::vector<Point*> oldPossibleMoves = b->possibleMoves;
    b->getPossibleMoves();
    assert(oldPossibleMoves.size() == b->possibleMoves.size());

    oldPossibleMoves = b->possibleMoves;
    b->oldGetPossibleMoves();
    if (oldPossibleMoves.size() != b->possibleMoves.size()) {
      printf("NextLastMove - %d %d\n", b->secondLastMove->row, b->secondLastMove->column);
      printf("LastMove - %d %d\n", b->lastMove->row, b->lastMove->column);
      b->show();
      assert(oldPossibleMoves.size() == b->possibleMoves.size());
    }
}

void Board::updateStructures(Point* move) {
  std::vector<Group*>* groupsToSearch =
    (turn == Black) ? &blackGroups : &whiteGroups;
  Group* updatedGroup = NULL;
  for(Group* group : *groupsToSearch) {
    // if adjacent to structure of same color, then join that structure
    if (group->isAdjacent(move)) {
      group->addStone(move);
      updatedGroup = group;
      break;
    }
  }
  if (!updatedGroup) {
    Group* group = new Group((turn == Black) ? Black : White);
    group->addStone(move);
    groupsToSearch->push_back(group);
    // updatedGroup = group;
  }

  if (updatedGroup) {
    // Find groups adjacent to updated group
    std::vector<Group*> adjacentGroups;
    for(Group* group : *groupsToSearch) {
      if (group == updatedGroup)
        continue;
      for(Point* stone : updatedGroup->stones) {
        if (group->isAdjacent(stone)) {
          bool inAdjacent = false;
          for(Group* adjacentGroup : adjacentGroups) {
            if (group == adjacentGroup) {
              inAdjacent = true;
              break;
            }
          }
          if (!inAdjacent)
            adjacentGroups.push_back(group);
        }
      }
    }

    // combines adjacent groups
    for(Group* adjacentGroup : adjacentGroups) {
      if (adjacentGroup == updatedGroup)
        continue;
      while (adjacentGroup->stones.size() > 0) {
        updatedGroup->addStone(adjacentGroup->stones.front());
        adjacentGroup->stones.erase(adjacentGroup->stones.begin());
      }
    }
    // int j = 0; j < groupsToSearch->size(); j++)
    for(Group* adjacentGroup : adjacentGroups) {
      bool found = false;
      for (std::vector<Group*>::iterator it = groupsToSearch->begin();
        it != groupsToSearch->end(); ++it) {
        if ((*it) == adjacentGroup) {
          groupsToSearch->erase(it);
          found = true;
          break;
        }
      }
      assert(found);
      delete adjacentGroup;
    }
  }
}

void Board::updateEmptyGroups(Point *move, 
  const std::vector<Point*> &capturedStones) {
   // Update empty groups
  bool found = false;
  for (unsigned int i = 0; i < emptyGroups.size(); i++) {
    for (unsigned int j = 0; j < emptyGroups[i]->stones.size(); j++) {
      if (emptyGroups[i]->stones[j] == move) {
        found = true;
        emptyGroups[i]->stones[j] = 
          emptyGroups[i]->stones[emptyGroups[i]->stones.size() - 1];
        emptyGroups[i]->stones.pop_back();
        
        if (emptyGroups[i]->stones.size() == 0) {
          delete emptyGroups[i];
          emptyGroups[i] = emptyGroups[emptyGroups.size() - 1];
          emptyGroups.pop_back();
        }
        goto out;
      }
    }
  }
  out:
  assert(found);

  // Add captured stones to a group
  if (capturedStones.size() > 0) {
    Group* g = new Group(Empty);
    for (Point *p : capturedStones) {
      g->addStone(p);
    }
    emptyGroups.push_back(g);
  }


  // flood fill adjacent sides in order to divide up a group that was
  // separated during the move
  std::vector<Point*> neighbors = getNeighbors(move);
  std::vector<Point*> emptyNeighbors;
  for (Point *p : neighbors) {
    if (positions[p->row][p->column]->color == Empty)
      emptyNeighbors.push_back(p);
  }

  bool cantFindEachOther = false;
  if (move->row + 1 < boardSize && move->column + 1 < boardSize
    && positions[move->row+1][move->column+1]->color != Empty)
    cantFindEachOther = true;
  else if (move->row + 1 < boardSize && move->column - 1 >= 0
    && positions[move->row+1][move->column-1]->color != Empty)
    cantFindEachOther = true;
  else if (move->row - 1 >= 0 && move->column + 1 < boardSize
    && positions[move->row-1][move->column+1]->color != Empty)
    cantFindEachOther = true;
  else if (move->row - 1 >= 0 && move->column - 1 >= 0
    && positions[move->row-1][move->column-1]->color != Empty)
    cantFindEachOther = true;
  else if (move->row + 1 < boardSize
    && positions[move->row+1][move->column]->color != Empty)
    cantFindEachOther = true;
  else if (move->row - 1 >= 0
    && positions[move->row-1][move->column]->color != Empty)
    cantFindEachOther = true;
  else if (move->column + 1 < boardSize
    && positions[move->row][move->column+1]->color != Empty)
    cantFindEachOther = true;
  else if (move->column - 1 >= 0
    && positions[move->row][move->column-1]->color != Empty)
    cantFindEachOther = true;
  if (cantFindEachOther) {
  // for (Point p : emptyNeighbors) {
    cantFindEachOther = false;
    for (Point *p : emptyNeighbors) {
      if (emptyNeighbors[0] == p)
        continue;
      if (!hasPathDFS(emptyNeighbors[0], p)) {
        cantFindEachOther = true;
        break;
      }
    }
  }

  int numEmptyNeighbors = static_cast<int>(emptyNeighbors.size());
  int handledEmptyNeighbors = 0;
  if (numEmptyNeighbors > 1 && cantFindEachOther) {
    while (handledEmptyNeighbors != numEmptyNeighbors) {
      Point *emptyNeighbor = emptyNeighbors[0];
      // bool foundEmptyNeighborGroup = false;
      for (unsigned int i = 0; i < emptyGroups.size(); i++) {
        if (emptyGroups[i]->contains(emptyNeighbor)) {
          // foundEmptyNeighborGroup = true;
          for (Point *p : emptyGroups[i]->stones)
            p->group = NULL;
          delete emptyGroups[i];
          emptyGroups[i] = emptyGroups[emptyGroups.size() - 1];
          emptyGroups.pop_back();
          break;
        }
      }
      // assert(foundEmptyNeighborGroup);

      Group *newEmptyGroup = new Group(Empty);

      std::stack<Point*> s;
      std::vector<Point*> marked;
      s.push(emptyNeighbor);
      while(!s.empty()) {
        Point *p = s.top();
        s.pop();

        if (positions[p->row][p->column]->marked) {
          continue;
        } else if (positions[p->row][p->column]->color == Empty) {
          positions[p->row][p->column]->marked = true;
          marked.push_back(p);

          std::vector<Point*> pNeighbors = getNeighbors(p);
          for (Point *pNeighbor : pNeighbors) {
            s.push(pNeighbor);
          }
          newEmptyGroup->addStone(p);
          for (unsigned int i = 0; i < emptyNeighbors.size(); i++) {
            if (p == emptyNeighbors[i]) {
              handledEmptyNeighbors++;
              emptyNeighbors[i] = 
                emptyNeighbors[emptyNeighbors.size() - 1];
              emptyNeighbors.pop_back();
            }
          }
        } // if copiedPositions == Mark
      }  // while (!s.empty())

      for (Point* p : marked)
        p->marked = false;

      emptyGroups.push_back(newEmptyGroup);

    }  // while (handledEmptyNeighbors != numEmptyNeighbors) {
  }  // if (numEmptyNeighbors > 1) {
}

unsigned int Board::removeDeadStones(const Player &color, Point* move) {
  std::vector<Point*> capturedStones;

  std::vector<Group*>* firstGroups =
    (color == Black) ? &blackGroups : &whiteGroups;
  // vector<Group*> secondGroup = (turn == Black) ? blackGroups : whiteGroups;

  std::vector<Group*> deadGroups;
  for(Group* group : *firstGroups)
    if (!(group->hasLiberties()))
      deadGroups.push_back(group);

  // beg1:
  for(Group* deadGroup : deadGroups) {
    // remove dead stones on board
    for(Point* stone : deadGroup->stones) {
      stone->color = Empty;
      stone->group = NULL;
      capturedStones.push_back(stone);
      // possibleMoves.push_back(Point(stone->row, stone->column));
    }

    for (std::vector<Group*>::iterator groupIterator = firstGroups->begin();
      groupIterator != firstGroups->end(); ++groupIterator) {
      if (deadGroup == (*groupIterator)) {
        firstGroups->erase(groupIterator);
        break;
      }
    }
    delete deadGroup;
    // deadGroups.erase(it);
    // goto beg1;
  }

  if (capturedStones.size() == 1) {
    for (Point* p : capturedStones) {
      if (move->isAdjacent(*p)) {
        koPoint = p;
        break;
      }
    }
  } else {
    koPoint = NULL;
  }

  updateEmptyGroups(move, capturedStones);

  std::vector<Group*> neighborGroupsToBeUpdated;
  for (Point* p : capturedStones) {
    bool found;
    if (p->north != NULL && p->north->group != NULL) {
      found = false;
      for (Group* g : neighborGroupsToBeUpdated) {
        if (g == p->north->group) {
          found = true;
          break;
        }
      }
      if (!found)
        neighborGroupsToBeUpdated.push_back(p->north->group);
    }

    if (p->east != NULL && p->east->group != NULL) {
      found = false;
      for (Group* g : neighborGroupsToBeUpdated) {
        if (g == p->east->group) {
          found = true;
          break;
        }
      }
      if (!found)
        neighborGroupsToBeUpdated.push_back(p->east->group);
    }

    if (p->south != NULL && p->south->group != NULL) {
      found = false;
      for (Group* g : neighborGroupsToBeUpdated) {
        if (g == p->south->group) {
          found = true;
          break;
        }
      }
      if (!found)
        neighborGroupsToBeUpdated.push_back(p->south->group);
    }

    if (p->west != NULL && p->west->group != NULL) {
      found = false;
      for (Group* g : neighborGroupsToBeUpdated) {
        if (g == p->west->group) {
          found = true;
          break;
        }
      }
      if (!found)
        neighborGroupsToBeUpdated.push_back(p->west->group);
    }
  }

  for (Group* g : neighborGroupsToBeUpdated)
    g->recalculateLiberties();

  return static_cast<unsigned int>(capturedStones.size());
  /*deadGroups.clear();
  for(vector<Group*>::iterator it = secondGroup.begin(); it != secondGroup.end(); ++it)
  {
    if(!(*it)->hasLiberties(positions))
    {
      deadGroups.push_back((*it));
    }
  }
  
  beg:
  for(vector<Group*>::iterator it = deadGroups.begin(); it != deadGroups.end(); ++it)
  {
    for(int i = 0; i < (*it)->stones.size(); i++)
    {
      Point* p = (*it)->stones.at(i);
      positions[p->row][p->column] = Empty;
    }
    for(vector<Group*>::iterator it2 = secondGroup.begin(); it2 != secondGroup.end(); ++it2)
    {
      if((*it) == (*it2))
      {
        secondGroup.erase(it2);
        break;
      }
    }
    delete (*it);
    goto beg;
  }*/

  /*std::vector<Point*> whiteStones;
  std::vector<Point*> blackStones;
  //find all stones
  for(int row = 0; row < BOARD_SIZE; row++)
  {
    for(int column = 0; column < BOARD_SIZE; column++)
    {
      if(positions[row][column] == White)
        whiteStones.push_back(new Point(row, column));
      else if(positions[row][column] == Black)
        blackStones.push_back(new Point(row, column));
    }
  }
  
  //find all structures
  for(int i = 0; i < whiteStones.size(); i++)
  {
    bool inStructure = false;
    for(int j = 0; j < structures.size(); j++)
    {
      Structure structure = structures.at(j);
      //if adjacent to structure of same color, then join that structure
      if(structure.color != White)
        continue;
      if(structure.isAdjacent(whiteStones.at(i)))
      {
        structure.addStone(whiteStones.at(i));
      }
    }
    if(!inStructure)
    {
      Structure structure(positions, White);
      structure.addStone(whiteStones.at(i));
      structures.push_back(structure);
    }
  }*/
}

void Board::makeMove(Point move) {
  Point* movePointer = NULL;
  for (Point* p : possibleMoves) {
    if (*p == move) {
      movePointer = p;
      break;
    }
  }
  if (movePointer == NULL)
    assert(false);
  makeMove(movePointer);
}

void Board::makeMove(Point* move) {
  // Don't place stone for passes
  if (!(*move == *pass)) {
    bool foundMove = false;
    for (unsigned int i = 0; i < possibleMoves.size(); i++) {
      if (possibleMoves[i] == move) {
        // possibleMoves[i] = possibleMoves.back();
        // possibleMoves.pop_back();
        foundMove = true;
        break;
      }
    }
    if (!foundMove) {
      printf("Illegal Move\nRow: %d\nColumn: %d\n",
        move->row, move->column);
      printf("Ko Point Row: %d Column: %d\n",
        koPoint->row, koPoint->column);
      for(Point* p : possibleMoves) {
        printf("  PossibleMove Row: %d Column: %d\n",
          p->row, p->column);
      }
      show();
      assert(foundMove);
    }


    if (move->color != Empty) {
      printf("Illegal Move\nRow: %d\nColumn: %d\nColor: %d\n",
        move->row, move->column, move->color);
      show();
      assert(false);
    }
    move->color = turn;
    std::vector<Group*> decrementedGroups;
    if (move->north != NULL && move->north->color != Empty) {
      move->north->group->numberLiberties--;
      decrementedGroups.push_back(move->north->group);
    }
    if (move->east != NULL && move->east->color != Empty) {
      bool found = false;
      for (Group* g : decrementedGroups) {
        if (g == move->east->group) {
          found = true;
        }
      }
      if (!found) {
        move->east->group->numberLiberties--;
        decrementedGroups.push_back(move->east->group);
      }
    }
    if (move->south != NULL && move->south->color != Empty) {
      bool found = false;
      for (Group* g : decrementedGroups) {
        if (g == move->south->group) {
          found = true;
        }
      }
      if (!found) {
        move->south->group->numberLiberties--;
        decrementedGroups.push_back(move->south->group);
      }
    }
    if (move->west != NULL && move->west->color != Empty) {
      bool found = false;
      for (Group* g : decrementedGroups) {
        if (g == move->west->group) {
          found = true;
        }
      }
      if (!found) {
        move->west->group->numberLiberties--;
        decrementedGroups.push_back(move->west->group);
      }
    }

    /*for (Group* g : blackGroups) {
      g->recalculateLiberties();
    }
    for (Group* g : whiteGroups) {
      g->recalculateLiberties();
    }*/
    updateStructures(move);
    int numCaptured =
        removeDeadStones((turn == Black ? White : Black), move);
    if (turn == Black)
      capturedWhite += numCaptured;
    else
      capturedBlack += numCaptured;



    // removeDeadStones((turn == Black ? Black : White));

    // TODO: replace with getNeighbors
    std::vector<Point*> neighbors;
    if (move->north != NULL)
      neighbors.push_back(move->north);
    if (move->east != NULL)
      neighbors.push_back(move->east);
    if (move->south != NULL)
      neighbors.push_back(move->south);
    if (move->west != NULL)
      neighbors.push_back(move->west);

    bool neighborOfSameColor = false;
    for (Point *neighbor : neighbors) {
      if (neighbor->color == move->color) {
      neighborOfSameColor = true;
      break;
      }
    }

    // if the move has a neighbor of the same color,
    // then the next move cannot be ko
    if (neighborOfSameColor) {
      koPoint = NULL;
    }
  } else {
    koPoint = NULL;
  }

  // cout << whiteGroups.size() << " " << blackGroups.size() << "\n";

  turn = (turn == Black ? White : Black);

  secondLastMove = lastMove;
  lastMove = move;

  getPossibleMoves();
  #ifdef RUNNING_TESTS
    // testPossibleMoves(this);
  #endif
}

void Board::makeRandomMove() {
  static unsigned int seed = static_cast<unsigned int>(time(NULL));
  unsigned int choice = rand_r(&seed) %
    static_cast<unsigned int>(possibleMoves.size());
  Point *chosenMove = possibleMoves[choice];
  makeMove(chosenMove);
}

GameResult Board::playRandomGame() {
  GameResult result;
  while (!isGameOver(&result)) {
    makeRandomMove();
  }
  return result;
}

void Board::show() {
  std::stringstream boardString;

  for (int row = 0; row < boardSize+1; row++) {
    for (int column = 0; column < boardSize+1; column++) {
      if (row == boardSize && column == boardSize) {
        boardString << "  ";
      } else if (row == boardSize) {
        boardString << column+1 << " ";
      } else if (column == boardSize) {
        boardString << static_cast<char>(row+'a' + (row > 7 ? 1 : 0))
          << " ";
      } else {
        switch (positions[row][column]->color) {
        case 0:
          boardString << "- ";
          break;
        case White:
          boardString << "W ";
          break;
        case Black:
          boardString << "B ";
          break;
        case Mark:
          boardString << "M ";
          break;
        default:
          assert(false);
        }
      }
    }

    boardString << "\n";
  }
  boardString << "\n";
  if (lastMove == pass)
    boardString << ((turn == Black) ? "White" : "Black")
      << " passed\n";
  Board *bClone = this->clone();
  float komi = 7.5;
  float score = bClone->getTaylorScore(komi);
  delete bClone;
  printf("Score: %f\n", score);
  printf("%s", boardString.str().c_str());
}

bool Board::isSuicide(Point move) {
  return isSuicide(getPoint(&move));
}

// http://en.wikibooks.org/wiki/Computer_Go/Recognizing_Illegal_Moves
bool Board::isSuicide(Point* move) {
  std::vector<Point*> neighbors;
  if (move->north != NULL)
    neighbors.push_back(move->north);
  if (move->east != NULL)
    neighbors.push_back(move->east);
  if (move->south != NULL)
    neighbors.push_back(move->south);
  if (move->west != NULL)
    neighbors.push_back(move->west);

  // Check for empty neighbors
  for(Point *p : neighbors) {
    if (p->color == Empty)
      return false;
  }

  // Check for neighbors of same color with more than one liberty
  for(Point *p : neighbors) {
    std::vector<Group*> groupsOfSameColor = turn == Black ?
      blackGroups : whiteGroups;

    for(Group* g : groupsOfSameColor) {
      if (!g->contains(p))
        continue;
      if (g->numLiberties() > 1)
        return false;
    }
  }

  // Check for neighbors of opposite color with only one liberty
  for(Point *p : neighbors) {
    std::vector<Group*> groupsOfOppositeColor = turn == Black ?
      whiteGroups : blackGroups;

    for(Group* g : groupsOfOppositeColor) {
      if (!g->contains(p))
        continue;
      if (g->numLiberties() == 1)
        return false;
    }
  }

  return true;
}

void printPossibleMoves(Board* b) {
  printf("Possible Moves: \n");
  for(Point* p : b->possibleMoves) {
    printf("  R: %d C: %d\n", p->row, p->column);
  }
}

void Board::getPossibleMoves() {
  if (possibleMoves.size() > 0)
    possibleMoves.clear();

  for (Group* g : emptyGroups) {
    if (g->stones[0] != koPoint && !isSuicide((*g->stones[0]))) {
      for (Point* p : g->stones) {
        possibleMoves.push_back(p);
      }
    }
  }

  possibleMoves.push_back(pass);
}

void Board::oldGetPossibleMoves() {
  if (possibleMoves.size() > 0)
    possibleMoves.clear();

  for (int r = 0; r < boardSize; r++)
    for (int c = 0; c < boardSize; c++)
      if (positions[r][c]->color == Empty
        && positions[r][c] != koPoint
        && !isSuicide(positions[r][c]))
        possibleMoves.push_back(positions[r][c]);
  possibleMoves.push_back(pass);
}

Point* Board::getPoint(Point *p) {
  Point* point = NULL;
  if (p == NULL)
    return NULL;
  if (*p == *pass)
    point = pass;
  else {
    for (int row = 0; row < boardSize; row++) {
      for (int column = 0; column < boardSize; column++) {
        if (*positions[row][column] == *p) {
          point = positions[row][column];
          break;
        }
      }
    }
  }

  if (point == NULL)
    assert(false);
  return point;
}
