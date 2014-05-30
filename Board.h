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
  std::vector<Group*> emptyGroups;
  Player turn;

  Point lastMove;
  Point secondLastMove;

  Point koPoint;

  unsigned int capturedBlack;
  unsigned int capturedWhite;

  std::vector<Point> possibleMoves;

  explicit Board(int newBoardSize);

 private:
  Board(const Board& b);
  Board& operator=(const Board& b);

 public:
  ~Board();

  void init();

  bool operator==(const Board &b);

  // void initialize(Player computer);
  Board* clone();

  bool isValidMove(const Point &move);
  std::vector<Point> getNeighbors(const Point &p);
  bool hasPathAStar(const Point &from, const Point &to);

  // Fuego
  void getSimpleScore(float* whiteScore, float* blackScore);
  float getTaylorScore(float komi);

  bool isGameOver(GameResult *result);

  void updateStructures(const Point &move);
  void updateEmptyGroups(const Point &move, 
    const std::vector<Point> &capturedStones);

  unsigned int removeDeadStones(const Player &color, const Point &move);

  void makeMove(Point move);
  void makeRandomMove();
  GameResult playRandomGame();
  void show();
  bool isSuicide(const Point &move);

  // private:
  void getPossibleMoves();
  void oldGetPossibleMoves();
};
#endif  // BOARD_H_
