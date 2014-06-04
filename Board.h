#ifndef BOARD_H_
#define BOARD_H_

#define RUNNING_TESTS

#include <vector>

#include "Group.h"

class Group;

class Board {
 public:
  int boardSize;
  Point ***positions;
  std::vector<Group*> blackGroups;
  std::vector<Group*> whiteGroups;
  std::vector<Group*> emptyGroups;
  Player turn;

  Point* lastMove;
  Point* secondLastMove;

  Point* koPoint;

  Point* pass;

  unsigned int capturedBlack;
  unsigned int capturedWhite;

  std::vector<Point*> possibleMoves;

  explicit Board(int newBoardSize);

 private:
  Board(const Board& b);
  Board& operator=(const Board& b);

  void makeMove(Point* move);

 public:
  ~Board();
  void init();

  bool operator==(const Board &b);

  Board* clone();

  bool isValidMove(Point move);
  std::vector<Point*> getNeighbors(Point *p);
  bool hasPathAStar(const Point &from, const Point &to);
  bool hasPathDFS(Point *from, Point *to);

  // Fuego
  void getSimpleScore(float* whiteScore, float* blackScore);
  float getTaylorScore(float komi);

  bool isGameOver(GameResult *result);

  void updateStructures(Point *move);
  void updateEmptyGroups(Point *move, 
    const std::vector<Point*> &capturedStones);

  unsigned int removeDeadStones(const Player &color, Point *move);

  void makeMove(Point move);

  void makeRandomMove();
  GameResult playRandomGame();

  void show();

  bool isSuicide(Point move);
  bool isSuicide(Point* move);

  // private:
  void getPossibleMoves();
  void oldGetPossibleMoves();
  
  Point* getPoint(Point* p);
};
#endif  // BOARD_H_
