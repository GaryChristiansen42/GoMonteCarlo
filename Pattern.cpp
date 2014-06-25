#include "Pattern.h"


Pattern::Pattern() : hash("") {

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
  for (char c : hash) {
    if (c == 'B')
      c = 'W';
    else if (c == 'W')
      c = 'B';
  }
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
  std::cout << std::endl << " Pattern: \n" << pattern.hash << std::endl;

  return is;
}
