#include "Board.h"

#include <assert.h>

#include <cstdio>
#include <cstring>
#include <functional>
#include <sstream>
#include <stack>

Board::Board() :
  numLegalMoves(0),
  blackGroups(std::list<Group*>()),
  whiteGroups(std::list<Group*>()),
  lastMove(NULL),
  secondLastMove(NULL),
  koPoint(NULL),
  pass(new Point(BOARD_SIZE, BOARD_SIZE)),
  turn(Black) {
}

Board::~Board() {
  for(Group* g : blackGroups)
    delete g;
  for(Group* g : whiteGroups)
    delete g;

  if (pass != NULL)
    delete pass;
}

void Board::init() {
  for (int row = 0; row < BOARD_SIZE; ++row) {
    for (int column = 0; column < BOARD_SIZE; ++column) {
      positions[row][column].row = row;
      positions[row][column].column = column;
    }
  }
  for (int row = 0; row < BOARD_SIZE; ++row) {
    for (int column = 0; column < BOARD_SIZE; ++column) {
      if (row + 1 < BOARD_SIZE)
        positions[row][column].north = &positions[row+1][column];
      if (column + 1 < BOARD_SIZE)
        positions[row][column].east = &positions[row][column+1];
      if (row - 1 >= 0)
        positions[row][column].south = &positions[row-1][column];
      if (column - 1 >= 0)
        positions[row][column].west = &positions[row][column-1];
    }
  }

  getPossibleMoves();
}

bool Board::operator==(const Board &b) {
  for (int row = 0; row < BOARD_SIZE; row++)
    for (int column = 0; column < BOARD_SIZE; column++)
      if (positions[row][column] != b.positions[row][column])
        return false;
  if (secondLastMove == NULL && b.secondLastMove == NULL
    && !(*lastMove == *b.lastMove))
    return false;
  if (secondLastMove != NULL && b.secondLastMove != NULL
    && !(*secondLastMove == *b.secondLastMove))
    return false;
  return true;
}

Board* Board::clone() {
  Board* b = new Board();

  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int column = 0; column < BOARD_SIZE; column++) {
      b->positions[row][column].row = row;
      b->positions[row][column].column = column;
    }
  }

  cloneInto(b);

  return b;
}

void Board::cloneInto(Board* b) {
  b->numLegalMoves = 0;
  b->legalMoves[b->numLegalMoves++] = b->pass;
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      b->positions[r][c].group = NULL;
      b->positions[r][c].color = positions[r][c].color;
      b->positions[r][c].legal = positions[r][c].legal;
      if (positions[r][c].legal) {
        b->legalMoves[b->numLegalMoves++] = &b->positions[r][c];
      }
    }
  }

  for (int r = 0; r < BOARD_SIZE; ++r) {
    for (int c = 0; c < BOARD_SIZE; ++c) {
      if (r + 1 < BOARD_SIZE)
        b->positions[r][c].north = &b->positions[r+1][c];
      else
        b->positions[r][c].north = NULL;
      if (c + 1 < BOARD_SIZE)
        b->positions[r][c].east = &b->positions[r][c+1];
      else
        b->positions[r][c].east = NULL;
      if (r - 1 >= 0)
        b->positions[r][c].south = &b->positions[r-1][c];
      else
        b->positions[r][c].south = NULL;
      if (c - 1 >= 0)
        b->positions[r][c].west = &b->positions[r][c-1];
      else
        b->positions[r][c].west = NULL;
    }
  }

  b->turn = turn;

  b->lastMove = b->getPoint(lastMove);
  b->secondLastMove = b->getPoint(secondLastMove);

  b->koPoint = b->getPoint(koPoint);

  for(Group* g : blackGroups) {
    Group* gClone = new Group(Black);
    for(Point* p : g->stones) {
      Point temp(p->row, p->column);
      Point* pClone = b->getPoint(&temp);
      gClone->stones.push_back(pClone);
      pClone->group = gClone;
    }
    gClone->numberLiberties = g->numberLiberties;
    b->blackGroups.push_back(gClone);
  }
  for(Group* g : whiteGroups) {
    Group* gClone = new Group(White);
    for(Point* p : g->stones) {
      Point temp(p->row, p->column);
      Point* pClone = b->getPoint(&temp);
      gClone->stones.push_back(pClone);
      pClone->group = gClone;
    }
    gClone->numberLiberties = g->numberLiberties;
    b->whiteGroups.push_back(gClone);
  }
}

bool Board::isValidMove(Point move) {
  if (move == *pass)
    return true;
  if (move.row < 0 || move.row >= BOARD_SIZE)
    return false;
  if (move.column < 0 || move.column >= BOARD_SIZE)
    return false;
  return positions[move.row][move.column].legal;
}

// Fuego
void Board::getSimpleScore(float* whiteScore, float* blackScore) {
  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int column = 0; column < BOARD_SIZE; column++) {
      if (positions[row][column].color == White) {
        *whiteScore = *whiteScore + 1;
      } else if (positions[row][column].color == Black) {
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
  for (int row = 0; row < BOARD_SIZE; ++row) {
    for (int column = 0; column < BOARD_SIZE; ++column) {
      if (positions[row][column].marked) {
        // Already visited empty space territory
        continue;
      } else if (positions[row][column].color == Black) {
        ++score;
        continue;
      } else if (positions[row][column].color == White) {
        --score;
        continue;
      }

      // Unseen empty space
      std::stack<Point*> s;
      s.push(&positions[row][column]);
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
          if (p->north != NULL && !p->north->marked)
            s.push(p->north);
          if (p->east != NULL && !p->east->marked)
            s.push(p->east);
          if (p->south != NULL && !p->south->marked)
            s.push(p->south);
          if (p->west != NULL && !p->west->marked)
            s.push(p->west);
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
  }

  for (int r = 0; r < BOARD_SIZE; ++r)
    for (int c = 0; c < BOARD_SIZE; ++c)
      positions[r][c].marked = false;
  return score;
}

bool Board::isGameOver(GameResult *result) {
  if (lastMove == pass && secondLastMove == pass) {
    float komi = 7.5;
    float score = getTaylorScore(komi);
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
  Group* groupsToCombine[4];
  unsigned char numGroupsToCombine = 0;
  bool inGroup = false;
  if (move->north != NULL && move->north->color == move->color) {
    move->north->group->addStone(move);
    groupsToCombine[0] = move->north->group;
    ++numGroupsToCombine;
    inGroup = true;
  }
  if (move->east != NULL && move->east->color == move->color) {
    if (!inGroup) {
      move->east->group->addStone(move);
      inGroup = true;
    }
    bool found = false;
    for (unsigned char i = 0; i < numGroupsToCombine; ++i)
      if (groupsToCombine[i] == move->east->group)
        found = true;
    if (!found) {
      groupsToCombine[numGroupsToCombine] = move->east->group;
      ++numGroupsToCombine;
    }
  }
  if (move->south != NULL && move->south->color == move->color) {
    if (!inGroup) {
      move->south->group->addStone(move);
      inGroup = true;
    }
    bool found = false;
    for (unsigned char i = 0; i < numGroupsToCombine; ++i)
      if (groupsToCombine[i] == move->south->group)
        found = true;
    if (!found) {
      groupsToCombine[numGroupsToCombine] = move->south->group;
      ++numGroupsToCombine;
    }
  }
  if (move->west != NULL && move->west->color == move->color) {
    if (!inGroup) {
      move->west->group->addStone(move);
      inGroup = true;
    }
    bool found = false;
    for (unsigned char i = 0; i < numGroupsToCombine; ++i)
      if (groupsToCombine[i] == move->west->group)
        found = true;
    if (!found) {
      groupsToCombine[numGroupsToCombine] = move->west->group;
      ++numGroupsToCombine;
    }
  }


  std::list<Group*>* groupsSameColor =
    (move->color == Black) ? &blackGroups : &whiteGroups;
  if (!inGroup) {
    Group* group = new Group((turn == Black) ? Black : White);
    group->addStone(move);
    groupsSameColor->push_back(group);
  }

  if (numGroupsToCombine > 1) {
    std::list<Group*> toDelete;
    for (unsigned char i = 0; i < numGroupsToCombine; ++i) {
      Group* g = groupsToCombine[i];
      if (g == move->group) {  // move's group will survive, others removed
        continue;
      }
      while (g->stones.size() > 0) {
        move->group->addStone(*g->stones.begin());
        g->stones.pop_front();
      }

      toDelete.push_back(g);

    }
    while(toDelete.size() > 0) {
      bool found = false;
      for (auto it = groupsSameColor->begin(); it != groupsSameColor->end();
        ++it) {
        if (toDelete.front() == *it) {
          delete (*it);
          groupsSameColor->erase(it);
          found = true;
          break;
        }
      }
      assert(found);
      toDelete.remove(toDelete.front()); 
    }
  }
}

unsigned int Board::removeDeadStones(Player color, Point* move) {
  std::list<Point*> capturedStones;

  std::list<Group*>* firstGroups =
    (color == Black) ? &blackGroups : &whiteGroups;
  // vector<Group*> secondGroup = (turn == Black) ? blackGroups : whiteGroups;

  std::list<Group*> deadGroups;
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
    }

    for (std::list<Group*>::iterator groupIterator = firstGroups->begin();
      groupIterator != firstGroups->end(); ++groupIterator) {
      if (deadGroup == (*groupIterator)) {
        firstGroups->erase(groupIterator);
        break;
      }
    }
    delete deadGroup;
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

  std::list<Group*> neighborGroupsToBeUpdated;
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
    g->recalculateLiberties(this);

  return static_cast<unsigned int>(capturedStones.size());
}

unsigned long int Board::getHash() {
  std::string s;
  for (int row = 0; row < BOARD_SIZE; row++)
    for (int column = 0; column < BOARD_SIZE; column++)
      s += (char)positions[row][column].color;
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
  assert(movePointer != NULL);
  makeMove(movePointer);
}

void decrementNeighborGroups(Point* move) {
  // TODO(GaryChristiansen): Optimize this, maybe add 'marked' for groups and eliminate decrementedGroups?
  std::list<Group*> decrementedGroups;
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
}


void Board::makeMove(Point* move) {
  // Don't place stone for p asses
  if (!(*move == *pass)) {
    if (!move->legal) {
      printf("Illegal Move\nRow: %d\nColumn: %d\n",
        move->row, move->column);
      if (koPoint != NULL)
        printf("Ko Point Row: %d Column: %d\n",
          koPoint->row, koPoint->column);
      for (int row = 0; row < BOARD_SIZE; row++)
        for (int column = 0; column < BOARD_SIZE; column++)
          if (positions[row][column].legal)
            printf("  PossibleMove Row: %d Column: %d\n",
              row, column);
      show();
      assert(move->legal);
    }


    if (move->color != Empty) {
      printf("Illegal Move\nRow: %d\nColumn: %d\nColor: %d\n",
        move->row, move->column, move->color);
      show();
      assert(false);
    }
    move->color = turn;

    decrementNeighborGroups(move);
    updateStructures(move);
    removeDeadStones((turn == Black ? White : Black), move);

    // if the move has a neighbor of the same color,
    // then the next move cannot be ko
    if ((move->north != NULL && move->north->color == move->color)
      || (move->east != NULL && move->east->color == move->color)
      || (move->south != NULL && move->south->color == move->color)
      || (move->west != NULL && move->west->color == move->color))
      koPoint = NULL;

  } else {
    koPoint = NULL;
  }


  turn = (turn == Black ? White : Black);

  secondLastMove = lastMove;
  lastMove = move;

  getPossibleMoves();
  #ifdef RUNNING_TESTS
    // testPossibleMoves(this);
  #endif
}

Point* Board::getRandomMove() {
  static unsigned int seed = static_cast<unsigned int>(time(NULL));
  unsigned int choice = rand_r(&seed) % numLegalMoves;
  return legalMoves[choice];
}

void Board::makeRandomMove() {
  makeMove(getRandomMove());
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

  for (int row = 0; row < BOARD_SIZE+1; row++) {
    for (int column = 0; column < BOARD_SIZE+1; column++) {
      if (row == BOARD_SIZE && column == BOARD_SIZE) {
        boardString << "  ";
      } else if (row == BOARD_SIZE) {
        boardString << column+1 << " ";
      } else if (column == BOARD_SIZE) {
        boardString << static_cast<char>(row+'a' + (row > 7 ? 1 : 0))
          << " ";
      } else {
        switch (positions[row][column].color) {
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

bool Board::isSuicide(Point move, const Player &sameColor, const Player &oppositeColor) {
  return isSuicide(getPoint(&move), sameColor, oppositeColor);
}

// http://en.wikibooks.org/wiki/Computer_Go/Recognizing_Illegal_Moves
bool Board::isSuicide(Point* move, const Player &sameColor, const Player &oppositeColor) {

  // if neighbor != NULL
  //   Check for empty neighbors
  //   Check for neighbors of same color with more than one liberty
  //   Check for neighbors of opposite color with only one liberty
  if (move->north != NULL) {
    if (move->north->color == Empty)
      return false;
    if (move->north->group->color == sameColor && move->north->group->numLiberties() > 1)
      return false;
    if (move->north->group->color == oppositeColor && move->north->group->numLiberties() == 1)
      return false;
  }
  if (move->east != NULL) {
    if (move->east->color == Empty)
      return false;
    if (move->east->group->color == sameColor && move->east->group->numLiberties() > 1)
      return false;
    if (move->east->group->color == oppositeColor && move->east->group->numLiberties() == 1)
      return false;
  }
  if (move->south != NULL) {
    if (move->south->color == Empty)
      return false;
    if (move->south->group->color == sameColor && move->south->group->numLiberties() > 1)
      return false;
    if (move->south->group->color == oppositeColor && move->south->group->numLiberties() == 1)
      return false;
  }
  if (move->west != NULL) {
    if (move->west->color == Empty)
      return false;
    if (move->west->group->color == sameColor && move->west->group->numLiberties() > 1)
      return false;
    if (move->west->group->color == oppositeColor && move->west->group->numLiberties() == 1)
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

void Board::eliminatePositionalSuperKo(std::list<unsigned long int> previousHashes) {
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      if (positions[r][c].legal
        && isPositionalSuperKo(&positions[r][c], previousHashes)) {
        positions[r][c].legal = false;
        for (int i = 0; i < numLegalMoves; i++) {
          if (legalMoves[i] == &positions[r][c]) {
            legalMoves[i] = legalMoves[numLegalMoves-1];
            numLegalMoves--;
          }
        }
      }
    }
  }
}

bool Board::isPositionalSuperKo(Point* p, std::list<unsigned long int> previousHashes) {
  std::string s;
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      if (p != &positions[r][c]) {
        if (positions[r][c].group != NULL
          && positions[r][c].group->isAdjacent(p)
          && positions[r][c].group->numLiberties() == 1) {
          s += (char)Empty;
        } else {
          s += (char)positions[r][c].color;
        }
      } else {
        s += (char)turn;
      }
    }
  }
  std::hash<std::string> strHash;
  unsigned long int pHash = strHash(s);
  for (unsigned long int pastHash : previousHashes) {
    if (pastHash == pHash) {
      return true;
    }
  }
  return false;
}

void Board::getPossibleMoves() {
  Player sameColor = turn == Black ? Black : White;
  Player oppositeColor = turn == Black ? White : Black;
  numLegalMoves = 0;
  pass->legal = true;
  legalMoves[numLegalMoves] = pass;
  ++numLegalMoves;
  for (int r = 0; r < BOARD_SIZE; ++r) {
    for (int c = 0; c < BOARD_SIZE; ++c) {
      if (positions[r][c].color == Empty
        && &positions[r][c] != koPoint
        && !isSuicide(&positions[r][c], sameColor, oppositeColor)) {
        positions[r][c].legal = true;
        legalMoves[numLegalMoves] = &positions[r][c];
        ++numLegalMoves;
      } else {
        positions[r][c].legal = false;
      }
    }
  }
}

Point* Board::getPoint(Point *p) {
  if (p == NULL) {
    return NULL;
  } else if (*p == *pass) {
    return pass;
  } else {
    return &positions[p->row][p->column];
  }
}
