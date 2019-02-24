#include "Board.h"

#include <assert.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>
#include <list>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

Board::Board() :
  emptySpaces(std::vector<Point*>()),
  blackGroups(std::vector<Group*>()),
  whiteGroups(std::vector<Group*>()),
  lastMove(nullptr),
  secondLastMove(nullptr),
  koPoint(nullptr),
  pass(new Point(BOARD_SIZE, BOARD_SIZE)),
  outOfBoundsGroup(new Group(OutOfBounds)),
  outOfBoundsPoint(new Point(-1, -1, OutOfBounds, outOfBoundsGroup,
    nullptr, nullptr, nullptr, nullptr)),
  turn(Black) {
    outOfBoundsPoint->north = outOfBoundsPoint;
    outOfBoundsPoint->east = outOfBoundsPoint;
    outOfBoundsPoint->south = outOfBoundsPoint;
    outOfBoundsPoint->west = outOfBoundsPoint;
}

Board::~Board() {
  for (Group* g : blackGroups) {
    delete g;
  }
  for (Group* g : whiteGroups) {
    delete g;
  }

  if (pass != nullptr) {
    delete pass;
  }
  if (outOfBoundsGroup != nullptr) {
    delete outOfBoundsGroup;
  }
  if (outOfBoundsPoint != nullptr) {
    delete outOfBoundsPoint;
  }
}

void Board::init() {
  emptySpaces.reserve(BOARD_SIZE*BOARD_SIZE+1);
  for (unsigned char row = 0; row < BOARD_SIZE; ++row) {
    for (unsigned char column = 0; column < BOARD_SIZE; ++column) {
      positions[row*BOARD_SIZE + column].row = row;
      positions[row*BOARD_SIZE + column].column = column;

      emptySpaces.push_back(&positions[row*BOARD_SIZE + column]);
    }
  }
  emptySpaces.push_back(pass);

  for (unsigned char row = 0; row < BOARD_SIZE; ++row) {
    for (unsigned char column = 0; column < BOARD_SIZE; ++column) {
      if (row + 1 < BOARD_SIZE) {
        positions[row*BOARD_SIZE + column].north = &positions[(row+1)*BOARD_SIZE + column];
      } else {
        positions[row*BOARD_SIZE + column].north = outOfBoundsPoint;
      }
      if (column + 1 < BOARD_SIZE) {
        positions[row*BOARD_SIZE + column].east = &positions[row*BOARD_SIZE + column+1];
      } else {
        positions[row*BOARD_SIZE + column].east = outOfBoundsPoint;
      }
      if (row - 1 >= 0) {
        positions[row*BOARD_SIZE + column].south = &positions[(row-1)*BOARD_SIZE + column];
      } else {
        positions[row*BOARD_SIZE + column].south = outOfBoundsPoint;
      }
      if (column - 1 >= 0) {
        positions[row*BOARD_SIZE + column].west = &positions[row*BOARD_SIZE + column-1];
      } else {
        positions[row*BOARD_SIZE + column].west = outOfBoundsPoint;
      }
    }
  }
}

bool Board::operator==(const Board &b) {
  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int column = 0; column < BOARD_SIZE; column++) {
      if (positions[row*BOARD_SIZE + column] != b.positions[row*BOARD_SIZE + column]) {
        return false;
      }
    }
  }
  if (secondLastMove == nullptr && b.secondLastMove == nullptr
    && !(*lastMove == *b.lastMove)) {
    return false;
  }
  if (secondLastMove != nullptr && b.secondLastMove != nullptr
    && !(*secondLastMove == *b.secondLastMove)) {
    return false;
  }
  return true;
}

Board* Board::clone() {
  auto b = new Board();

  for (unsigned char row = 0; row < BOARD_SIZE; row++) {
    for (unsigned char column = 0; column < BOARD_SIZE; column++) {
      b->positions[row*BOARD_SIZE + column].row = row;
      b->positions[row*BOARD_SIZE + column].column = column;
    }
  }

  cloneInto(b);

  return b;
}

void Board::cloneInto(Board* clone) {
  for (uint16_t i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
    clone->positions[i].group = nullptr;
    clone->positions[i].color = positions[i].color;
  }
  clone->emptySpaces.clear();
  clone->emptySpaces.reserve(emptySpaces.size());
  for (auto& emptySpace : emptySpaces) {
    clone->emptySpaces.push_back(clone->getPoint(emptySpace));
  }

  for (unsigned char r = 0; r < BOARD_SIZE; ++r) {
    for (unsigned char c = 0; c < BOARD_SIZE; ++c) {
      if (r + 1 < BOARD_SIZE) {
        clone->positions[r*BOARD_SIZE + c].north = &clone->positions[(r+1)*BOARD_SIZE + c];
      } else {
        clone->positions[r*BOARD_SIZE + c].north = outOfBoundsPoint;
      }
      if (c + 1 < BOARD_SIZE) {
        clone->positions[r*BOARD_SIZE + c].east = &clone->positions[r*BOARD_SIZE + c+1];
      } else {
        clone->positions[r*BOARD_SIZE + c].east = outOfBoundsPoint;
      }
      if (r - 1 >= 0) {
        clone->positions[r*BOARD_SIZE + c].south = &clone->positions[(r-1)*BOARD_SIZE + c];
      } else {
        clone->positions[r*BOARD_SIZE + c].south = outOfBoundsPoint;
      }
      if (c - 1 >= 0) {
        clone->positions[r*BOARD_SIZE + c].west = &clone->positions[r*BOARD_SIZE + c-1];
      } else {
        clone->positions[r*BOARD_SIZE + c].west = outOfBoundsPoint;
      }
    }
  }

  clone->turn = turn;

  clone->lastMove = clone->getPoint(lastMove);
  clone->secondLastMove = clone->getPoint(secondLastMove);

  clone->koPoint = clone->getPoint(koPoint);

  cloneGroupsInto(clone);
}

void Board::cloneGroupsInto(Board* clone) {
  for (Group* g : blackGroups) {
    auto gClone = new Group(Black);
    for (Point* p : g->stones) {
      Point* pClone = &clone->positions[static_cast<unsigned int>(p->row)*BOARD_SIZE + static_cast<unsigned int>(p->column)];
      gClone->stones.push_back(pClone);
      pClone->group = gClone;
    }
    gClone->numberLiberties = g->numberLiberties;
    clone->blackGroups.push_back(gClone);
  }
  for (Group* g : whiteGroups) {
    auto gClone = new Group(White);
    for (Point* p : g->stones) {
      Point* pClone = &clone->positions[static_cast<unsigned int>(p->row)*BOARD_SIZE + static_cast<unsigned int>(p->column)];
      gClone->stones.push_back(pClone);
      pClone->group = gClone;
    }
    gClone->numberLiberties = g->numberLiberties;
    clone->whiteGroups.push_back(gClone);
  }
}

bool Board::isValidMove(Point move, std::list<uint64_t> *previousHashes) {
  if (move == *pass) {
    return true;
  }
  if (move.row < 0 || move.row >= BOARD_SIZE) {
    return false;
  }
  if (move.column < 0 || move.column >= BOARD_SIZE) {
    return false;
  }
  auto legalMoves = getPossibleMoves();
  if (previousHashes != nullptr) {
    eliminatePositionalSuperKo(legalMoves, previousHashes);
  }
  for (auto& legalMove : legalMoves) {
    if (move == *legalMove) {
      return true;
    }
  }
  return false;
}

// Fuego
void Board::getSimpleScore(float* whiteScore, float* blackScore) {
  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int column = 0; column < BOARD_SIZE; column++) {
      if (positions[row*BOARD_SIZE + column].color == White) {
        *whiteScore = *whiteScore + 1;
      } else if (positions[row*BOARD_SIZE + column].color == Black) {
        *blackScore = *blackScore + 1;
      }
    }
  }
  // printf("%f %f\n", *whiteScore, *blackScore);
}

float Board::getTaylorScore(float komi) {
  float score = -komi;  // score = blackScore - whiteScore
  // score += static_cast<float>(capturedWhite) -
    // static_cast<float>(capturedBlack);
  for (auto& position : positions) {
    if (position.marked) {
      // Already visited empty space territory
      continue;
    } else if (position.color == Black) {
      ++score;
      continue;
    } else if (position.color == White) {
      --score;
      continue;
    }

    // Unseen empty space
    std::stack<Point*> s;
    s.push(&position);
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
      } else if (p->color == Empty && !p->marked) {
        if (!p->north->marked) {
          s.push(p->north);
        }
        if (!p->east->marked) {
          s.push(p->east);
        }
        if (!p->south->marked) {
          s.push(p->south);
        }
        if (!p->west->marked) {
          s.push(p->west);
        }
        sizeOfEmptySpace++;

        p->marked = true;
      }
    }

    if (adjacentToBlack && !adjacentToWhite) {
      score += static_cast<float>(sizeOfEmptySpace);
    } else if (!adjacentToBlack && adjacentToWhite) {
      score -= static_cast<float>(sizeOfEmptySpace);
    }
  }

  for (auto& position : positions) {
    position.marked = false;
  }
  return score;
}

bool Board::isGameOver(GameResult *result) {
  if (lastMove == pass && secondLastMove == pass) {
    float komi = 7.5;
    float score = getTaylorScore(komi);
    if (score > 0) {
      *result = BlackWon;
    } else if (score < 0) {
      *result = WhiteWon;
    } else {
      *result = Draw;
    }
    return true;
  }
  return false;
}

void Board::updateStructures(Point& move) {
  Group* groupsToCombine[4];
  unsigned char numGroupsToCombine = 0;
  bool inGroup = false;
  if (move.north->color == move.color) {
    move.north->group->addStone(move);
    groupsToCombine[0] = move.north->group;
    ++numGroupsToCombine;
    inGroup = true;
  }
  if (move.east->color == move.color) {
    if (!inGroup) {
      move.east->group->addStone(move);
      inGroup = true;
    }
    bool found = false;
    for (unsigned char i = 0; i < numGroupsToCombine; ++i) {
      if (groupsToCombine[i] == move.east->group) {
        found = true;
      }
    }
    if (!found) {
      groupsToCombine[numGroupsToCombine] = move.east->group;
      ++numGroupsToCombine;
    }
  }
  if (move.south->color == move.color) {
    if (!inGroup) {
      move.south->group->addStone(move);
      inGroup = true;
    }
    bool found = false;
    for (unsigned char i = 0; i < numGroupsToCombine; ++i) {
      if (groupsToCombine[i] == move.south->group) {
        found = true;
      }
    }
    if (!found) {
      groupsToCombine[numGroupsToCombine] = move.south->group;
      ++numGroupsToCombine;
    }
  }
  if (move.west->color == move.color) {
    if (!inGroup) {
      move.west->group->addStone(move);
      inGroup = true;
    }
    bool found = false;
    for (unsigned char i = 0; i < numGroupsToCombine; ++i) {
      if (groupsToCombine[i] == move.west->group) {
        found = true;
      }
    }
    if (!found) {
      groupsToCombine[numGroupsToCombine] = move.west->group;
      ++numGroupsToCombine;
    }
  }


  std::vector<Group*>* groupsSameColor =
    (move.color == Black) ? &blackGroups : &whiteGroups;
  if (!inGroup) {
    auto group = new Group((turn == Black) ? Black : White);
    group->addStone(move);
    groupsSameColor->push_back(group);
  }

  if (numGroupsToCombine > 1) {
    Group* largest = groupsToCombine[0];
    for (unsigned char i = 1; i < numGroupsToCombine; ++i) {
      if (largest->stones.size() < groupsToCombine[i]->stones.size()) {
        largest = groupsToCombine[i];
      }
    }

    for (unsigned char i = 0; i < numGroupsToCombine; ++i) {
      Group* g = groupsToCombine[i];
      if (g == largest) {  // move's group will survive, others removed
        continue;
      }

      for (auto& x : g->stones) {
        largest->addStone(*x);
      }
      g->stones.clear();

      std::remove(groupsSameColor->begin(), groupsSameColor->end(), g);
      groupsSameColor->pop_back();
      delete g;
      /*
      bool found = false;
      for (auto it = groupsSameColor->begin(); it != groupsSameColor->end();
        ++it) {
        if (g == *it) {
          delete (*it);
          groupsSameColor->erase(it);
          found = true;
          break;
        }
      }
      assert(found);*/
    }
  }
}

void Board::removeDeadStones(Point& move, Player color) {
  auto firstGroups = (color == Black) ? &blackGroups : &whiteGroups;

  unsigned int numDeadStones = 0;

  std::set<Group*> deadGroups;
  // Group* groupsToKill[4];
  if (move.north->color == color && move.north->group->numberLiberties == 0) {
    deadGroups.insert(move.north->group);
  }
  if (move.east->color == color && move.east->group->numberLiberties == 0) {
    deadGroups.insert(move.east->group);
  }
  if (move.south->color == color && move.south->group->numberLiberties == 0) {
    deadGroups.insert(move.south->group);
  }
  if (move.west->color == color && move.west->group->numberLiberties == 0) {
    deadGroups.insert(move.west->group);
  }

  // beg1:
  for (auto deadGroup : deadGroups) {
    // remove dead stones on board
    unsigned int numStones = static_cast<unsigned int>(deadGroup->stones.size());
    if (numStones == 1) {
      koPoint = *deadGroup->stones.begin();
    }
    numDeadStones += numStones;
    for (auto stone : deadGroup->stones) {
      emptySpaces.push_back(stone);
    }
    deadGroup->removeStones();

    std::remove(firstGroups->begin(), firstGroups->end(), deadGroup);
    firstGroups->pop_back();
    /*
    for (std::vector<Group*>::iterator groupIterator = firstGroups->begin();
      groupIterator != firstGroups->end(); ++groupIterator) {
      if (deadGroup == (*groupIterator)) {
        firstGroups->erase(groupIterator);
        break;
      }
    }*/
    delete deadGroup;
  }

  if (numDeadStones != 1) {
    koPoint = nullptr;
  }
/*
  std::list<Group*> neighborGroupsToBeUpdated;
  for (Point* p : capturedStones) {
    bool found;
    if (p->north != nullptr && p->north->group != nullptr) {
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

    if (p->east != nullptr && p->east->group != nullptr) {
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

    if (p->south != nullptr && p->south->group != nullptr) {
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

    if (p->west != nullptr && p->west->group != nullptr) {
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
    g->recalculateLiberties(this);*/
}

uint64_t Board::getHash() {
  std::string s;
  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int column = 0; column < BOARD_SIZE; column++) {
      s += static_cast<char>(positions[row*BOARD_SIZE + column].color);
    }
  }
  std::hash<std::string> strHash;
  return strHash(s);
}

/*void testPossibleMoves(Board* b) {
  std::list<Point*> oldPossibleMoves = b->possibleMoves;
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
}*/

void Board::makeMove(Point move) {
  Point* movePointer = getPoint(&move);
  assert(movePointer != nullptr);
  makeMove(movePointer);
}

void Board::makeMove(Point* move) {
  // Don't place stone for passes
  if (!(*move == *pass)) {
    /*if (!move->legal) {
      printf("Illegal Move\nRow: %d\nColumn: %d\n",
        move->row, move->column);
      if (koPoint != nullptr)
        printf("Ko Point Row: %d Column: %d\n",
          koPoint->row, koPoint->column);
      for (int row = 0; row < BOARD_SIZE; row++)
        for (int column = 0; column < BOARD_SIZE; column++)
          if (positions[row][column].legal)
            printf("  PossibleMove Row: %d Column: %d\n",
              row, column);
      show();
      assert(move->legal);
    }*/


    if (move->color != Empty) {
      printf("Illegal Move\nRow: %d\nColumn: %d\nColor: %d\n",
        move->row, move->column, move->color);
      show();
      assert(false);
    }
    move->color = turn;

    move->decrementNeighborGroups();
    updateStructures(*move);
    removeDeadStones(*move, (turn == Black ? White : Black));

    // if the move has a neighbor of the same color,
    // then the next move cannot be ko
    if ((move->north->color == move->color)
      || (move->east->color == move->color)
      || (move->south->color == move->color)
      || (move->west->color == move->color)) {
      koPoint = nullptr;
    }

  } else {
    koPoint = nullptr;
  }


  turn = (turn == Black ? White : Black);

  secondLastMove = lastMove;
  lastMove = move;

  #ifdef RUNNING_TESTS
    // testPossibleMoves(this);
  #endif
}

Point* Board::getRandomMove(std::default_random_engine& engine) {
  Player sameColor = turn;  // == Black ? Black : White;
  Player oppositeColor = turn == Black ? White : Black;

  while (true) {
    std::uniform_int_distribution<> dist(0, static_cast<unsigned int>(emptySpaces.size()-1));
    unsigned int choice = dist(engine);
    // unsigned int choice = rand()%emptySpaces.size();
    if (emptySpaces[choice] == pass) {
      return pass;
    }
    if (emptySpaces[choice]->color != Empty) {
      emptySpaces[choice] = emptySpaces[emptySpaces.size()-1];
      emptySpaces.pop_back();
    } else if (!isSuicide(emptySpaces[choice], sameColor, oppositeColor)
      && emptySpaces[choice] != koPoint) {
      return emptySpaces[choice];
    }
  }
}

void Board::makeRandomMove(std::default_random_engine& engine) {
  makeMove(getRandomMove(engine));
}

GameResult Board::playRandomGame(std::default_random_engine& engine) {
  GameResult result;
  while (!isGameOver(&result)) {
    makeRandomMove(engine);
  }
  return result;
}

GameResult Board::playGame(Patterns* patterns, std::default_random_engine& engine) {
  GameResult result;
  while (!isGameOver(&result)) {
    makeMove(patterns->getMove(*this, engine));
  }
  return result;
}

void Board::show() {
  std::stringstream boardString;

  for (int row = 0; row < BOARD_SIZE+1; row++) {
    for (int column = 0; column < BOARD_SIZE+1; column++) {
      if (row == BOARD_SIZE && column == BOARD_SIZE) {
        boardString << "  ";
      } else if (row == BOARD_SIZE) {
        boardString << column << " ";
      } else if (column == BOARD_SIZE) {
        boardString << row << " ";  // static_cast<char>(row+'a' + (row > 7 ? 1 : 0))
          // << " ";
      } else {
        switch (positions[row*BOARD_SIZE + column].color) {
        case 0:
          boardString << "- ";
          break;
        case White:
          boardString << "W ";
          break;
        case Black:
          boardString << "B ";
          break;
        case OutOfBounds:
          boardString << "O ";
          break;
        default:
          printf("Position[%d][%d] = %d", row, column, positions[row*BOARD_SIZE + column].color);
          assert(false);
        }
      }
    }

    boardString << "\n";
  }
  boardString << "\n";
  if (lastMove == pass) {
    boardString << ((turn == Black) ? "White" : "Black")
      << " passed\n";
  }
  Board *bClone = this->clone();
  float komi = 7.5;
  float score = bClone->getTaylorScore(komi);
  delete bClone;
  printf("Score: %f\n", score);
  printf("%s", boardString.str().c_str());
}

bool Board::isSuicide(Point move, const Player &sameColor, const Player &oppositeColor) {
  return isSuicide(getPoint(&move), sameColor, oppositeColor);
}

// http://en.wikibooks.org/wiki/Computer_Go/Recognizing_Illegal_Moves
bool Board::isSuicide(Point* move, const Player &sameColor, const Player &oppositeColor) {
  //   Check for empty neighbors
  //   Check for neighbors of same color with more than one liberty
  //   Check for neighbors of opposite color with only one liberty
  if (move->north->color == Empty) {
    return false;
  }
  if (move->east->color == Empty) {
    return false;
  }
  if (move->south->color == Empty) {
    return false;
  }
  if (move->west->color == Empty) {
    return false;
  }

  if (move->north->group->color == sameColor && move->north->group->numberLiberties > 1) {
    return false;
  }
  if (move->north->group->color == oppositeColor && move->north->group->numberLiberties == 1) {
    return false;
  }

  if (move->east->group->color == sameColor && move->east->group->numberLiberties > 1) {
    return false;
  }
  if (move->east->group->color == oppositeColor && move->east->group->numberLiberties == 1) {
    return false;
  }

  if (move->south->group->color == sameColor && move->south->group->numberLiberties > 1) {
    return false;
  }
  if (move->south->group->color == oppositeColor && move->south->group->numberLiberties == 1) {
    return false;
  }

  if (move->west->group->color == sameColor && move->west->group->numberLiberties > 1) {
    return false;
  }
  if (move->west->group->color == oppositeColor && move->west->group->numberLiberties == 1) {
    return false;
  }
  return true;
}

/*void printPossibleMoves(Board* b) {
  printf("Possible Moves: \n");
  for(Point* p : b->possibleMoves) {
    printf("  R: %d C: %d\n", p->row, p->column);
  }
}*/

void Board::eliminatePositionalSuperKo(std::vector<Point*> &legalMoves, std::list<uint64_t> *previousHashes) {
  for (uint16_t i = 0; i < legalMoves.size(); ++i) {
    if (isPositionalSuperKo(legalMoves[i], *previousHashes)) {
      legalMoves[i] = legalMoves[legalMoves.size()-1];
      legalMoves.pop_back();
      i--;
    }
  }
}

bool Board::isPositionalSuperKo(Point* p, std::list<uint64_t> previousHashes) {
  std::string s;
  for (auto& position : positions) {
    if (p != &position) {
      if (position.group != nullptr
        && position.group->isAdjacent(p)
        && position.group->numberLiberties == 1) {
        s += static_cast<char>(Empty);
      } else {
        s += static_cast<char>(position.color);
      }
    } else {
      s += static_cast<char>(turn);
    }
  }
  std::hash<std::string> strHash;
  uint64_t pHash = strHash(s);
  for (uint64_t pastHash : previousHashes) {
    if (pastHash == pHash) {
      return true;
    }
  }
  return false;
}

std::vector<Point*> Board::getPossibleMoves() {
  Player sameColor = turn;  // == Black ? Black : White;
  Player oppositeColor = turn == Black ? White : Black;

  // To replace ko check below
  Player koColor = Empty;
  if (koPoint != nullptr) {
    koColor = koPoint->color;
    koPoint->color = Black;
  }

  std::vector<Point*> legalMoves;
  legalMoves.push_back(pass);
  for (auto& position : positions) {
    if (position.color == Empty
        //  !isSuicide(&positions[r][c], sameColor, oppositeColor)
        &&  (position.west->color == Empty
          || position.east->color == Empty
          || position.north->color == Empty
          || position.south->color == Empty

          || (position.west->color == sameColor && position.west->group->numberLiberties > 1)
          || (position.west->color == oppositeColor && position.west->group->numberLiberties == 1)

          || (position.east->color == sameColor && position.east->group->numberLiberties > 1)
          || (position.east->color == oppositeColor && position.east->group->numberLiberties == 1)

          || (position.north->color == sameColor && position.north->group->numberLiberties > 1)
          || (position.north->color == oppositeColor && position.north->group->numberLiberties == 1)

          || (position.south->color == sameColor && position.south->group->numberLiberties > 1)
          || (position.south->color == oppositeColor && position.south->group->numberLiberties == 1)
          )
       ) {  // && &positions[r][c] != koPoint) {
      legalMoves.push_back(&position);
    }
  }

  // To replace ko check
  if (koPoint != nullptr) {
    koPoint->color = koColor;
  }
  return legalMoves;
}

// TODO(GaryChristiansen): Don't use this in Board, just positions[row][column]
Point* Board::getPoint(Point *p) {
  if (p == nullptr) {
    return nullptr;
  }
  if (*p == *pass) {
    return pass;
  }
  return &positions[p->row*BOARD_SIZE + p->column];
}
