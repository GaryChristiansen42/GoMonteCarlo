#include "Pattern.h"

#include <assert.h>

#include "Board.h"

Pattern::Pattern() : hash(""),
  goodMoves(std::vector<std::pair<char, char>>()) {

}

char pointToChar(Point* p) {
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
}

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

}


bool operator==(const Pattern &p1, const Pattern &p2) {
  if (p1.hash.compare(p2.hash))
    return false;
  return true;
}

void Pattern::rotate90() {

  // transpose
  // std::swap(matrix[1][0], matrix[0][1]);
  // std::swap(matrix[2][0], matrix[0][2]);
  // std::swap(matrix[1][2], matrix[2][1]);

  // reverse rows
  // std::swap(matrix[0][0], matrix[0][2]);
  // std::swap(matrix[1][0], matrix[1][2]);
  // std::swap(matrix[2][0], matrix[2][2]);

  std::swap(hash[3], hash[1]);
  std::swap(hash[6], hash[2]);
  std::swap(hash[5], hash[7]);

  std::swap(hash[0], hash[2]);
  std::swap(hash[3], hash[5]);
  std::swap(hash[6], hash[8]);

  std::vector<std::pair<char, char>> newGoodMoves;
  for (auto move : goodMoves) {
    if (move.first == -1 && move.second == -1) {
      newGoodMoves.push_back(std::make_pair(move.first, 1));
    } else if (move.first == -1 && move.second == 0) {
      newGoodMoves.push_back(std::make_pair(0, 1));
    } else if (move.first == -1 && move.second == 1) {
      newGoodMoves.push_back(std::make_pair(1, move.second));
    } else if (move.first == 0 && move.second == -1) {
      newGoodMoves.push_back(std::make_pair(-1, 0));
    } else if (move.first == 0 && move.second == 0) {
      newGoodMoves.push_back(move);
    } else if (move.first == 0 && move.second == 1) {
      newGoodMoves.push_back(std::make_pair(1, 0));
    } else if (move.first == 1 && move.second == -1) {
      newGoodMoves.push_back(std::make_pair(-1, move.second));
    } else if (move.first == 1 && move.second == 0) {
      newGoodMoves.push_back(std::make_pair(0, -1));
    } else if (move.first == 1 && move.second == 1) {
      newGoodMoves.push_back(std::make_pair(move.first, -1));
    }
  }
  goodMoves = newGoodMoves;
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


std::vector<Point*> Pattern::getGoodMoves(Board* b, Point move) {
  std::vector<Point*> movesToReturn;
  for (auto p : goodMoves) {
    Point temp((char)(p.first + move.row), (char)(p.second + move.column));
    movesToReturn.push_back(b->getPoint(&temp));
  }
  assert(movesToReturn.size() > 0);
  return movesToReturn;
}

char Pattern::getRandomColor() {
  int i = rand() % 4;
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

void Pattern::determineRandomGoodMoves() {
  int i = -1, j = -1;
  for (char c : hash) {
    if (c == '_' && (rand() % 2 == 0 || goodMoves.size() == 0))
      goodMoves.push_back(std::make_pair(i, j));
    if (j == 1) {
      j = -2;
      i++;
    }
    j++;
  }
}

bool Pattern::isLegalPattern() {
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
}

void Pattern::mutate() {
  goodMoves.clear();
  determineRandomGoodMoves(); 
}
Pattern Pattern::getMutated() {
  Pattern p = *this;
  p.mutate();
  return p;
}

Pattern Pattern::getRandomPattern() {
  Pattern p;
  for (unsigned int i = 0; i < 4; ++i) {
    p.hash += getRandomColor();
  }
  p.hash += 'B';
  for (unsigned int i = 0; i < 4; ++i) {
    p.hash += getRandomColor();
  }

  p.determineRandomGoodMoves();

  if (p.goodMoves.size() > 0
    && p.isLegalPattern())
    return p;
  return getRandomPattern();
}

std::ostream& operator<<(std::ostream &os, const Pattern &pattern) {
  int i = 0, j = 0;
  for (char c : pattern.hash) {
    os << c;
    ++i;
    if (i == 3) {
      i = 0;
      ++j;
      os << std::endl;
    }
  }

  for (i = -1; i < 2; ++i) {
    for (j = -1; j < 2; ++j) {
      bool found = false;
      for (auto x : pattern.goodMoves) {
        if (x.first == i && x.second == j) {
          found = true;
          break;
        }
      }
      if (found)
        os << "1";
      else
        os << "0";
    }
    os << "\n";
  }
  os << "\n";

  return os;
}

std::istream& operator>>(std::istream &is, Pattern& pattern) {
  /* Example Pattern to be read in
    BWW
    BBB
    ___
    000
    000
    110
  */

  pattern = Pattern();

  char matrix[3][3];
  int i = 0, j = 0;
  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      is >> matrix[i][j];
    }
  }

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      pattern.hash += matrix[i][j];
    }
  }

  for (i = -1; i < 2; ++i) {
    for (j = -1; j < 2; ++j) {
      char input = 0;
      is >> input;
      if (input == '1') {
        pattern.goodMoves.push_back(std::pair<char, char>(i, j));
      }
    }
  }

  return is;
}
