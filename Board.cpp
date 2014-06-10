#include "Board.h"

#include <assert.h>

#include <cstdio>
#include <sstream>
#include <stack>

Board::Board(int newBoardSize) :
  positions(new Point**[newBoardSize]),
  blackGroups(std::list<Group*>()),
  whiteGroups(std::list<Group*>()),
  lastMove(NULL),
  secondLastMove(NULL),
  koPoint(NULL),
  pass(new Point(newBoardSize, newBoardSize)),
  capturedBlack(0),
  capturedWhite(0),
  boardSize(newBoardSize),
  turn(Black) {
  for (int row = 0; row < boardSize; row++) {
    positions[row] = new Point*[boardSize];
  }
}

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

  if (pass != NULL)
    delete pass;
}

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

Board* Board::clone() {
  Board* b = new Board(boardSize);
  for (int row = 0; row < boardSize; row++) {
    for (int column = 0; column < boardSize; column++) {
      Point * p = positions[row][column];
      b->positions[row][column] = new Point(p->row, p->column, p->color,
        NULL, NULL, NULL, NULL, NULL, p->legal);
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
      gClone->stones.push_back(pClone);
      pClone->group = gClone;
    }
    gClone->recalculateLiberties();
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
    gClone->recalculateLiberties();
    b->whiteGroups.push_back(gClone);
  }

  return b;
}

bool Board::isValidMove(Point move) {
  if (move == *pass)
    return true;
  if (move.row < 0 || move.row >= boardSize)
    return false;
  if (move.column < 0 || move.column >= boardSize)
    return false;
  return positions[move.row][move.column]->legal;
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
  // score += static_cast<float>(capturedWhite) -
    // static_cast<float>(capturedBlack);
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
  std::list<Group*> groupsToCombine;
  bool inGroup = false;
  if (move->north != NULL && move->north->color == move->color) {
    move->north->group->addStone(move);
    groupsToCombine.push_back(move->north->group);
    inGroup = true;
  }
  if (move->east != NULL && move->east->color == move->color) {
    if (!inGroup) {
      move->east->group->addStone(move);
      inGroup = true;
    }
    groupsToCombine.push_back(move->east->group);
  }
  if (move->south != NULL && move->south->color == move->color) {
    if (!inGroup) {
      move->south->group->addStone(move);
      inGroup = true;
    }
    groupsToCombine.push_back(move->south->group);
  }
  if (move->west != NULL && move->west->color == move->color) {
    if (!inGroup) {
      move->west->group->addStone(move);
      inGroup = true;
    }
    groupsToCombine.push_back(move->west->group);
  }


  std::list<Group*>* groupsSameColor =
    (move->color == Black) ? &blackGroups : &whiteGroups;
  if (!inGroup) {
    Group* group = new Group((turn == Black) ? Black : White);
    group->addStone(move);
    groupsSameColor->push_back(group);
  }

  if (groupsToCombine.size() > 1) {
    std::list<Group*> toDelete;
    for (Group* g : groupsToCombine) {
      if (g == move->group) {  // move's group will survive, others removed
        continue;
      }
      while (g->stones.size() > 0) {
        move->group->stones.push_back(*g->stones.begin());
        (*g->stones.begin())->group = move->group;
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

    move->group->recalculateLiberties();
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
      // possibleMoves.push_back(Point(stone->row, stone->column));
    }

    for (std::list<Group*>::iterator groupIterator = firstGroups->begin();
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
    g->recalculateLiberties();

  return static_cast<unsigned int>(capturedStones.size());
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

void Board::makeMove(Point* move) {
  // Don't place stone for passes
  if (!(*move == *pass)) {
    if (!move->legal) {
      printf("Illegal Move\nRow: %d\nColumn: %d\n",
        move->row, move->column);
      if (koPoint != NULL)
        printf("Ko Point Row: %d Column: %d\n",
          koPoint->row, koPoint->column);
      for (int row = 0; row < boardSize; row++)
        for (int column = 0; column < boardSize; column++)
          if (positions[row][column]->legal)
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

    updateStructures(move);
    int numCaptured =
        removeDeadStones((turn == Black ? White : Black), move);
    if (turn == Black)
      capturedWhite += numCaptured;
    else
      capturedBlack += numCaptured;

    std::list<Point*> neighbors;
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

  turn = (turn == Black ? White : Black);

  secondLastMove = lastMove;
  lastMove = move;

  getPossibleMoves();
  #ifdef RUNNING_TESTS
    // testPossibleMoves(this);
  #endif
}

Point* Board::getRandomMove() {
  int numPossibleMoves = 1;
  for (int r = 0; r < boardSize; r++)
    for (int c = 0; c < boardSize; c++)
      if (positions[r][c]->legal)
        numPossibleMoves++;
  static unsigned int seed = static_cast<unsigned int>(time(NULL));
  unsigned int choice = rand_r(&seed) % numPossibleMoves;
  int row = 0;
  int column = 0;
  while (true) {
    if (choice == 0) {
      if (row == boardSize && column == boardSize) {
        return pass;
      } else if (positions[row][column]->legal) {
        return positions[row][column];
      }
    } else {
      if (row < boardSize && column < boardSize) {
        if (positions[row][column]->legal) {
          choice--;
        }
      } else {
        choice--;
      }
    }

    column++;
    if (column == boardSize) {
      row++;
      if (row != boardSize) {
        column = 0;
      }
    } else if (column == boardSize + 1) {
      column = 0;
      row = 0;
    }
  }
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

  Player sameColor = turn == Black ? Black : White;
  Player oppositeColor = turn == Black ? White : Black;
  // if neighbor != NULL
  //   Check for empty neighbors
  //   Check for neighbors of same color with more than one liberty
  //   Check for neighbors of opposite color with only one liberty
  if (move->north != NULL
    && (move->north->color == Empty
      || (move->north->group->color == sameColor && move->north->group->numLiberties() > 1)
      || (move->north->group->color == oppositeColor && move->north->group->numLiberties() == 1)))
    return false;
  if (move->east != NULL
    && (move->east->color == Empty
    || (move->east->group->color == sameColor && move->east->group->numLiberties() > 1)
    || (move->east->group->color == oppositeColor && move->east->group->numLiberties() == 1)))
    return false;
  if (move->south != NULL
    && (move->south->color == Empty
    || (move->south->group->color == sameColor && move->south->group->numLiberties() > 1)
    || (move->south->group->color == oppositeColor && move->south->group->numLiberties() == 1)))
    return false;
  if (move->west != NULL &&
    (move->west->color == Empty
    || (move->west->group->color == sameColor && move->west->group->numLiberties() > 1)
    || (move->west->group->color == oppositeColor && move->west->group->numLiberties() == 1)))
    return false;

  return true;
}

/*void printPossibleMoves(Board* b) {
  printf("Possible Moves: \n");
  for(Point* p : b->possibleMoves) {
    printf("  R: %d C: %d\n", p->row, p->column);
  }
}*/

void Board::getPossibleMoves() {
  for (int r = 0; r < boardSize; r++) {
    for (int c = 0; c < boardSize; c++) {
      if (positions[r][c]->color == Empty
        && positions[r][c] != koPoint
        && !isSuicide(positions[r][c])) {
        positions[r][c]->legal = true;
      } else {
        positions[r][c]->legal = false;
      }
    }
  }
  pass->legal = true;
}

Point* Board::getPoint(Point *p) {
  if (p == NULL) {
    return NULL;
  } else if (*p == *pass) {
    return pass;
  } else {
    return positions[p->row][p->column];
  }
}
