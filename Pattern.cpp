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


void Pattern::rotate90() {
  char matrix[3][3];
  int i = 0, j = 0;
  for (char c : hash) {
    matrix[i][j] = c;
    ++i;
    if (i == 3) {
      i = 0;
      ++j;
    }
  }

  // transpose
  std::swap(matrix[1][0], matrix[0][1]);
  std::swap(matrix[2][0], matrix[0][2]);
  std::swap(matrix[1][2], matrix[2][1]);

  // reverse rows
  std::swap(matrix[0][0], matrix[0][2]);
  std::swap(matrix[1][0], matrix[1][2]);
  std::swap(matrix[2][0], matrix[2][2]);

  hash = "";

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      hash += matrix[i][j];
    }
  }
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
  os << std::endl;

  os << "NumGoodMoves - " << pattern.goodMoves.size() << std::endl;
  for (auto x : pattern.goodMoves) {
    os << (int)x.first << " " << (int)x.second << std::endl;
  }

  printf("TESTING\n%s\nTESTING END\n", pattern.hash.c_str());
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

  printf("reading pattern\n");
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
      char input;
      is >> input;
      if (input == '1') {
        pattern.goodMoves.push_back(std::pair<char, char>(i, j));
      }
    }
  }

  char temp = 'f';
  is >> temp; // newline
  /*

  pattern.hash.clear();
  
  std::string temp;
  std::getline(is, temp);
  temp.erase(temp.end());

  pattern.hash += temp;

  std::getline(is, temp);
  temp.erase(temp.end());

  pattern.hash += temp;

  std::getline(is, temp);
  temp.erase(temp.end());

  pattern.hash += temp;
*/
  // std::cout << std::endl << " Pattern: \n" << pattern.hash << std::endl;

  return is;
}
