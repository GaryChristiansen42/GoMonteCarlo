#ifndef POINT_H_
#define POINT_H_

// (BOARD_SIZE, BOARD_SIZE) is pass
struct Point{
  int row;
  int column;

  Point();
  Point(int newRow, int newColumn);

  bool operator==(const Point &p);
  bool operator!=(const Point &p);
};
#endif  // POINT_H_
