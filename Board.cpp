#include "Board.h"

#include <assert.h>
#include <boost/foreach.hpp>

#include <cstdio>
#include <sstream>
#include <vector>
#include <stack>

#define foreach BOOST_FOREACH

Board::Board(int newBoardSize) :
  boardSize(newBoardSize),
  positions(new Point**[newBoardSize]),
  blackGroups(std::vector<Group*>()),
  whiteGroups(std::vector<Group*>()),
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
    // memset(positions[row], 0, boardSize*sizeof(Point*));
    for (int column = 0; column < boardSize; column++) {
      positions[row][column] =
        new Point(row, column, Empty, NULL, NULL, NULL, NULL);
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

  // memset(&positions, 0, sizeof(int)*boardSize*boardSize);
  // TODO(GaryChristiansen): this is being thrown away for clones
  getPossibleMoves();
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

  foreach(Group* g, b.blackGroups)
  {
    Group* gClone = new Group(Black);
    foreach(Point* p, g->stones)
    {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    blackGroups.push_back(gClone);
  }
  foreach(Group* g, b.whiteGroups)
  {
    Group* gClone = new Group(White);
    foreach(Point* p, g->stones)
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

  foreach(Group* g, b.blackGroups)
  {
    Group* gClone = new Group(Black);
    foreach(Point* p, g->stones)
    {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    blackGroups.push_back(gClone);
  }
  foreach(Group* g, b.whiteGroups)
  {
    Group* gClone = new Group(White);
    foreach(Point* p, g->stones)
    {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    whiteGroups.push_back(gClone);
  }
  return *this;
}*/

Board::~Board() {
  /*
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      delete positions[row][column];
    }
    delete[] positions[row];
  }
  delete[] positions;

  foreach(Group* g, blackGroups)
    delete g;
  foreach(Group* g, whiteGroups)
    delete g;*/
}

bool Board::operator==(const Board &b) {
  if (boardSize != b.boardSize)
    return false;
  for (int row = 0; row < boardSize; row++)
    for (int column = 0; column < boardSize; column++)
      if (*positions[row][column] != *b.positions[row][column])
        return false;
  if (!(*lastMove == *b.lastMove))
    return false;
  if (!(*secondLastMove == *b.secondLastMove))
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
        NULL, NULL, NULL, NULL);
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

  foreach(Group* g, blackGroups) {
    Group* gClone = new Group(Black);
    foreach(Point* p, g->stones) {
      Point* pClone = new Point(p->row, p->column, p->color, p->north,
        p->east, p->south, p->west);
      gClone->addStone(pClone);
    }
    b->blackGroups.push_back(gClone);
  }
  foreach(Group* g, whiteGroups) {
    Group* gClone = new Group(White);
    foreach(Point* p, g->stones) {
      Point* pClone = new Point(p->row, p->column, p->color, p->north,
        p->east, p->south, p->west);
      gClone->addStone(pClone);
    }
    b->whiteGroups.push_back(gClone);
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
  foreach(Point *p, possibleMoves) {
    if (*p == move)
      return true;
  }
  return false;
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

void Board::updateStructures(Point* move) {
  std::vector<Group*>* groupsToSearch =
    (turn == Black) ? &blackGroups : &whiteGroups;
  Group* updatedGroup = NULL;
  foreach(Group* group, *groupsToSearch) {
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
    foreach(Group* group, *groupsToSearch) {
      if (group == updatedGroup)
        continue;
      foreach(Point* stone, updatedGroup->stones) {
        if (group->isAdjacent(stone)) {
          bool inAdjacent = false;
          foreach(Group* adjacentGroup, adjacentGroups) {
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
    foreach(Group* adjacentGroup, adjacentGroups) {
      if (adjacentGroup == updatedGroup)
        continue;
      while (adjacentGroup->stones.size() > 0) {
        updatedGroup->stones.push_back(adjacentGroup->stones.front());
        adjacentGroup->stones.erase(adjacentGroup->stones.begin());
      }
    }
    // int j = 0; j < groupsToSearch->size(); j++)
    foreach(Group* adjacentGroup, adjacentGroups) {
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

unsigned int Board::removeDeadStones(Player color) {
  std::vector<Point*> capturedStones;

  std::vector<Group*>* firstGroups =
    (color == Black) ? &blackGroups : &whiteGroups;
  // vector<Group*> secondGroup = (turn == Black) ? blackGroups : whiteGroups;

  std::vector<Group*> deadGroups;
  foreach(Group* group, *firstGroups)
    if (!(group->hasLiberties()))
      deadGroups.push_back(group);

  // beg1:
  foreach(Group* deadGroup, deadGroups) {
    // remove dead stones on board
    foreach(Point* stone, deadGroup->stones) {
      stone->color = Empty;
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

  if (capturedStones.size() == 1)
    koPoint = capturedStones[0];
  else
    koPoint = NULL;

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

void testPossibleMoves(Board* b) {
  std::vector<Point*> oldPossibleMoves = b->possibleMoves;
  b->getPossibleMoves();
  if (oldPossibleMoves.size() != b->possibleMoves.size()) {
    printf("%d %d\n",
      (int)oldPossibleMoves.size(), (int)b->possibleMoves.size());
    for (Point* p : oldPossibleMoves) {
      printf("  %d %d\n", p->row, p->column);
    }
    printf("%d %d\n",
      (int)oldPossibleMoves.size(), (int)b->possibleMoves.size());
    for (Point* p : b->possibleMoves) {
      printf("  %d %d\n", p->row, p->column);
    }
    b->show();
    printf("  %d %d\n", b->secondLastMove->row, b->secondLastMove->column);
    printf("  %d %d\n", b->lastMove->row, b->lastMove->column);
    printf("  %d %d\n", b->koPoint->row, b->koPoint->column);
    assert(oldPossibleMoves.size() == b->possibleMoves.size());
  }
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
      printf("Illegal Move\nRow: %d\nColumn: %d\n", move->row, move->column);
      foreach(Point* p, possibleMoves) {
        printf("  PossibleMove Row: %d Column: %d\n", p->row, p->column);
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

    updateStructures(move);
    int numCaptured = removeDeadStones((turn == Black ? White : Black));
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

    // if the move has a neighbor of the same color, then the next move cannot be ko
    if (neighborOfSameColor) {
      koPoint = NULL;
    }
  }
  else {
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
        boardString << column << " ";
      } else if (column == boardSize) {
        boardString << row << " ";
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
  foreach(Point *p, neighbors) {
    if (p->color == Empty)
      return false;
  }

  // Check for neighbors of same color with more than one liberty
  foreach(Point *p, neighbors) {
    std::vector<Group*> groupsOfSameColor = turn == Black ?
      blackGroups : whiteGroups;

    foreach(Group* g, groupsOfSameColor) {
      if (!g->contains(p))
        continue;
      if (g->numLiberties() > 1)
        return false;
    }
  }

  // Check for neighbors of opposite color with only one liberty
  foreach(Point *p, neighbors) {
    std::vector<Group*> groupsOfOppositeColor = turn == Black ?
      whiteGroups : blackGroups;

    foreach(Group* g, groupsOfOppositeColor) {
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
  foreach(Point* p, b->possibleMoves) {
    printf("  R: %d C: %d\n", p->row, p->column);
  }
}

void Board::getPossibleMoves() {
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
