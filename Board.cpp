#include "Board.h"

#include <assert.h>

#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>
#include <stack>

Board::Board(int newBoardSize) :
  boardSize(newBoardSize),
  positions(new int*[newBoardSize]),
  blackGroups(std::vector<Group*>()),
  whiteGroups(std::vector<Group*>()),
  emptyGroups(std::vector<Group*>()),
  turn(Black),
  lastMove(Point(-1, -1)),
  secondLastMove(Point(-1, -1)),
  koPoint(Point(-1, -1)),
  capturedBlack(0),
  capturedWhite(0),
  possibleMoves(std::vector<Point>()) {
  for (int r = 0; r < boardSize; r++) {
    positions[r] = new int[boardSize];
    memset(positions[r], 0, boardSize*sizeof(int));
  }
  // memset(&positions, 0, sizeof(int)*boardSize*boardSize);
  // TODO(GaryChristiansen): this is being thrown away for clones
  // getPossibleMoves();
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
  for (int r = 0; r < boardSize; r++)
    delete[] positions[r];
  delete[] positions;

  for(Group* g : blackGroups)
    delete g;
  for(Group* g : whiteGroups)
    delete g;
  for(Group* g : emptyGroups)
    delete g;
}

// Should only be called when creating a new board, not a clone
void Board::init() {
  // Fill emptyGroups with correct data
  Group* g = new Group(Empty);
  for (int row = 0; row < boardSize; row++)
    for (int column = 0; column < boardSize; column++)
      g->stones.push_back(new Point(row, column));
  emptyGroups.push_back(g);

  getPossibleMoves();
}

bool Board::operator==(const Board &b) {
  if (boardSize != b.boardSize)
    return false;
  for (int row = 0; row < boardSize; row++)
    for (int column = 0; column < boardSize; column++)
      if (positions[row][column] != b.positions[row][column])
        return false;
  if (!(lastMove == b.lastMove))
    return false;
  if (!(secondLastMove == b.secondLastMove))
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
  // b->positions = new int*[BOARD_SIZE];
  for (int r = 0; r < boardSize; r++) {
    // b->positions[r] = new int[BOARD_SIZE];
    memcpy(b->positions[r], positions[r], boardSize*sizeof(int));
  }

  b->possibleMoves.clear();
  for (unsigned int i = 0; i < possibleMoves.size(); i++)
    b->possibleMoves.push_back(possibleMoves[i]);

  b->turn = turn;

  b->lastMove = lastMove;
  b->secondLastMove = secondLastMove;

  b->koPoint = koPoint;

  b->capturedBlack = capturedBlack;
  b->capturedWhite = capturedWhite;

  for(Group* g : blackGroups) {
    Group* gClone = new Group(Black);
    for(Point* p : g->stones) {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    b->blackGroups.push_back(gClone);
  }
  for(Group* g : whiteGroups) {
    Group* gClone = new Group(White);
    for(Point* p : g->stones) {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    b->whiteGroups.push_back(gClone);
  }
  for(Group* g : emptyGroups) {
    Group* gClone = new Group(White);
    for(Point* p : g->stones) {
      Point* pClone = new Point(p->row, p->column);
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
bool Board::isValidMove(const Point &move) {
  for(Point p : possibleMoves) {
    if (p == move)
      return true;
  }
  return false;
}

std::vector<Point> Board::getNeighbors(const Point &p) {
  std::vector<Point> neighbors;
  if (p.row - 1 >= 0)
    neighbors.push_back(Point(p.row - 1, p.column));
  if (p.row + 1 < boardSize)
    neighbors.push_back(Point(p.row + 1, p.column));
  if (p.column - 1 >= 0)
    neighbors.push_back(Point(p.row, p.column - 1));
  if (p.column + 1 < boardSize)
    neighbors.push_back(Point(p.row, p.column + 1));
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

bool Board::hasPathDFS(const Point &from, const Point &to) {
  int** copiedPositions = new int*[boardSize];
  for (int i = 0; i < boardSize; i++) {
    copiedPositions[i] = new int[boardSize];
    memcpy(copiedPositions[i], positions[i], boardSize*sizeof(int));
  }

  bool found = false;
  std::stack<Point> s;
  s.push(from);
  while (!s.empty()) {
    Point p = s.top();
    s.pop();

    if (copiedPositions[p.row][p.column] == Empty) {
      copiedPositions[p.row][p.column] = Mark;
      std::vector<Point> neighbors = getNeighbors(p);
      for (Point p2 : neighbors) {
        if (p2 == to) {
          found = true;
          goto out;
        }
        if (positions[p2.row][p2.column] == Empty)
          s.push(p2);
      }
    } else if (copiedPositions[p.row][p.column] != Mark) {
      continue;
    }
  }

  out:

  for (int i = 0; i < boardSize; i++)
    delete[] copiedPositions[i];
  delete[] copiedPositions;
  return found;
}

// Fuego
void Board::getSimpleScore(float* whiteScore, float* blackScore) {
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      if (positions[row][column] == White) {
        *whiteScore = *whiteScore + 1;
      } else if (positions[row][column] == Black) {
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
  for (int x = 0; x < boardSize; x++) {
    for (int y = 0; y < boardSize; y++) {
      if (positions[x][y] == Black) {
        score++;
        continue;
      } else if (positions[x][y] == White) {
        score--;
        continue;
      } else if (positions[x][y] == Mark) {
        // Already visited empty space territory
        continue;
      }

      // Unseen empty space
      std::stack<Point> s;
      s.push(Point(x, y));
      bool adjacentToBlack = false;
      bool adjacentToWhite = false;
      int sizeOfEmptySpace = 0;
      while (!s.empty()) {
        Point p = s.top();
        s.pop();
        if (p.row >= boardSize || p.row < 0
          || p.column >= boardSize || p.column < 0)
          continue;
        if (positions[p.row][p.column] == Black) {
          adjacentToBlack = true;
        } else if (positions[p.row][p.column] == White) {
          adjacentToWhite = true;
        } else if (positions[p.row][p.column] == Empty) {
          s.push(Point(p.row+1, p.column));
          s.push(Point(p.row-1, p.column));
          s.push(Point(p.row, p.column+1));
          s.push(Point(p.row, p.column-1));
          sizeOfEmptySpace++;

          positions[p.row][p.column] = Mark;
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
  Point pass(boardSize, boardSize);
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
    std::vector<Point> oldPossibleMoves = b->possibleMoves;
    b->getPossibleMoves();
    assert(oldPossibleMoves.size() == b->possibleMoves.size());

    oldPossibleMoves = b->possibleMoves;
    b->oldGetPossibleMoves();
    if (oldPossibleMoves.size() != b->possibleMoves.size()) {
      printf("NextLastMove - %d %d\n", b->secondLastMove.row, b->secondLastMove.column);
      printf("LastMove - %d %d\n", b->lastMove.row, b->lastMove.column);
      b->show();
      assert(oldPossibleMoves.size() == b->possibleMoves.size());
    }
}

void Board::updateStructures(const Point &move) {
  std::vector<Group*>* groupsToSearch =
    (turn == Black) ? &blackGroups : &whiteGroups;
  Group* updatedGroup = NULL;
  for(Group* group : *groupsToSearch) {
    // if adjacent to structure of same color, then join that structure
    if (group->isAdjacent(move)) {
      group->addStone(new Point(move.row, move.column));
      updatedGroup = group;
      break;
    }
  }
  if (!updatedGroup) {
    Group* group = new Group((turn == Black) ? Black : White);
    group->addStone(new Point(move.row, move.column));
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
        if (group->isAdjacent(*stone)) {
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
        updatedGroup->stones.push_back(adjacentGroup->stones.front());
        adjacentGroup->stones.erase(adjacentGroup->stones.begin());
      }
    }
    // int j = 0; j < groupsToSearch->size(); j++)
    for(Group* adjacentGroup : adjacentGroups) {
      for (std::vector<Group*>::iterator it = groupsToSearch->begin();
        it != groupsToSearch->end(); ++it) {
        if ((*it) == adjacentGroup) {
          groupsToSearch->erase(it);
          break;
        }
      }
      delete adjacentGroup;
    }
  }
}

void Board::updateEmptyGroups(const Point &move, 
  const std::vector<Point> &capturedStones) {
   // Update empty groups
  bool found = false;
  for (unsigned int i = 0; i < emptyGroups.size(); i++) {
    for (unsigned int j = 0; j < emptyGroups[i]->stones.size(); j++) {
      if ((*emptyGroups[i]->stones[j]) == move) {
        found = true;
        delete emptyGroups[i]->stones[j];
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
    for (Point p : capturedStones) {
      g->stones.push_back(new Point(p.row, p.column));
    }
    emptyGroups.push_back(g);
  }


  // flood fill adjacent sides in order to divide up a group that was
  // separated during the move
  std::vector<Point> neighbors = getNeighbors(move);
  std::vector<Point> emptyNeighbors;
  for (Point p : neighbors) {
    if (positions[p.row][p.column] == Empty)
      emptyNeighbors.push_back(p);
  }

  bool cantFindEachOther = false;
  if (move.row + 1 < boardSize && move.column + 1 < boardSize
    && positions[move.row+1][move.column+1] != Empty)
    cantFindEachOther = true;
  else if (move.row + 1 < boardSize && move.column - 1 >= 0
    && positions[move.row+1][move.column-1] != Empty)
    cantFindEachOther = true;
  else if (move.row - 1 >= 0 && move.column + 1 < boardSize
    && positions[move.row-1][move.column+1] != Empty)
    cantFindEachOther = true;
  else if (move.row - 1 >= 0 && move.column - 1 >= 0
    && positions[move.row-1][move.column-1] != Empty)
    cantFindEachOther = true;
  else if (move.row + 1 < boardSize
    && positions[move.row+1][move.column] != Empty)
    cantFindEachOther = true;
  else if (move.row - 1 >= 0
    && positions[move.row-1][move.column] != Empty)
    cantFindEachOther = true;
  else if (move.column + 1 < boardSize
    && positions[move.row][move.column+1] != Empty)
    cantFindEachOther = true;
  else if (move.column - 1 >= 0
    && positions[move.row][move.column-1] != Empty)
    cantFindEachOther = true;
  if (cantFindEachOther) {
  // for (Point p : emptyNeighbors) {
    cantFindEachOther = false;
    for (Point p : emptyNeighbors) {
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
      Point emptyNeighbor = emptyNeighbors[0];
      int** copiedPositions = new int*[boardSize];
      for (int i = 0; i < boardSize; i++) {
        copiedPositions[i] = new int[boardSize];
        memcpy(copiedPositions[i], positions[i], boardSize*sizeof(int));
      }

      Group *newEmptyGroup = new Group(Empty);

      std::stack<Point> s;
      s.push(emptyNeighbor);
      while(!s.empty()) {
        Point p = s.top();
        s.pop();

        if (copiedPositions[p.row][p.column] != Empty) {
          continue;
        } else if (copiedPositions[p.row][p.column] == Empty) {
          copiedPositions[p.row][p.column] = Mark;

          std::vector<Point> pNeighbors = getNeighbors(p);
          for (Point pNeighbor : pNeighbors) {
            s.push(pNeighbor);
          }
          newEmptyGroup->addStone(new Point(p.row, p.column));
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

      // bool foundEmptyNeighborGroup = false;
      for (unsigned int i = 0; i < emptyGroups.size(); i++) {
        if (emptyGroups[i]->contains(emptyNeighbor)) {
          // foundEmptyNeighborGroup = true;
          delete emptyGroups[i];
          emptyGroups[i] = emptyGroups[emptyGroups.size() - 1];
          emptyGroups.pop_back();
          break;
        }
      }
      // assert(foundEmptyNeighborGroup);
      emptyGroups.push_back(newEmptyGroup);

      for (int i = 0; i < boardSize; i++) {
        delete[] copiedPositions[i];
      }
      delete[] copiedPositions;
    }  // while (handledEmptyNeighbors != numEmptyNeighbors) {
  }  // if (numEmptyNeighbors > 1) {
}

unsigned int Board::removeDeadStones(const Player &color, 
  const Point &move) {
  std::vector<Point> capturedStones;

  std::vector<Group*>* firstGroups =
    (color == Black) ? &blackGroups : &whiteGroups;
  // vector<Group*> secondGroup = (turn == Black) ? blackGroups : whiteGroups;

  std::vector<Group*> deadGroups;
  for(Group* group : *firstGroups)
    if (!(group->hasLiberties(*this)))
      deadGroups.push_back(group);

  // beg1:
  for(Group* deadGroup : deadGroups) {
    // remove dead stones on board
    for(Point* stone : deadGroup->stones) {
      positions[stone->row][stone->column] = Empty;
      capturedStones.push_back(Point(stone->row, stone->column));
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
    for (Point p : capturedStones) {
      if (move.isAdjacent(p)) {
        koPoint = p;
        break;
      }
    }
  } else {
    koPoint = Point(-1, -1);
  }

  updateEmptyGroups(move, capturedStones);

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
  // Don't place stone for passes
  Point pass(boardSize, boardSize);
  if (!(move == pass)) {
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
      printf("Illegal Move\nRow: %d\nColumn: %d\n", move.row, move.column);
      printf("Ko Point Row: %d Column: %d\n", koPoint.row, koPoint.column);
      for(Point p : possibleMoves) {
        printf("  PossibleMove Row: %d Column: %d\n", p.row, p.column);
      }
      show();
      assert(foundMove);
    }


    if (positions[move.row][move.column] != Empty) {
      printf("Illegal Move\nRow: %d\nColumn: %d\n", move.row, move.column);
      show();
      assert(false);
    }
    positions[move.row][move.column] = turn;

    updateStructures(move);
    int numCaptured = removeDeadStones((turn == Black ? White : Black), move);
    if (turn == Black)
      capturedWhite += numCaptured;
    else
      capturedBlack += numCaptured;



    // removeDeadStones((turn == Black ? Black : White));

    // TODO(Gary): replace with getNeighbors
    std::vector<Point> neighbors = getNeighbors(move);
    bool neighborOfSameColor = false;
    for (Point neighbor : neighbors) {
      if (positions[neighbor.row][neighbor.column]
        == positions[move.row][move.column]) {
      neighborOfSameColor = true;
      break;
      }
    }

    // if the move has a neighbor of the same color,
    // then the next move cannot be ko
    if (neighborOfSameColor) {
      koPoint = Point(-1, -1);
    }
  } else {
    koPoint = Point(-1, -1);
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
  Point chosenMove = possibleMoves[choice];
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
        switch (positions[row][column]) {
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
  Point pass(boardSize, boardSize);
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

// http://en.wikibooks.org/wiki/Computer_Go/Recognizing_Illegal_Moves
bool Board::isSuicide(const Point &move) {
  std::vector<Point> neighbors;
  if (move.row > 0)
    neighbors.push_back(Point(move.row - 1, move.column));
  if (move.row < boardSize - 1)
    neighbors.push_back(Point(move.row + 1, move.column));
  if (move.column > 0)
    neighbors.push_back(Point(move.row, move.column - 1));
  if (move.column < boardSize - 1)
    neighbors.push_back(Point(move.row, move.column + 1));

  // Check for empty neighbors
  for(Point p : neighbors) {
    if (positions[p.row][p.column] == Empty)
      return false;
  }

  // Check for neighbors of same color with more than one liberty
  for(Point p : neighbors) {
    std::vector<Group*> groupsOfSameColor = turn == Black ?
      blackGroups : whiteGroups;

    for(Group* g : groupsOfSameColor) {
      if (!g->contains(p))
        continue;
      if (g->numLiberties(*this) > 1)
        return false;
    }
  }

  // Check for neighbors of opposite color with only one liberty
  for(Point p : neighbors) {
    std::vector<Group*> groupsOfOppositeColor = turn == Black ?
      whiteGroups : blackGroups;

    for(Group* g : groupsOfOppositeColor) {
      if (!g->contains(p))
        continue;
      if (g->numLiberties(*this) == 1)
        return false;
    }
  }

  return true;
}

void printPossibleMoves(Board* b) {
  printf("Possible Moves: \n");
  for(Point p : b->possibleMoves) {
    printf("  R: %d C: %d\n", p.row, p.column);
  }
}

void Board::getPossibleMoves() {
  if (possibleMoves.size() > 0)
    possibleMoves.clear();

  for (Group* g : emptyGroups) {
    if ((*g->stones[0]) != koPoint && !isSuicide((*g->stones[0]))) {
      for (Point* p : g->stones) {
        possibleMoves.push_back(Point(p->row, p->column));
      }
    }
  }

  possibleMoves.push_back(Point(boardSize, boardSize));
}

void Board::oldGetPossibleMoves() {
  if (possibleMoves.size() > 0)
    possibleMoves.clear();

  for (int r = 0; r < boardSize; r++)
    for (int c = 0; c < boardSize; c++)
      if (positions[r][c] == Empty
        && Point(r, c) != koPoint
        && !isSuicide(Point(r, c)))
        possibleMoves.push_back(Point(r, c));
  possibleMoves.push_back(Point(boardSize, boardSize));
}
