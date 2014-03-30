#ifndef BOARD_H_
#define BOARD_H_

#define RUNNING_TESTS

#include <vector>

#include "Group.h"

class Group;

class Board {
 public:
  int boardSize;
  int **positions;
  std::vector<Group*> blackGroups;
  std::vector<Group*> whiteGroups;
  Player turn;

  Point lastMove;
  Point secondLastMove;

  std::vector<Point> possibleMoves;

  explicit Board(int newBoardSize);

 private:
  Board(const Board& b);
  Board& operator=(const Board& b);

 public:
  ~Board();

  bool operator==(const Board &b);

  // void initialize(Player computer);
  Board* clone();

  bool isValidMove(Point move);

  // Fuego
  void getSimpleScore(float* whiteScore, float* blackScore);
  float getTaylorScore(float komi);

  bool isGameOver(GameResult *result);

  void updateStructures(Point move);

  void removeDeadStones(Player color);

  void makeMove(Point move);
  void makeRandomMove();
  GameResult playRandomGame();
  void show();
  bool isSuicide(Point move);

  // private:
  void getPossibleMoves();
};
#endif  // BOARD_H_
