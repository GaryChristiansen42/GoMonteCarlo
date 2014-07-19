#include "Pattern3x3.h"

#include "Board.h"
#include <assert.h>

Pattern3x3::Pattern3x3() {

}

Pattern3x3::Pattern3x3(Point* p) {

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
  
  hash.reserve(9);
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

Pattern3x3::Pattern3x3(Board& b, Point& p) {

  if (&p == b.pass)
    return;
  
  Point* s = p.south;
  Point* sw = s->west;
  Point* se = s->east;
  Point* w = p.west;
  Point* e = p.east;
  Point* n = p.north;
  Point* nw = n->west;
  Point* ne = n->east;
  
  hash.reserve(9);
  hash += pointToChar(sw);
  hash += pointToChar(s);
  hash += pointToChar(se);
  hash += pointToChar(w);
  hash += pointToChar(&p);
  hash += pointToChar(e);
  hash += pointToChar(nw);
  hash += pointToChar(n);
  hash += pointToChar(ne);

}

void Pattern3x3::rotate90() {

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

  for (auto& move : goodMoves) {
    std::swap(move.first, move.second);
    if (move.second == -1)
      move.second = 1;
    else if (move.second == 1)
      move.second = -1;
  }
}

bool Pattern3x3::isLegalPattern() {
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

void Pattern3x3::determineRandomGoodMoves(std::default_random_engine& engine) {
  goodMoves.clear();
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
}

void Pattern3x3::mutate(std::default_random_engine& engine) {
  determineRandomGoodMoves(engine); 
}

Pattern3x3 Pattern3x3::getMutated(std::default_random_engine& engine) {
  Pattern3x3 p = *this;
  p.mutate(engine);
  return p;
}

std::ostream& operator<<(std::ostream &os, const Pattern3x3 &pattern) {
      assert(pattern.goodMoves.size() > 0);
  int i = 0, j = 0;
  for (char c : pattern.hash) {
    os << c;
    ++i;
    if (i == 3) {
      i = 0;
      ++j;
      os << '\n';
    }
  }

  bool foundOne = false;
  for (i = -1; i < 2; ++i) {
    for (j = -1; j < 2; ++j) {
      bool found = false;
      for (auto& x : pattern.goodMoves) {
        if (x.first == i && x.second == j) {
          found = true;
          break;
        }
      }
      if (found) {
        os << "1";
        foundOne = true;
      } else {
        os << "0";
      }
    }
    os << "\n";
  }
  os << "\n";
  assert(foundOne);

  return os;
}

std::istream& operator>>(std::istream &is, Pattern3x3& pattern) {
  // Example Pattern to be read in
  // BWW
  // BBB
  // ___
  // 000
  // 000
  // 110
  

  pattern = Pattern3x3();

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
