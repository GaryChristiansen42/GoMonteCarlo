#ifndef GO_H_
#define GO_H_

#define BOARD_SIZE 9

enum GameResult {
  None = -1,
  Draw = 0,
  WhiteWon = 1,
  BlackWon = 2
};

enum Player {
  Empty = 0,
  White = 1,
  Black = 2,
  Mark = 3
};
#endif  // GO_H_
