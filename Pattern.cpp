#include "Pattern.h"

#include <assert.h>

#include "Board.h"

static char pointToCharMap[] = { '_', 'B', 'W', 'O' };

Pattern::Pattern() : hash(""),
  goodMoves(std::vector<std::pair<char, char>>()) {

}

char Pattern::pointToChar(Point* p) {
  return pointToCharMap[p->color];
  /*
  if (p == NULL)
    return 'O';
  if (p->color == White)
    return 'W';
  else if (p->color == Black)
    return 'B';
  else if (p->color == Empty)
    return '_';
  else if (p->color == OutOfBounds)
    return 'O';
  else assert(false);
  */
}
/*
Pattern::Pattern(Point* p) : hash(""),
  goodMoves(std::vector<std::pair<char, char>>()) {

  if (*p == Point(BOARD_SIZE, BOARD_SIZE))
    return;
  
  Point* s = p->south;
  Point* sw = s->west;
  Point* se = s->east;
  Point* w = p->west;
  Point* e = p->east;
  Point* n = p->north;
  Point* nw = n->west;
  Point* ne = n->east;
  
  hash += pointToChar(sw);
  hash += pointToChar(s);
  hash += pointToChar(se);
  hash += pointToChar(w);
  hash += pointToChar(p);
  hash += pointToChar(e);
  hash += pointToChar(nw);
  hash += pointToChar(n);
  hash += pointToChar(ne);

}*/

Pattern::~Pattern() {

}

bool operator==(const Pattern &p1, const Pattern &p2) {
  if (p1.hash.compare(p2.hash))
    return false;
  return true;
}

void Pattern::invertColor() {
  std::string newHash = "";
  for (char c : hash) {
    if (c == 'B')
      c = 'W';
    else if (c == 'W')
      c = 'B';
    newHash += c;
  }
  hash = newHash;
}

Point* Pattern::getRandomGoodMove(Board* b, Point& move, std::default_random_engine& engine) {
  if (goodMoves.size() == 0)
    return NULL;
  std::uniform_int_distribution<> dist(0, (int)goodMoves.size()-1);
  int choice = dist(engine);
  return &b->positions[goodMoves[choice].first+move.row][goodMoves[choice].second+move.color];
}

std::vector<Point*> Pattern::getGoodMoves(Board* b, Point& move) {
  std::vector<Point*> movesToReturn;
  for (auto p : goodMoves) {
    movesToReturn.push_back(&b->positions[p.first+move.row][p.second+move.column]);
  }
  return movesToReturn;
}

char Pattern::getRandomColor(std::default_random_engine& engine) {
  std::uniform_int_distribution<> dist(0, 3);
  int i = dist(engine);
  char randomColor;
  if (i == 0) {
    randomColor = 'B';
  } else if (i == 1) {
    randomColor = 'W';
  } else if (i == 2) {
    randomColor = '_';
  } else {
    randomColor = 'O';
  }
  return randomColor;
}

/*void Pattern::determineRandomGoodMoves(std::default_random_engine& engine) {
  std::uniform_int_distribution<> boolDist(0, 1);
  int i = -1, j = -1;
  for (char c : hash) {
    if (c == '_' && (boolDist(engine) == 0))
      goodMoves.push_back(std::make_pair(i, j));
    if (j == 1) {
      j = -2;
      i++;
    }
    j++;
  }
}*/

/*bool Pattern::isLegalPattern() {
  if (hash[4] != 'B' && hash[4] != 'W')
    return false;

  char matrix[3][3];
  int i = 0, j = 0, pos = 0;
  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      matrix[i][j] = hash[pos++];
    }
  }

  bool foundEmpty = false;
  for (char c : hash)
    if (c == '_')
      foundEmpty = true;
  if (!foundEmpty) {
    // std::cout << *this;
    return false;
  }

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      if (matrix[i][j] == 'O') {
        // corners
        if (i == 0 && j == 0) {
          if (matrix[i+1][j] != 'O' && matrix[i][j+1] != 'O') {
            return false;
          }
        } else if (i == 0 && j == 2) {
          if (matrix[i+1][j] != 'O' && matrix[i][j-1] != 'O') {
            return false;
          }
        } else if (i == 2 && j == 0) {
          if (matrix[i-1][j] != 'O' && matrix[i][j+1] != 'O') {
            return false;
          }
        } else if (i == 2 && j == 2) {
          if (matrix[i-1][j] != 'O' && matrix[i][j-1] != 'O') {
            return false;
          }
        } // non corners/middle
        else if (i == 1) {
          if (matrix[i-1][j] != 'O' || matrix[i+1][j] != 'O') {
            return false;
          }
        } else if (j == 1) {
          if (matrix[i][j-1] != 'O' || matrix[i][j+1] != 'O') {
            return false;
          }
        }
      }
    }
  }
  return true;
}*/

/*void Pattern::mutate(std::default_random_engine& engine) {
  goodMoves.clear();
  determineRandomGoodMoves(engine); 
}

Pattern Pattern::getRandomPattern(std::default_random_engine& engine) {
  Pattern p;
  for (unsigned int i = 0; i < 4; ++i) {
    p.hash += getRandomColor(engine);
  }
  p.hash += 'B';
  for (unsigned int i = 0; i < 4; ++i) {
    p.hash += getRandomColor(engine);
  }

  p.determineRandomGoodMoves(engine);

  if (p.isLegalPattern())
    return p;
  return getRandomPattern(engine);
}*/

