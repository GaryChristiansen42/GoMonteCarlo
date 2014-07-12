#include "Pattern5x5.h"

Pattern5x5::Pattern5x5() {

}

Pattern5x5::Pattern5x5(Point* p) {

  if (*p == Point(BOARD_SIZE, BOARD_SIZE))
    return;
  
  Point* s = p->south;
  Point* sw = s->west;
  Point* sww = sw->west;
  Point* se = s->east;
  Point* see = se->east;

  Point* ss = s->south;
  Point* ssw = ss->west;
  Point* ssww = ssw->west;
  Point* sse = ss->east;
  Point* ssee = sse->east;

  Point* w = p->west;
  Point* ww = w->west;
  Point* e = p->east;
  Point* ee = e->east;

  Point* n = p->north;
  Point* nw = n->west;
  Point* nww = nw->west;
  Point* ne = n->east;
  Point* nee = ne->east;
  
  Point* nn = n->north;
  Point* nnw = nn->west;
  Point* nnww = nnw->west;
  Point* nne = nn->east;
  Point* nnee = nne->east;

  hash += pointToChar(ssww);
  hash += pointToChar(ssw);
  hash += pointToChar(ss);
  hash += pointToChar(sse);
  hash += pointToChar(ssee);

  hash += pointToChar(sww);
  hash += pointToChar(sw);
  hash += pointToChar(s);
  hash += pointToChar(se);
  hash += pointToChar(see);

  hash += pointToChar(ww);
  hash += pointToChar(w);
  hash += pointToChar(p);
  hash += pointToChar(e);
  hash += pointToChar(ee);

  hash += pointToChar(nww);
  hash += pointToChar(nw);
  hash += pointToChar(n);
  hash += pointToChar(ne);
  hash += pointToChar(nee);

  hash += pointToChar(nnww);
  hash += pointToChar(nnw);
  hash += pointToChar(nn);
  hash += pointToChar(nne);
  hash += pointToChar(nnee);
}

void Pattern5x5::rotate90() {

  /*

    __B__
    __B__
    BBBBB
    __B__
    __B__

  */


  // transpose
  // std::swap(matrix[1][0], matrix[0][1]);
  // std::swap(matrix[2][0], matrix[0][2]);
  // std::swap(matrix[3][0], matrix[0][3]);
  // std::swap(matrix[4][0], matrix[0][4]);
  std::swap(hash[1*5+0], hash[0*5+1]);
  std::swap(hash[2*5+0], hash[0*5+2]);
  std::swap(hash[3*5+0], hash[0*5+3]);
  std::swap(hash[4*5+0], hash[0*5+4]);

  // std::swap(matrix[2][1], matrix[1][2]);
  // std::swap(matrix[3][1], matrix[1][3]);
  // std::swap(matrix[4][1], matrix[1][4]);
  std::swap(hash[2*5+1], hash[1*5+2]);
  std::swap(hash[3*5+1], hash[1*5+3]);
  std::swap(hash[4*5+1], hash[1*5+4]);

  // std::swap(matrix[3][2], matrix[2][3]);
  // std::swap(matrix[4][2], matrix[2][4]);
  std::swap(hash[3*5+2], hash[2*5+3]);
  std::swap(hash[4*5+2], hash[2*5+4]);

  // std::swap(matrix[3][4], matrix[4][3]);
  std::swap(hash[3*5+4], hash[4*5+3]);

  // reverse rows
  // std::swap(matrix[0][0], matrix[0][4]);
  // std::swap(matrix[0][1], matrix[0][3]);
  std::swap(hash[0*5+0], hash[0*5+4]);
  std::swap(hash[0*5+1], hash[0*5+3]);

  // std::swap(matrix[1][0], matrix[1][4]);
  // std::swap(matrix[1][1], matrix[1][3]);
  std::swap(hash[1*5+0], hash[1*5+4]);
  std::swap(hash[1*5+1], hash[1*5+3]);

  // std::swap(matrix[2][0], matrix[2][4]);
  // std::swap(matrix[2][1], matrix[2][3]);
  std::swap(hash[2*5+0], hash[2*5+4]);
  std::swap(hash[2*5+1], hash[2*5+3]);

  // std::swap(matrix[3][0], matrix[3][4]);
  // std::swap(matrix[3][1], matrix[3][3]);
  std::swap(hash[3*5+0], hash[3*5+4]);
  std::swap(hash[3*5+1], hash[3*5+3]);

  // std::swap(matrix[4][0], matrix[4][4]);
  // std::swap(matrix[4][1], matrix[4][3]);
  std::swap(hash[4*5+0], hash[4*5+4]);
  std::swap(hash[4*5+1], hash[4*5+3]);

  for (auto& move : goodMoves) {
    std::swap(move.first, move.second);
    if (move.second == -2)
      move.second = 2;
    else if (move.second == -1)
      move.second = 1;
    else if (move.second == 1)
      move.second = -1;
    else if (move.second == 2)
      move.second = -2;
  }
}

bool Pattern5x5::isLegalPattern() {
  if (hash[12] != 'B' && hash[12] != 'W')
    return false;

  char matrix[5][5];
  int i = 0, j = 0, pos = 0;
  for (i = 0; i < 5; ++i) {
    for (j = 0; j < 5; ++j) {
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

void Pattern5x5::determineRandomGoodMoves(std::default_random_engine& engine) {
  goodMoves.clear();
  std::uniform_int_distribution<> boolDist(0, 1);
  int i = -2, j = -2;
  for (char c : hash) {
    if (c == '_' && (boolDist(engine) == 0))
      goodMoves.push_back(std::make_pair(i, j));
    if (j == 2) {
      j = -3;
      i++;
    }
    j++;
  }
}

void Pattern5x5::mutate(std::default_random_engine& engine) {
  determineRandomGoodMoves(engine); 
}

Pattern5x5 Pattern5x5::getMutated(std::default_random_engine& engine) {
  Pattern5x5 p = *this;
  p.mutate(engine);
  return p;
}

Pattern5x5 Pattern5x5::getRandomPattern(std::default_random_engine& engine) {
  Pattern5x5 p;
  for (unsigned int i = 0; i < 12; ++i) {
    p.hash += getRandomColor(engine);
  }
  p.hash += 'B';
  for (unsigned int i = 0; i < 12; ++i) {
    p.hash += getRandomColor(engine);
  }

  p.determineRandomGoodMoves(engine);

  if (p.isLegalPattern())
    return p;
  return getRandomPattern(engine);
}

std::ostream& operator<<(std::ostream &os, const Pattern5x5 &pattern) {
  int i = 0, j = 0;
  for (char c : pattern.hash) {
    os << c;
    ++i;
    if (i == 5) {
      i = 0;
      ++j;
      os << '\n';
    }
  }

  for (i = -2; i < 3; ++i) {
    for (j = -2; j < 3; ++j) {
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

std::istream& operator>>(std::istream &is, Pattern5x5& pattern) {
  // Example Pattern to be read in
  // BWW
  // BBB
  // ___
  // 000
  // 000
  // 110
  

  pattern = Pattern5x5();

  char matrix[5][5];
  int i = 0, j = 0;
  for (i = 0; i < 5; ++i) {
    for (j = 0; j < 5; ++j) {
      is >> matrix[i][j];
    }
  }

  for (i = 0; i < 5; ++i) {
    for (j = 0; j < 5; ++j) {
      pattern.hash += matrix[i][j];
    }
  }

  for (i = -2; i < 3; ++i) {
    for (j = -2; j < 3; ++j) {
      char input = 0;
      is >> input;
      if (input == '1') {
        pattern.goodMoves.push_back(std::pair<char, char>(i, j));
      }
    }
  }

  return is;
}
