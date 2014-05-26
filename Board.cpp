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
  positions(new int*[newBoardSize]),
  blackGroups(std::vector<Group*>()),
  whiteGroups(std::vector<Group*>()),
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
  for (int r = 0; r < boardSize; r++)
    delete[] positions[r];
  delete[] positions;

  foreach(Group* g, blackGroups)
    delete g;
  foreach(Group* g, whiteGroups)
    delete g;
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

  foreach(Group* g, blackGroups) {
    Group* gClone = new Group(Black);
    foreach(Point* p, g->stones) {
      Point* pClone = new Point(p->row, p->column);
      gClone->addStone(pClone);
    }
    b->blackGroups.push_back(gClone);
  }
  foreach(Group* g, whiteGroups) {
    Group* gClone = new Group(White);
    foreach(Point* p, g->stones) {
      Point* pClone = new Point(p->row, p->column);
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
  foreach(Point p, possibleMoves) {
    if (p == move)
      return true;
  }
  return false;
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

void Board::updateStructures(Point move) {
  std::vector<Group*>* groupsToSearch =
    (turn == Black) ? &blackGroups : &whiteGroups;
  Group* updatedGroup = NULL;
  foreach(Group* group, *groupsToSearch) {
    // if adjacent to structure of same color, then join that structure
    if (group->isAdjacent(&move)) {
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

unsigned int Board::removeDeadStones(Player color, Point move) {
  std::vector<Point> capturedStones;

  std::vector<Group*>* firstGroups =
    (color == Black) ? &blackGroups : &whiteGroups;
  // vector<Group*> secondGroup = (turn == Black) ? blackGroups : whiteGroups;

  std::vector<Group*> deadGroups;
  foreach(Group* group, *firstGroups)
    if (!(group->hasLiberties(this)))
      deadGroups.push_back(group);

  // beg1:
  foreach(Group* deadGroup, deadGroups) {
    // remove dead stones on board
    foreach(Point* stone, deadGroup->stones) {
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

  if (capturedStones.size() > 0) {
    for (Point p : capturedStones) {
      if (move.isAdjacent(p)) {
        koPoint = p;
	break;
      }
    }
  } else {
    koPoint = Point(-1, -1);
  }

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
    std::vector<Point> oldPossibleMoves = b->possibleMoves;
    b->getPossibleMoves();
    assert(oldPossibleMoves.size() == b->possibleMoves.size());
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
      foreach(Point p, possibleMoves) {
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

    // TODO: replace with getNeighbors
    std::vector<Point> neighbors;
    if (move.row - 1 >= 0)
      neighbors.push_back(Point(move.row - 1, move.column));
    if (move.row + 1 < boardSize)
      neighbors.push_back(Point(move.row + 1, move.column));
    if (move.column - 1 >= 0)
      neighbors.push_back(Point(move.row, move.column - 1));
    if (move.column + 1 < boardSize)
      neighbors.push_back(Point(move.row, move.column + 1));

    bool neighborOfSameColor = false;
    for (Point neighbor : neighbors) {
      if (positions[neighbor.row][neighbor.column] == positions[move.row][move.column]) {
      neighborOfSameColor = true;
      break;
      }
    }

    // if the move has a neighbor of the same color, then the next move cannot be ko
    if (neighborOfSameColor) {
      koPoint = Point(-1, -1);
    }
  }
  else {
    koPoint = Point(-1, -1);
  }

  // cout << whiteGroups.size() << " " << blackGroups.size() << "\n";

  turn = (turn == Black ? White : Black);

  secondLastMove = lastMove;
  lastMove = move;

  getPossibleMoves();
  #ifdef RUNNING_TESTS
    //testPossibleMoves(this);
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
        boardString << column << " ";
      } else if (column == boardSize) {
        boardString << row << " ";
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
bool Board::isSuicide(Point move) {
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
  foreach(Point p, neighbors) {
    if (positions[p.row][p.column] == Empty)
      return false;
  }

  // Check for neighbors of same color with more than one liberty
  foreach(Point p, neighbors) {
    std::vector<Group*> groupsOfSameColor = turn == Black ?
      blackGroups : whiteGroups;

    foreach(Group* g, groupsOfSameColor) {
      if (!g->contains(&p))
        continue;
      if (g->numLiberties(this) > 1)
        return false;
    }
  }

  // Check for neighbors of opposite color with only one liberty
  foreach(Point p, neighbors) {
    std::vector<Group*> groupsOfOppositeColor = turn == Black ?
      whiteGroups : blackGroups;

    foreach(Group* g, groupsOfOppositeColor) {
      if (!g->contains(&p))
        continue;
      if (g->numLiberties(this) == 1)
        return false;
    }
  }

  return true;
}

void printPossibleMoves(Board* b) {
  printf("Possible Moves: \n");
  foreach(Point p, b->possibleMoves) {
    printf("  R: %d C: %d\n", p.row, p.column);
  }
}

void Board::getPossibleMoves() {
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
